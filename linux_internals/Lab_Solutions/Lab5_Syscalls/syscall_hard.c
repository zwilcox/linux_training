#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>

// Make sure to extern the function name so that the linker knows what to look
// for when you are linking against your assembly
extern int mysyscall(const char* filename, mode_t mode);

int main(int argc, char **argv)
{
    // Call your function that is defined in assembly.
    //
    // Things to remember for your assembly
    // 1. What architecture are we building for? x86, x86_64, ARM etc
    // 2. What is the calling convetion for that architecture?
    // 3. What is the syscall calling convention for that architecture
    mysyscall("mysyscalldir", S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH);
    return 0;
}
