#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void lab_pause(pid_t pid);
void lab_resume(pid_t pid);
void lab_stop(pid_t pid);
void lab_slowdown(pid_t pid);
void lab_speedup(pid_t pid);

int main() {
    char *args[1];
    pid_t pid;
    int wstatus;
    int looping;

    args[0] = "./toofast";
    args[1] = NULL;
    
    pid = fork();
    if (pid == 0) {
        execv(args[0], args);
    }
    printf("spawned child %d\n", pid);
    
    looping = 1;
    while (looping) {
        int ch = getchar();
        switch(ch) {
            case 'u':
                lab_speedup(pid);
                break;
            case 'd':
                lab_slowdown(pid);
                break;
            case 'p':
                lab_pause(pid);
                break;
            case 's':
                lab_resume(pid);
                break;
            case 'q':
                lab_stop(pid);
                looping = 0;
                break;
        }
    }
    wait(&wstatus);
    return 0;
}

void lab_pause(pid_t pid) {
    kill(pid, SIGSTOP);
}

void lab_resume(pid_t pid) {
    kill(pid, SIGCONT);
}

void lab_stop(pid_t pid) {
    kill(pid, SIGKILL);
}

void lab_slowdown(pid_t pid) {
    kill(pid, SIGUSR2);
}

void lab_speedup(pid_t pid) {
    kill(pid, SIGUSR1);
}
