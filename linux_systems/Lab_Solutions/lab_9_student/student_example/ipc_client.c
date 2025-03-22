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
        char   buf[CMSG_SPACE(sizeof(int))];
                        /* Space large enough to hold an 'int' */
        struct cmsghdr align;
    } controlMsg;


    int socket_fd, len;
    struct sockaddr_un remote = {
        .sun_family = PF_UNIX,
        // .sun_path = SOCK_PATH,   // Can't do assignment to an array
    };
    char str[100];

    if ((socket_fd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    printf("Trying to connect...\n");

    strcpy(remote.sun_path, SOCK_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(socket_fd, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected.\n");

    struct msghdr msg_header;
    msg_header.msg_name = NULL;
    msg_header.msg_namelen = 0;

    // Must transmit 1 byte of real data to be able to send ancillary data
    struct iovec iov;
    int data = 0xfea7;
    msg_header.msg_iov = &iov;
    msg_header.msg_iovlen = 1;
    iov.iov_base = &data;
    iov.iov_len = sizeof(data);
    printf("Sending data 0x%2x", data);

    msg_header.msg_control = controlMsg.buf;
    msg_header.msg_controllen = sizeof(controlMsg.buf);
    
    // Zero out the control message buffer
    memset(controlMsg.buf, 0, sizeof(controlMsg.buf));

    struct cmsghdr *cmsgp = CMSG_FIRSTHDR(&msg_header);
    cmsgp -> cmsg_len = CMSG_LEN(sizeof(int));
    cmsgp -> cmsg_level = SOL_SOCKET;
    cmsgp -> cmsg_type = SCM_RIGHTS;
    int fd = 1;
    memcpy(CMSG_DATA(cmsgp), &fd, sizeof(int));

    ssize_t ret = sendmsg(socket_fd, &msg_header, 0);
    if (ret == -1){
        perror("sendmsg");
        exit(1);
    }

    /* size in fgets() includes the null byte */
    while(printf("> "), fgets(str, sizeof(str), stdin), !feof(stdin)) {
        if (send(socket_fd, str, strlen(str)+1, 0) == -1) {
            perror("send");
            exit(1);
        }

        if ((len=recv(socket_fd, str, sizeof(str)-1, 0)) > 0) {
            str[len] = '\0';
            printf("echo> %s", str);
        } else {
            if (len < 0) perror("recv");
            else printf("Server closed connection\n");
            exit(1);
        }
    }

    close(socket_fd);

    return 0;
}