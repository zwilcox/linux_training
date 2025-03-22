#include <stdio.h>
#include <stddef.h>
#include "structures_extra.h"

int main(int argc, char** argv){
	printf("struct STUDENT member offsets:\n");
	printf("Age: %lu (0x%02lx)\n", offsetof(STUDENT,Age), offsetof(STUDENT,Age));
	printf("Gender: %lu (0x%02lx)\n", offsetof(STUDENT,Gender), offsetof(STUDENT,Gender));
	printf("BIOS: %lu (0x%02lx)\n", offsetof(STUDENT,Bios), offsetof(STUDENT,Bios));
	printf("Grade: %lu (0x%02lx)\n", offsetof(STUDENT,Grade), offsetof(STUDENT,Grade));
	printf("Class: %lu (0x%02lx)\n", offsetof(STUDENT,Class), offsetof(STUDENT,Class));
	printf("Name: %lu (0x%02lx)\n", offsetof(STUDENT,Name), offsetof(STUDENT,Name));
	printf("Total Size of struct STUDENT: %lu (0x%02lx)\n\n", sizeof(STUDENT), sizeof(STUDENT));

	printf("struct BIOS member offsets:\n");
	printf("Blood: %lu (0x%02lx)\n", offsetof(BIOS,Blood), offsetof(BIOS,Blood));
	printf("Donor: %lu (0x%02lx)\n", offsetof(BIOS,Donor), offsetof(BIOS,Donor));
	printf("Eyes: %lu (0x%02lx)\n", offsetof(BIOS,Eyes), offsetof(BIOS,Eyes));
	printf("Hair: %lu (0x%02lx)\n", offsetof(BIOS,Hair), offsetof(BIOS,Hair));
	printf("Hand: %lu (0x%02lx)\n", offsetof(BIOS,Hand), offsetof(BIOS,Hand));
	printf("Total Size of struct BIOS: %lu (0x%02lx)\n", sizeof(BIOS), sizeof(BIOS));
	return 0;	
}
