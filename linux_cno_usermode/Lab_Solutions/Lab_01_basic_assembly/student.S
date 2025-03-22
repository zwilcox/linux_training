.intel_syntax noprefix

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
	mov rcx, rdx
	xor r8, r8
	mov r8d, edi 
xorprint_loop:
	dec edi
	jl xorprint_done
	xor [rdx], sil 
	inc rdx
	jmp xorprint_loop
xorprint_done:
	/* call sys_write(stdout_fd, buf, count) */
	xor rax, rax
	inc rax
	mov rdi, rax
	mov rsi, rcx
	mov rdx, r8
	syscall
	ret
.endfunc

/* void printTime(int year, int month, int day, int hour, int minute, float second) */
.func printTime
printTime:
	push r8		/* push on minute */		;note: this push will also align the stack back to a 0xX0 boundary
	mov r9, rcx					;which gets misaligned by main() and is key when using the xmm (128-byte) register
	mov rcx, rdi					;students who elect NOT to add in the AD/BC strings will likely run into a segfault
	lea r8, [adstring]				;if they don't do a push (which wouldn't be needed in that case) or a sub rsp, 8
	test ecx, ecx					;before the call to printf
	jns printTime_1
	lea r8, [bcstring]
	neg ecx
printTime_1:	
	/* rsi is already month, rdx is already day */
	lea rdi, [formatstring]
	/* rdi fmt, rsi month, rdx day, rcx year, r8 adstr, r9 hour, stack1 minute, xmm0 second */
	/* convert float to double */
	cvtss2sd xmm0, xmm0
	/* rax hold number of xmm reg used */
	mov rax, 1
	call printf
	/* clear pushed arg */
	pop rcx						;this pop is needed AFTER the return from printf to reset the stack
	ret
.endfunc
