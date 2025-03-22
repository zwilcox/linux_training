Using AFL

You have been given a old version of the binutils source code. We will use AFL (American Fuzzy Lop) to instrument and fuzz readelf.
The version of binutils we are working with is 2.15 (because it is old and has a lot of bugs)

1. extract the archive:
	tar -xvf ./binutils-2.15.tar.bz2

2. enter the directory and configure the project to build with afl-gcc:
	cd ./binutils-2.15
	CC=afl-gcc ./configure

3. build the repo:
	make

4. set up your fuzzing folders in a separate location:
	mkdir ../in_elf ../out_elf

5. copy a few different elf files of your choice or build your own binary (the lab05 use-after-free binary is a good one with lots of crashes)

6. as root, change the core dump pattern to something afl can use:
	echo core > /proc/sys/kernel/core_pattern

^^ Note: might need to set a root user first:
	[student@localhost binutils-2.15]$ sudo passwd root
	[sudo] password for student: 
	Changing password for user root.
	New password: 
	Retype new password: 
	passwd: all authentication tokens updated successfully.
	[student@localhost binutils-2.15]$ su root
	Password: 
	[root@localhost binutils-2.15]# 

7. as root, change your CPU frequency scaling to be performant for afl:
	cd /sys/devices/system/cpu
    	echo performance | tee cpu*/cpufreq/scaling_governor

^^ Note: seems like the NoMachine VMs for the virtual students don't have this --- should be able to just ignore this step

Get out of ROOT here and drop back into student - once student, cd up back into the lab_02_aft directory to run step 8

8. run the fuzzer for some time:
	afl-fuzz -i ./in_elf -o ./out_elf ./binutils-2.15/binutils/readelf -a @@

^^ Note: might get this if the binary in your /in_elf is too big <--- choose a smaller binary - might also look in /usr/bin/
[!] WARNING: Some test cases are huge (662 kB) - see /usr/share/doc/american-fuzzy-lop/perf_tips.txt! 

Examine some of the afl_out/crashes files and try to determine what a few of the bugs found are, and if they are exploitable

A README with crash analysis steps/output is located in Lab_Solutions/lab02/
