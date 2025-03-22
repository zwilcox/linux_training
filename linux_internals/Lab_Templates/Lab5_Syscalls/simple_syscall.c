#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, char **argv)
{
    // This is using the built in syscall function that glibc provides
    // It will handle the architecture dependent calling conventions for us and
    // even has all the sys calls mapped to nice SYS_... macros
    // Note: solution file provided at the end of the lab is making a syscall to 'gettid' (if you want to match)
    // Code goes here
}
