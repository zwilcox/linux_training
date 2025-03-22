#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// to get this I just set a watch point on this global
uint64_t desired_text_hash __attribute__ ((section ("hash")));
uint64_t password_ct = 0xce88a4be8eb95d69;

extern char __etext;
extern char __stext;

sem_t hashsem = {0};
pthread_barrier_t barrier;

static void wait_timer_key(int totalsec, uint64_t* key);
static void* watcher(void* key);

int main(int argc, char* argv[]) {
	uint64_t passwordkey = 0x0123456789ABCDEF;
	uint64_t password_pt = 0x0;
	pthread_t watcher_thread = {0};
	pthread_attr_t watcher_attr = {0};
	int err = 0;

	// initalize our stuff
	err = pthread_attr_init(&watcher_attr);	
	if (err != 0) {
		printf("Error on attr_init. 0x%x\n", err);
		goto END;
	}

	err = sem_init(&hashsem, 0, 0);
	if (err != 0) {
		printf("Error on sem_init. 0x%x\n", err);
		goto END_1;
	}

	pthread_barrier_init(&barrier, NULL, 2);
	// start the watcher thread
	err = pthread_create(
		&watcher_thread,
		&watcher_attr,
		watcher,
		&passwordkey
	);
	if (err != 0) {
		printf("Error on ptc. 0x%x\n", err);
		goto END_2;
	}

	pthread_barrier_wait(&barrier);
	// start the timer
	wait_timer_key(60 * 60 * 3, &passwordkey);

	password_pt = password_ct ^ passwordkey;
	printf(
		"Thank you for waiting. Your flag is \"%s\"\n",
		((char*)(&password_pt))
	);

	// signal and join thread
	sem_post(&hashsem);
	pthread_join(watcher_thread, NULL);


END_2:
	sem_destroy(&hashsem);
END_1:
	pthread_attr_destroy(&watcher_attr);
END:
	printf("Goodbye\n");

	return err;
}

void wait_timer_key(int totalsec, uint64_t* key) {
	printf("patience is a virtue\n");
	while (totalsec > 0) {
		sleep(1);
		totalsec--;

		// update the key
		*key ^= (totalsec) << (totalsec & 0x3f);

		// have some nice color timer output
		printf("Seconds left: %d\n", totalsec);
	}
}

void* watcher(void* key) {
	uint64_t sum = 0x0;
	uint64_t* cur = NULL;
	// sneaky thing, change the inital key before it is used for the first time
	*((uint64_t*)key) = 0xCEFAC1CEFAC1CEFA;
	// now hash the .text section over and over, checking for a change
	pthread_barrier_wait(&barrier);
	while (sem_trywait(&hashsem)) {
		sum = 0;
		/* See linker script to see how we got __stext and __etext */
		//printf("\n");
		for (cur = (uint64_t*)&__stext; cur < (uint64_t*)&__etext; cur++) {
			sum += *cur;
		}
		
		if (sum != desired_text_hash) {
			printf("HACKING DETECTED\n");
			exit(-1);
		}
	}
	
	return NULL;
}
