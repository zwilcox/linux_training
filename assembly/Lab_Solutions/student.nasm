BITS 64            ;you must specify bits mode
SEGMENT .text   ;you must specify a section

DEFAULT REL

GLOBAL lab0_1, labSize0_1

GLOBAL lab1_1, lab1_1_size
GLOBAL lab1_2, lab1_2_size
GLOBAL lab1_3, lab1_3_size

GLOBAL lab2_1, lab2_1_size
GLOBAL lab2_2, lab2_2_size
GLOBAL lab2_3, lab2_3_size
GLOBAL lab2_4, lab2_4_size

GLOBAL lab3_1, lab3_1_size
GLOBAL lab3_2, lab3_2_size
GLOBAL lab3_3, lab3_3_size
GLOBAL lab3_4, lab3_4_size
GLOBAL lab3_5, lab3_5_size

GLOBAL lab4_1, lab4_1_size
GLOBAL lab4_2, lab4_2_size
GLOBAL lab4_3, lab4_3_size
GLOBAL lab4_4, lab4_4_size
GLOBAL lab4_5, lab4_5_size

GLOBAL lab5_1, lab5_1_size
GLOBAL lab5_2, lab5_2_size
GLOBAL lab5_3, lab5_3_size

GLOBAL lab6_1, lab6_1_size
GLOBAL lab6_2, lab6_2_size
GLOBAL lab6_3, lab6_3_size
GLOBAL lab6_4, lab6_4_size
GLOBAL lab6_5, lab6_5_size

GLOBAL lab7_1, lab7_1_size
GLOBAL lab7_2, lab7_2_size
GLOBAL lab7_3, lab7_3_size
GLOBAL lab7_4, lab7_4_size

GLOBAL lab8_1, lab8_1_size
GLOBAL lab8_2, lab8_2_size
GLOBAL lab8_3, lab8_3_size
GLOBAL lab8_4, lab8_4_size
GLOBAL lab8_5, lab8_5_size
GLOBAL lab8_6, lab8_6_size

GLOBAL lab9_1, lab9_1_size
GLOBAL lab9_2, lab9_2_size

GLOBAL lab10_1, lab10_1_size
GLOBAL lab10_2, lab10_2_size

GLOBAL lab11_1, lab11_1_size
GLOBAL lab11_2, lab11_2_size
GLOBAL lab11_3, lab11_3_size
GLOBAL lab11_4, lab11_4_size
GLOBAL lab11_5, lab11_5_size
GLOBAL lab11_6, lab11_6_size
GLOBAL lab11_7, lab11_7_size

lab0_1:
; GOAL:
;   Put a breakpoint in the code
; STEPS:
;   Run this with F5,  what happens?
;   Then try running this again with CTRL-F5
;   use CTRL-F5 for all future labs
;;;;;;;;;;;;; YOUR CODE BELOW
    ;int3
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab0_1_Size dq $-lab0_1 -1

lab1_1:
; GOAL:
;   Exchange values in registers RAX and RBX
;   PRESERVE: nothing
;       Do -NOT- use xchg
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rcx, rax
    mov rax, rbx
    mov rbx, rcx
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab1_1_size dq $-lab1_1 -1

lab1_2:
; GOAL:
;   Perform this action:
;     RAX = RBX + RCX + 0x42
;   PRESERVE: All others
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rax, 0x42
    add rax, rbx
    add rax, rcx
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab1_2_size dq $-lab1_2 -1

lab1_3:
; NEW INSTRUCTION: xchg <reg>, <reg>
;  -exchanges the values in registers of op1 and op2
;
; GOAL:
;   Exchange values in registers RAX and RBX
;   PRESERVE: All but RAX and RBX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    xchg rax, rbx
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab1_3_size dq $-lab1_3 -1

lab2_1:
; NEW INSTRUCTION: jmp <labEL>
; GOAL:
;   Use a single jmp instruction to create an infinite loop.
;   PRESERVE: all registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    jmp lab2_1
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab2_1_size dq $-lab2_1 -1

lab2_2:
; NEW INSTRUCTIONS: je <labEL>; jne <labEL>; cmp <op1>,<op2>
; GOAL: Create assembly version of this pseudo-code
;   if( RAX == 42 )
;      RBX = 1
;   else
;      RBX = 2
;
;   if( rcx == 42 )
;      rdx = 1
;   else
;      rdx = 2
;
;   PRESERVE: RAX, R10
;
;  Coding Convention:
;     Put spaces BEFORE each branch target and AFTER each BRANCH
;
;    BAD:
;       add bla, bla
;    .copyLoop:
;       cmp bla bla
;       jne .copyLoop
;       mov bla bla
;
;    GOOD:
;       add bla, bla
;
;    .copyLoop:
;       cmp bla bla
;       jne .copyLoop
;
;       mov bla bla
;
;    BONUS: Use the minimum number of instructions!
;    NOTE: That is a DECIMAL 42, not hex 42.
;;;;;;;;;;;;; YOUR CODE BELOW
    mov     rbx, 2
    cmp     rax, 42
    jne     .secondCompare

    mov     rbx, 1

