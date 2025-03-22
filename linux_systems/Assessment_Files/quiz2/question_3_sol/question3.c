#include <stdio.h>
#include "question3.h"

void on_load(void) __attribute__((constructor));
void on_unload(void) __attribute__((destructor));

void on_load(void) {
    printf("Loading library!\n");
}

void print_the_answer_to_question_3(void) {
    printf("42\n");
}

void on_unload(void) {
    printf("Unloading library!\n");
}
