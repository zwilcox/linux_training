#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

unsigned char kbdus[128] = {
	0,	27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
   '9', '0', '-', '=', '\b',	/* Backspace */
   '\t',			/* Tab */
   'q', 'w', 'e', 'r',	/* 19 */
   't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
	0,			/* 29	- Control */
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
   '\'', '`',   0,		/* Left shift */
   '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
   'm', ',', '.', '/',	0,				/* Right shift */
   '*',
	0,	/* Alt */
   ' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,	 0,   0,   0,	0,	 0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
   '-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
   '+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,	 0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};	

unsigned char kbdus_shift[128] = {
	0,	27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
   '(', ')', '_', '+', '\b',	/* Backspace */
   '\t',			/* Tab */
   'Q', 'W', 'E', 'R',	/* 19 */
   'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
	0,			/* 29	- Control */
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
   '\"', '~',   0,		/* Left shift */
   '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
   'M', '<', '>', '?',	0,				/* Right shift */
   '*',
	0,	/* Alt */
   ' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,	 0,   0,   0,	0,	 0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
   '-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
   '+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,	 0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};

int shift_pressed;

void printkey(unsigned short scancode) {
	//printf("%04x\n", scancode);
	if (scancode == 0x1c2a || scancode == 0x1c36) {
		shift_pressed = 1;
		return;
	}
	if (scancode == 0x1caa || scancode == 0x1cb6) {
		shift_pressed = 0;
		return;
	}

	if (scancode & 0x80) {
	}
	else {
		if (shift_pressed == 1) {	
			printf("%c", kbdus_shift[scancode & 0xFF]);
			fflush(stdout);
		}
		else {
			printf("%c", kbdus[scancode & 0xFF]);
			fflush(stdout);
		}
	}
	return;
}

int main() {
	int fd;
	unsigned short scancode;
	ssize_t bytes_read;
	fd = open("ghostboard", O_RDONLY);
	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}
	//printf("0x1e is %c\n", kbdus[0x1e]);
	while (1) {
		bytes_read = read(fd, &scancode, 2);
		if (bytes_read != 2) {
			perror("read");
			close(fd);
			return EXIT_FAILURE;
		}
		printkey(scancode);
	}
	return EXIT_SUCCESS;
}

