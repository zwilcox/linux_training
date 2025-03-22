#include <stdio.h>
#include <stdlib.h>
#include "actp_factorial.h"

int main() {
	unsigned int i;
	unsigned int result;
	for (i = 0; i < 10; i++) {
		result = actp_factorial(i);
		printf("Factorial of %u is %u\n", i, result);
	}

	return 0;
}

