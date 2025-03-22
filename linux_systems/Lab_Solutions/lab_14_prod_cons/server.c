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
#include "queue.h"

#define TMP_BUF_SIZE    1024
#define QUEUE_MAX_SIZE  10

struct thread_args {
    int id;
};

/* network functios */
int create_server_socket(char* port, int type);
int accept_clients(int s_sock);
int handle_client(int sock);

ssize_t recv_str(int sock, char** str_ptr);
ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len, 
    unsigned char** buf_ptr);

/* threading functions */
struct thread_args* create_args(int num_threads);
pthread_t* create_threads(int num_threads, struct thread_args* args);
void wait_for_threads(int num_threads, pthread_t* tids);
void* thread_func(void* arg);

/* support functions */
unsigned char* memmem(unsigned char* haystack, size_t haystack_len,
     unsigned char* needle, size_t needle_len);
void print_usage(char* name);
void sig_handler(int signum);

/* shared memory */
sig_atomic_t g_running;
queue_t* sock_queue;
#ifdef __SOLUTION__
sem_t queue_not_empty;
sem_t queue_not_full;
pthread_mutex_t queue_mutex;
#endif

int main(int argc, char* argv[]) {
    struct sigaction sa = {
        .sa_handler = sig_handler
    };
    int num_threads;
    pthread_t* tids;
    struct thread_args* targs;
    int sock;

    g_running = 1;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    num_threads = strtol(argv[1], NULL, 10);
    if (num_threads == LONG_MIN || num_threads == LONG_MAX) {
        fprintf(stderr, "Invalid number of threads\n");
        return EXIT_FAILURE;
    }

    #ifdef __SOLUTION__
    pthread_mutex_init(&queue_mutex, NULL);
    if (sem_init(&queue_not_empty, 0, 0) == -1) {
        perror("sem_init");
        return EXIT_FAILURE;
    }
    if (sem_init(&queue_not_full, 0, QUEUE_MAX_SIZE) == -1) {
        perror("sem_init");
        return EXIT_FAILURE;
    }
    #endif
    sock_queue = make_queue();

    sock = create_server_socket("8080", SOCK_STREAM);
    if (sock == -1) {
        return EXIT_FAILURE;
    }


    targs = create_args(num_threads);
    if (targs == NULL) {
        fprintf(stderr, "failed to create thread params\n");
        return EXIT_FAILURE;
    }

    tids = create_threads(num_threads, targs);
    if (tids == NULL) {
        fprintf(stderr, "failed to create threads\n");
        return EXIT_FAILURE;
    }

    accept_clients(sock);

    printf("Shuttin' Down\n");

    /* clean up */
    wait_for_threads(num_threads, tids);
    free_queue(sock_queue);
    #ifdef __SOLUTION__
    if (sem_destroy(&queue_not_full) == -1 ||
        sem_destroy(&queue_not_empty) == -1) {
        perror("sem_close");
        return EXIT_FAILURE;
    }
    pthread_mutex_destroy(&queue_mutex);
    #endif
    free(tids);
    free(targs);
    return EXIT_SUCCESS;
}

void print_usage(char* name) {
    printf("Usage: %s num_threads\n", name);
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

        #ifdef __SOLUTION__
        if (sem_wait(&queue_not_full) == -1) {
            if (errno == EINTR) continue;
            perror("sem_wait");
            return -1;
        }
        pthread_mutex_lock(&queue_mutex);
        #endif
        enqueue(sock_queue, (void*)(size_t)c_sock);
        #ifdef __SOLUTION__
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&queue_not_empty);
        #endif
    }
    return 0;
}

int handle_client(int sock) {
    char* message;
    while (1) {
        message = NULL;
        if (recv_str(sock, &message) == -1) {
            break;
        }
        printf("Received: %s\n", message);
        send(sock, message, strlen(message) + 1, 0);
        free(message);
    }
    close(sock);

    return 0;
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

unsigned char* memmem(unsigned char* haystack, size_t haystack_len,
     unsigned char* needle, size_t needle_len) {

    size_t i;
    if (needle_len == 0 || haystack_len < needle_len) {
        return NULL;
    }

    haystack_len -= (haystack_len % needle_len);

    for (i = 0; i < haystack_len; i++) {
        if (memcmp(&haystack[i], needle, needle_len) == 0) {
            return &haystack[i];
        }
    }
    return NULL;
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
    int sock;
    struct thread_args* targs = (struct thread_args*)arg;
    printf("Thread %d reporting for duty\n", targs->id);

    while (g_running) {
        #ifdef __SOLUTION__
        if (sem_wait(&queue_not_empty) == -1) {
            if (errno == EINTR) continue;
            perror("sem_wait");
            return NULL;
        }
        pthread_mutex_lock(&queue_mutex);
        #endif
        sock = (int)(size_t)dequeue(sock_queue);
        #ifdef __SOLUTION__
        pthread_mutex_unlock(&queue_mutex);
        sem_post(&queue_not_full);
        #endif
        handle_client(sock);
    }
    return NULL;
}

void sig_handler(int signum) {
    g_running = 0;
    return;
}
