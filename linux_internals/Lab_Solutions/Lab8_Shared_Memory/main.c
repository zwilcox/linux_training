#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
    key_t key = 0;
    int shmid = 0;
    char *str = NULL;

    if (argc != 2)
    {
        goto USAGE;
    }

    if (access("./shared_memory_file", F_OK) == -1)
    {
        FILE *f = fopen("shared_memory_file", "w");
        fclose(f);
    }

    if (strncmp(argv[1], "read", strlen("read")) == 0)
    {
        key = ftok("shared_memory_file", 14689);
        if (-1 == key)
        {
            perror("ftok failed");
            goto CLEANUP;
        }

        shmid = shmget(key, BUF_SIZE, 0666 | IPC_CREAT);
        if (-1 == shmid)
        {
            perror("shmget failed");
            goto CLEANUP;
        }

        str = (char*) shmat(shmid, (void*)0, 0);
        if ((void*)-1 == str)
        {
            perror("shmat failed");
            goto CLEANUP;
        }

        printf("Reader: %s\n", str);

    }
    else if (strncmp(argv[1], "write", 5) == 0)
    {
        char wait_char = '\0';
        key = ftok("shared_memory_file", 14689);
        if (-1 == key)
        {
            perror("ftok failed");
            goto CLEANUP;
        }

        shmid = shmget(key, BUF_SIZE, 0666 | IPC_CREAT);
        if (-1 == shmid)
        {
            perror("shmget failed");
            goto CLEANUP;
        }

        str = (char*) shmat(shmid, (void*)0, 0);
        if ((void*)-1 == str)
        {
            perror("shmat failed");
            goto CLEANUP;
        }

        printf("Enter some text and press enter\n");
        fgets(str, BUF_SIZE, stdin);

        printf("Write: %s\n", str);
        //fgets(str, BUF_SIZE, stdin);
        getchar();
    }

CLEANUP:
    if ((void*)-1 != str)
    {
        shmdt(str);
    }

    if (-1 != shmid)
    {
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
    
USAGE:
    printf("USAGE: %s <read/write>\n", argv[0]);
    return 0;
}
