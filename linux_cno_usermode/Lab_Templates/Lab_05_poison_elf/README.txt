Linux CNO usermode

Infected Elves/home/student/linux-cno-usermode/Lab_Templates/Lab_05_poison_elf/infector.c

Objectives:
	Provided a clean copy of ssh, insert code that will run before any other code.
	Cause ssh to always connect to bad.evil.com regardless of input parameters.
	a diff of objdump -d ./ssh_infected and ./ssh_unifected should show no difference in code.

Provided:
	infector.c
		Boilerplate code for infecting a ELF64 file with a payload.
		Students need to fill out this file.

	payload.S
		A assembly file that generates code to be injected into ssh.
		Students need to fill out this file.

	ssh_uninfected
		Just a copy of /usr/bin/ssh in a convenient location.

	Makefile
		A provided Makefile that works with infector.c, payload.S, and ssh_uninfected

Tips:
	You can reference elf.h at "/usr/include/elf.h" for the elf header format.
	You care about the program headers, not the section headers.
	int getpagesize(void) gives you the size of a memory page.
	The program headers will list their size as number of used bytes, but...
		The program headers acutally round up to page size.
		Any extra area will have the same permissions and be copied with the rest.
	ssh -o StrictHostKeyChecking=no -o LogLevel=ERROR user@host.com
		Connects without checking the key, and won't output "Added key to host.com"
		Also add -o UserKnownHostsFile=/dev/null to not add to the known hosts

