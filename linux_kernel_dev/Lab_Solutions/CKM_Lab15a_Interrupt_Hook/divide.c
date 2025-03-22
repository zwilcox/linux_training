#include <stdio.h>
#include <stdlib.h>

int main() {
	int a = 1;
	int b = 0;
	printf("Attempting to perform %d/%d=%d\n", a, b, a/b);
	printf("Successfully divided by zero\n");
	return EXIT_SUCCESS;
}