.secondCompare:
    mov     rdx, 2
    cmp     rcx, 42
    jne     .exitFunction
    mov     rdx, 1
.exitFunction:
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab2_2_size dq $-lab2_2 -1

lab2_3:
; GOAL:
;   Initialize RCX to 0
;   Loop, adding 5 to RCX each time.
;   When RCX == RAX, exit the loop.
;   NOTE: only check RCX==RAX after the first
;    pass through the loop body. This is equivalent
;    to a 'do' loop in C:
;   do {
;     RCX += 5
;   } while( !(RCX==RAX) );
;
;  PRESERVE: RAX, RBX, RDX
;
;  The verifier should only take a split second to run your loop:
;   if it is more than a second or two, something is wrong!
;
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rcx, 0

.addLoop:
    add rcx, 5
    cmp rcx, rax
    jne .addLoop
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab2_3_size dq $-lab2_3 -1

lab2_4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   Multiply 333 by 51 using only the instructions
;    discussed so far!
;   Store the result in RAX.
;
;   PRESERVE: RBX, RDX
;
;   HINT: you will only need one loop
;   HINT2: Once you've done it with one loop, can you "cheat" to make it smaller
;          WITHOUT using any opcodes not covered in the class (ie the multiply opcode)
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rax, 0
    mov rcx, 0

.addLoop:
    add rax, 333
    add rcx, 1
    cmp rcx, 51
    jne .addLoop
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab2_4_size dq $-lab2_4 -1

lab3_1:
; GOAL:
;   Exchange AL and AH
;   PRESERVE: EBX, ECX, EDX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    xchg al, ah
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab3_1_size dq $-lab3_1 -1

lab3_2:
; GOAL:
;   Move upper 8-bits of RAX into DL
;   PRESERVE: RAX, RBX, RCX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rdx, rax
    shr rdx, 56
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab3_2_size dq $-lab3_2 -1

lab3_3:
; GOAL:
;   Set RAX equal to the value in AX
;   NOTE: What happens if you try to set the value of AH to RAX?
;   PRESERVE: RBX, RCX, RDX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    movzx rax, ax
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab3_3_size dq $-lab3_3 -1

lab3_4:
; GOAL:
;   Divide RAX by 64 (integer division)
;   PRESERVE: RBX, RCX, RDX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    shr rax, 6
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab3_4_size dq $-lab3_4 -1

lab3_5:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   Convert RAX from host byte order to network byte order.
;   This will mean reversing the byte order.
;   For example, if RAX contained the value 0xfedcba9876543210, you
;    would convert it to 0x1032547698badcfe.
;
;   Try to minimize the number of instructions!
;
;   PRESERVE: RDX and RBX
;   Note: In x64, a 32-bit operand will zero-extend to a 64-bit result!
;   ex. xor eax, eax ; RAX = 0
;
;;;;;;;;;;;;; YOUR CODE BELOW
    ;bswap rax ;this is the simplest:)

    mov rcx, rax ; copy of rax in rcx  rax=0xfedcba9876543210    rcx=0xfedcba9876543210
    shr rcx, 32  ; ecx has the upper half of rax rax=0xfedcba9876543210 rcx=0xfedcba98

    xchg cl, ch  ; swap byte of cl and ch rcx= 0xfedc98ba
    ror ecx, 16  ; rotate upper half or ecx into cx rcx=0x98bafedc
    xchg cl, ch  ; swap byte of cl and ch rcx=0x98badcfe

    xchg ah, al  ; rax=0xfedcba9876541032
    ror eax, 16  ; !!! this will zero out top half of rax!!! rax=0x10327654
    xchg ah, al  ; rax=0x10325476
    shl rax, 32  ; rax=0x1032547600000000
    or rax, rcx  ; rax=0x1032547698badcfe
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab3_5_size dq $-lab3_5 -1


lab4_1:
; GOAL: Exchange RAX and RBX using:
;       -ONLY MOV instructions
;       -the memory pointed to by RCX as temp storage for the swap
;   PRESERVE: RCX, RDX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    ;xchg rax, rbx
    mov [rcx], rax
    mov rax, rbx
    mov rbx, [rcx]
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab4_1_size dq $-lab4_1 -1

