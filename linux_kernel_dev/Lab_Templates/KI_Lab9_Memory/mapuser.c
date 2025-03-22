#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>

int main(int argc, char** argv) {
    int fd;
	unsigned long i;
    char* mapped;
	fd_set rfd;

    if (argc < 2) {
        printf("Usage: %s /path/to/mapdevice\n", argv[0]);
        return -1;
    }

    // TODO: get file path from argv
    // TODO: call mmap

	while (1) {
		FD_ZERO(&rfd);
		FD_SET(fd, &rfd);
		if (select(fd+1, &rfd, NULL, NULL, NULL) < 1) {
			perror("select");
			return -1;
		}
		// TODO: print the pid of each user space process that is mapped to this memory

		printf("-\n");
	}

    munmap(mapped, 0x1000);

    return 0;
}
