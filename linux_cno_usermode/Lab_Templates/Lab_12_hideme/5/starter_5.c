// Hide Me Option 5
//
// if the file "/tmp/password" exists, then maps it. Every 300 ms replaces the 0th byte
// in the file with the next character from the flag, until the flag is written out completely
//

// flag must start with "FLAG{" and end with "}"
// flag must only consist of alphanumeric characters and underscores
#define FLAG	"FLAG{default_flag}"

#define PATH	"/tmp/password"

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	(void)argc; (void)argv;

	size_t i = 0;
	int fd = -1;
	char* mapped = NULL;

	fd = open(PATH, O_RDWR);
	if (fd == -1) {
		goto END;
	}

	mapped = (char*)mmap(NULL, 1, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, fd, 0);
	if (mapped == (char*)-1) {
		perror("mmap");
		goto END;
	}

	for (i = 0; i < sizeof(FLAG); i++) {
		if (usleep(1000 * 100)) {
			perror("usleep");
		}

		*mapped = FLAG[i];

		if (msync(mapped, 1, MS_SYNC | MS_INVALIDATE)) {
			perror("msync");
		}
	}

END:
	if (mapped != (char*)-1) {
		munmap(mapped, 1);
	}
	if (fd != -1) {
		close(fd);
	}

	return 0;
}
