#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/un.h>

int connect_to_unix_sock(char* path);
int send_fd(int sock, int fd);

int main() {
	int sock;
	sock = connect_to_unix_sock("/tmp/mysock");
	if (sock == -1) {
		return EXIT_FAILURE;
	}
	if (send_fd(sock, STDOUT_FILENO) == -1) {
		return EXIT_FAILURE;
	}
	while (1) {
		sleep(1);
	}
	return EXIT_SUCCESS;
}

int connect_to_unix_sock(char* path) {
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

	if (connect(sock, (struct sockaddr*)&addr, addrlen) == -1) {
		perror("connect");
		return -1;
	}
	return sock;
}

int send_fd(int sock, int fd) {
	struct cmsghdr* cmsg;
	int* fdptr;
	char buffer[] = "In class blah. Psst. I gave you an fd";
	struct iovec io = {
		.iov_base = buffer,
		.iov_len = sizeof(buffer)
	};
	union {
		char buf[CMSG_SPACE(sizeof(fd))];
		struct cmsghdr align;
	} u;
	struct msghdr msg = { 
		.msg_iov = &io,
		.msg_iovlen = 1,
		.msg_control = u.buf,
		.msg_controllen = sizeof(u.buf)
	};
	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_SOCKET; /* makes no sense, but manpage says so */
	cmsg->cmsg_type = SCM_RIGHTS; /* for passing fds */
	cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
	fdptr = (int*)CMSG_DATA(cmsg);
	memcpy(fdptr, &fd, sizeof(int));
	if (sendmsg(sock, &msg, MSG_NOSIGNAL) == -1) {
		perror("sendmsg");
		return -1;
	}
	return 0;
}


