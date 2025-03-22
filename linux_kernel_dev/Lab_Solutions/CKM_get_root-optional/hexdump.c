#include <stdio.h>

#include "ctype.h"
#include "hexdump.h"

#define MAX_ADDR_FMT_LEN	16
#define DUMP_COLS			16

void hexdump(const unsigned char* buf, unsigned int len) {
	char addr_buf[MAX_ADDR_FMT_LEN + 1];
	unsigned int i;
	unsigned int j;
	unsigned int power;
	unsigned int limit;
	unsigned int pad;
	char c;
	power = 1;
	for (i = 0; i < 64; i++) {
		if (len >= power) {
			power = power << 2;
			continue;
		}
		break;
	}
	snprintf(addr_buf, MAX_ADDR_FMT_LEN, "%%0%ullx:", i);
	for (i = 0; i < len; i++) {
		if (i % DUMP_COLS == 0) {
			printf(addr_buf, i);
		}
		if (i % 2 == 0) printf(" ");
		printf("%02x", buf[i]);
		if (i % DUMP_COLS == DUMP_COLS-1 || i == (len - 1)) {
			limit = (i % DUMP_COLS);
			pad = DUMP_COLS - limit;
			if (i == (len - 1)) {
				for (j = 0; j < pad-1; j++) {
					if (j % 2 != 0) {
						printf(" ");
					}
					printf("  ");
				}
			}
			printf("  ");
			for (j = 0; j <= limit; j++) {
				c = buf[i-limit+j];
				if (isalnum(c)) {
					printf("%c", c);
				}
				else {
					printf(".");
				}
			}
			printf("\n");
		}
	}
	printf("\n");
}