lab4_2:
; GOAL:
;   Set the unsigned long64 pointed to by RAX to the value
;    of the byte pointed to by RBX.
;   PRESERVE: RAX, RCX, RDX, value pointed to by RBX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    movzx rbx, byte[rbx]
    mov [rax], rbx
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab4_2_size dq $-lab4_2 -1

lab4_3:
; GOAL: Add RCX and RBX and store in RAX in one instruction.
;      RAX <- RCX + RBX
;   PRESERVE: RBX, RCX, RDX
;
;;;;;;;;;;;;; YOUR CODE BELOW
	lea rax, [rbx + rcx]
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab4_3_size dq $-lab4_3 -1

lab4_4:
; GOAL:
;   Fill the buffer pointed to by RAX with the byte-sized integers from 0-64 inclusive
;   So, a memory dump of RAX will look like: 00 01 02 03 04.....
;   PRESERVE: RAX, RBX, RDX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    int3
    mov        rcx, 64
.copyLoop:
    mov        byte [rax+rcx], cl
    loop    .copyLoop

    mov        byte [rax], cl
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab4_4_size dq $-lab4_4 -1

lab4_5:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   RAX points to a NULL-terminated ASCII string.
;   RBX points to memory to which you will write a 16-bit integer
;    followed by the string without a NULL-terminator.
;   The 16-bit integer will be the string length.
;    rax -> "Some string"
;   rbx -> buf (should look like the following when done...)
;    ----------------------------
;   |0B|Some string|
;    ----------------------------
;       ^^^^^^^^^^^ - original string w/o NULL
;    ^^ - First 2 bytes are length of string
;   PRESERVE: RAX, RBX, Memory at RAX.
;
;;;;;;;;;;;;; YOUR CODE BELOW
    xor     ecx, ecx ; set to zero

.copyLoop:
    mov     dl, [rax+rcx]
    cmp     dl, 0
    je      .doneCopyingString

    mov     [rbx+rcx+2],dl
    inc     rcx
    jmp     .copyLoop

.doneCopyingString:
    mov     [rbx], cx
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab4_5_size dq $-lab4_5 -1

lab5_1:
; GOAL:
;   Exchange RAX and RBX using ONLY push and pop instructions
;   PRESERVE: all but RAX, RBX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    push     rax
    push     rbx
    pop     rax
    pop     rbx
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab5_1_size dq $-lab5_1 -1

lab5_2:
; GOAL: Remove 0x800 bytes from the top of the stack.
;   NOTE: If you do it wrong, you'll likely crash!
;
;   PRESERVE: all but RSP
;
;
;;;;;;;;;;;;; YOUR CODE BELOW
    add rsp,0x800
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab5_2_size dq $-lab5_2 -1

lab5_3:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL: Copy the the NULL-terminated ASCII string pointed to by RAX
;   to the buffer pointed to by RBX, including trailing NULL byte.
;   Store the string length in RAX.
;
;   PRESERVE: all registers but RAX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    push rcx
    push rdx
    xor rcx, rcx ; set to zero

.copyLoop:
    mov dl, [rax+rcx]
    mov [rbx+rcx], dl
    inc rcx
    cmp dl, 0
    jne .copyLoop

    mov rax, rcx
    dec rax
    pop rdx
    pop rcx
;;;;;;;;;;;;; YOUR CODE ABOVE
    ret
lab5_3_size dq $-lab5_3 -1

lab6_1:
; GOAL:
;   Implement entire function:
;     int getStrLen(const char *sz);
;
;   Return the length of the NULL-terminated ASCII string.
;   Do NOT use RBP or previously unintroductred instructions, and don't overwrite RDI.
;
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    xor rax, rax ; set to zero

.mainLoop:
    mov dl, [rdi+rax]
    inc rax
    cmp dl, 0
    jne .mainLoop

    dec rax
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_1_size dq $-lab6_1


lab6_2:
; GOAL:
;   Implement entire function:
;     int addFSeven(int a, int b, int c, int d, int e, int f, int g);
;
;   Add the seven arguments together and return the result.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    lea rax, [rdi + rsi] ; a + b
    add rax, rcx ; c
    add rax, rdx ; d
    add rax, r8 ; e
    add rax, r9 ; f
    add rax, [rsp + 8]
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_2_size dq $-lab6_2

