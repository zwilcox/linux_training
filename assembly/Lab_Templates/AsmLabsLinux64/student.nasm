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

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab3_1_size dq $-lab3_1 -1

lab3_2:
; GOAL:
;   Move upper 8-bits of RAX into DL
;   PRESERVE: RAX, RBX, RCX
;
;;;;;;;;;;;;; YOUR CODE BELOW

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

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab3_3_size dq $-lab3_3 -1

lab3_4:
; GOAL:
;   Divide RAX by 64 (integer division)
;   PRESERVE: RBX, RCX, RDX
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab3_4_size dq $-lab3_4 -1

lab3_5:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; GOAL:
;   Convert RAX from host byte order to network byte order.
;   This will mean reversing the byte order.
;   For example, if RAX contained the value 0xfedcba9876543210, you
;    would convert it to 0x1032547698badcfe.
;
;   Try to minimize the number of instructions!
;   PRESERVE: RDX and RBX
;   Note: In x64, a 32-bit operand will zero-extend to a 64-bit result!
;	ex. xor eax, eax ; RAX = 0
;
;;;;;;;;;;;;; YOUR CODE BELOW

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

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab4_1_size dq $-lab4_1 -1



lab4_2:
; GOAL:
;   Set the unsigned long 64 pointed to by RAX to the value
;    of the byte pointed to by RBX.
;   PRESERVE: RAX, RCX, RDX, value pointed to by RBX
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab4_2_size dq $-lab4_2 -1

lab4_3:
; GOAL: Add RCX and RBX and store in RAX in one instruction.
;      RAX <- RCX + RBX
;   PRESERVE: RBX, RCX, RDX
;
;;;;;;;;;;;;; YOUR CODE BELOW

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

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab4_4_size dq $-lab4_4 -1

lab4_5:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; GOAL:
;   RAX points to a NULL-terminated ASCII string.
;   RBX points to memory to which you will write a 16-bit integer
;    followed by the string without a NULL-terminator.
;   The 16-bit integer will be the string length.
;	rax -> "Some string"
;   rbx -> buf (should look like the following when done...)
;	----------------------------
;   |0B|Some string|
;	----------------------------
;       ^^^^^^^^^^^ - original string w/o NULL
;    ^^ - First 2 bytes are length of string
;   PRESERVE: RAX, RBX, Memory at RAX.
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab4_5_size dq $-lab4_5 -1

lab5_1:
; GOAL:
;   Exchange RAX and RBX using ONLY push and pop instructions
;   PRESERVE: all but RAX, RBX
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab5_1_size dq $-lab5_1 -1

lab5_2:
; GOAL: Remove 0x800 bytes from the top of the stack.
;   NOTE: If you do it wrong, you'll likely crash!
;   PRESERVE: all but RSP
;
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab5_2_size dq $-lab5_2 -1

lab5_3:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; GOAL: Copy the the NULL-terminated ASCII string pointed to by RAX
;   to the buffer pointed to by RBX, including trailing NULL byte.
;   Store the string length in RAX.
;   PRESERVE: all registers but RAX
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab5_3_size dq $-lab5_3 -1

lab6_1:
; GOAL:
;   Implement entire function:
;	 int getStrLen(const char *sz);
;
;   Return the length of the NULL-terminated ASCII string.
;   Do NOT use RBP or previously unintroducted instructions, and don't overwrite RDI.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_1_size dq $-lab6_1


lab6_2:
; GOAL:
;   Implement entire function:
;	 int  addSeven(int a, int b, int c, int d, int e, int f, int g);
;
;   Add the seven arguments together and return the result.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_2_size dq $-lab6_2

lab6_3:
; GOAL:
;   Implement entire function:
;	 char * copyString(const char *sz);
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

;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_3_size dq $-lab6_3

lab6_4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   Implement entire function:
;	 unsigned long addNumbers(ULONG count, ...);
;
;   Arguments consist of a count then that many unsigned integers.
;   Add all the arguments after the count, and return the result.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_4_size dq $-lab6_4

lab6_5:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   Call the C library function:
;	int printf(const char *format, ...)
;   and print out the string "Assembly rocks!\n"
;   
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab6_5_size dq $-lab6_5

lab7_1:
; GOAL:
;   Implement internals of a function:
;   USE hardcoded integer offsets from RBP to access arguments.
;
;   unsigned long addNumbers(unsigned long a, unsigned long b, unsigned long c, unsigned long d, unsigned long e, unsigned long f, unsigned long g);
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	pop rbp
	ret
lab7_1_size dq $-lab7_1

