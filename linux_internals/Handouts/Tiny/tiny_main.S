# This is a tiny program that will compile and use the c stdlib
# it can be build with:
#	gcc -o tiny tiny_main.S
# compare the output to that of tiny_nolib.S

.global main
main:
	mov $50, %rdi
	mov $60, %rax
	syscall
