#include <curses.h> // ncurses library - https://www.sbarjatiya.com/notes_wiki/index.php/Using_ncurses_library_with_C
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t create_process(char *argv[]);

int main() {
  pid_t pid;
  int c;

  char *child_argv[2];
  char name[] = "./toofast";
  child_argv[0] = name;
  child_argv[1] = NULL;

  pid = create_process(child_argv);
  if (pid == -1) {
    return EXIT_FAILURE;
  }

  initscr(); // Initialize the window. It allocates memory for present window.

  /*
      Normally the terminal driver buffers the characters a user types until a
     new line or carriage return is encountered. Used to disable line buffering.
  */
  cbreak();

  /*
      Control the echoing of characters typed by the user to the terminal.
     noecho() switches off echoing. The reason you might want to do this is to
     gain more control over echoing or to suppress unnecessary echoing while
     taking input from the user through the getch() etc. functions.
  */
  noecho();

  while (c = getc(stdin)) {
    if (c == 'u') {
      kill(pid, SIGUSR1);
    } else if (c == 'd') {
      kill(pid, SIGUSR2);
    } else if (c == 's') {
      kill(pid, SIGCONT);
    } else if (c == 'p') {
      kill(pid, SIGSTOP);
    } else if (c == 'q') {
      kill(pid, SIGINT);
      break;
    }
  }

  endwin(); // Always use endwin() when you end the program. Forgetting to use
            // it may result in crashing the terminal window.

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
    if (execv(argv[0], argv) == -1) {
      perror("execv");
      exit(EXIT_FAILURE);
    }
  }

  return pid;
}
