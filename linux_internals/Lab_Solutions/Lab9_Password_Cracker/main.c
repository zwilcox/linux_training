#include <stdbool.h>
#include <stdio.h>
#include <crypt.h>
#include <string.h>
#include <stdlib.h>

// array containing the alphabet shifted by 1 letter
char translate[] = {'b',
                    'c',
                    'd',
                    'e',
                    'f',
                    'g',
                    'h',
                    'i',
                    'j',
                    'k',
                    'l',
                    'm',
                    'n',
                    'o',
                    'p',
                    'q',
                    'r',
                    's',
                    't',
                    'u',
                    'v',
                    'w',
                    'x',
                    'y',
                    'z',
                    'a'};

// Return value indicates a carry out
bool inc_char(char *c)
{
    *c = translate[(int)*c - 'a'];
    return *c == 'a';
}

// Takes as input a pointer to a character sequence and will
// return the next permutation of the sequence. For example:
//   'aaaa' -> 'aaab'
//   'azzz' -> 'baaa'
//   'zzzz' -> 'aaaaa'
//
// NOTE: This function is dumb and WILL  write beyond the end
// of *seq. Make sure seq points to a buffer large enough for
// the maximum desired sequence length.
char *next_seq(char *seq)
{
    size_t len = strlen(seq);
    bool carry_out = false;

    // start at end of sequence and move towards the beginning
    for (int i = len - 1; i >= 0; i--)
    {
        carry_out = inc_char(&seq[i]);
        if (!carry_out)
        {
            // no carry out means a normal increment occurred
            // ie. 'b'->'c' so we are done and can break
            break;
        }
        // a carry out means we incremented from 'z' to 'a'
        // and the character seq[i-1] must also be incremented
    }
    if (carry_out)
    {
        // means we incremented all 'z's ie. 'zzz' -> 'aaa'
        // and must add another character to the sequence.
        seq[len] = 'a';
        seq[len + 1] = 0;
    }
    return seq;
}

// Returns true if haystack begins with needle, false otherwise
bool begins_with(char *haystack, char *needle)
{
    size_t len_h = strlen(haystack);
    size_t len_n = strlen(needle);
    return len_h < len_n ? false : strncmp(haystack, needle, len_n) == 0;
}

// Loops attempting to crack the password for the user provided as an argument
// The crypt function is slow by design which means this won't be fast.
// Best to set the desired user to have a simple (short) password for this lab
int main(int argc, char *argv[])
{
    char salt[50] = {0};
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Tries up to a 16 character password
    char password[] = {'a', 'a', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if (argc < 2)
    {
        printf("Usage: %s USERNAME\n", argv[0]);
        return 0;
    }

    // Open the shadow file to get the hash and salt
    FILE *fd = fopen("/etc/shadow", "rb");
    if (fd == NULL)
    {
        perror("fopen");
        printf("Did you run the program as root?\n");
        return 0;
    }

    // Read through the shadow file until we find the user specified in argv[1]
    while ((read = getline(&line, &len, fd)) != -1)
    {
        if (begins_with(line, argv[1]))
        {
            // printf("Found user!!\n");
            // printf("%s", line);
            break;
        }
        line = NULL;
    }
    if (!line)
    {
        // Didn't find user, bail
        printf("Failed to find user: %s\n", argv[1]);
        return 0;
    }

    // We found the user's entry in /etc/shadow
    char *begin = strchr(line, '$');
    char *end = strrchr(line, '$');
    if (!begin || !end)
    {
        perror("begin/end");
        return 0;
    }

    printf("begin: %s\n", begin);
    printf("end: %s\n", end);
    
    // get the salt from the /etc/shadow entry
    memcpy(salt, begin, end - begin);
    printf("Salt: %s\n", salt);

    // Loop over passwords and try all the permutations
    for (int i = 0;; i++)
    {
	    // Print out where we are every 1000 iterations
        if ((i % 1000) == 0)
        {
            printf("attempting: %s\n", password);
        }

	    // Try the crypt function with our current permutation and salt
        char *attempt = crypt(password, salt);
	    // printf("attempt: %s\n", attempt);

	    // If it matches the hash that was in the /etc/shadow file then we win!
        if (begins_with(begin, attempt))
        {
            printf("CRACKED!!! password is: %s\n", password);
            break;
        }
        next_seq(password);
    }

    if (line)
    {
        free(line);
        line = NULL;
    }
    fclose(fd);

    return 0;
}
