#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_NAME 32UL
typedef enum {
	ECONOMICS = 100,
	CHEMISTRY,
	CALCULUS,
	HISTORY,
} CLASS;

typedef struct {
	unsigned long Age;
	unsigned char Gender;
	unsigned long Grade;
	CLASS Class;
	char Name[MAX_NAME];
} STUDENT;

#endif
