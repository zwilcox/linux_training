#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>
#include <signal.h>

#define TMP_BUF_SIZE	1024
#define MSG_BUF_SIZE	1024
#define QUEUE_MAX_SIZE	10

struct thread_args {
	int id;
	int num_iters;
};

/* network functions */
int connect_to_server(char* host, char* port);
ssize_t recv_str(int sock, char** str_ptr);
ssize_t recv_line(int sock, char** str_ptr);
ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len,
	unsigned char** buf_ptr);

/* threading functions */
struct thread_args* create_args(int num_threads, int num_iters);
pthread_t* create_threads(int num_threads, struct thread_args* args);
void wait_for_threads(int num_threads, pthread_t* tids);
void* thread_func(void* arg);

/* support functions */
void print_usage(char* name);
void sig_handler(int signum);

int main(int argc, char* argv[]) {
	struct sigaction sa = {
		.sa_handler = sig_handler
	};
	int num_threads;
	int num_iters;
	pthread_t* tids;
	struct thread_args* targs;

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		return EXIT_FAILURE;
	}

	if (argc < 3) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}
	num_threads = strtol(argv[1], NULL, 10);
	if (num_threads == LONG_MIN || num_threads == LONG_MAX) {
		fprintf(stderr, "Invalid number of threads\n");
		return EXIT_FAILURE;
	}
	num_iters = strtol(argv[2], NULL, 10);
	if (num_iters == LONG_MIN || num_iters == LONG_MAX) {
		fprintf(stderr, "Invalid number of iterations\n");
		return EXIT_FAILURE;
	}

	targs = create_args(num_threads, num_iters);
	if (targs == NULL) {
		fprintf(stderr, "failed to create thread params\n");
		return EXIT_FAILURE;
	}

	tids = create_threads(num_threads, targs);
	if (tids == NULL) {
		fprintf(stderr, "failed to create threads\n");
		return EXIT_FAILURE;
	}

	/* clean up */
	wait_for_threads(num_threads, tids);
	printf("All threads finished sucessfully\n");
	return EXIT_SUCCESS;
}

void print_usage(char* name) {
	printf("Usage: %s num_threads num_iterations\n", name);
	return;
}

/* Creates a bound socket of the given type on the given port
   Calls listen() for stream sockets
   Returns socket descriptor on success, or a value <0 on failure */
int connect_to_server(char* host, char* port) {
	struct addrinfo hints = { 0 };
	struct addrinfo* ai_result;
	struct addrinfo* ai;
	int ret;
	int sock;

	hints.ai_family = AF_INET; /* IPv4 */
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(host, port, &hints, &ai_result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
	}

	for (ai = ai_result; ai != NULL; ai = ai->ai_next) {
		sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
		if (sock == -1) {
			perror("socket");
			continue;
		}

		if (connect(sock, ai->ai_addr, ai->ai_addrlen) == -1) {
			perror("connect");
			close(sock);
			continue;
		}
		break; /* if we made it here, we were sucessful */
	}

	freeaddrinfo(ai_result);

	if (ai == NULL) {
		fprintf(stderr, "Could not find a suitable address\n");
		return -1;
	}

	return sock;
}

ssize_t recv_line(int sock, char** str_ptr) {
	return recv_until(sock, 
			(unsigned char*)"\n",
			1, 
			(unsigned char**)str_ptr);
}

ssize_t recv_str(int sock, char** str_ptr) {
	return recv_until(sock, 
			(unsigned char*)"\0", 
			1, 
			(unsigned char**)str_ptr);
}

ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len, 
	unsigned char** buf_ptr) {

	ssize_t total_bytes_recvd = 0;
	ssize_t bytes_read;
	unsigned char* buffer = NULL;

	while (1) {
		buffer = (unsigned char*)realloc(buffer, 
				total_bytes_recvd ? total_bytes_recvd * 2 : TMP_BUF_SIZE);
		if (buffer == NULL) {
			return -1;
		}
		bytes_read = recv(sock, buffer+total_bytes_recvd, TMP_BUF_SIZE, 0);
		if (bytes_read == 0) {
			free(buffer);
			return -1;
		}
		total_bytes_recvd += bytes_read;
		if (memmem(buffer, total_bytes_recvd, needle, needle_len) != NULL) {
			break;
		}
	}

	*buf_ptr = buffer;
	return total_bytes_recvd;	
}

struct thread_args* create_args(int num_threads, int num_iters) {
	int i;
	struct thread_args* args;
	args = (struct thread_args*)malloc(
			num_threads * sizeof(struct thread_args));
	if (args == NULL) {
		return NULL;
	}
	for (i = 0; i < num_threads; i++) {
		args[i].id = i;
		args[i].num_iters = num_iters;
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
	struct thread_args* targs = (struct thread_args*)arg;
	char message[MSG_BUF_SIZE];
	char* response;
	int i;
	int sock;
	snprintf(message, MSG_BUF_SIZE, "Hello from thread %d", targs->id);
	printf("thread %d starting %d iters\n", targs->id, targs->num_iters);
	for (i = 0 ; i < targs->num_iters; i++) {
		response = NULL;
		sock = connect_to_server("localhost", "8080");
		if (sock == -1) {
			fprintf(stderr, "Thread %d could not connect\n", targs->id);
			exit(EXIT_FAILURE);
		}
		send(sock, message, strlen(message) + 1, 0);
		if (recv_str(sock, &response) == -1) {
			fprintf(stderr, "Thread %d did not receive msg\n", targs->id);
			exit(EXIT_FAILURE);
		}
		free(response);
		printf("Thread %d finished %d iterations\n", targs->id, i+1);
		close(sock);
	}
	return NULL;
}

void sig_handler(int signum) {
	_exit(0);
	return;
}
