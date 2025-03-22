// Hide Me Option 6
//
// If a FIFO exists in the current working directory named "./password",
// then reads from it, using the value read as a file path. If that file
// exists, writes the flag to it
//

// flag must start with "FLAG{" and end with "}"
// flag must only consist of alphanumeric characters and underscores
#define FLAG	"FLAG{default_flag}"

#define PATH	"./password"

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	(void)argc; (void)argv;

	int fifofd = -1;
	struct stat statbuf = {0};

	fifofd = open(PATH, O_WRONLY);
	if (fifofd == -1) {
		goto END;
	}

	if (-1 == fstat(fifofd, &statbuf)) {
		perror("fstat");
		goto END;
	}

	if (S_ISFIFO(statbuf.st_mode)) {
		if (-1 == write(fifofd, FLAG, sizeof(FLAG))) {
			perror("write");
			goto END;
		}
	}
	
END:
	if (fifofd != -1) {
		close(fifofd);
	}

	return 0;
}
