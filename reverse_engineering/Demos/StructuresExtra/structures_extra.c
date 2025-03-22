#include <stdio.h>
#include <string.h>
#include "structures_extra.h"

char *get_class_str(CLASS Class)
{
	switch (Class) {
	case ECONOMICS:
		return "Economics";
	case CHEMISTRY:
		return "Chemistry";
	case CALCULUS:
		return "Calculus";
	case HISTORY:
		return "History";
	default:
		return "?";
	}
}

char *get_eyes_str(EYES Eyes)
{
	switch (Eyes) {
	case E_BLUE:
		return "Blue";
	case E_BROWN:
		return "Brown";
	case E_GREEN:
		return "Green";
	case E_GRAY:
		return "Gray";
	case E_HAZEL:
		return "Hazel";
	default:
		return "?";
	}
}

char *get_hair_str(HAIR Hair)
{
	switch (Hair) {
	case BLACK:
		return "Black";
	case BLONDE:
		return "Blonde";
	case BROWN:
		return "Brown";
	case GRAY:
		return "Gray";
	case RED:
		return "Red";
	default:
		return "?";
	}
}

void PrintBios(BIOS *Bios)
{
	printf("Blood Type: %c\n", Bios->Blood);
	printf("Donor?: %d\n", Bios->Donor);
	printf("Eye Color: %s\n", get_eyes_str(Bios->Eyes));
	printf("Hair Color: %s\n", get_hair_str(Bios->Hair));
	printf("Writing Hand: %c\n", Bios->Hand);
}

void PrintStudent(STUDENT *Student)
{
	printf("Name: %s\n", Student->Name);
	printf("Age: %ld\n", Student->Age);
	printf("Gender: %c\n", Student->Gender);
	PrintBios(&Student->Bios);
	printf("Class: %s\n", get_class_str(Student->Class));
	printf("Grade: %ld%%\n\n", Student->Grade);
}

void GetAmanda(STUDENT *Amanda)
{
	Amanda->Age = 21;
	Amanda->Gender = 'F';
	Amanda->Grade = 88;
	Amanda->Class = HISTORY;
	Amanda->Bios.Blood = 'A';
	Amanda->Bios.Donor = true;
	Amanda->Bios.Eyes = GRAY;
	Amanda->Bios.Hair = BLONDE;
	Amanda->Bios.Hand = 'L';
	strncpy(Amanda->Name, "Amanda", MAX_NAME);
}

void Stuff()
{
	STUDENT Amanda;
	STUDENT Bob;

	GetAmanda(&Amanda);

	Bob.Age = 23;
	Bob.Gender = 'M';
	Bob.Grade = 81;
	Bob.Class = CHEMISTRY;
	Bob.Bios.Blood = 'O';
	Bob.Bios.Donor = false;
	Bob.Bios.Eyes = BROWN;
	Bob.Bios.Hair = BROWN;
	Bob.Bios.Hand = 'R';
	strncpy(Bob.Name, "Bob", MAX_NAME);

	PrintStudent(&Amanda);
	PrintStudent(&Bob);
}

int main(int argc, char *argv[])
{
    Stuff();
    printf("Press any key to continue\n");
    int c = getchar();
}
