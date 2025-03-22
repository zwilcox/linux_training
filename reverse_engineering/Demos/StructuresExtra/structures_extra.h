#ifndef STRUCTURES_EXTRA_H
#define STRUCTURES_EXTRA_H
#include <stdbool.h>

#define MAX_NAME 50UL

typedef enum {
	BLACK,
	BLONDE,
	BROWN,
	GRAY,
	RED,
} HAIR;

typedef enum {
	E_BLUE,
	E_BROWN,
	E_GREEN,
	E_GRAY,
	E_HAZEL,
} EYES;

typedef enum {
	ECONOMICS = 100,
	CHEMISTRY,
	CALCULUS,
	HISTORY,
} CLASS;

typedef struct {
	unsigned char Blood;
	bool Donor;
	EYES Eyes;
	HAIR Hair;
	char Hand;
} BIOS;

typedef struct {
	unsigned long Age;
	unsigned char Gender;
	BIOS Bios;
	unsigned long Grade;
	CLASS Class;
	char Name[MAX_NAME];
} STUDENT;

#endif
