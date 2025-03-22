.globl my_do_handler
.globl error_entry_addr
.globl error_exit_addr

.text

.globl my_handler
.type my_handler,%function
my_handler:
	nop
	nop
	nop
	pushq $-1
	movq (error_entry_addr), %rax
	call *%rax
	mov %rsp, %rdi
	xor %esi, %esi
	callq my_do_handler
	movq (error_exit_addr), %rsi
	jmp *%rsi
	retq
