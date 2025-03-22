#include <stdio.h>
#include "actp_factorial.h"

void on_load(void) __attribute__((constructor));
void on_unload(void) __attribute__((destructor));

void on_load(void) {
	printf("Loading!\n");
}

unsigned int actp_factorial(unsigned int num) {
	unsigned int counter;
	unsigned int result = 1;
	for (counter = 1; counter <= num; counter++) {
		result *= counter;
	}
	return result;
}

void on_unload(void) {
	printf("Unloading!\n");
}
