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
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "queue.h"
#include <sys/syscall.h>

#define TMP_BUF_SIZE    1024
#define QUEUE_MAX_SIZE  10

struct thread_args {
    int id;
	char* path;
};

/* network functios */
int create_server_socket(char* port, int type);
int accept_clients(int s_sock);
int handle_client(int sock, int fd, int size);

/* threading functions */
struct thread_args* create_args(int num_threads, char* path);
pthread_t* create_threads(int num_threads, struct thread_args* args);
void wait_for_threads(int num_threads, pthread_t* tids);
void* thread_func(void* arg);

/* support functions */
void print_usage(char* name);
void sig_handler(int signum);

/* shared memory */
sig_atomic_t g_running;
sem_t queue_not_empty;
sem_t queue_not_full;
pthread_mutex_t queue_mutex;
queue_t* sock_queue;

int main(int argc, char* argv[]) {
    struct sigaction sa = {
        .sa_handler = sig_handler
    };
    int num_threads;
    pthread_t* tids;
    struct thread_args* targs;
	int sock;
	char* port;
	char* path;

	printf("I am the main thread. I have PID = %d, TID = %ld, and PPID = %d\n",
		getpid(), syscall(SYS_gettid), getppid());
    g_running = 1;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    if (argc < 4) {
        print_usage(argv[0]);
		return EXIT_FAILURE;
    }
    num_threads = strtol(argv[1], NULL, 10);
    if (num_threads == LONG_MIN || num_threads == LONG_MAX) {
        fprintf(stderr, "Invalid number of threads\n");
		return EXIT_FAILURE;
    }

	port = argv[2];
	path = argv[3];
	
    pthread_mutex_init(&queue_mutex, NULL);
    if (sem_init(&queue_not_empty, 0, 0) == -1) {
        perror("sem_init");
        return EXIT_FAILURE;
    }
    if (sem_init(&queue_not_full, 0, QUEUE_MAX_SIZE) == -1) {
        perror("sem_init");
        return EXIT_FAILURE;
    }
    sock_queue = make_queue();


    targs = create_args(num_threads, path);
    if (targs == NULL) {
        fprintf(stderr, "failed to create thread params\n");
		return EXIT_FAILURE;
    }

    tids = create_threads(num_threads, targs);
    if (tids == NULL) {
        fprintf(stderr, "failed to create threads\n");
		return EXIT_FAILURE;
    }

    sock = create_server_socket(port, SOCK_STREAM);
	if (sock == -1) {
		return EXIT_FAILURE;
	}

    accept_clients(sock);

    printf("Shuttin' Down\n");

    /* clean up */
    wait_for_threads(num_threads, tids);
    free_queue(sock_queue);
    if (sem_destroy(&queue_not_full) == -1 ||
        sem_destroy(&queue_not_empty) == -1) {
        perror("sem_close");
        return EXIT_FAILURE;
    }
    pthread_mutex_destroy(&queue_mutex);
    free(tids);
    free(targs);
	return EXIT_SUCCESS;
}

void print_usage(char* name) {
    printf("Usage: %s num_threads port path-to-payload\n", name);
    return;
}

/* Creates a bound socket of the given type on the given port
   Calls listen() for stream sockets
   Returns socket descriptor on success, or a value <0 on failure */
int create_server_socket(char* port, int type) {
	struct addrinfo hints = { 0 };
	struct addrinfo* ai_result;
	struct addrinfo* ai_cur;
	int ret;
	int sock;
	int optval = 1;

	hints.ai_family = AF_INET; /* IPv4 */
	hints.ai_socktype = type; /* SOCK_STREAM or SOCK_DGRAM */
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;

	ret = getaddrinfo(NULL, port, &hints, &ai_result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
	}

	for (ai_cur = ai_result; ai_cur != NULL; ai_cur = ai_cur->ai_next) {
		sock = socket(ai_cur->ai_family, ai_cur->ai_socktype,
			ai_cur->ai_protocol);
		if (sock == -1) {
			continue;
		}

		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, 
			sizeof(optval)) == -1) {
			close(sock);
			continue;
		}

		if (bind(sock, ai_cur->ai_addr, ai_cur->ai_addrlen) == -1) {
			close(sock);
			continue;
		}
		break; /* if we made it here, we were sucessful */
	}

	freeaddrinfo(ai_result);

	if (ai_cur == NULL) {
		fprintf(stderr, "Could not find a suitable address\n");
		return -1;
	}

	if (type == SOCK_STREAM) {
		if (listen(sock, SOMAXCONN) == -1) {
			perror("listen");
			close(sock);
			return -1;
		}
	}

	return sock;
}

int accept_clients(int s_sock) {
	struct sockaddr_storage client_addr;
	socklen_t addr_len;
	int c_sock;

    while (g_running) {
        addr_len = sizeof(client_addr);
    	c_sock = accept(s_sock, (struct sockaddr*)&client_addr, &addr_len);
    	if (c_sock == -1) {
            if (errno == EINTR) continue;
            perror("accept");
    		return -1;
    	}

        if (sem_wait(&queue_not_full) == -1) {
            if (errno == EINTR) continue;
            perror("sem_wait");
    		return -1;
        }
        pthread_mutex_lock(&queue_mutex);
        enqueue(sock_queue, (void*)(size_t)c_sock);
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&queue_not_empty);
    }
    return 0;
}

int handle_client(int sock, int fd, int size) {
	long int bytes_left;
	long int bytes_sent;
	long int pos;
	int netsize;
	netsize = htonl(size);
	send(sock, (unsigned char*)&netsize, sizeof(netsize), 0);
	bytes_left = size;
	pos = 0;
	while (bytes_left > 0) {
		bytes_sent = sendfile(sock, fd, &pos, bytes_left);
		if (bytes_sent == -1) {
			perror("sendfile");
			close(sock);
			return -1;
		}
		bytes_left = bytes_left - bytes_sent;
	}
	close(sock);

	return 0;
}

struct thread_args* create_args(int num_threads, char* path) {
    int i;
    struct thread_args* args;
    args = (struct thread_args*)malloc(
            num_threads * sizeof(struct thread_args));
    if (args == NULL) {
        return NULL;
    }
    for (i = 0; i < num_threads; i++) {
        args[i].id = i;
		args[i].path = path;
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
    int sock;
	int fd;
	struct stat st;
    struct thread_args* targs = (struct thread_args*)arg;

	printf("I am thread %d. I have PID = %d, TID = %ld, and PPID = %d\n",
		targs->id, getpid(), syscall(SYS_gettid), getppid());
	fd = open(targs->path, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Failed to open file %s\n", targs->path);
		return NULL;
	}
	if (fstat(fd, &st) == -1) {
		perror("stat");
		close(fd);
		return NULL;
	}

    while (g_running) {
        if (sem_wait(&queue_not_empty) == -1) {
            if (errno == EINTR) continue;
            perror("sem_wait");
    		return NULL;
        }
        pthread_mutex_lock(&queue_mutex);
        sock = (int)(size_t)dequeue(sock_queue);
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&queue_not_full);
        handle_client(sock, fd, st.st_size);
    }
	close(fd);
    return NULL;
}

void sig_handler(int signum) {
    g_running = 0;
    return;
}
