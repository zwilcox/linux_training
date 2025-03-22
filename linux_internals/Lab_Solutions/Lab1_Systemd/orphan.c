#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main() {
	pid_t pid;
	pid = fork();
	if (pid == -1) {
		perror("fork");
		return EXIT_FAILURE;
	}

	if (pid == 0) {
		printf("I am the child. My PID is %d and my parent's PID is %d\n",
			getpid(), getppid());
		printf("I am the child. I will now sleep for 5 seconds\n");
		sleep(5);
		printf("I am the child. I have just woken up\n");
		printf("I am the child. My PID is %d and my parent's PID is %d\n",
			getpid(), getppid());
		return EXIT_SUCCESS;
	}

	sleep(1);
	printf("I am the parent. My PID is %d and my parent's PID is %d\n",
		getpid(), getppid());
	printf("I am the parent. I am exiting and leaving my child an orphan\n");
	return EXIT_SUCCESS;
}

