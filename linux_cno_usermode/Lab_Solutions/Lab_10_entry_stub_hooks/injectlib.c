/*
 * This file contains code for hooking the encrypted_send routine of server.c.
 * This file is part of a shared library that gets loaded by server.c.
 * The on_load function gets called as soon as server.c loads this library.
 * on_load is responsible for hooking the encrypted_send routine of server.c.
 *
 * on_load first initializes the udis library. We use the udis library to get the length
 * of instructions at the start of encrypted_send. Next, we call entry_stub_create to create
 * our trampoline. Finally, we call entry_stub_hook to overwrite the first bytes of encrypted_send
 * with a jump to our wrapper function.
 *
 * Our wrapper function is my_encrypted_send. my_encrypted_send uses the trampoline created in
 * entry_stub_create to call the original encrypted_send routine.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <elf.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <dlfcn.h>
#include <udis86.h>

#define BUF_SIZE			0x1000
#define MAX_ADDR_FMT_LEN	16
#define DUMP_COLS			16
#define MAX_INSTRUCTION_SIZE 15
#define JMP_PATCH_SIZE		12

/* Entry Stub Trampoline structure */
typedef struct _entry_stub {
    void* original_entry;
    unsigned long entry_size;
    void* trampoline;
} entry_stub_t;

__attribute__((constructor)) void on_load(void);
__attribute__((destructor)) void on_unload(void);
int debug(char* param);
void hexdump(const unsigned char* buf, unsigned int len);
void patch_code(void* target, void* patch, unsigned long len);
void write_absolute_jump(void* jump_from, void* jump_to);
void* get_main_image_addr(void);
ssize_t my_encrypted_send(int sock, char* msg);


/* entry stub helpers */
void udis_init(void);
unsigned int get_instruction_length(void *addr);
int entry_stub_create(entry_stub_t* stub, void* original);
int entry_stub_hook(entry_stub_t* stub, void* wrapper_func);
int entry_stub_unhook(entry_stub_t* stub);
void entry_stub_free(entry_stub_t* stub);

ud_t g_ud_obj;
entry_stub_t stub;

int debug(char* param) {
	printf("Pfffttt... I ain't the real debuglib\n");
	return 0;
}

void on_load(void) {
	void* encrypted_send_addr;
	printf("Injected lib running...\n");

	/* we use dlsym here to not have to update our solution as the
	 * main binary changes. If doing this with a binary that did not
	 * export its own symbols to shared libraries, we could simply
	 * call get_main_image_addr and add the offset of encrypted_send
	 * to the result to get the address of encrypted_send */
	encrypted_send_addr = dlsym(NULL, "encrypted_send");
	printf("Address of encrypted_send is %p\n", encrypted_send_addr);

	udis_init();
	entry_stub_create(&stub, encrypted_send_addr);
    entry_stub_hook(&stub, (void*)my_encrypted_send);
	
	return;
}

void on_unload(void) {
	printf("Unloading\n");
	return;
}

ssize_t my_encrypted_send(int sock, char* msg) {
	ssize_t ret;
	
	ssize_t (*original)(int sock, char* msg);
	printf("I'm in your codez, stealin ur stuff\n");
	printf("msg is %s\n", msg);
	printf("Calling original...\n");
	original = stub.trampoline;
	ret = original(sock, msg);
	return ret;
}

void hexdump(const unsigned char* buf, unsigned int len) {
	char addr_buf[MAX_ADDR_FMT_LEN + 1];
	unsigned int i;
	unsigned int j;
	unsigned int power;
	unsigned int limit;
	unsigned int pad;
	char c;
	power = 1;
	for (i = 0; i < 64; i++) {
		if (len >= power) {
			power = power << 2;
			continue;
		}
		break;
	}
	snprintf(addr_buf, MAX_ADDR_FMT_LEN, "%%0%ullx:", i);
	for (i = 0; i < len; i++) {
		if (i % DUMP_COLS == 0) {
			printf(addr_buf, i);
		}
		if (i % 2 == 0) printf(" ");
		printf("%02x", buf[i]);
		if (i % DUMP_COLS == DUMP_COLS-1 || i == (len - 1)) {
			limit = (i % DUMP_COLS);
			pad = DUMP_COLS - limit;
			if (i == (len - 1)) {
				for (j = 0; j < pad-1; j++) {
					if (j % 2 != 0) {
						printf(" ");
					}
					printf("  ");
				}
			}
			printf("  ");
			for (j = 0; j <= limit; j++) {
				c = buf[i-limit+j];
				if (isalnum(c)) {
					printf("%c", c);
				}
				else {
					printf(".");
				}
			}
			printf("\n");
		}
	}
	printf("\n");
}

