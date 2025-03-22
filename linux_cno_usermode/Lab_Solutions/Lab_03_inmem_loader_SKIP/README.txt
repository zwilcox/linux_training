Linux CNO usermode
Mobilized Code
Loader Lab

I like big buffers

Objectives:
	Use the provided loader to load a .so file after getting execution in a program.
	Exploit nasmline, using the loader to load a .so into a remote version of it.

Bonus Objectives:
	Understand ldr.S and ldr_lib.c, and how they work.
	Edit the linker script provided to create the complete payload, without
	needing to concatenate the .so on as a separate step.
	Have your playload create a reverse shell that stays after nasmline ends.

Provided:
	nasmline.c
		A useful tool that was never meant to be exposed to the internet.
		Allows interactive assembling/disassembling of x86 code.
		Also allows interactive running of x86_64 code. ;)
		A build of this will be running on a remote VM. You should exploit it.

	shelltest.c
		This is a helper tool that allows you to test shellcode.

	ldr.S
		This is the entry point of the Loader. It grabs the arguments
		and passes them off to the code in ldr_lib.c. It expects 4 bytes for the
		.so length right after the loader, then the .so after that.

	ldr_lib.c
		This is the work horse of the loader, which will load the .so into the
		process that the loader is running in.

	host_nasmline.sh
		This is a script to host nasmline as a service.

	linkerscript.ld
		This is the linker script used to order the loader in the correct way.

	str_funcs.h
		This is a helper library of inline string functions for the loader.
	
	Makefile
		This is a makefile that will make all of the previous items.
