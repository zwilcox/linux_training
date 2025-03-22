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

int connect_to_server(char* host, char* port);
ssize_t recv_line(int sock, char** str_ptr);
ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len, unsigned char** buf_ptr);

int main(int argc, char* argv[])
{
	char message[MSG_BUF_SIZE];
	char* response;
	int sock;

	snprintf(message, MSG_BUF_SIZE, "Hello from client\n");
	sock = connect_to_server("localhost", "8080");
	if (sock == -1) {
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	for (;;) {
		response = NULL;
		send(sock, message, strlen(message), 0);
		if (recv_line(sock, &response) == -1) {
			fprintf(stderr, "Did not receive msg\n");
			exit(EXIT_FAILURE);
		}
		printf("response: %s\n", response);
		free(response);
	}
	close(sock);
	return EXIT_SUCCESS;
}

int connect_to_server(char* host, char* port)
{
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

ssize_t recv_line(int sock, char** str_ptr)
{
	ssize_t bytes_recvd = recv_until(sock, (unsigned char*)"\n", 1, (unsigned char**)str_ptr);
	if (bytes_recvd != -1) {
		(*str_ptr)[bytes_recvd - 1] = '\0';
	}
	return bytes_recvd;
}

ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len, unsigned char** buf_ptr)
{

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

