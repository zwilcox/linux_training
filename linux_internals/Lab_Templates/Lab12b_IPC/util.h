#ifndef UTIL_H
#define UTIL_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>

// Add this to enable debugging output
#define DEBUG

void *copy_buf(void *buf, ssize_t len);
uint8_t *write_file(char *pathname, void *buf, ssize_t len);
void render_ascii(uint8_t *data, ssize_t len);
void hexdump(void *data, ssize_t len);
uint8_t *read_file(char *pathname, ssize_t *file_size);
void _debug_log(const char *file, const char *func, int line, const char *msg, ...);
void generate_name(char *str, int name_len, int start_idx);
void strip_trailing_newline(char *str);

#ifdef DEBUG
#define debug_log(...) _debug_log(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

void _debug_log(const char *file, const char *func, int line, const char *fmt, ...)
{

    fprintf(stdout, "%s|%s():%d - ", file, func, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fflush(stdout);
}
#else
#define debug_log(...)
#endif

void *copy_buf(void *buf, ssize_t len)
{
    if (NULL == buf || len == 0)
    {
        return NULL;
    }

    void *copy = malloc(len);
    memcpy(copy, buf, len);
    return copy;
}

uint8_t *write_file(char *pathname, void *buf, ssize_t len)
{
    uint8_t *data = (uint8_t *)buf;
    int fd = -1;

    // If it exists try and open it for reading
    fd = open(pathname, O_WRONLY | O_CREAT);
    if (-1 == fd)
    {
        return NULL;
    }

    // Read the data from the file
    if (len != write(fd, buf, len))
    {
        goto CLEANUP;
    }

    // Close handle to the file and return
    close(fd);
    return data;

CLEANUP:
    if (NULL != data)
    {
        free(data);
        data = NULL;
    }
    if (-1 != fd)
    {
        close(fd);
        fd = -1;
    }
    return NULL;
}

uint8_t *read_file(char *pathname, ssize_t *file_size)
{
    uint8_t *data = NULL;
    int fd = -1;

    // Clear our the file size
    if (NULL != file_size)
    {
        *file_size = 0;
    }

    // Check if file exists
    if (-1 == access(pathname, F_OK))
    {
        return NULL;
    }

    // If it exists try and open it for reading
    fd = open(pathname, O_RDONLY);
    if (-1 == fd)
    {
        return NULL;
    }

    // If we opened it try and get the file size
    struct stat s = {0};
    if (-1 == stat(pathname, &s))
    {
        goto CLEANUP;
    }

    // Allocate a buffer for the data
    data = (uint8_t *)malloc(s.st_size);
    if (NULL == data)
    {
        perror("malloc");
        goto CLEANUP;
    }

    // Read the data from the file
    if (s.st_size != read(fd, data, s.st_size))
    {
        goto CLEANUP;
    }

    // If they passed in a file_size pointer then fill it
    if (NULL != file_size)
    {
        *file_size = s.st_size;
    }

    // Close handle to the file and return
    close(fd);
    return data;

CLEANUP:
    if (NULL != data)
    {
        free(data);
        data = NULL;
    }
    if (-1 != fd)
    {
        close(fd);
        fd = -1;
    }
    return NULL;
}

void render_ascii(uint8_t *data, ssize_t len)
{
    char c[2] = {0};
    uint8_t *idx = (uint8_t *)data;
    for (int i = 0; i < len; i++)
    {
        sprintf(c, "%c", idx[i]);
        if (c[0] < 32 || c[0] > 126)
        {
            printf(".");
        }
        else
        {
            printf("%s", c);
        }
    }
}

void hexdump(void *data, ssize_t len)
{
    uint8_t *idx = (uint8_t *)data;
    printf("       0  1  2  3  4  5  6  7    8  9  a  b  c  d  e  f\n");

    printf("0000  %02x ", idx[0]);
    for (int i = 1; i < len; i++)
    {
        if (((i % 8) == 0) && ((i % 16) != 0))
        {
            printf("| %02x ", idx[i]);
            continue;
        }
        if ((i % 16) == 0)
        {
            printf(" ");
            render_ascii(&idx[i - 16], 16);

            printf("\n%04x  %02x ", i, idx[i]);
            continue;
        }

        printf("%02x ", idx[i]);
    }
    printf("\n");
}


/**
 * @brief - Strips new line characters
 */
void strip_trailing_newline(char *str)
{
    char *index = str;
    while (*index != '\0')
    {
        index++;
    }
    index--;
    if (*index == '\n')
    {
        *index = '\0';
    }
}

/**
 * @brief - Generates a random name to be used as the socket name
 *
 * @param name[in/out] - Pass in a buffer that the random name will be placed in
 * @param name_len[in] - The size of the name passed int
 * @param start_idx[idx] - An index into the string to start inserting random
 *                         bytes. This is used if you want a standard prefix at
 *                         the beginning of your string
 *
 */
void generate_name(char *name, int name_len, int start_idx)
{
    if (NULL == name || 0 == name_len || start_idx < 0)
    {
        debug_log("Invalid parameters to");
        return;
    }

    int urandom_fd = open("/dev/urandom", O_RDONLY);
    if (0 == urandom_fd)
    {
        debug_log("Failed to open /dev/urandom");
    }
    unsigned int seed = 0;
    read(urandom_fd, &seed, sizeof(seed));
    int opt = 0;
    srand(seed);

    // Make sure to clear the name when it is generated
    for (int i = start_idx; i < name_len; i++)
    {
        opt = rand() % 3;
        if (0 == opt)
        {
            name[i] = (rand() % 26) + 0x61;
        }
        else if (1 == opt)
        {
            name[i] = (rand() % 26) + 0x41;
        }
        else
        {
            name[i] = (rand() % 10) + 0x30;
        }
    }
    name[name_len - 1] = '\0';
}
#endif //UTIL_H
