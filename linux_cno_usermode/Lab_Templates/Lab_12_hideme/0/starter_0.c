// Hide Me Option 0
//
// Checks if "/password/" is in the current working directory
// and outputs the flag as a new hidden file to that directory
//

// flag must start with "FLAG{" and end with "}"
// flag must only consist of alphanumeric characters and underscores
#define FLAG	"FLAG{default_flag}"

#define DIRNAME	"/password"

#define FNAME	".secretsauce"

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	char* cwd = NULL;
	char* found = NULL;
	int dirfd = -1;
	int newfd = -1;

	(void)argc; (void)argv;

	cwd = getcwd(NULL, 0);
	if (cwd == NULL) {
		// problem
		perror("getcwd");
		goto END;
	}

	found = strstr(cwd, DIRNAME);
	if (found) {
		*(found + strlen(DIRNAME)) = '\0';
		dirfd = open(cwd, O_DIRECTORY | O_PATH);
		if (dirfd == -1) {
			perror("open");
			goto END;
		}

		newfd = openat(dirfd, FNAME, O_WRONLY | O_CREAT | O_TRUNC, 0600);
		if (newfd == -1) {
			perror("openat");
			goto END;
		}

		if (-1 == write(newfd, FLAG, sizeof(FLAG))) {
			perror("write");
			goto END;
		}
	}
	
END:
	if (newfd != -1) {
		close(newfd);
	}
	if (dirfd != -1) {
		close(dirfd);
	}
	if (cwd != NULL) {
		free(cwd);
	}

	return 0;
}
