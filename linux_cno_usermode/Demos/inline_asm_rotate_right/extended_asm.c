#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t rotate_right(uint32_t num, uint8_t bits);


int main(int argc, char* argv[]) {
	uint32_t num = 0xff;

	printf("num is %08X\n", num);
	num = rotate_right(num, 2);
	printf("num is %08X\n", num);
	num = rotate_right(num, 1);
	printf("num is %08X\n", num);

	return 0;
}


uint32_t rotate_right(uint32_t num, uint8_t bits) {
	__asm__("mov %[n], %%cl\n\t"
		"rorl %%cl, %[regA]"
		: [regA] "+r" (num)
		: [n] "rm" (bits)
		: "cc", "%cl"
	);

	return num;
}

