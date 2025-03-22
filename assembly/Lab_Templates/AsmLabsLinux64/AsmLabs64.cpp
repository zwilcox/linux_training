#include "validation.h"
#include <dlfcn.h>

char YOURNAME[30] = "student_name";	// your nickname
char SERVER_IP[16] = "172.16.1.100"; 	// instructor IP running the scoreboard
int PORT = 8989;			// instuctor port running the scoreboard
submit_score submit = NULL;

int main(int argc, char* argv[])
{
    void *handle = dlopen("./scoreboard.so", RTLD_LAZY);
    if (NULL == handle)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();
    submit = (submit_score)dlsym(handle, "submit_score");
    char *error = dlerror();
    if (NULL != error)
    {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

/* Uncomment each line(s) below to validate */
//  lab0_1();
    validate_lab1_1();
/*    validate_lab1_2();
    validate_lab1_3();
rax
    validate_lab2_1();
    validate_lab2_2();
    validate_lab2_3();
    validate_lab2_4(); //BONUS

    validate_lab3_1();
    validate_lab3_2();
    validate_lab3_3();
    validate_lab3_4();
    validate_lab3_5(); //SINGLE OR DOUBLE BONUS (will print if achieved)

    validate_lab4_1();
    validate_lab4_2();
    validate_lab4_3();
    validate_lab4_4();
    validate_lab4_5(); //BONUS

    validate_lab5_1();
    validate_lab5_2();
    validate_lab5_3(); // Bonus

    validate_lab6_1();
    validate_lab6_2();
    validate_lab6_3();
    validate_lab6_4(); //Bonus 
    validate_lab6_5(); 

    validate_lab7_1(); 
    validate_lab7_2();
    validate_lab7_3(); //BONUS
    //validate_lab7_4(); // BONUS manually validated

    validate_lab8_1();
    validate_lab8_2();
    validate_lab8_3();
    validate_lab8_4();
    validate_lab8_5();
    validate_lab8_6(); //BONUS

    validate_lab9_1();
    validate_lab9_2();

    validate_lab10_1();
    validate_lab10_2();

    // Challenge labs
    validate_lab11_1();
    validate_lab11_2();
    validate_lab11_3();

    // Additional challenges
    validate_lab11_4();
    validate_lab11_5();
    validate_lab11_6();
    validate_lab11_7();
	*/
    dlclose(handle);
    printf( "------------------------\n"
            "Validation complete!\n"
            "Hit a key to exit\n"
            );
    fgetc(stdin);
    return 0;
}

