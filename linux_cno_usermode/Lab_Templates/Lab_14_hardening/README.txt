Linux CNO usermode
Hardening Lab

Metapod

Objectives:
	Modify an existing c program so that it can accomplish all of it's goals without being detected by a simple "antivirus"
	In order to run your program with the antivirus active, use "./antivirus -t ./infoFinder"

Bonus Objectives:
	Be able to run the program without the -t option on the antivirus (with tracing active)

Provided:
	antivirus
		An executable that will attempt to detect many of the actions that infoFinder will try to accomplish

	infoFinder.c
		A simple program that tries to get some system information.

	Makefile
		A starter makefile for infoFinder.c

Tips:
	The antivirus has some other useful commandline parameters. There is a parameter not listed in its -h output as well.
	The antivirus has a runtime component and a pre-run component.
