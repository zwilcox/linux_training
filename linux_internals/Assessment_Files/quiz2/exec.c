#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <error.h>

int main() {
    char read_buf[100] = {0};
    char fd_buf[100] = {0};
    char* ls_args[3];
    ls_args[0] = "question5_executable";
    ls_args[2] = NULL;
    pid_t pid;
    int fd[2];
    
    if (pipe(fd) == -1) {
        perror("pipe");
    }
    sprintf(fd_buf, "%d", fd[1]);
    ls_args[1] = fd_buf;

    pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    else if (pid == 0) {
        close(fd[0]);
        if (execv(ls_args[0], ls_args) == -1) {
            perror("execv");
            return -1;
        } 
    }
    else {
        close(fd[1]);
        read(fd[0], read_buf, 100);
        printf("FLAG IS:\n%s\n", read_buf);
        close(fd[0]);
    }
    return 0;
}
