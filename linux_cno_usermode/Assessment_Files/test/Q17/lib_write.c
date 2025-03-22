#define _GNU_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <dlfcn.h>

ssize_t write(int fd, const void* buf, size_t count) {
    if (((char*)buf)[0] == 'A') {
        printf("%c", ((char*)buf)[1]);
    }
    return count;
}