lab6_3:
; GOAL:
;   Implement entire function:
;     char *copyString(const char *sz);
;
;   Return a copy of the null-terminated string by calling the
;    following functions:
;
;        void * my_malloc(int size); //allocates heap buffer of specified size
;        int my_strlen(const char *str); //gets length of null-terminated string not including terminator
;        void my_memcpy(void *dst, const void *src, int size); //copies memory
;
;   NOTES:
;     You MUST call each of these functions, and no others.
;     You MUST NOT have any loops in your function.
;     You may assume that my_malloc will not fail.
;     The lab validation code will attempt to free the returned buffer.
;     Assume all the called functions preserve standard registers.
;     DO NOT forget to include to copy the string terminator!
;     You MAY use RSI and RDI as general registers.
;
;   CONVENTION: Put spaces between before and after a chunk of code associated
;               with a function call such as pushing parameters, saving registers
;               or restoring stack
;   PRESERVE: standard registers
;
extern my_malloc
extern my_strlen
extern my_memcpy
;;;;;;;;;;;;; YOUR CODE BELOW
    int3
    push r12
    push r13
    push r14

    mov r14, rdi ; Save off the param
    call my_strlen
    inc rax ; include the null!
    mov r13, rax ; Save off the size

    mov rdi, rax ; Set up the argument to my_malloc
    call my_malloc

    mov r12, rax ; Save off the malloc pointer
    mov rdx, r13 ; arg3
    mov rsi, r14 ; arg2
    mov rdi, rax ; arg1
    call my_memcpy
    mov rax, r12

    pop r14
    pop r13
    pop r12
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_3_size dq $-lab6_3

lab6_4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; GOAL:
;   Implement entire function:
;     unsigned long addNumbers(unsigned long count, ...);
;
;   Arguments consist of a count then that many unsigned integers.
;   Add all the arguments after the count, and return the result.
;
;   Restriction:
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
%define DWORD_SIZE 4

    push rbp
    mov rbp, rsp
    xor rax, rax

    cmp rdi, 0
    je .done
    add rax, rsi

    dec rdi
    cmp rdi, 0
    je .done
    add rax, rdx

    dec rdi
    cmp rdi, 0
    je .done
    add rax, rcx

    dec rdi
    cmp rdi, 0
    je .done
    add rax, r8

    dec rdi
    cmp rdi, 0
    je .done
    add rax, r9
    dec rdi

    lea rdx, [rbp + rdi*8 + 0x8]
.loop:
    cmp rdi, 0
    je .done
    add rax, [rdx]
    sub rdx, 8
    dec rdi
    jmp .loop

.done:
    pop rbp
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_4_size dq $-lab6_4

lab6_5:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   Call the C library function:
;    int printf(const char *format, ...)
;   and print out the string "Assembly rocks!\n"
;
;;;;;;;;;;;;; YOUR CODE BELOW
extern printf
section .data
lab6_5_string db "Assembly rocks", 0x0a, 0x00
section .text
lab6_6:
    lea rdi, [lab6_5_string]
	; variadic functions like printf need to know how many
	; vector registers are being used (zero in this case)
	; set rax to zero
	xor rax,rax
    call printf
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_5_size dq $-lab6_5

lab7_1:
; GOAL:
;   Implement internals of a function:
;
;   USE hardcoded integer offsets from RBP to access arguments.
;
;   unsigned long addNumbers(unsigned long a, unsigned long b, unsigned long c, unsigned long d, unsigned long e, unsigned long f, unsigned long g);
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
	struc PrettyStack
		.lastBaseFrame	resq	1
		.returnAddress	resq	1
		.param7		resq	1
	endstruc

    push rbp
    mov rbp, rsp

    lea rax, [rdi + rsi]
    add rax, rdx
    add rax, rcx
    add rax, r8
    add rax, r9
    ;add rax, [rbp + 0x10]
    add rax, [rbp + PrettyStack.param7]

    pop rbp
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab7_1_size dq $-lab7_1

lab7_2:
; GOAL:
;   Access arguments using stack frame base pointer (RBP) and a struc
;   if needed
;
;   Define your struc within the YOUR CODE section, name it 'locals7_2'
;
;   Alternatively, use %define to declare the location of your locals
;
;   You will implement entire function.
;     int my_strncmp(const char *szA, const char *szB, int maxLength);
;
;   Similar to but different than standard C strncmp:
;      You will return 0 if strings match.
;      You will return 1 if strings do NOT match.
;      Returns a result when either:
;         -maxLength characters have been examined
;         -the first difference between the strings is found
;         -a null terminator is reached in either string
;
;    Hint: check maxLength, THEN check char, THEN check for null terminator
;
;    TEST CASES:
;         my_strncmp("","",0) == 0
;         my_strncmp("","",1) == 0
;         my_strncmp("X","",1) == 1
;         my_strncmp("X","Y",1) == 1
;         my_strncmp("X","Y",0) == 0
;         my_strncmp("XX","XY",1) == 0
;         my_strncmp("XX","XY",2) == 1
;         my_strncmp("XX","XXX",2) == 0
;         my_strncmp("XX","XXX",3) == 1
;         my_strncmp("XX","XXX",2000); == 1
;
;   PRESERVE: standard registers
;;;;;;;;;;;;; YOUR CODE BELOW
    push rbp
    mov rbp, rsp

    %define QWORD_SIZE 8
    %define local_1 qword [rbp - QWORD_SIZE*1]
    %define max_length rdx
    %define szA rdi
    %define szB rsi

    xor ecx, ecx ; set to zero
    xor eax, eax ; set to zero

