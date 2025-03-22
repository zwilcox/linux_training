#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

/**
 * @brief - Sets the soft limit on the given resource
 */
int set_soft_limit(int resource, int limit)
{
    // Get the existing values
    struct rlimit rlim = {0};
    if (-1 == getrlimit(resource, &rlim))
    {
        perror("getrlimit before set");
        return -1;
    }

    // Set the new soft max
    rlim.rlim_cur = limit;
    if (-1 == setrlimit(resource, &rlim))
    {
        perror("setrlimit");
        return -1;
    }

    return 0;
}

/**
 * @brief - Handles getting the softlimit from 
 */
int get_soft_limit(int resource)
{
    struct rlimit rlim = {0};
    if (-1 == getrlimit(resource, &rlim))
    {
        perror("getrlimit");
        return -1;
    }

    return rlim.rlim_cur;
}

/**
 * @brief - Creates num empty files in a directory called files
 */
void create_files(int num)
{
    int i = 0;
    char filename[128] = {0};
    FILE *f = NULL;
    FILE **file_handles = (FILE **)calloc(num, sizeof(FILE*));
    
    printf("Creating files...\n");

    for (; i < num; i++)
    {
        sprintf(filename, "./files/file_%d", i);
        f = fopen(filename, "wb");
        if (NULL == f)
        {
            printf("Failed on: %d\n", i);
            perror("fopen");
            goto CLEANUP;
        }
        else
        {
            file_handles[i] = f;
        }
    }

// Make sure to clean up file handles between calls of this or else it will
// throw off your metrics
CLEANUP:
    for(i = 0; i < num; i++)
    {
        if (file_handles[i] != NULL)
        {
            fclose(file_handles[i]);
            file_handles[i] = NULL;
        }
    }
    free(file_handles);
}

int main (int argc, char **argv)
{
    int max_files = get_soft_limit(RLIMIT_NOFILE);

    // Make a directory so we don't clutter things
    mkdir("files", S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH);

    // Get the soft max file limit for our process
    printf("Soft max: %d\n", max_files);
    create_files(max_files);

    // Double the limit and try again
    max_files *= 2;
    printf("\nDoubling limit. New limit is %d\n", max_files);
    set_soft_limit(RLIMIT_NOFILE, max_files);

    create_files(max_files);

    // Clean things up when we are done
    system("rm -rf files");
    return 0;
}
