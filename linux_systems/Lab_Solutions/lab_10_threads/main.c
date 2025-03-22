#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>

#include "colors.h"

#define NUM_COLORS  3
#define NUM_NAMES	3

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

const char* names[NUM_NAMES] = { 
	"what",
	"who",
	"Slim Shady"
};

int main(int argc, char* argv[]) {
	int num_threads;
	pthread_t* tids;
	struct thread_args* targs;

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

	printf_color(YLW, "Counter is %d\n", g_counter);
	tids = create_threads(num_threads, targs);
	if (tids == NULL) {
		fprintf(stderr, "failed to create threads\n");
		exit(EXIT_FAILURE);
	}

	wait_for_threads(num_threads, tids);
	printf_color(YLW, "Counter is %d\n", g_counter);
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
		pthread_join(tids[i], NULL);
	}
	return;
}

void* thread_func(void* arg) {
	int i;
	struct thread_args* targs = (struct thread_args*)arg;
	printf_color(targs->color, "Thread %d Counter starting at %d, incrementing...\n", targs->id, g_counter);
	for (i = 0; i < 100; i++) {
		g_counter++;
	}
	printf_color(targs->color, "Thread %d Counter is now %d\n", targs->id, g_counter);
	return NULL;
}

