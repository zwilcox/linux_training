#ifndef VALIDATION_H
#define VALIDATION_H

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/user.h>

extern "C" void save_n_go(void (*func)(), struct user_regs_struct* regs);
extern "C" void modify_stack_lab5_2(void);
extern "C" int val_lab9_1(void);
extern "C" int val_lab9_2(struct user_regs_struct*);
void failLab(const char* fmt, ...);

extern char YOURNAME[30];
extern char SERVER_IP[16];
extern int PORT;
typedef int(* submit_score)(const char * ip, int port, const char * studentName, int lab, int lab_num, int score);
extern submit_score submit;

#define LAB(level, num) \
	extern "C" void lab##level##_##num(); \
	extern "C" unsigned long lab##level##_##num##_size; \
	extern "C" void validate_lab##level##_##num();

LAB(0, 1)

LAB(1, 1)
LAB(1, 2)
LAB(1, 3)

LAB(2, 1)
LAB(2, 2)
LAB(2, 3)
LAB(2, 4)

LAB(3, 1)
LAB(3, 2)
LAB(3, 3)
LAB(3, 4)
LAB(3, 5)

LAB(4, 1)
LAB(4, 2)
LAB(4, 3)
LAB(4, 4)
LAB(4, 5)


LAB(5, 1)
LAB(5, 2)
LAB(5, 3)
LAB(5, 4)

LAB(6, 1)
LAB(6, 2)
LAB(6, 3)
LAB(6, 4)
LAB(6, 5)

LAB(7, 1)
LAB(7, 2)
LAB(7, 3)
LAB(7, 4)

LAB(8, 1)
LAB(8, 2)
LAB(8, 3)
LAB(8, 4)
LAB(8, 5)
LAB(8, 6)

LAB(9, 1)
LAB(9, 2)

LAB(10, 1)
LAB(10, 2)

LAB(11, 1)
LAB(11, 2)
LAB(11, 3)
LAB(11, 4)
LAB(11, 5)
LAB(11, 6)
LAB(11, 7)

#endif
