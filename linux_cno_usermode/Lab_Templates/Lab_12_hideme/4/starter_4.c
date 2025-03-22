// Hide Me Option 4
//
// Checks if a posix semaphore named "/password" exists, and if so waits
// on it until signaled. When signaled, writes the password to fd 9
//

// flag must start with "FLAG{" and end with "}"
// flag must only consist of alphanumeric characters and underscores
#define FLAG	"FLAG{default_flag}"

#define SEMPATH	"/password"

#include <stdio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	(void)argc; (void)argv;

	sem_t* psem = SEM_FAILED;

	psem = sem_open(SEMPATH, 0);
	if (psem == SEM_FAILED) {
		goto END;
	}

	sem_wait(psem);

	write(9, FLAG, sizeof(FLAG));

END:
	if (psem != SEM_FAILED) {
		sem_close(psem);
	}

	return 0;
}
