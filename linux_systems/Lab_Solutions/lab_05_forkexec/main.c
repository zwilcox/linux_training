#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t create_process(char *argv[]);

int main() {
  pid_t pid;
  int i;
  // int status;

  char *child_argv[4];
  char name[] = "/usr/bin/firefox";
  child_argv[0] = name;
  child_argv[1] = "--private-window";
  child_argv[1] = "mantech.instructure.com";
  child_argv[2] = NULL;

  for (i = 0; i < 5; i++) {
    pid = create_process(child_argv);
    if (pid == -1) {
      return EXIT_FAILURE;
    }
    printf("I'm the parent, and my child's PID is %ld\n", (long)pid);

    if (wait(NULL) == -1) {
      perror("waitpid");
    }
    printf("Someone terminated the child! Let's restart it...\n");
  }

  kill(pid, SIGINT);

  return EXIT_SUCCESS;
}

pid_t create_process(char *argv[]) {
  pid_t pid;

  pid = fork();
  if (pid == -1) {
    perror("fork");
    return -1;
  }

  if (pid == 0) {
    printf("I'm the child\n");
    if (execv(argv[0], argv) == -1) {
      perror("execv");
      exit(EXIT_FAILURE);
    }
  }

  return pid;
}
