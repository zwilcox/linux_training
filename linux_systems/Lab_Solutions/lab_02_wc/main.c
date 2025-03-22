#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int print_wc_info(char* filepath);
int static count_words(char* buffer);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filepath\n", argv[0]);
        return EXIT_FAILURE;
    }

    return print_wc_info(argv[1]);
}

int print_wc_info(char* filepath) {
    FILE* fp;
    char* line_buffer;
    size_t chars_read;
    int lines = 0;
    int words = 0;
    int chars = 0;

    /* open the file for reading */
    fp = fopen(filepath, "rb");
    if (fp == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    line_buffer = NULL;
    while (1) {
        chars_read = getline(&line_buffer, &chars_read, fp);
        if (chars_read == -1) {
            if (feof(fp)) {
                break;
            }
            perror("getline");
            continue;
        }
        lines++;
        chars += chars_read;
        words += count_words(line_buffer);
    }

    free(line_buffer);

    if (fclose(fp) == EOF) {
        perror("fclose");
    }

    printf("%d %d %d %s\n", lines, words, chars, filepath);

    return EXIT_SUCCESS;
}

int count_words(char* buffer) {
    int words = 0;
    int word_found = 0;
    while (*buffer != '\0') {
        while (*buffer != '\0' && isspace(*buffer)) {
            buffer++;
        }
        word_found = 0;
        while (*buffer != '\0' && !isspace(*buffer)) {
            buffer++;
            word_found = 1;
        }
        if (word_found == 1) {
            words++;
        }
    }
    return words;
}

