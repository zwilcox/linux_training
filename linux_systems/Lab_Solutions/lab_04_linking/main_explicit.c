#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
	unsigned int result;
	void* handle;
	unsigned int (*explicit_factorial)(unsigned int num);
	printf("Before we load the file\n");
	handle = dlopen("./libactp.so", RTLD_LAZY);
	if (handle == NULL) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	explicit_factorial = dlsym(handle, "actp_factorial");

	printf("Explicit:\n");
	result = explicit_factorial(5);
	printf("Factorial of %u is %u\n", 5, result);
	
	dlclose(handle);
	
	return EXIT_SUCCESS;
}
