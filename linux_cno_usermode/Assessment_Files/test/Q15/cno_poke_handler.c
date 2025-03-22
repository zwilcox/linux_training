#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main() {
    int wstatus = 0;
    int err = 0;
    pid_t pid = 0;

    // fork and exec
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(-1);
    }

    if (pid == 0) {
        // child
        execl("./cno_poke", "./cno_poke", NULL);
        perror("execl");
        exit(-1);
    }

    // we don't PTRACE_ATTACH, because the cno_poke will do a PTRACE_TRACEME for us and raise(SIGSTOP);
    do {
        err = waitpid(pid, &wstatus, 0);
    } while (err == -1 && errno == EINTR);
    if (err == -1) {
        perror("waitpid");
        exit(-1);
    }

    fprintf(stderr, "Attached to child! Changing changeme\n\n");

    /*************************************************************/
    /*************************************************************/
    /* STUDENTS INSERT CODE HERE TO READ AND WRITE TO "changeme" */
    /*************************************************************/
    /*************************************************************/


    // continue the child
    //ptrace(PTRACE_DETACH, pid, 0, 0);
    ptrace(PTRACE_CONT, pid, 0, 0);

    // wait for child to finish
    waitpid(pid, &wstatus, 0);

    fprintf(stderr, "\nChild finished\n");
    return err;
}