.cmpLoop:
    cmp rcx, max_length ; Compare count to max length
    je .exitFunction

    mov r8b, [szA+rcx]
    cmp r8b, [szB+rcx]
    jne .stringsMismatch

    cmp r8b, 0
    je  .exitFunction

    inc rcx
    jmp .cmpLoop

.stringsMismatch:
    inc rax
.exitFunction:
    pop rbp
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab7_2_size dq $-lab7_2

lab7_3:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   Reimplement _lab 6.3 (allocating copy of string), using
;    local variables in some way.
;
;   As helpful motivation to use locals, you may NOT use EBX, ESI, or EDI,
;     even if you store and restore them!
;
;   Use EBP to access both arguments and locals, in whichever manner you see fit.
;
;   PRESERVE: standard registers
;
;   Did you know??
;     You can use enter and leave opcodes in place of "push rbp/mov rbp, rsp".
;     Try it for this _lab
;
;;;;;;;;;;;;; YOUR CODE BELOW
extern my_malloc
extern my_strlen
extern my_memcpy
%define param  [rbp - 0x08]
%define length [rbp - 0x10]
%define buffer [rbp - 0x18]

    push rbp
    mov rbp, rsp
    sub rsp, 0x18

    mov param, rdi
    call my_strlen
    inc rax ; include the null!
    mov length, rax

    mov rdi, rax ; Set up the argument to my_malloc
    call my_malloc

    mov buffer, rax
    mov rcx, length ; arg3
    mov rsi, param ; arg2
    mov rdi, rax ; arg1
    call my_memcpy
    mov rax, buffer

    add rsp, 0x18
    pop rbp
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab7_3_size dq $-lab7_3

lab7_4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;    print the return addresses on the callstack to your function using printf
;    Use gdb command "bt" to view the callstack and verify your answer
;
;    PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
extern printf
section .data
lab7_4_string db "%p", 0x0a, 0x00
section .text
	;int3
	mov rsi, [rsp]
	lea rdi, [lab7_4_string]
	; variadic functions like printf need to know how many
	; vector registers are being used (zero in this case)
	; set rax to zero
	xor rax, rax
	call printf
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab7_4_size dq $-lab7_4

lab8_1:
; GOAL:
;   Implement entire function:
;      void memcpy(void *dst, void *src, int count);
;
;   You MUST use lodsb and stosb!
;
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rcx, rdx ; Move count into rcx
.copyLoop:
        cmp rcx, 0 ; Check if our count is at 0
    je .doneCopying
    lodsb
    stosb
        dec rcx
    jmp .copyLoop
.doneCopying:
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_1_size dq $-lab8_1

lab8_2:
; GOAL:
;   Implement entire function:
;      void memcpy(void *dst, void *src, int count);
;
;   You MUST use rep movsb!
;
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rcx, rdx
    rep movsb
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_2_size dq $-lab8_2

lab8_3:
; GOAL:
;   Implement entire function:
;        void memset(void *buf, int c, int count);
;
;   You MUST use rep stosb!
;
;   Hint: Nearly identical to _lab 8.2!
;
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rax, rsi
    mov rcx, rdx
    rep stosb
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_3_size dq $-lab8_3

lab8_4:
; GOAL:
;   Implement entire function:
;        void slideUp(void *dst, void *src, int size);
;
;   You must use rep movsb.
;   Buffers are overlapping and src is lower than dst: you will need
;    to copy from top down instead of bottom up.
;   If you set DF, don't forget to clear it before returning.
;
;   Example:
;   src
;   V
;   _________________
;   |A|B|C|D|E|F|G|H|
;   _________________________
;           | | | | | | | | |
;           _________________
;           ^
;           dst
;
;   **Notice moving [src] to [dst] would corrupt 'E' in src. Thus, we must
;   start from the end of the buffers and work backwards.
;
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    std
    lea rdi, [rdi + rdx - 1]
    lea rsi, [rsi + rdx - 1]
    mov rcx, rdx
    rep movsb
    cld
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_4_size dq $-lab8_4

