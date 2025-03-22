#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>

#define TMP_BUF_SIZE	1024
#define MSG_BUF_SIZE	1024

int create_datagram_socket(void);
int resolve_addr(char* host, char* port, struct sockaddr* addr, socklen_t* addrlen);
ssize_t recv_line(int sock, char** str_ptr);
ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len,
    unsigned char** buf_ptr);

int main(int argc, char* argv[]) {
	char message[MSG_BUF_SIZE];
	struct sockaddr_in6 addr;
	socklen_t addrlen = sizeof(addr);
	char* response;
	int sock;
	int i;
	int count = 100;

	snprintf(message, MSG_BUF_SIZE, "Hello from client\n");
	sock = create_datagram_socket();
	if (sock == -1) {
		fprintf(stderr, "Could not create socket\n");
		exit(EXIT_FAILURE);
	}

	if (resolve_addr("localhost", "8080", (struct sockaddr*)&addr, &addrlen) == -1) {
		fprintf(stderr, "Failed to resolve server address\n");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < count; i++) {
		response = NULL;
		if (sendto(sock, message, strlen(message), 0,
				(struct sockaddr*)&addr, addrlen) == -1) {
			perror("sendto");
			exit(EXIT_FAILURE);
		}
		if (recv_line(sock, &response) == -1) {
			fprintf(stderr, "Did not receive msg\n");
			exit(EXIT_FAILURE);
		}
		printf("response: %s", response);
		free(response);
		snprintf(message, MSG_BUF_SIZE, "Hello from client [%d]\n", i);
		usleep(100000);
	}
	close(sock);
	return EXIT_SUCCESS;
}

int create_datagram_socket(void) {
	int sock;
	sock = socket(PF_INET6, SOCK_DGRAM, 0);
	if (sock == -1) {
		perror("socket");
		return -1;
	}
	return sock;
}

int resolve_addr(char* host, char* port, struct sockaddr* addr, socklen_t* addrlen) {
	struct addrinfo hints = { 0 };
	struct addrinfo* ai_result;
	struct addrinfo* ai;
	int ret;

	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;

	ret = getaddrinfo(host, port, &hints, &ai_result);
	if (ret != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
	}

	for (ai = ai_result; ai != NULL; ai = ai->ai_next) {
		memcpy((void*)addr, ai->ai_addr, ai->ai_addrlen);
		*addrlen = ai->ai_addrlen;
		break; /* if we made it here, we were sucessful */
	}

	freeaddrinfo(ai_result);

	if (ai == NULL) {
		fprintf(stderr, "Could not find a suitable address\n");
		return -1;
	}

	return 0;
}
ssize_t recv_line(int sock, char** str_ptr) {
	return recv_until(sock, 
			(unsigned char*)"\n",1 ,(unsigned char**)str_ptr);
}


ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len, 
	unsigned char** buf_ptr) {

	struct sockaddr_in6 src_addr;
	socklen_t addrlen = sizeof(src_addr);
	ssize_t total_bytes_recvd = 0;
	ssize_t bytes_read;
	unsigned char* buffer = NULL;

	while (1) {
		buffer = (unsigned char*)realloc(buffer, 
				total_bytes_recvd ? total_bytes_recvd * 2 : TMP_BUF_SIZE);
		if (buffer == NULL) {
			return -1;
		}
		bytes_read = recvfrom(sock, buffer+total_bytes_recvd, TMP_BUF_SIZE, 0,
							(struct sockaddr*)&src_addr, &addrlen);
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

