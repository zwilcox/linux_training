#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

char secret[0x100];

int main() {
    printf("Enter a secret:\n\t");
    if (fgets(secret, sizeof(secret) - 1, stdin) == NULL) {
        perror("fgets");
        return EXIT_FAILURE;
    }

	getchar();

    printf("Secret is %s\n", secret);

    return EXIT_SUCCESS;
}
