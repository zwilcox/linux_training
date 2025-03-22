// Hide Me Option 1
//
// Checks if a connection is made to an abstract unix domain
// socket named "\0password\0" then responds with the flag
//

// flag must start with "FLAG{" and end with "}"
// flag must only consist of alphanumeric characters and underscores
#define FLAG	"FLAG{default_flag}"

#define SUNPATH	"\0password"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
	(void)argc; (void)argv;

	int un = -1;
	int cli = -1;
	struct sockaddr_un saddr = {0};

	un = socket(PF_UNIX, SOCK_STREAM, 0);
	if (un == -1) {
		perror("socket");
		goto END;
	}

	saddr.sun_family = AF_UNIX;
	memcpy(saddr.sun_path, SUNPATH, sizeof(SUNPATH));

	if (bind(un, (const struct sockaddr *) &saddr, sizeof(saddr.sun_family) + sizeof(SUNPATH))) {
		perror("bind");
		goto END;
	}

	if (listen(un, 0)) {
		perror("listen");
		goto END;
	}

	cli = accept(un, NULL, NULL);

	if (-1 == write(cli, FLAG, sizeof(FLAG))) {
		perror("write");
		goto END;
	}

END:
	if (cli != -1) {
		close(cli);
	}
	if (un != -1) {
		close(un);
	}
	return 0;
}
