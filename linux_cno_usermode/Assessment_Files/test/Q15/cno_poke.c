#include <sys/ptrace.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#define FLAG_LEN    33
volatile int changeme = 0;

int main() {
    int i = 0;
    int j = 0;
    int test = 0;
    int k = 0;
    //char flag[] = "flag{POKING_AND_PEEKING_ARE_RUDE}\0\0\0\0\0\0\0\0";
	char flag[] = {0x97, 0xd7, 0x07, 0x0d, 0xd0, 0x07, 0x4d, 0x47, 0x6e, 0x49, 0x0d, 0x09, 0x10, 0x84, 0x04, 0x7d, 0x42, 0x53, 0xfe, 0x3c, 0x13, 0xf7, 0x35, 0x18, 0x72, 0xb8, 0xb6, 0x7e, 0x92, 0xa3, 0x8f, 0x3d, 0x94, 0x0a};
    srand(getpid());

    changeme = rand() & 0xffff;
    test = changeme * 2;

    // first try and ptrace TRACEME
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) {
        // exit out early because we don't like their stuff
        fprintf(stderr, "ERROR: Run me with the cnd_poke_handler\n");
        exit(-1);
    }

    if (ptrace(PTRACE_TRACEME, 0, 0, 0) != -1) {
        goto END;
    }

    raise(SIGSTOP);

    if (changeme != test) {
        fprintf(stderr, "ERROR: changeme didn't change to the correct answer. Expected 0x%x, got 0x%x\n", test, changeme);
        exit(-1);
        goto END;
    }
    // unobfuscate the flag
    srand(0x41435450);

    // put a check here as well
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) != -1) {
        goto END;
    }

    for (i = 0; i < 3; i++) {
        for (j=0; j < FLAG_LEN; j += sizeof(k)) {
            k = rand();
           *((int*)(&flag[j])) ^= k;
        }
    }

    for (i=0; i < FLAG_LEN; i++) {
        fprintf(stdout, "%c", flag[i]);
    }
    fprintf(stdout, "\n");

END:
    return 0;
}
