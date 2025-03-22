#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sleep_msec;

void sig_handler(int signum);

#define NUM_MESSAGES    13
#define USLEEP_MAX      1000000
#define USLEEP_INTERVAL 20000

char* messages[NUM_MESSAGES] = {
    "If",
    "you",
    "can",
    "read",
    "this",
    "you've",
    "done",
    "well",
    "the",
    "password",
    "is",
    "deathbyllama",
    "goodbye"
};

int main() {
    int i;

    struct sigaction sa = {0};
    struct sigaction old_sa;

    sa.sa_handler = sig_handler;
    sa.sa_flags = SA_RESTART;
    sleep_msec = 10000;

    if (sigaction(SIGUSR1, &sa, &old_sa) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    if (sigaction(SIGUSR2, &sa, &old_sa) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    
    raise(SIGSTOP);

    while (1) {
        for (i = 0; i < NUM_MESSAGES; i++) {
            printf("\r%s", messages[i]);
            fflush(stdout);
            usleep(sleep_msec);
            printf("\r                 ");
            fflush(stdout);
        }
    }
    return EXIT_SUCCESS;
}

void sig_handler(int signum) {
    if (signum == SIGUSR1) {
        if (sleep_msec >= USLEEP_INTERVAL) {
            sleep_msec -= USLEEP_INTERVAL;
        }
    }
    else if (signum == SIGUSR2) {
        if (sleep_msec <= USLEEP_MAX) {
            sleep_msec += USLEEP_INTERVAL;
        }
    }
    return;
}
