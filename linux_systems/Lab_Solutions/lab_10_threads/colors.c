#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "colors.h"

#define BLD  "\x1B[1m"
#define RST  "\x1B[0m"

int printf_color(char* color, char* fmt, ...) {
	char* color_fmt;
	char* color_code;
	va_list args;
	int ret;

	color_code = color;

	color_fmt = malloc(strlen(fmt) + strlen(BLD) + strlen(color_code)
			+ strlen(RST) + strlen(RST) + 1);
	if (color_fmt == NULL) {
		return 0;
	}

	sprintf(color_fmt, "%s%s%s%s%s", BLD, color_code, fmt, RST, RST);

	va_start(args, fmt);
	ret = vprintf(color_fmt, args);
	va_end(args);

	free(color_fmt);

	return ret;
}
