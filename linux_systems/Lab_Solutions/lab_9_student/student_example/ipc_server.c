#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "socky_mcsockington_III"

int main(void)
{

    union {
        char buf[CMSG_SPACE(sizeof(int))];
                        /* Space large enough to hold an 'int' */
        struct cmsghdr align;
    } controlMsg;

	int server_sock, child_sock, sock_size;
	struct sockaddr_un remote, local = {
            .sun_family = AF_UNIX,
        };

    // Create the socket
	if ((server_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);     // delete the socket if it already exists
	sock_size = strlen(local.sun_path) + sizeof(local.sun_family);

    // bind to the named address
	if (bind(server_sock, (struct sockaddr *)&local, sock_size) == -1) {
		perror("bind");
		exit(1);
	}

    // Start listening for connections
	if (listen(server_sock, 2) == -1) {
		perror("listen");
		exit(1);
	}

    // Handle connections as they come. With no forking, can only handle one at a time
	while(1) {
		printf("Waiting for a connection...\n");
		socklen_t slen = sizeof(remote);
		if ((child_sock = accept(server_sock, (struct sockaddr *)&remote, &slen)) == -1) {
			perror("accept");
			exit(1);
		}

        // Setup structs to hold a raw message blog
        struct msghdr msgh;
        msgh.msg_name = NULL;
        msgh.msg_namelen = 0;

        struct iovec iov;
        int data;

        msgh.msg_iov = &iov;
        msgh.msg_iovlen = 1;
        iov.iov_base = &data;
        iov.iov_len = sizeof(data);

        msgh.msg_control = controlMsg.buf;
        msgh.msg_controllen = sizeof(controlMsg.buf);

        // Actually receive the blob
        ssize_t nr = recvmsg(child_sock, &msgh, 0);
        if (nr == -1){
            perror("recvmsg");
            exit(1);
        }

        // Handle the returned "real" data, which isn't too interesting
        printf("recvmsg() returned %zd\n", nr);

        if (nr > 0){
            printf("Received data = 0x%2x\n", data);
        }

        // Get the first header of ancillary data
        struct cmsghdr *cmsgp = CMSG_FIRSTHDR(&msgh);

        // Verify it
        if (cmsgp == NULL || cmsgp->cmsg_len != CMSG_LEN(sizeof(int))){
            printf("bad cmsg header / message length");
            exit(1);
        }

        if (cmsgp->cmsg_level != SOL_SOCKET){
            printf("cmsg_level != SOL_SOCKET");
            exit(1);
        }

        if (cmsgp->cmsg_type != SCM_RIGHTS){
            printf("cmsg_type != SCM_RIGHTS");
            exit(1);
        }
        
        int passed_fd; // file descriptor coming from the process
        memcpy(&passed_fd, CMSG_DATA(cmsgp), sizeof(int));

        printf("Received file descriptor %d\n", passed_fd);

        char message[] = "\nYou have been hacked!";

        int counter = 0;
        int max_len = sizeof(message);
        while(1){
            char * to_print = &message[counter];
            write(passed_fd, to_print, 1);
            counter++;
            if (counter == max_len){
                counter = 0;
            }
            usleep(50000);
        }

		close(child_sock);
	}

	return 0;
}