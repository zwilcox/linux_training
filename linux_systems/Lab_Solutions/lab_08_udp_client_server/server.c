#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <arpa/inet.h>

#define TMP_BUF_SIZE    1024

/* network functions */
int create_server_socket(char* port, int type);
int handle_clients(int sock);
ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len, 
	unsigned char** buf_ptr, struct sockaddr* addr, socklen_t* addrlen);
ssize_t recv_line(int sock, char** str_ptr, struct sockaddr* addr, socklen_t* addrlen);

/* support functions */
void print_addr(struct sockaddr *addr);
unsigned char* memmem(unsigned char* haystack, size_t haystack_len,
	 unsigned char* needle, size_t needle_len);

int main(int argc, char* argv[]) {
	int sock;

    sock = create_server_socket("8080", SOCK_DGRAM);
	if (sock == -1) {
		return EXIT_FAILURE;
	}

    handle_clients(sock);

	return EXIT_SUCCESS;
}

int create_server_socket(char* port, int type) {
	struct addrinfo hints = { 0 };
	struct addrinfo* ai_result;
	struct addrinfo* ai_cur;
	int ret;
	int sock;

	hints.ai_family = AF_INET6; /* IPv4 or IPv6 */
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

		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, 
			sizeof(int)) == -1) {
			close(sock);
			continue;
		}

		if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &(int){0}, 
			sizeof(int)) == -1) {
			close(sock);
			continue;
		}

		if (bind(sock, ai_cur->ai_addr, ai_cur->ai_addrlen) == -1) {
			close(sock);
			continue;
		}
		print_addr(ai_cur->ai_addr);
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

int handle_clients(int sock) {
	char* message;
	struct sockaddr_storage src_addr;
	socklen_t addrlen;
	while (1) {
		message = NULL;
		addrlen = sizeof(src_addr);
		if (recv_line(sock, &message, 
				(struct sockaddr*)&src_addr, &addrlen) == -1) {
			break;
		}
		printf("Received: %s\n", message);
		printf("From:");
		print_addr(&src_addr);
		sendto(sock, message, strlen(message), 0,
			(struct sockaddr*)&src_addr, addrlen);
		free(message);
	}

	return 0;
}

ssize_t recv_line(int sock, char** str_ptr, struct sockaddr* addr, socklen_t* addrlen) {
	return recv_until(sock, 
            (unsigned char*)"\n", 1, (unsigned char**)str_ptr, addr, addrlen);
}

ssize_t recv_until(int sock, unsigned char* needle, size_t needle_len, 
	unsigned char** buf_ptr, struct sockaddr* addr, socklen_t* addrlen) {

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
				addr, addrlen);
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

void print_addr(struct sockaddr *addr) {
	/* Make sure there's enough room for IPv6 addresses */
	char str[INET6_ADDRSTRLEN];
	unsigned long ip_addr;
	struct in6_addr ip6_addr;
	int port;
	if (addr->sa_family == AF_INET) {
		ip_addr = ((struct sockaddr_in*)addr)->sin_addr.s_addr;
		inet_ntop(AF_INET, &ip_addr, str, INET_ADDRSTRLEN);
		port = (int)ntohs(((struct sockaddr_in*)addr)->sin_port);
	} else {
		ip6_addr = ((struct sockaddr_in6*)addr)->sin6_addr;
		inet_ntop(AF_INET6, &ip6_addr, str, INET6_ADDRSTRLEN);
		port = (int)ntohs(((struct sockaddr_in6*)addr)->sin6_port);
	}
	printf("%s:%d\n", str, port);
	return;
}
