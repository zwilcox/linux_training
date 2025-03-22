#include <stdio.h>
#include <stddef.h>
#include "structures.h"

int main(int argc, char** argv){
	printf("struct STUDENT member offsets:\n");
	printf("Age: %lu (0x%02lx)\n", offsetof(STUDENT,Age), offsetof(STUDENT,Age));
	printf("Gender: %lu (0x%02lx)\n", offsetof(STUDENT,Gender), offsetof(STUDENT,Gender));
	printf("Grade: %lu (0x%02lx)\n", offsetof(STUDENT,Grade), offsetof(STUDENT,Grade));
	printf("Class: %lu (0x%02lx)\n", offsetof(STUDENT,Class), offsetof(STUDENT,Class));
	printf("Name: %lu (0x%02lx)\n", offsetof(STUDENT,Name), offsetof(STUDENT,Name));
	printf("Total Size of struct: %lu (0x%02lx)\n", sizeof(STUDENT), sizeof(STUDENT));
	return 0;	
}
