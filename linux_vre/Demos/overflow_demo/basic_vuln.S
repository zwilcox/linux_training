
.global _start

_start:
	# push the stack address
	# to make for an easy leak
	push %rsp

	# push a return address
	# to be overflown into
	lea (_exit), %rax
	push %rax

	# write out "What is your favorite color?\n"
	mov $0x0a3f726f6c, %rax
	push %rax
	mov $0x6f6320657469726f, %rax
	push %rax
	mov $0x7661662072756f79, %rax
	push %rax
	mov $0x2073692074616857, %rax
	push %rax

	# write
	xor %rdi, %rdi
	inc %rdi
	mov %rsp, %rsi
	mov $0x1d, %rdx
	xor %rax, %rax
	inc %rax
	syscall

	# read in too much
	xor %rdi, %rdi
	mov $0x48, %rdx
	xor %rax, %rax
	syscall

	add $0x20, %rsp
	ret

_exit:
	pop %rsp
	xor %rdi, %rdi
	xor %rax, %rax
	mov $0x3c, %al
	syscall