lab7_2:
; GOAL:
;   Access arguments using stack frame base pointer (RBP) and a struc
;   if needed
;
;   (optional) Define your struc within the YOUR CODE section, name it 'locals7_2'
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
;   PRESERVE: standard registers
;;;;;;;;;;;;; YOUR CODE BELOW

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
;   As helpful motivation to use locals, you may NOT use RBX and R12-R15 even
;   if you store and restore them! 
;
;   Use RBP to access both arguments and locals, in whichever manner you see fit 
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab7_3_size dq $-lab7_3

lab7_4:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;    print the return addresses on the callstack to your function using printf
;    Use gdb command "bt" to view the callstack and  verify your answer   
;    PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab7_4_size dq $-lab7_4

lab8_1:
; GOAL:
;   Implement entire function:
;      void memcpy(void *dst, void *src, int count);
;
;   You MUST use lodsb and stosb!
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_1_size dq $-lab8_1

lab8_2:
; GOAL:
;   Implement entire function:
;      void memcpy(void *dst, void *src, int count);
;
;   You MUST use rep movsb!
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

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
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

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
;   -----------------
;   |A|B|C|D|E|F|G|H|
;   _________________________
;           | | | | | | | | |
;           _________________
;           ^
;           dst
;
;   **Notice moving [src] to [dst] would corrupt 'E' in src. Thus, we must
;     start from the end of the buffers and work backwards.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_4_size dq $-lab8_4

lab8_5:
; GOAL:
;   Re-Implement _lab 7.2 using repe or repne and cmpsb
;   The %define arg access and frame stuff is optional 
;
;   You will need to test the termination condition of the cmpsb
;    to determine if it exited because of mismatch or ECX==0 (try JNE).
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_5_size dq $-lab8_5

lab8_6:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;  BONUS _lab  ;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; GOAL:
;   Implement the entire function:
;	 const char *my_memchr(void *buf, char ch, int count);
;
;   Return NULL if ch is not present in buf within count bytes.
;   Return pointer to ch otherwise.
;
;   You MUST use scasb either in a loop or with a repe OR repne prefix.
;   HINT: one form is easier...
;   You will need to test for whether it found it or exited because it
;    ran out of bytes.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab8_6_size dq $-lab8_6

lab9_1:
; GOAL:
;   Return with ZF true.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab9_1_size dq $-lab9_1

lab9_2:
; GOAL:
;   CF and OF are almost never set at the same time.
;   Find a way to return with CF and OF both set.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab9_2_size dq $-lab9_2

lab10_1:
; GOAL:
;
;	Implement entire function:
;		int my_divider(ULONG dividend, ULONG divisor);
;
;	Return Quotient.
;   PRESERVE: standard registers
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE

lab10_1_size dq $-lab10_1

lab10_2:
; GOAL:
;	Set RAX to 0.
;	test and complement bit 0 of RCX, if it was 1 increment RAX
;	test and complement bit 1 of RCX, if it was 0 increment RAX
;	test and reset bit 2 of RCX, if it was 1 increment RAX
;	test and reset bit 3 of RCX, if it was 0 increment RAX
;	test bit 4 of RCX, if it is 0 increment RAX
;	test bit 5 of RCX, if it is 1 increment RAX
;	if bit 6 of RCX is 1, set RDX to 2, else set RDX to 0 (do NOT use a jump!)
;   PRESERVE: standard registers
;             Make only the designated changes to RCX
;
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
	ret
lab10_2_size dq $-lab10_2

lab11_1:
; Write a function that takes a single argument (N)
; and returns the Nth fibonacci number. Assume the sequence starts at 0,
; and is 1-based (that is, the first number is 0, the second is 1, and there is no 0th number).
;  * This can be recursive or non-recursive. If you're ambitious, write both
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_1_size dq $-lab11_1

lab11_2:
; Write a function that takes a single argument (N)
; and returns the sum of the natural numbers from 0 to N
; that are multiples of both 3 and 5
;  * Bonus if you only use add/sub (no mul/divide)
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_2_size dq $-lab11_2

lab11_3:
; Write the function
; int largestProduct(unsigned int arrayLen, int *array)
; Return the largest product of five consecutive integers in the array.
;  * Worded another way: Find the largest product of five consecutive integers within an N-length array of integers.
; On error return -1
;;;;;;;;;;;;; YOUR CODE BELOW

;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_3_size dq $-lab11_3

lab11_4:
; Write the function
;	void xorEncDec(char *inOutStream, unsigned char key, unsigned long len)
; This is an in-place xor encoder/decoder. First use string instructions, then implement it without.
;;;;;;;;;;;;; YOUR CODE BELOW

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

;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_5_size dq $-lab11_5

lab11_6:
; Modify your solution to the Fibonacci problem to return the sum of even Fibonacci
; numbers up to the input number.
;;;;;;;;;;;;; YOUR CODE BELOW

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

;;;;;;;;;;;;; YOUR CODE ABOVE
lab11_7_size dq $-lab11_7
