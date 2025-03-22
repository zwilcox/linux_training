#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <time.h>

#include "colors.h"

#define NUM_COLORS  3
#define NUM_NAMES   3

void sig_handler(int signum);
void* thread_func(void* arg);
struct thread_args* create_args(int num_threads);
pthread_t* create_threads(int num_threads, struct thread_args* args);
void wait_for_threads(int num_threads, pthread_t* tids);
void print_usage(char* name);


struct thread_args {
    int id;
    char name[32];
    char color[32];
};

volatile int g_counter;
volatile int g_running;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t g_sem;

const char* names[NUM_NAMES] = { 
    "what",
    "who",
    "Slim Shady"
};

int main(int argc, char* argv[]) {
    int num_threads;
    pthread_t* tids;
    struct thread_args* targs;
	g_running = 1;

    if (argc < 2) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    num_threads = strtol(argv[1], NULL, 10);
    if (num_threads == LONG_MIN || num_threads == LONG_MAX) {
        fprintf(stderr, "Invalid number of threads\n");
        exit(EXIT_FAILURE);
    }

    targs = create_args(num_threads);
    if (targs == NULL) {
        fprintf(stderr, "failed to create thread params\n");
        exit(EXIT_FAILURE);
    }

	struct sigaction sa = { .sa_handler = sig_handler}; /* rest auto initalized to zero */
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	//sem_init(&g_sem, 0, 5);  // Initial value of 5
	sem_init(&g_sem, 0, 0);

    printf_color(YLW, "Counter is %d\n", g_counter);
    tids = create_threads(num_threads, targs);
    if (tids == NULL) {
        fprintf(stderr, "failed to create threads\n");
        exit(EXIT_FAILURE);
    }

	while (g_running == 1) {
		if (getc(stdin) == 'q') {
			break;
		}
		else {
			sem_post(&g_sem);  // sem_post increments the value
		}
	}


    wait_for_threads(num_threads, tids);

    printf_color(YLW, "Counter is %d\n", g_counter);
	pthread_mutex_destroy(&g_mutex);
	sem_destroy(&g_sem);
    free(tids);
    free(targs);
	return EXIT_SUCCESS;
}

void print_usage(char* name) {
    printf("Usage: %s num_threads\n", name);
    return;
}

struct thread_args* create_args(int num_threads) {
    int i;
    struct thread_args* args;
    args = (struct thread_args*)malloc(
            num_threads * sizeof(struct thread_args));
    if (args == NULL) {
        return NULL;
    }
    for (i = 0; i < num_threads; i++) {
        strcpy(args[i].name, names[i % NUM_NAMES]);
		if (i % NUM_COLORS == 0) {

			strcpy(args[i].color, RED);
		}
		else if (i % NUM_COLORS == 1) {
			strcpy(args[i].color, GRN);
		}
		else if (i % NUM_COLORS == 2) {
			strcpy(args[i].color, BLU);
		}
        args[i].id = i;
    }
    return args;
}

pthread_t* create_threads(int num_threads, struct thread_args* args) {
    int i;
    int ret;
    pthread_t* tids;
    tids = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    if (tids == NULL) {
        return NULL;
    }
    for (i = 0; i < num_threads; i++) {
        ret = pthread_create(&tids[i], NULL, thread_func, &args[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(ret));
            free(tids);
            return NULL;
        }
    }
    return tids;
}

void wait_for_threads(int num_threads, pthread_t* tids) {
    int i;    
    for (i = 0; i < num_threads; i++) {
		pthread_kill(tids[i], SIGINT);
        pthread_join(tids[i], NULL);
    }
    return;
}

void* thread_func(void* arg) {
    int i;
    struct thread_args* targs = (struct thread_args*)arg;

	if (sem_wait(&g_sem) == -1) {  //sem_wait decrements the value
		if (g_running != 1) {
			printf("Thread should stop! Quitting...\n");
			return NULL;
		}
	}
    printf_color(targs->color, "Thread %d incrementing...\n", targs->id);
    for (i = 0; i < 100; i++) {
		pthread_mutex_lock(&g_mutex);
        g_counter++;
		pthread_mutex_unlock(&g_mutex);
    }
    printf_color(targs->color, "Counter is now %d\n", g_counter);
    return NULL;
}


void sig_handler(int signum) {
	g_running = 0;
	return;
}

