.global xorprint
.global printTime

.section .text


//void xorprint(int len, char key, char* ciphertext);

xorprint:
    movq %rdx, %r9 
    movq %rdi, %r8

    .loop:
        xor %sil, (%rdx)
        inc %rdx
        dec %rdi
        cmp $0, %rdi
    jne .loop

    mov %r9, %rsi
    mov $1, %rax  
    mov $1, %rdi 
    mov %r8, %rdx  

    syscall
    ret
