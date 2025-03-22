#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

unsigned char kbdus[128] = {
/* Making a table that translates scancodes to characters might be helpful. */
};	

unsigned char kbdus_shift[128] = {
/* Having another table that translates scancodes when shift is pressed might be helpful */
};

int shift_pressed;

void printkey(unsigned short scancode) {
	//printf("%04x\n", scancode);
	if (/* How can I tell if a shift key has been pressed */) {
		shift_pressed = 1;
		return;
	}
	if (/* How can I tell if a shift key has been released?*/) {
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

