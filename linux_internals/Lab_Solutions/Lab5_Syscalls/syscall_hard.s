.section .text
.globl mysyscall
.type mysyscall, @function

// Things to remember for your assembly
// 1. What architecture are we building for? x86, x86_64, ARM etc
// 2. What is the calling convetion for that architecture?
// 3. What is the syscall calling convention for that architecture

mysyscall:
    // How many parameters does mkdir take?
    // What registers do the first two parameters go in on x86_64?
    // How do I pass the c code supplied arguments to the syscall?
    mov $83, %rax
    syscall
    ret

