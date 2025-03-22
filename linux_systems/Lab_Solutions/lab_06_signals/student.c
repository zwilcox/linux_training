#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int main() {
    pid_t pid;
    pid_t temp_pid;
    int status;
    char input;

    // Fork the process
    pid = fork();

    if (pid == -1) {
        // Fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        // Child process: print some PID information
        printf("In child process with a PID of %d from fork()\n", pid);
        printf("In child process with a PID of %d from getpid()\n", getpid());
        printf("In child process with a PID of %d from getppid()\n", getppid());

        // Child process: Launch Firefox using execv()
        char *args[] = {"./toofast", NULL}; // Path to firefox executable
        execv(args[0], args);

        // If execv() fails
        perror("execv");
        exit(EXIT_FAILURE);
    }
    else {
        // Parent process: Wait for the child process (Firefox) to terminate
        printf("In parent process, child PID of %d running toofast\n", pid);
        printf("In parent process with a PID of %d from getpid()\n", getpid());
        printf("In parent process with a PID of %d from getppid()\n", getppid());

        printf("Sending SIGCONT %d\n", SIGCONT);
        sleep(10);
		kill(pid, SIGCONT);

        while(1) {
            printf("Enter a character (u, d, or q): ");

            // Get a character from the user
            input = getchar();

            // Consume the newline character from the input buffer
            getchar();

            // Check the input
            if (input == 'u') {
                kill(pid, SIGUSR2);
                printf("You entered 'u' (up)\n");
            } else if (input == 'd') {
                kill(pid, SIGUSR1);
                printf("You entered 'd' (down)\n");
            } else if (input == 'q') {
                printf("You entered 'q' (quit)\n");
                kill(pid, SIGKILL);
                break;
            } else {
                printf("Invalid input\n");
            }
        }

        // Wait for state changes of child process PID
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("toofast closed with exit status %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status)) {
            printf("toofast terminated by signal %d\n", WTERMSIG(status));
        }
        else {
            printf("toofast terminated unexpectedly.\n");
        }
    }
    

    return 0;
}