void patch_code(void* target, void* patch, unsigned long len) {
	void* target_page = (void*)((size_t)target & 0xfffffffffffff000);
	unsigned long protlen = len + (target - target_page);
	if (mprotect(target_page, protlen, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
		fprintf(stderr, "Failed to change permissions of memory before patch:\n%s\n", strerror(errno));
	}
	memcpy(target, patch, len);
	if (mprotect(target_page, protlen, PROT_READ | PROT_EXEC) == -1) {
		fprintf(stderr, "Failed to change permissions of memory back to original:\n%s\n", strerror(errno));
	}
	return;
}

void write_absolute_jump(void* jump_from, void* jump_to) {
	/*	currently:
	 *		movabs rax, 0x0000000000000000
	 *		jmp rax */
	unsigned char jmp_template[] = {0x48, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xe0};
	*((uint64_t*)(jmp_template + 2)) = (uint64_t)(jump_to);
	patch_code(jump_from, jmp_template, 12); /* patch is 12 bytes */
	return;
}

void* get_main_image_addr(void) {
	FILE* stream = NULL;
	unsigned char buf[BUF_SIZE];
	void* main_image_addr = NULL;
	char* eoi_char = NULL;

	stream = fopen("/proc/self/maps", "rb");
	if (stream == NULL) {
		fprintf(stderr, "Failure opening maps file\n");
		return NULL;
	}

	if (fread(buf, 1, BUF_SIZE, stream) <= 0) {
		fprintf(stderr, "Failed to read file\n");
		fclose(stream);
		return NULL;
	}
	fclose(stream);
	
	main_image_addr = (void *)strtol(buf, &eoi_char, 16);
	if (eoi_char == NULL || *eoi_char != '-') {
		fprintf(stderr, "Couldn't find main image address\n");
		return NULL;
	}
	return main_image_addr;
}

void udis_init(void) {
    ud_init(&g_ud_obj);
    ud_set_mode(&g_ud_obj, 64);
    ud_set_syntax(&g_ud_obj, UD_SYN_ATT);
	return;
}

unsigned int get_instruction_length(void* addr) {
    unsigned int len = 0;
    ud_set_input_buffer(&g_ud_obj, addr, MAX_INSTRUCTION_SIZE + JMP_PATCH_SIZE); 
    len = ud_disassemble(&g_ud_obj);
    printf("%d:\t%s %u\n", len, ud_insn_hex(&g_ud_obj), ud_insn_len(&g_ud_obj));
    return len;
}

int entry_stub_create(entry_stub_t* stub, void* original) {
    int ret;
    unsigned int instr_len;
    void* trampoline;
    int num_saved_bytes = 0;
    
    /* Determine how many bytes we need to save off */
    while (num_saved_bytes < JMP_PATCH_SIZE) {
        instr_len = get_instruction_length(original + num_saved_bytes);
        if (instr_len == 0) {
            fprintf(stderr, "Diassembler failed\n");
			return -1;
        }
        num_saved_bytes += instr_len;
    }

    /* Allocate memory for trampoline and change protections */
    trampoline = mmap(NULL, num_saved_bytes + JMP_PATCH_SIZE, 
            PROT_EXEC | PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (trampoline == NULL) {
        fprintf(stderr, "Could not allocate memory for trampoline\n");
		return -1;
    }

    /* Back up the original bytes that we overwrote by placing them in
	 * the trampoline */
    memcpy(trampoline, original, num_saved_bytes);
	printf("saved %d bytes\n", num_saved_bytes);
	hexdump(trampoline, num_saved_bytes);

	/* Write a jump at the end of our trampoline to go back to the original
	 * function at the end of our overwritten section of it */
    write_absolute_jump(trampoline + num_saved_bytes, original + num_saved_bytes);

    /* Populate entry stub structure */
    stub->trampoline = trampoline;
    stub->original_entry = original;
    stub->entry_size = num_saved_bytes;

    return 0;
}

int entry_stub_hook(entry_stub_t* stub, void* wrapper_func) {
    write_absolute_jump(stub->original_entry, wrapper_func);
    return 0;
}

int entry_stub_unhook(entry_stub_t* stub) {
    patch_code(stub->original_entry, stub->trampoline, stub->entry_size);
    return 0;
}

void entry_stub_free(entry_stub_t* stub) {
    if (munmap(stub->trampoline, stub->entry_size + JMP_PATCH_SIZE)) {
        fprintf(stderr, "Cleanup failed\n");
    }
	return;
}
