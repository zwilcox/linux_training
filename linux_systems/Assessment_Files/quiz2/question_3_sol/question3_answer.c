#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
	unsigned char *result;
	void* handle;
	unsigned char *(*explicit_answer)(void);
	handle = dlopen("./question3.so", RTLD_LAZY);
	if (handle == NULL) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	explicit_answer = dlsym(handle, "print_the_answer_to_question_3");

	result = explicit_answer();
	
	dlclose(handle);
	
	return EXIT_SUCCESS;
}