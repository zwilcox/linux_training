#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int helloworld(void);

int main() {
	long ret;

	printf("Attempting syscall\n");
	//ret = syscall(335);
	ret = helloworld();
	if (ret != 0) {
		printf("System call failed\n");
	}
	return EXIT_SUCCESS;
}

int helloworld(void) {
	unsigned long int resultvar;
	asm volatile (
		"movq $335, %%rax;" \
		"syscall\n\t" \
		: "=a" (resultvar) \
		: 
		: "memory", "cc", "r11", "cx"
	);
	if (resultvar != 0) {
		errno = resultvar;
		return -1;
	}
	return 0;
}
