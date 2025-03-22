#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define WRAP_FUNC   write
#define FLAG_LEN    34
#define SENTINEL_CHAR   'A'

#define NUM_DESCRIPTORS 6
#define PRINTABLE_MAX   126
#define ALPHA_CAP_BEGIN SENTINEL_CHAR

char charset[] = "abcdefghijklmnopqrstuvwxyzBCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";

/*ssize_t my_write(int fd, const void* buf, size_t count) {
    if (count == 2 && ((char*)buf)[0] == 'A') {
        printf("%c", ((char*)buf)[1]);
    }
    //printf("%.*s", 2, (char*)buf);
    return write(fd, buf, count);
}*/

void unobfuscate_flag(char* flag) {
    int i;
    int j;
    int k;
    srand(0x41435450);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < FLAG_LEN; j += sizeof(k)) {
            k = rand();
           *((int*)(&flag[j])) ^= k;
        }
    }
    return;
}

void write_all(int* fds, char* flag) {
    int i;
    int j;
    int chosen_fd;
    srand(time(NULL));
    char str[2];

    for (i = 0; i < FLAG_LEN; i++) {
        chosen_fd = rand() % NUM_DESCRIPTORS;
        for (j = 0; j < NUM_DESCRIPTORS; j++) {
            if (j == chosen_fd) {
                str[0] = SENTINEL_CHAR;
                str[1] = flag[i];
                if (WRAP_FUNC(fds[j], str, 2) == -1) {
                    perror("oh noes! write failed");
                    exit(-1);
                }
            }
            else {
                str[0] = charset[rand() % (sizeof(charset)-1)];
                str[1] = charset[rand() % (sizeof(charset)-1)];
                if (WRAP_FUNC(fds[j], str, 2) == -1) {
                    perror("oh noes! write failed");
                    exit(-1);
                }
            }
        }
    }
}

void create_fds(int* fds) {
    int pipefd[2];
    fds[0] = memfd_create("", MFD_CLOEXEC);
    if (fds[0] == -1) {
        perror("memfd_create");
        exit(-1);
    }
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(-1);
    }
    fds[1] = pipefd[1];
    fds[2] = memfd_create("", MFD_CLOEXEC);
    if (fds[3] == -1) {
        perror("memfd_create");
        exit(-1);
    }
    fds[3] = memfd_create("", MFD_CLOEXEC);
    if (fds[3] == -1) {
        perror("memfd_create");
        exit(-1);
    }
    fds[4] = open("/tmp/maifile", O_CREAT | O_RDWR, 0777);
    if (fds[4] == -1) {
        perror("open the sandbox");
        exit(-1);
    }
    fds[5] = open("/dev/null", O_WRONLY);
    if (fds[5] == -1) {
        perror("open the void");
        exit(-1);
    }
    return;
}

int main() {
    //char flag[] = "flag{TWO_WRITES_DONT_MAKE_A_WRONG}\0\0\0\0\0\0\0\0";
  	char flag[] = {0x97, 0xd7, 0x07, 0x0d, 0xd0, 0x03, 0x55, 0x43, 0x78, 0x50, 0x18, 0x1f, \
	  0x05, 0x8f, 0x13, 0x7d, 0x56, 0x59, 0xf5, 0x23, 0x05, 0xf4, 0x33, 0x0c, \
	  0x76, 0xb5, 0xb2, 0x7e, 0x97, 0xa4, 0x84, 0x36, 0xae, 0x51};
    int fds[NUM_DESCRIPTORS];
    create_fds(fds);
    unobfuscate_flag(flag);
    write_all(fds, flag);
    return 0;
}