lab8_5:
; GOAL:
;   Re-Implement _lab 7.2 using repe or repne and cmpsb
;   The %define arg access and frame stuff is optional
;
;   You will need to test the termination condition of the cmpsb
;   to determine if it exited because of mismatch or ECX==0 (try JNE).
;
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

	;We only need to compare up to min(strlen([szA|szB]), maxLength) characters:
	;**NOTE: This solution checks strlen(szA), but works just the same if checkgin strlen(szB)**

	;szA is in rdi already
	mov r9, rdi 	      ; save a copy of szA
	;szB is in rsi already
	mov rcx, rdx          ; maxLength
	xor rax,rax

	cmp rcx, 0
	je  .outMatch

	repne scasb           ; initial scan to determine maxLength = min(strlen(szA), maxLength)
	jne .brokeOnRcx       ; broke because rcx == 0, means didn't find end of szA up to provided maxlength
						  ; else, broke because we found end of szA before rcx == 0, thus strlen(szA) < maxlength
    sub rdi, r9           ; compute strlen(szA)
	mov rcx, rdi          ; and overwrite maxLength with strlen(szA)
	jmp .beginCmp

.brokeOnRcx:
	mov rcx, rdx          ; maxLength < strlen(szA), so just use provided maxLength

.beginCmp:
	mov rdi, r9           ;rdi was modified above by scasb, so reset it
	repe cmpsb
	je .outMatch          ; broke because rcx == 0, means strings are equal up to RCX. So return pass

.outMismatch:             ; else, broke beacuse szA[rcx] != szB[rcx]. So return fail
	inc rax
.outMatch:
	ret

; Here is how you could do it without using the 'rep' prefix. It's admittedly
;  less confusing it just doesn't fulfill the lab requirements.
;    xor rax, rax
;    mov rcx, rdx
;
;.compareLoop:
;    cmp rcx, 0
;    je .exitFunction
;    dec rcx
;    cmpsb
;    jne .stringsMismatch
;    cmp [rsi-1], al
;    je .exitFunction
;    jmp .compareLoop
;
;.stringsMismatch:
;    inc rax
;.exitFunction:
;    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_5_size dq $-lab8_5

lab8_6:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   Implement the entire function:
;     const char *my_memchr(void *buf, char ch, int count);
;
;   Return NULL if ch is not present in buf within count bytes.
;   Return pointer to ch otherwise.
;
;   You MUST use scasb either in a loop or with a repe OR repne prefix.
;   HINT: one form is easier...
;   You will need to test for whether it found it or exited because it
;    ran out of bytes.
;
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    mov rcx, rdx
.scanLoop:
    xor rax, rax
    cmp rcx, 0
    je .exitFunction

    mov rax, rsi
    scasb
    lea rax, [rdi -1]
    je .exitFunction
    dec rcx
    jmp .scanLoop
.exitFunction:
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_6_size dq $-lab8_6

lab9_1:
; GOAL:
;    Return with ZF true.
;
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    xor rax, rax

    ;cmp eax, eax
    ;mov al, 0xff
    ;inc al
;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab9_1_size dq $-lab9_1

lab9_2:
; GOAL:
;    CF and OF are almost never set at the same time.
;    Find a way to return with CF and OF both set.
;    PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
;    mov eax, 0x80000000
;    add eax, eax

;    mov eax, 0x7FFFFFFF
;    add eax, 1
;    stc

;    pushf
;    pop eax
;    or eax, 0x801
;    push eax
;    popf


;    mov eax, 0x8fffffff
;    shl eax, 1

    mov al, -128
    add al, -1
;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab9_2_size dq $-lab9_2

lab10_1:
; GOAL:
;    Implement entire function:
;        int my_divider(unsigned long dividend, unsigned long divisor);
;
;    Return Quotient.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW
    mov     rax, rdi
    mov     rcx, rsi
    xor     rdx, rdx
    div     rcx
;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab10_1_size dq $-lab10_1

lab10_2:
; GOAL:
;    Set RAX to 0.
;    test and complement bit 0 of RCX, if it was 1 increment RAX
;    test and complement bit 1 of RCX, if it was 0 increment RAX
;    test and reset bit 2 of RCX, if it was 1 increment RAX
;    test and reset bit 3 of RCX, if it was 0 increment RAX
;    test bit 4 of RCX, if it is 0 increment RAX
;    test bit 5 of RCX, if it is 1 increment RAX
;    if bit 6 of RCX is 1, set RDX to 2, else set RDX to 0 (do NOT use a jump!)
;
;   PRESERVE: standard registers
;             Make only the designated changes to RCX
;
;;;;;;;;;;;;; YOUR CODE BELOW
    xor        rax, rax
    xor        rdx, rdx

    btc        rcx, 0
    jnc        .BIT0_WAS_NOT_1
    inc        rax
