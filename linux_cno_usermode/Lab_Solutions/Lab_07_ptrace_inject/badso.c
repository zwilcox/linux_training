#include <stdio.h>
#include <unistd.h>

#define MAX_FD_TRY	0xff

#define DEBUGAUTHMSG	"DEBUGAUTH"

void onload() __attribute__((constructor));

void onload() {
	int i;

	fprintf(stderr, "Injected in child process!\n");

	for (i=3; i < MAX_FD_TRY; i++) {
		write(i, DEBUGAUTHMSG, sizeof(DEBUGAUTHMSG));
	}
}
