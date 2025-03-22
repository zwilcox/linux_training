Linux CNO usermode
Assembly Lab 1

Linux Assembly Primer

Objectives:
	Understand the calling convention used on linux x86_64.
	Use gcc to compile a .S file.
	Make raw system calls from within assembly.

Provided:
	base.c
		A base program written in C that will call the student's code.
	student.h
		A header giving declarations for functions the student must define in a .S file.
	student.S
		The only file the student should edit.
	Makefile
		A make file that the student must modify to include a new .S file.
	binutils docs
		Docs for the binutils programs, including gas, the gnu assembler.
	Linux x86_64 system call table
		Information on how to set up the systemcalls.

Tips:
	Linux x86_64 systemcall calling convention:
		System call number is passed in rax.
		Parameters are passed in rdi, rsi, rdx, r10, r8, and r9.
		syscall instuction clobbers rcx and r11.
		rax is the return value.

	Linux x86_64 calling convention:
		parameters: rdi, rsi, rdx, rcx, r8, r9, stack.
		floating point parameters: xmm0 - xmm7.
		return: rax, rax:rdx, or xmm0:xmm1 .
		callee saved registers: rbx, rbp, r12 - r15.
		variadic function:
			# of xmm registers used in al register.
			floats must first be promoted to double (cvtss2sd)
		No shadow space, but you should still make sure the stack is aligned to 0x10.
	
	A few important assembler directives for gas:
		.align
			Pads to a alignment.
		.asciz
			declars a string followed by a zero byte. Use .ascii to not have the null.
		.byte
			.byte expects zero or more expressions, separated by commas.
			Each expression is assembled into the next byte.
		.data
			Puts the following into the data section.
		.extern
			For listing symbols you expect to be able to use.
			Ignored, but you can use it if it makes you feel good.
			If ld can't find something like printf, try printf@PLT.
			Use objdump to see what symbols are available in the program.
		.func
			Paired with .endfunc. This is useful for debugging info.
		.global
			Makes the symbol visible to ld.
		.include
			A way to include supporting files.
			The code from the file is assembled as if it followed the point of the .include.
			Different from #include.
		.macro
			Paired with .endm, allows for macros with names and arguments.
		.section
			Puts what follows into an arbitrary section, with flags for permissions.
		.set
			Set a symbol to expression. Similar to a #define.
		.text
			Tells as to assemble what follows into the text section.

	When using gcc, a .S file is an assembler file that needs to be pre-processed.
		Use .s if you don't need to be preprocessed.

	GAS uses # for single line comments, as opposed to ;
		You can also use /* ...comment... */ to avoid confusion.

	Linux sets fd 0 to stdin, fd 1 to stdout, and fd 2 to stderr.

	
