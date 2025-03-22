#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#define AUTH_USER	0x0123456789
#define AUTH_OWNR	0x5245505553

uint64_t usertype = AUTH_USER;

void dowin() {
	printf("Congrats, you win!\n");
	exit(0);
}

void authcheck(uint64_t* out_usertype) {
	printf("Please login: ");
	fgets((uint8_t*)&usertype, sizeof(uint32_t), stdin);
}

int main(int argc, char* argv[]) {
	usertype = AUTH_USER;

	while (1) {
		authcheck(&usertype);
		if (usertype == AUTH_OWNR) {
			dowin();
			break;
		}
	}
	return -1;
}
