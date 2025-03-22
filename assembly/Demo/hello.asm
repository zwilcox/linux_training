section .data
    message db "Hello, world!", 10  ; String to be printed, including newline

section .text
    global _start

_start:
    mov rcx, rsp
    mov dword [rsp], 0x12345678
    inc rsp
    mov dword [rsp], 0x12345678
    xor rdx, rdx
    mov edx, dword [rcx]
