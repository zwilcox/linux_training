#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void printcpuinfo(int option);

#define CACHELINE(x)		(((x & 0x0000ff00) >> 8) * 8)
#define BRAND(x)		(x & 0x000000ff)
#define MODEL(x)		((x & 0x000000f0) >> 4)
#define EXTENDED_MODEL(x)	((x & 0x000f0000) >> 16)
#define FAMILY_ID(x)		((x & 0x00000f00) >> 8)

int main(int argc, char* argv[]) {
	long option;
	if (argc < 2) {
		fprintf(stderr, "Usage: %s input\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	option = strtol(argv[1], NULL, 16);

	printcpuinfo(option);

	return 0;
}

void printcpuinfo(int option) {
	//TODO: define printcpuinfo()
	return;
}

