	.file	"main.c"
	.text
	.section	.rodata
.LC0:
	.string	"Usage: %s filepath\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	cmpl	$2, -4(%rbp)
	je	.L2
	movq	-16(%rbp), %rax
	movq	(%rax), %rdx
	movq	stderr(%rip), %rax
	movl	$.LC0, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %eax
	jmp	.L3
.L2:
	movq	-16(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_wc_info
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.section	.rodata
.LC1:
	.string	"rb"
.LC2:
	.string	"fopen"
.LC3:
	.string	"getline"
.LC4:
	.string	"fclose"
.LC5:
	.string	"%d %d %d %s\n"
	.text
	.globl	print_wc_info
	.type	print_wc_info, @function
print_wc_info:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -56(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
	movq	-56(%rbp), %rax
	movl	$.LC1, %esi
	movq	%rax, %rdi
	call	fopen
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	jne	.L5
	movl	$.LC2, %edi
	call	perror
	movl	$1, %eax
	jmp	.L13
.L5:
	movq	$0, -32(%rbp)
.L11:
	movq	-24(%rbp), %rdx
	leaq	-40(%rbp), %rcx
	leaq	-32(%rbp), %rax
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	getline
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	cmpq	$-1, %rax
	jne	.L7
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	feof
	testl	%eax, %eax
	jne	.L15
	movl	$.LC3, %edi
	call	perror
	jmp	.L10
.L7:
	addl	$1, -4(%rbp)
	movq	-40(%rbp), %rax
	movl	%eax, %edx
	movl	-12(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	count_words
	addl	%eax, -8(%rbp)
.L10:
	jmp	.L11
.L15:
	nop
	movq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	free
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	fclose
	cmpl	$-1, %eax
	jne	.L12
	movl	$.LC4, %edi
	call	perror
.L12:
	movq	-56(%rbp), %rsi
	movl	-12(%rbp), %ecx
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	movq	%rsi, %r8
	movl	%eax, %esi
	movl	$.LC5, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
.L13:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	print_wc_info, .-print_wc_info
	.type	count_words, @function
count_words:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L17
.L20:
	addq	$1, -24(%rbp)
.L18:
	movq	-24(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L19
	call	__ctype_b_loc
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	jne	.L20
.L19:
	movl	$0, -8(%rbp)
	jmp	.L21
.L23:
	addq	$1, -24(%rbp)
	movl	$1, -8(%rbp)
.L21:
	movq	-24(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L22
	call	__ctype_b_loc
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	movzbl	(%rax), %eax
	movsbq	%al, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	movzwl	(%rax), %eax
	movzwl	%ax, %eax
	andl	$8192, %eax
	testl	%eax, %eax
	je	.L23
.L22:
	cmpl	$1, -8(%rbp)
	jne	.L17
	addl	$1, -4(%rbp)
.L17:
	movq	-24(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L18
	movl	-4(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	count_words, .-count_words
	.ident	"GCC: (GNU) 9.3.1 20200408 (Red Hat 9.3.1-2)"
	.section	.note.GNU-stack,"",@progbits
