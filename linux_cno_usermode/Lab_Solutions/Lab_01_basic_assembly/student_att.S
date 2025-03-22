.global xorprint
.global printTime

.section .data
adstring:
	.asciz "AD"
bcstring:
	.asciz "BC"
formatstring:
	.asciz "%02u/%02u/%04u %s %02u:%02u:%3.2f\n"

.section .text

/* void xorprint(int len, char key, char* ciphertext) */
.func xorprint
xorprint: 
	mov %rdx, %rcx /* put &ciphertext in rcx */
	xor %r8, %r8   /* zero r8 */
	mov %edi, %r8d /* put len in r8 */
xorprint_loop:
	dec %edi        /* loop counter dec */
	jl xorprint_done
	xor %sil,(%rdx) /* xor key with ciphertext[i] */
	inc %rdx        /* increment ciphertext pointer */
	jmp xorprint_loop
xorprint_done:
	/* call sys_write(stdout_fd, buf, count) */
	xor %rax, %rax /* clear rax */
	inc %rax       /* rax = 1 */
	mov %rax, %rdi /* 1st arg for write() */
	mov %rcx, %rsi /* 2nd arg for write() */
	mov %r8, %rdx  /* 3rd arg for write() */
	syscall
	ret
.endfunc


/* void printTime(int year, int month, int day, int hour, int minute, float second) */
.func printTime
printTime:
	push %r8	    /* save minute - note: this push will also align the stack back to a 0xX0 boundary (see student.S notes) */ 
	mov %rcx, %r9       /* put hour in r9 - which gets misaligned when using the xmm (128-byte) register */
	mov %rdi, %rcx      /* put year in rcx - students who elect NOT to add in the AD/BC strings will likely run into a segfault */
	lea (adstring), %r8 /* put AD string in r8 - if they don't do a push (which wouldn't be needed in that case) or a subq $8, %rsp */
	test %ecx, %ecx	    /* before the call to printf */
	jns printTime_1
	lea (bcstring), %r8
	neg %ecx
printTime_1:	
	/* rdi fmt, rsi month, rdx day, rcx year, r8 adstr, r9 hour, stack1 minute, xmm0 second */
	lea formatstring, %rdi /* rsi is already month, rdx is already day */
	cvtss2sd %xmm0, %xmm0  /* convert float to double */
	movq $1, %rax          /* rax hold number of xmm reg used */
	call printf
	pop %rcx               /* clear pushed arg - this pop is needed AFTER the return from printf to reset the stack */
	ret
.endfunc
