Linux CNO usermode
Injection Lab 1

ProcSystemParty

Objectives:
	Without modifying auth.c, cause the function dowin to run.
	Using the proc file system, defeat ASLR and overwrite a saved return address.

Provided:
	auth.c
		A simple program that asks for a password, and contains the function dowin.

	passauth.py
		A simple python file to be filled out by the student.

	Makefile
		A provided makefile that builds auth.c into the executable auth.

Tips:
	"ps aux | grep auth" can help find the pid of the auth program.
	see "man proc.5" to understand /proc/[pid]/maps and /proc/[pid]/mem
	/proc/sys/kernel/yama/ptrace_scope determines who can read/debug what.