.BIT0_WAS_NOT_1:

    btc        rcx, 1
    jc        .BIT1_WAS_NOT_0
    inc        rax
.BIT1_WAS_NOT_0:

    btr        rcx, 2
    jnc        .BIT2_WAS_NOT_1
    inc        rax
.BIT2_WAS_NOT_1:

    btr        rcx, 3
    jc        .BIT3_WAS_NOT_0
    inc        rax
.BIT3_WAS_NOT_0:

    bt        rcx, 4
    jc        .BIT4_WAS_NOT_0
    inc        rax
.BIT4_WAS_NOT_0:

    bt        rcx, 5
    jnc        .BIT5_WAS_NOT_1
    inc        rax
.BIT5_WAS_NOT_1:

    bt        rcx, 6
    setc    dl
    add        rdx, rdx
;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab10_2_size dq $-lab10_2

lab11_1:
; Write a function that takes a single argument (N)
; and returns the Nth fibonacci number. Assume the sequence starts at 0,
; and is 1-based (that is, the first number is 0, the second is 1, and there is no 0th number).
;  * This can be recursive or non-recursive. If you're ambitious, write both
;;;;;;;;;;;;; YOUR CODE BELOW
    ;recursive solution
    ;push  rdx
    ;cmp rdi, 1
    ;je .noAddition

    ;cmp rdi, 2
    ;je .noAddition

    ;push rdi
    ;dec rdi
    ;call lab11_1
    ;pop rdi
    ;mov rdx, rax

    ;sub rdi, 2
    ;call lab11_1
    ;add rdx, rax
    ;mov rax, rdx
    ;jmp .finish

;.noAddition:
    ;mov rax, rdi
    ;dec rax

;.finish:
    ;pop rdx
    ;ret

    ;non-recursive
    xor eax, eax
    cmp rdi, 1
    jbe .end

    mov edx, 1
    mov rcx, rdi
    dec rcx

.fib:
    xadd rax, rdx
    loop .fib

.end:
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_1_size dq $-lab11_1

lab11_2:
; Write a function that takes a single argument (N)
; and returns the sum of the natural numbers from 0 to N
; that are multiples of both 3 and 5
; * Bonus if you only use add/sub (no mul/divide)
;;;;;;;;;;;;; YOUR CODE BELOW
    ;using div
    xor r8d, r8d

.loopBegin:
    test rdi, rdi
    je .finish

    xor edx, edx ; Begin setup for division by 5
    mov rax, rdi
    mov r9d, 5
    div r9
    test rdx, rdx
    jnz .nextIteration

    xor edx, edx ; Begin setup for division by 3
    mov rax, rdi
    mov r9d, 3
    div r9
    test rdx, rdx
    jnz .nextIteration

    add r8, rdi

.nextIteration:
    dec rdi
    jmp .loopBegin

.finish:
    mov rax, r8
	ret

    ;no mul/div
    ;xor eax, eax
    ;mov ecx, 15

;.loopBegin:
    ;test rdi, rdi
    ;jz .finish
    ;mov rdx, rdi
;.mod:
    ;cmp rdx, rcx
    ;jl .modFin
    ;sub rdx, rcx
    ;jmp .mod
;.modFin:
    ;test rdx, rdx
    ;jnz .nextIteration
    ;add rax, rdi
;.nextIteration:
    ;dec rdi
    ;jmp .loopBegin

;.finish:
    ;ret
;;;;;;;;;;;;; YOUR CODE ABOVE

lab11_2_size dq $-lab11_2

lab11_3:
; Write the function
; int largestProduct(unsigned int arrayLen, int *array)
; Return the largest product of five consecutive integers in the array.
;  * Worded another way: Find the largest product of five consecutive integers within an N-length array of integers.
; On error return -1.
;;;;;;;;;;;;; YOUR CODE BELOW
    mov eax, 0xffffffff
    cmp edi, 5
    jb .finish
    test rsi, rsi
    jz .finish

    mov eax, 0x80000000
    sub edi, 4

.loop:
    mov r8d, dword [rsi]
    imul r8d, dword [rsi + 0x4]
    imul r8d, dword [rsi + 0x8]
    imul r8d, dword [rsi + 0xc]
    imul r8d, dword [rsi + 0x10]

    cmp eax, r8d
    cmovl eax, r8d

    add rsi, 0x4
    dec edi
    test edi, edi
    jnz .loop

.finish:
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_3_size dq $-lab11_3

