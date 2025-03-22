Linux CNO usermode
Injection Lab 1

LD_PRELOAD

Objectives:
	Use LD_PRELOAD to modify the program "patience" to output a password quickly.

Provided:
	./patience
		A striped binary that will output a password after a long wait period.

		Instructor: create the patience binary with provided Makefile

Tips:
	Use "strace" in order to quickly understand how the target program runs.
	use "objdump -T" to identify loaded dynamic symbols that can be replaced.

Note:
	Simplesum is an additional program for demoing purposes (if so desired)
