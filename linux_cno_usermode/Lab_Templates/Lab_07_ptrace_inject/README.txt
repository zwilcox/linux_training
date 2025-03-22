Linux CNO usermode
Injection Lab 

Debugger? I 'ardly know 'er!

Objectives:
	Inject a .so file into a running process.
	Use the ptrace command to act as a debugger on a process.
	Cause whosthebest to execute your script as root with injection

Provided:
	whosthebest.c
		source to make whosthebest binary to target with injector
	injector.c
		A starter for creating a .so injector, with helper functions.
		The student should fill this out.
	badso.c
		A starter for a shared library.
	Makefile
		A makefile that works on injector and badso

Tips:
	A "void onload() __attribute__((constructor));" function will run when your .so is loaded.
	Carefully read the "man ptrace.2" document.
	You can get libc's base from the /proc/[pid]/maps file.
	Libc's Elf64_Ehdr.e_entry is a safe spot to write executable bytes to.
		It is probably unused by other threads.
	You can force a library call or a system call by only clobbering 3 bytes.
			call rax
			int3
		or
			syscall
			int3
	
	dlopen's symbol name is "__libc_dlopen_mode"
	
	whosthebest
		Note: the parent process can't be attached to, but the child can
			- the child closes stdout, but not stderr
			- the parent never seeds its random
