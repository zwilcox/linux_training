#include <stdio.h>
#include <unistd.h>

void onload() __attribute__((constructor));

void onload() {
	printf("Shared Object Ran!\n");
}
