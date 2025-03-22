
#; Custom section name for our linker script
.section .ldr_preface, "x"

	#; assume state saved before this
	jmp	PING
PONG:
	#; get rip
	pop	%rdi

	#; get buflen and buf
	mov	%rdi, %rsi
	addq	$4, %rsi
	mov	(%rdi), %edi
	mov	%edi, %eax
	add	%rsi, %rax
	
	#; align the stack
	push	%rbp
	push	%rax
	mov	%rsp, %rbp
	and	$-0x10, %rsp
	
	#; Call our C code
	call	ldBuf

	#; restore the stack
	mov	%rbp, %rsp
	pop	%rdi
	pop	%rbp
	
	#; jump to the end of the buffer
	jmp	%rdi

#; helper functions for out c code without libc
.global OPEN
OPEN:
	mov	$2, %rax
	syscall
	ret

.global CLOSE
CLOSE:
	mov	$3, %rax
	syscall
	ret

.global READ
READ:
	mov	$0, %rax
	syscall
	ret

.global WRITE
WRITE:
	mov	$1, %rax
	syscall
	ret

#; C file get linked here
#; void ldBuf(uint32_t buflen, void* buf);
.extern ldBuf


#; Custom section for out linker script
.section .ldr_end, "x"

PING:
	call PONG
	#; Data beyond file:
	#; DWORD of buffer len
	#; Buffer
