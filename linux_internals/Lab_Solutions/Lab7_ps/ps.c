#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief - The cmdline file will have the arguments in null terminated strings
 * In light of that you need to read the whole file and then replace the null
 * bytes with spaces to make it look like a normally formatted string
 *
 * @param str[in, out] - The string of data with possible null bytes
 * @param bytes_read[in] - The length of the string in bytes
 */
void replace_char(char *str, size_t len, char token, char replace)
{
    size_t i = 0;
    for(; i < len; i++)
    {
        if (str[i] == token)
        {
            str[i] = replace;
        }
    }
}

/**
 * @brief - Read in a file and strip out any unwanted characters
 *
 * @param path[in] - The path to the file
 * @param output[out] - The buffer that will hold the file data
 * @return - The number of bytes read
 */
size_t get_cmdline(char *path, char *output)
{
    char cmdline[1024] = { 0 };
    size_t bytes_read = 0;
    FILE *fp = NULL;

    fp = fopen(path, "rb");
    if (NULL == fp)
    {
        return bytes_read;
    }

    bytes_read = fread(cmdline, sizeof(char), 1024, fp);
    if (bytes_read == 0)
    {
        goto CLEANUP;
    }
    else
    {
        memcpy(output, cmdline, bytes_read);
        replace_char(output, bytes_read, '\0', ' ');
        replace_char(output, bytes_read, '\n', '\0');
        goto CLEANUP;
    }

CLEANUP:
    fclose(fp);
    return bytes_read;

}

/**
 * @brief - Checks the cmdline file in /proc/[pid]. If that file is empty it
 * will check comm in /proc/[pid] to see if a name is present there - can also `strings /proc/[pid]/cmdline`
 *
 * @param pid[in] - The pid we are searching
 * @param output[out] - The buffer to hold the output data
 * @return - The number of bytes in the name/cmdline
 */
size_t get_process_name(char *pid, char *output)
{
    char path[1024] = { 0 };
    size_t bytes_read = 0;

    // Check if cmd line file exists
    snprintf(path, sizeof(path), "/proc/%s/cmdline", pid);
    if (access(path, F_OK) == -1)
    {
        return bytes_read;
    }

    // If get_cmdline returns a non-zero number then we are done
    bytes_read = get_cmdline(path, output);
    if (0 != bytes_read)
    {
        return bytes_read;
    }

    // else check the comm file for the processes name
    snprintf(path, sizeof(path), "/proc/%s/comm", pid);
    if (access(path, F_OK) == -1)
    {
        return bytes_read;
    }

    return get_cmdline(path, output);
}

/**
 * @brief - Does a walk of /proc and searches all of the /proc/[pid]
 * directories for their name
 */
void print_processes()
{
    DIR *dir = NULL;
    struct dirent *entry = { 0 };
    char cmdline[1024] = { 0 };

    if (!(dir = opendir("/proc")))
    {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            if (get_process_name(entry->d_name, cmdline))
            {
                printf("%s - %s\n", entry->d_name, cmdline);
                memset(cmdline, 0, sizeof(cmdline));
            }
        }
    }
    closedir(dir);
}


// Step 1: Print out all the processes and their command lines
int main(int argc, char **argv)
{
    print_processes();
    return 0;
}