lab11_4:
; Write the function
;	void xorEncDec(char *inOutStream, unsigned char key, unsigned long len)
; This is an in-place xor encoder/decoder. First use string instructions, then implement it without.
;;;;;;;;;;;;; YOUR CODE BELOW
    ;with string instructions
    mov rcx, rdx
    mov dl, sil
    mov rsi, rdi

.loop:
    lodsb
    xor al, dl
    stosb
    loop .loop
    ret

    ;without
    ;mov rcx, rdx

;.loop:
    ;mov al, byte[rdi + rcx - 1]
    ;xor al, sil
    ;mov byte[rdi + rcx - 1], al
    ;loop .loop

    ;ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_4_size dq $-lab11_4

lab11_5:
; Write the function
;	char* addCommas(char *numberString, unsigned long len)
; numberString is a string representing a number (e.g. "1234567")
; return a new string containing that number with commas inserted (e.g. "1,234,567").
; You may use any of the functions given to you in previous _labs.
; For convenience, the ascii representation of "," is 0x2c
;;;;;;;;;;;;; YOUR CODE BELOW
    push rbx
    push r12
    push r13
    mov rbx, rdi
    mov r12, rsi

    ;calc len of new string
    mov rax, rsi
    xor edx, edx
    xor r8d, r8d
    mov ecx, 3
    div ecx
    test rdx, rdx
    setz r8b
    lea rdi, [rsi + rax]
    sub rdi, r8
    mov r13, rdi
	inc rdi

    call my_malloc

    xor ecx, ecx
    lea rdx, [rax + r13 - 1]
    lea r8, [rbx + r12 - 1]
	mov byte [rdx + 1], 0x0

.loop:
    inc ecx
    and ecx, 0x3
    jnz .copybyte
    mov byte [rdx], 0x2c
    jmp .loopend

.copybyte:
    mov r9b, byte[r8]
    mov byte[rdx], r9b
    dec r8

.loopend:
    dec rdx
    cmp r8, rbx
    jge .loop

    pop r13
    pop r12
    pop rbx
    ret

;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_5_size dq $-lab11_5

lab11_6:
; Modify your solution to the Fibonacci problem to return the sum of even Fibonacci
; numbers up to the input number.
;;;;;;;;;;;;; YOUR CODE BELOW
    xor eax, eax
    cmp rdi, 1
    jbe .end

    xor r8d, r8d
    mov edx, 1
    mov rcx, rdi

.fib:
    xadd r8, rdx
    test rdx, 1
    jne .loop
    add rax, rdx
.loop:
    loop .fib

.end:
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_6_size dq $-lab11_6

lab11_7:
; Write the function
;	BOOL validatePassword(char *password, unsigned long len)
; This function should return 1 if the password is valid, 0 otherwise.
;
; Passwords must begin with an alphabetic character [a-zA-Z], contain at
; least one number [0-9], at least one symbol [!@#$%^&*()], at least one
; uppercase alphabetic character [A-Z], and at least one lowercase alphabetic
; character [a-z].
; For convenience
; Ascii value list: [a-z] are 0x61-0x7a, [A-Z] are 0x41-0x5a, [0-9] are 0x30-0x39.
; [!, @, #, $, %, ^, &, *, (, )] is [0x21, 0x40, 0x23, 0x24, 0x25, 0x5e, 0x26, 0x2a, 0x28, 0x29]
;;;;;;;;;;;;; YOUR CODE BELOW

    ; using 4 bits to store which requirements are met
    ; 3210 bit 0 for lower, bit 1 for upper, bit 2 numeric, bit 3 symbol
    xor eax, eax
    ;check first byte is alpha
    mov dl, byte [rdi]
    ;lower to upper
    and dl, 0xdf

    sub dl, 0x41
    cmp dl, 0x19
    ja .end

    xor ecx, ecx

.loop:
    cmp rsi, rcx
    je .check
    mov dl, byte [rdi + rcx]
    lea r8, [rdx - 0x61]
    cmp r8b, 0x19
    ja .upper
    or al, 0x1
.upper:
    lea r8, [rdx - 0x41]
    cmp r8b, 0x19
    ja .num
    or al, 0x2
.num:
    lea r8, [rdx - 0x30]
    cmp r8b, 0x9
    ja .sym
    or al, 0x4
;0x21 - 0x2a not 0x22,0x27
.sym:
    lea r8, [rdx - 0x21]
    cmp r8b, 0x9
    ja .sym2
    cmp r8b, 1
    je .looping
    cmp r8b, 6
    jne .hasSym
;0x40
.sym2:
    cmp dl, 0x40
    jne .looping
.hasSym:
    or al, 0x8

.looping:
    inc rcx
    jmp .loop

.check:
    cmp al, 0xf
    setz al
    movzx eax, al

.end:
    ret
;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_7_size dq $-lab11_7
