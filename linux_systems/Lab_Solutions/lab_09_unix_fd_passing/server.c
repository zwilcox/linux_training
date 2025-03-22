#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/un.h>

int serve_unix_sock(char* path);
int recv_fd(int sock);

int main() {
	int passed_fd;
	int sock;
	int client_sock;
	struct sockaddr_storage addr;
	socklen_t addrlen;
	unlink("/tmp/mysock");
	sock = serve_unix_sock("/tmp/mysock");
	if (sock == -1) {
		return EXIT_FAILURE;
	}

	while (1) {
		addrlen = sizeof(addr);
		client_sock = accept(sock, (struct sockaddr*)&addr, &addrlen);
		if (client_sock == -1) {
			perror("accept");
			continue;
		}
		printf("Got a connection from a client\n");
		passed_fd = recv_fd(client_sock);
		if (write(passed_fd, "whoa", sizeof("whoa")) == -1) {
			perror("write");
		}
	}
	return EXIT_SUCCESS;
}

int serve_unix_sock(char* path) {
	int sock;
	socklen_t addrlen;
	struct sockaddr_un addr = {
		.sun_family = AF_UNIX,
	};
	strncpy(addr.sun_path, path, sizeof(addr.sun_path));
	addrlen = sizeof(sa_family_t) + strlen(addr.sun_path) + 1;
	sock = socket(PF_UNIX, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("socket");
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, 
		sizeof(int)) == -1) {
		close(sock);
		return -1;
	}

	if (bind(sock, (struct sockaddr*)&addr, addrlen) == -1) {
		perror("bind");
		return -1;
	}
	if (listen(sock, SOMAXCONN) == -1) {
		perror("listen");
		return -1;
	}
	return sock;
}

int recv_fd(int sock) {
	int fd;
	int* fdptr;
	struct cmsghdr* cmsg;
	char buffer[32];
	struct iovec io = {
		.iov_base = buffer,
		.iov_len = sizeof(buffer)
	};
	union {
		char buf[CMSG_SPACE(sizeof(fd))];
		struct cmsghdr align;
	} u;
	struct msghdr msg = {
		.msg_name = NULL, /* we don't care about source addr*/
		.msg_namelen = 0,
		.msg_iov = &io,
		.msg_iovlen = 1,
		.msg_control = u.buf,
		.msg_controllen = sizeof(u.buf)
	};
	
	if (recvmsg(sock, &msg, 0) == -1) {
		perror("recvmsg");
		return -1;
	}
	cmsg = CMSG_FIRSTHDR(&msg);
	fdptr = (int*)CMSG_DATA(cmsg);
	fd = *fdptr;
	printf("Received: %s\n", buffer);
	printf("The fd is %d\n", fd);
	return fd;
}

