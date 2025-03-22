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

    // get file path from argv
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        printf("Unable to open file %s\n", argv[1]);
        perror("open");
        return -1;
    }

    mapped = mmap(NULL, 0x1000, PROT_READ, MAP_SHARED, fd, 0);
    if (mapped == (char*)-1) {
        perror("mmap");
        return -1;
    }

	while (1) {
		FD_ZERO(&rfd);
		FD_SET(fd, &rfd);
		if (select(fd+1, &rfd, NULL, NULL, NULL) < 1) {
			perror("select");
			return -1;
		}
		for (i = 0; i < (0x1000 / sizeof(int)); i++) { //print the pid of each user space process that is mapped to this memory
			if (((int*)mapped)[i] == 0) {
				break;
			}
			printf("%d ", ((int*)mapped)[i]);
		} 
		printf("-\n");
	}

    munmap(mapped, 0x1000);

    return 0;
}
