#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/auxv.h>

int double_check(char* envp[]) {
	uint64_t* p;

	for (p = (uint64_t*)envp; *p != 0; p++) {
	}
	for (p++; *p != 0; p += 2) {
		if (*p == AT_EUID || *p == AT_UID) {
			printf("I see the id as %lu\n", *(p+1));
			if (*(p+1) != 0) {
				return -1;
			}
		}
	}

	return 0;
}

int main(int argc, char* argv[], char* envp[]) {
	(void)argc;
	(void)argv;

	if (getuid() != 0) {
		printf("Please run this program as root\n");
		return -1;
	}

	if (double_check(envp)) {
		printf("No, really, run it as root\n");
		return -1;
	}

	printf("Nice\n");

	return 0;
}
