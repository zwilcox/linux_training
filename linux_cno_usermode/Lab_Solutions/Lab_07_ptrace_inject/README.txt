Linux CNO usermode
Injection Lab 

Debugger? I 'ardly know 'er!

NOTE: Onload function in badso.so will run - however, there will be no setup of the tmp/ file path or debug 'script'. The goal is the get the print statement within the badso.so to run as part of the constructor when the .so is loaded via dlopen.

Objectives:
	Inject a .so file into a running process.
	Use the ptrace command to act as a debugger on a process.

Provided:
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
			- the parent never seeds it's random

CMD: [student@localhost ~]$ ./injector <pid of child> ./badso.so  
Note: LD_LIBRARY_PATH can be used when starting whoisthebest to allow you the ability to not need the ./ in the path given to the injector
Ex: LD_LIBRARY_PATH=$PWD ./whoisthebest
then ./injector <pid> badso.so will work

Otherwise whoisthebest will only look in standard .so locations and will require a ./badso.so

