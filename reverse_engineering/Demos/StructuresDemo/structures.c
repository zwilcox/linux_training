#include <stdio.h>
#include <string.h>
#include "structures.h"

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

void PrintStudent(STUDENT *Student)
{
	printf("Name: %s\n", Student->Name);
	printf("Age: %ld\n", Student->Age);
	printf("Gender: %c\n", Student->Gender);
	printf("Class: %s\n", get_class_str(Student->Class));
	printf("Grade: %ld%%\n\n", Student->Grade);
}

void GetAmanda(STUDENT *Amanda)
{
	Amanda->Age = 21;
	Amanda->Gender = 'F';
	Amanda->Grade = 88;
	Amanda->Class = HISTORY;
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
