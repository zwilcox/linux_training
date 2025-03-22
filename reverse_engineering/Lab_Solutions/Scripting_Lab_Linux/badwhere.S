.intel_syntax noprefix

.global domodify

domodify:
	mov rax, [rdi]
	bswap rax
	mov [rdi], rax
	add rdi, 0x8
	mov rax, rdi
	add rax, 0x8
	cmp rax, rsi
	jle domodify
	ret
	 
