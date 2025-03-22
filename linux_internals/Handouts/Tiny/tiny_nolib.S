# This is a tiny example piece of code that will compile without stdlib
# This can be built with:
#	gcc -nostdlib -o tiny_nolib ./tiny_nolib.S

.global _start
_start:
	mov $50, %rdi
	mov $60, %rax
	syscall
