#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char **argv)
{
    // This is using the built in syscall function that glibc provides
    // It will handle the architecture dependent calling conventions for us and
    // even has all the sys calls mapped to nice SYS_... macros
    pid_t tid = syscall(SYS_gettid);
    printf("Hello world from: %d\n", tid);
    return 0;
}
