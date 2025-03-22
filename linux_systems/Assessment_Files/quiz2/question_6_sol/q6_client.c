#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netdb.h>



void send_flag() {
    char *host = "localhost";
    char *port = "8080";
    char *message = "443\n";
    struct addrinfo hints = { 0 };
    struct addrinfo* ai_result;
    struct addrinfo* ai;
    int ret;
    int sock;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    ret = getaddrinfo(host, port, &hints, &ai_result);
    if (ret != 0) {
        fprintf(stderr, "getaddrunfo: %s\n", gai_strerror(ret));
        return;
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
        break;
    }
    freeaddrinfo(ai_result);

    if (ai == NULL) {
        fprintf(stderr, "Could not find a suitable address\n");
        return;
    }

    send(sock, message, strlen(message), 0);
    close(sock);
}

void bob() {
	static char arr[0x80000];
	arr[423] = 5;
	return;
}

int main() {
    raise(SIGSTOP);

    send_flag();

    return 0;
}
