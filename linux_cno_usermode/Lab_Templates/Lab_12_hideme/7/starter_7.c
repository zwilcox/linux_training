// Hide Me Option 7
//
// if argv[0] is "password" then accepts from stdin a filepath, and
// if it exists, xors the file's contents with the flag
//

// flag must start with "FLAG{" and end with "}"
// flag must only consist of alphanumeric characters and underscores
#define FLAG	"FLAG{default_flag}"

#define ARG0	"password"
#define PATHSZ	0x400

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char* argv[]) {
	char pathbuf[PATHSZ] = {0};
	int fd = -1;
	ssize_t amt = 0;
	char c = 0;
	size_t i = 0;
	
	if (argc < 1) {
		goto END;
	}

	if (strcmp(argv[0], ARG0)) {
		goto END;
	}
	
	if (!fgets(pathbuf, PATHSZ, stdin)) {
		perror("fgets");
		goto END;
	}


	fd = open(pathbuf, O_RDWR);
	if (-1 == fd) {
		goto END;
	}

	do {
		for (i = 0; i < sizeof(FLAG); i++) {
			amt = read(fd, &c, 1);
			if (amt == -1) {
				perror("read");
				goto END;
			}
			if (amt == 0) {
				break;
			}
			
			c ^= FLAG[i];
			
			if (-1 == lseek(fd, -1, SEEK_CUR)) {
				perror("lseek");
				goto END;
			}
			if (-1 == write(fd, &c, 1)) {
				perror("write");
				goto END;
			}
		}
	} while (amt != 0);
	

END:
	if (fd != -1) {
		close(fd);
	}

	return 0;
}
