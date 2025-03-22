#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <dlfcn.h>

#define RET_INS		0xC3

extern void *__libc_dlopen_mode (const char *__name, int __mode);

int main(int argc, char* argv[]) {
	int err = 0;
	int shellfd = -1;
	size_t buflen = 0;
	void* buf = NULL;
	int shellret = 0;

	if (argc < 2) {
		printf("Usage: %s ./shellcode.bin\n", argv[0]);
		err = -1;
		goto END;
	}

	if (argc >= 3) {
		printf("Testing loading of .so\n");
		__libc_dlopen_mode(argv[1], RTLD_LAZY);
		return 0;
	}

	shellfd = open(argv[1], 0);
	if (shellfd == -1) {
		printf("Unable to open file \"%s\". %s\n", argv[1], strerror(errno));
		err = -1;
		goto END;
	}

	buflen = lseek(shellfd, 0, SEEK_END);
	if (buflen < 0) {
		printf("Problem seeking in file. %s\n", strerror(errno));
		err = -1;
		goto END;
	}

	if (buflen == 0) {
		printf("Empty file!\n");
		err = -1;
		goto END;
	}

	if (-1 == lseek(shellfd, 0, SEEK_SET)) {
		printf("Problem seeking to beginning of file. %s\n", strerror(errno));
		err = -1;
		goto END;
	}
	
	buf = mmap(
		NULL,
		buflen+1,
		PROT_EXEC | PROT_READ | PROT_WRITE,
		MAP_PRIVATE,
		shellfd,
		0
	);
	if (buf == (void*)-1) {
		printf("Problem mapping the file. %s\n", strerror(errno));
		err = -1;
		goto END;
	}

	// write a return at the end, so it returns to us hopefully
	((char*)buf)[buflen] = RET_INS;

	// call into the shellcode
	printf("Calling shellcode\n");
	shellret = ((int(*)())buf)();
	printf("Shellcode returned %d\n", shellret);

END:
	if (buf != NULL) {
		munmap(buf, buflen);
	}

	if (shellfd != -1) {
		close(shellfd);
	}

	return err;
}


