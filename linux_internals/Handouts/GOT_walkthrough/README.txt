This demo can be used to walk through understanding the dynamic loading of a ELF binary, and how the PLT and GOT work.

First make, and run the "file" utility on each of the output files, showing they are both "pie executable" and use "interpreter /lib64/ld-linux-x86-64.so.2".
The "readelf -h" output should also be nearly identitcal, with example_now having a few more headers. We will talk about that.

Now, in the slides, under Demo Part 1 it says to use /bin/echo, but we can use ./example_now instead if we want.
Following are some steps you can follow to step through the demo.

	gdb ./example_now
		starti AAAA BBBB CCCC
		!cat /proc/`pidof example_now`/maps
		# notice that the very first instruction to execute is not in our main executable's code, but in ld.so
		# explain that because it is a ELF file with an interpreter, it needs to be fixedup so it can run at a relocated spot in memory
		# this is why ld.so is an "interpreter", it has to translate things before they can run correctly
		
		# let's move on to the first bit of the program's code, _start
		# we haven't stripped symbols, so we can just put a breakpoint there
		# if the symbol didn't exists, we could use readelf -h to get the entry point address or offset to find our _start
		
		b _start
		cont

		# now let's look at the arguments to _start, where does it get argc, argv, environ, and everything main needs?
		x/64x $rsp
		# We see lots of pointers, but we need to start dereferencing things to really know what is going on
		
		x/1x $rsp
		# first thing is our argc, which we can verify by changing how many arguments we pass when we do starti
		
		x/s ((char**)$rsp)[1]
		# next we can see the following pointers, up until the NULL, are the pointers to the strings of our args

		x/s ((char**)$rsp)[6]
		# if we continue on after the first NULL, we will start printing out our environment variables
		# if we continue, we will find that the environment variables also end with a NULL
		# after that NULL are the auxvp pointers, which provided information to the loader about the current program
		# such as the entry point, if it is a setuid program, etc.
		# see Elf64_auxv_t defined in /usr/include/elf.h

		# we can step through until we find the first call from _start, this is a libc function called __libc_start_main
		# this function takes its first argument as a pointer to our main, point this out
		# after that, let's go to main

		b main
		cont

		# step over the first few instructions to get to the first call to puts
		# before we step in, let's discuss the scenario
		# the scenario is we want to be able to call a libc function, but because that library can get placed different places, we don't know where "puts" is
		# the solution we use is to have small snippets of code that we call to, that perform a jump to the real library location of the function
		# ld.so can then just fill out the table of function pointers that these snippets use, and the snippets will go to the correct function locations
		# these snippets are our PLT stubs (Procedure Linkage Table) and the table of pointers is our GOT (Global Offset Table)
		# let's step through it

		si
		...
		
		# we should see something like "0x0000555555555030 <puts@plt+0> jmp  QWORD PTR [rip+0x2f92]   # 0x555555557fc8 <puts@got.plt>"
		# this jump is jumping to what is stored at the value contained 0x2f92 bytes away from the current rip
		# we can see what this value is, you may have to to change to pointer to match the output you see

		x/1gx 0x555555557fc8
		# gdb will output and be able to tell us that, yes, this address is the address in the got for puts, and if we follow the value contained
		# we will see it is puts in libc
		
		# we can dump the instructions by using the address we saw there in the got, or by dereferencing it
		x/12i 0x00007ffff7e5????
		x/12i *(void**)0x555555557fc8

		# if we step again we will see that we end up safely in "puts" in libc
		
		si

		# at this point catch up in the slides, viewing again the steps to use the got and plt with RTLD_NOW

		# explain that the got entries don't have to always be fixed up at the beginning of the program
		# we build example_now with -znow so that it would have all it's got entries be correct before the primary ELF's _start even ran
		# but by default gcc will actually be lazy
		# now we can start up example_lazy to see what happens then

		quit
		y

	gdb ./example_lazy
		b main
		run
		
		# now let's step through into the first call to puts again
		si
		...
		
		# the PLT code is exactly the same, nothign changed, it is still trying to get the pointer it wants to go to from the GOT
		# however, this time the GOT doesn't have a pointer to libc's "puts"
		x/1gx 0x555555557350
		
		# If we check, we see that pointer actually points to the next instruction after the first line we were at in the PLT
		# This will send us down the path that will lead to us asking ld.so to tell us where the real "puts" is
		# then ld.so will fixup the entry in the GOT and things will work correctly
		# let's step a bit until we see that we end up in ld.so's code
		
		si
		...
		
		# we should end up in a function called "_dl_runtime_resolve_xsavec"
		# it does what it says, resolve at runtime
		
		# we can skip over this function, and see that "puts" did get called successfully, and printed out "One"
		
		finish
		
		# now let's step into puts a second time, and this time see that the GOT entry did indeed get changed

		si
		si
		x/1gx 0x555555557350
		
		si
		
		# and we are now in puts! Every call from our ELF to puts that passes through the plt will have it fixed up for it, only once does it have to resolve
		# this can be faster than resolving them all in the beginning, because maybe not all the functions will be called for any given execution

		# go through the rest of the slides and see this same thing happen in the diagrams

		# if we wanted we could look at example_now again, using starti, to see the unadjusted starti before our _start get's called, and after ld.so has finished setting up
		# we would see all the GOT entries fixed up, and in fact, because we used -zrelro that portion of the GOT will even be set to readonly after!

		quit
		y
	exit
