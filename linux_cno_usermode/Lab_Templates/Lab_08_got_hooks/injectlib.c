#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <elf.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE			0x1000
#define MAX_ADDR_FMT_LEN	16
#define DUMP_COLS			16

__attribute__((constructor)) void on_load(void);
__attribute__((destructor)) void on_unload(void);
int debug(char* param);
int printf_wrapper(const char* fmt, ...);
ssize_t send_wrapper(int sockfd, const void *buf, size_t len, int flags);
void* get_main_image_addr(void);
void* get_got_addr(void *image_addr);
int hook_got_entry(void* got_addr, void* orig_func_ptr, void* hook_func_ptr);
void hexdump(const unsigned char* buf, unsigned int len);
void* attempt_hooks(void* got_addr);
void create_hooking_thread(void* got_addr);


int debug(char* param) {
	printf("Pfffttt... I ain't the real debuglib\n");
	return 0;
}

int printf_wrapper(const char* fmt, ...) {
	unsigned int maxlen;
	char* new_fmt;
	va_list args;
	int ret;
	char prefix[] = "[Hooked] ";

	maxlen = strlen(fmt) + sizeof(prefix);
	new_fmt = malloc(maxlen);
	if (new_fmt == NULL) {
		return -1;
	}
	snprintf(new_fmt, maxlen, "%s%s", prefix, fmt);
	va_start(args, fmt);
	ret = vprintf(new_fmt, args);
	va_end(args);
	free(new_fmt);
	return ret;
}

ssize_t send_wrapper(int sockfd, const void *buf, size_t len, int flags) {
	int ret;
	ret = send(sockfd, buf, len, flags);
	if (ret != -1) {
		printf("Someone is sending data:\n");
		hexdump(buf, ret);
	}
	return ret;
}

int hook_got_entry(void* got_addr, void* orig_func_ptr, void* hook_func_ptr) {

	//TODO: Hook!

	return 0;
}

void* get_got_addr(void *image_addr) {

	//TODO: Define this function
}

void* get_main_image_addr(void) {

	//TODO: Define this function
}

void on_load(void) {
	void* main_image_addr;
	void* got_addr;

	printf("Injected lib running...\n");
	main_image_addr = get_main_image_addr();
	if (main_image_addr == NULL) {
		return;
	}
	printf("Found main image base address at %p\n", main_image_addr);

	got_addr = get_got_addr(main_image_addr);
	if (got_addr == NULL) {
		return;
	}
	printf("Found GOT at %p\n", got_addr);

	create_hooking_thread(got_addr);
	return;
}

void on_unload(void) {
	printf("Unloading\n");
	return;
}

void create_hooking_thread(void* got_addr) {
	pthread_t tid;
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &tattr, attempt_hooks, got_addr);
	return;
}

void* attempt_hooks(void* got_addr) {
	int hooked_send = 0;
	int hooked_printf = 0;
	while (hooked_send == 0 || hooked_printf == 0) {
		printf("Attempting to hook GOT entries...\n");
		if (hooked_printf == 0) {
			hooked_printf = hook_got_entry(got_addr, printf, printf_wrapper);
		}
		if (hooked_send == 0) {
			hooked_send = hook_got_entry(got_addr, send, send_wrapper);
		}
		sleep(1);
	}
	return NULL;
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
