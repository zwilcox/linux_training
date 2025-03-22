#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

#define DEBUG_LIB_PATH	"libdebug.so"
#define TMP_BUF_SIZE	1024
#define DBG(x) do {\
	if (debug) {\
		debug(x);\
	}\
} while (0)


int (*debug)(char* str);
void load_debug_lib(char* path);
int connect_to_server(char* host, char* port);
int create_server_socket(char* port, int type);
int accept_clients(int s_sock);
int handle_client(int sock);
void* client_func(void* args);
ssize_t recv_str(int sock, char** str_ptr);
ssize_t recv_line(int sock, char** str_ptr);
ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len,
	unsigned char** buf_ptr);
ssize_t send_str(int sock, char* str);
ssize_t encrypted_send(int sock, char* str);

unsigned char pad[] = {
  0x1c, 0x5e, 0xdd, 0x79, 0x13, 0x07, 0xd2, 0x58, 0x1d, 0xf8, 0xb5, 0x0c,
  0x6e, 0xa6, 0x9a, 0x5b, 0x84, 0x59, 0xad, 0x9c, 0xfc, 0x68, 0xec, 0x08,
  0xcf, 0x76, 0xb9, 0x60, 0xc1, 0x4b, 0x73, 0x9d, 0xc3, 0xc8, 0xc1, 0x0f,
  0xd7, 0x8b, 0x99, 0x55, 0x90, 0x66, 0x5d, 0xe2, 0x87, 0x19, 0xcb, 0x5c,
  0x16, 0x1d, 0x45, 0x29, 0xc2, 0x5d, 0xf0, 0xce, 0x89, 0xc4, 0xce, 0x72,
  0xa0, 0x7e, 0x49, 0xc8, 0xbc, 0x2c, 0x1e, 0xc0, 0xaf, 0xb6, 0x8a, 0x02,
  0x22, 0xf6, 0x5a, 0x67, 0x60, 0xed, 0x11, 0x28, 0xc4, 0x16, 0xb6, 0x49,
  0xfb, 0x78, 0x5b, 0x51, 0xfd, 0x34, 0xbc, 0x81, 0x95, 0xb1, 0xf5, 0x54,
  0x25, 0x2e, 0x8d, 0x8d, 0xa6, 0x1b, 0x3e, 0x64, 0x58, 0x6c, 0xfb, 0xa1,
  0xf2, 0x42, 0xee, 0xc3, 0xe6, 0xa3, 0x5a, 0x40, 0x76, 0x43, 0x51, 0x59,
  0x87, 0x9d, 0x81, 0xdc, 0x72, 0x46, 0xe9, 0xe7
};

int main() {
	pthread_t tid;
	int sock;
	printf("Starting up...\n");
	load_debug_lib(DEBUG_LIB_PATH);
	sock = create_server_socket("8080", SOCK_STREAM);
	pthread_create(&tid, NULL, client_func, NULL);
	accept_clients(sock);
	close(sock);
    return 0;
}

void load_debug_lib(char* path) {
	void* dbg_handle;
	struct stat st;
	
	if (stat(path, &st) == -1) {
		return;
	}

	dbg_handle = dlopen(path, RTLD_NOW);
	if (dbg_handle == NULL) {
		return;
	}
	debug = dlsym(dbg_handle, "debug");
	printf("Debugging enabled!\n");
	return;
}

int accept_clients(int s_sock) {
	struct sockaddr_storage client_addr;
	socklen_t addr_len;
	int c_sock;
	pid_t pid;

    while (1) {
        addr_len = sizeof(client_addr);
    	c_sock = accept(s_sock, (struct sockaddr*)&client_addr, &addr_len);
    	if (c_sock == -1) {
            if (errno == EINTR) continue;
            perror("accept");
    		return -1;
    	}
		handle_client(c_sock);
    }
    return 0;
}

int connect_to_server(char* host, char* port) {
	struct addrinfo hints = { 0 };
	struct addrinfo* ai_result = NULL;
	struct addrinfo* ai = NULL;
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

void* client_func(void* args) {
	int sock;
	unsigned int i;
	sock = connect_to_server("localhost", "8080");
	i = 0;
	while (1) {
		send_str(sock, "This is super secret data. No one should see it!\n");
		printf("I sent a message %u times\n", i++);
		sleep(1);
	}
	close(sock);
	return NULL;
}

int handle_client(int sock) {
	char* message;
	DBG("Handling a client");
	while (1) {
		message = NULL;
		if (recv_str(sock, &message) == -1) {
			break;
		}
		free(message);
		char secret_msg[128] = "This is very well encrypted. No one will ever see it except its intended party\n";
		encrypted_send(sock, secret_msg);
	}
	close(sock);
	return 0;
}

ssize_t recv_line(int sock, char** str_ptr) {
	DBG("Receving a line");
	return recv_until(sock, 
			(unsigned char*)"\n",
			1, 
			(unsigned char**)str_ptr);
}

ssize_t recv_str(int sock, char** str_ptr) {
	DBG("Receving a string");
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

ssize_t send_str(int sock, char* str) {
	ssize_t bytes_sent;
	int bytes_to_send = strlen(str) + 1;
	int total_bytes_sent = 0;
	DBG("Sending data");
	while (bytes_to_send > 0) {
		bytes_sent = send(sock, str + total_bytes_sent, bytes_to_send, 0);
		if (bytes_sent == -1) {
			perror("send");
			return -1;
		}
		total_bytes_sent += bytes_sent;
		bytes_to_send -= bytes_sent;
	}
	return total_bytes_sent;
}

ssize_t encrypted_send(int sock, char* str) {
	int i;
	ssize_t bytes_sent;
	int bytes_to_send = 128;
	int total_bytes_sent = 0;
	DBG("Sending data");

	for (i = 0; i < bytes_to_send; i++) {
		str[i] = str[i] ^ pad[i];
	}

	while (bytes_to_send > 0) {
		bytes_sent = send(sock, str + total_bytes_sent, bytes_to_send, 0);
		if (bytes_sent == -1) {
			perror("send");
			return -1;
		}
		total_bytes_sent += bytes_sent;
		bytes_to_send -= bytes_sent;
	}
	printf("encrypted send finished\n");
	return total_bytes_sent;
}
