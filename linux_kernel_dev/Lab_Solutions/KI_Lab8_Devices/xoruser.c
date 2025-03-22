#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include "xorshared.h"

#define BUF_MAX XOR_BUF_SZ

void convert_buffer(int fd, char* buffer, int len);
void* map_path(char* path, size_t* out_size);

#define DEV_FILE_NAME   "mydev"

int main(int argc, char* argv[]) {
    int fd;
    int err;
    dev_t dev;
    int len;
    char buffer[BUF_MAX];
    unsigned char xor_val;
    void* module_data;
    size_t module_len;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s string xorval\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    module_data = map_path("xordriver.ko", &module_len);
    if (module_data == NULL) {
        fprintf(stderr, "Failed to load LKM from disk\n");
        exit(EXIT_FAILURE);
    }
    err = syscall(__NR_init_module, module_data, module_len, "");
    if (err == -1) {
        munmap(module_data, module_len);
        perror("init_module");
        exit(EXIT_FAILURE);
    }
    if (munmap(module_data, module_len) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    xor_val = atoi(argv[2]);
    dev = makedev(XOR_MAJ, XOR_MIN);
    err = mknod(DEV_FILE_NAME, S_IFCHR , dev);
    if (err == -1) {
        if (errno != EEXIST) {
            perror("mknod");
            exit(EXIT_FAILURE);
        }
    }

    fd = open(DEV_FILE_NAME, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fd, KEYCMD, xor_val) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }

    len = snprintf(buffer, BUF_MAX-1, "%s", argv[1]);
    if (len < 0) {
        fprintf(stderr, "failed to sprintf\n");
        exit(EXIT_FAILURE);
    }

    convert_buffer(fd, buffer, len);
    convert_buffer(fd, buffer, len);
    close(fd);

    if (syscall(__NR_delete_module, "xordriver", 0) == -1) {
        perror("delete_module");
        exit(EXIT_FAILURE);
    }

    if (unlink(DEV_FILE_NAME) == -1) {
        perror("delete_module");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

void convert_buffer(int fd, char* buffer, int len) {
    if (write(fd, buffer, len) != len) {
        fprintf(stderr, "failed to write everything\n");
        exit(EXIT_FAILURE);
    }
    lseek(fd, 0, SEEK_SET);
    memset(buffer, 0, BUF_MAX);
    if (read(fd, buffer, len) != len) {
        fprintf(stderr, "failed to read everything\n");
        exit(EXIT_FAILURE);
    }
    lseek(fd, 0, SEEK_SET);
    printf("XOR yields: %s\n", buffer);
    return;
}

void* map_path(char* path, size_t* out_size) {
    int fd;
    struct stat st;
    void* data = NULL;

    fd = open(path, O_APPEND | O_RDWR);
    if (fd < 0) {
        perror("open");
        return NULL;
    }

    if (fstat(fd, &st) == -1) {
        perror("stat");
        close(fd);
        return NULL;
    }

    data = mmap(0, st.st_size,
        PROT_READ | PROT_WRITE, MAP_SHARED,
        fd, 0);
    close(fd);
    if (data == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    *out_size = st.st_size;
    return data;
}
