/**
 * The goal of this lab is to allow the students to use their RE skills as well
 * as many of the skills they have learned in this class in order to exploit
 * this binary.
 * 
 * The way to exploit it is you must first be able to connect to the UNIX
 * socket. The UNIX server socke tis an abstract socket and it only accepts
 * a client with a specific randomly generated name. However there is a fifo
 * that you can write to that is listening for a name change command. If that
 * fifo is written to it will update the global client_name variable and then 
 * the server can accept any name specified.
 * 
 * Once connected to the server there are 3 commands login, stop and execute.
 * Login only exists as a red herring and doesn't really do anything. The stop
 * command will terminate the server. And the exeucte command is where the
 * vulnerability exists.
 * 
 * The server is set to run the binary pointed to by the path in a shared
 * memory segment. The idea here is that the path is hard coded in the binary
 * but it mapped into the shared memory segment. The student can get access to
 * that shared memory and then overwrite the path in order to change what file
 * is being run. 
 * 
 * If the task seems to hard we could give them bits and pieces. For example
 * maybe we can give them the code to connect to an abstract socket. Or maybe
 * we give them the structure definition that is being used to store the
 * username, password and path. Or maybe just anything else that might be
 * helpful
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include "main.h"

#define NAME_LEN 20
#define RECV_BUF_SIZE 4096
char name[NAME_LEN] = {0};
char client_name[NAME_LEN] = {0};

void populate_pipe_with_secret_message();
void *listen_for_connections(void *addr);
void *listen_for_name_config(void *param);
int send_msg(int client, const char *msg, ...);
int recv_msg(int client, uint8_t *data, size_t bufsize);

#define config_size sizeof(struct config)
#define key 0x7bbbccaa

struct config *conf = NULL;

/**
 * @brief -  Maps in the config struct into a shared memory block
 */
int create_shared_memory(struct config **config)
{
    // Create our shared memory segment if it is not already created
    int shmid = shmget(key, config_size, 0666 | IPC_CREAT);
    if (-1 == shmid)
    {
        perror("shmget failed");
        return -1;
    }

    // Get a pointer to our shared memory block
    *config = shmat(shmid, (void *)0, 0);
    debug_log("Shared memory address: %p\n", *config);
    debug_log("Key: %d\n", key);
    if ((void *)-1 == *config)
    {
        perror("shmat failed");
        return -1;
    }
    memset(*config, 0, config_size);
    return 0;
}

void delete_shared_memory(struct config *config)
{
    if (NULL != config)
    {
        debug_log("Detaching shared memory\n");
        shmdt(config);
    }

    int shmid = shmget(key, config_size, 0666);
    if (-1 == shmid)
    {
        perror("shmget failed");
    }
    else
    {
        debug_log("IPC_RMID\n");
        shmctl(shmid, IPC_RMID, NULL);
    }
}

int prompt_for_command(struct config *config, int client)
{
    char cmd[1024] = {0};

    if(-1 == send_msg(client, "Enter a command (login, stop, execute): "))
    {
        return -1;    
    }
    recv_msg(client, (uint8_t*)cmd, 1024);

    if (strncmp(cmd, "login", strlen("login")) == 0)
    {
        debug_log("config->cmd set to login\n");
        config->cmd = login;
    }
    else if (strncmp(cmd, "stop", strlen("stop")) == 0)
    {
        debug_log("config->cmd set to stop\n");
        config->cmd = stop;
    }
    else if (strncmp(cmd, "execute", strlen("execute")) == 0)
    {
        debug_log("config->cmd set to execute\n");
        config->cmd = execute;
    }
    return 0;
}

/**
 *  @brief - The thread function that handles communicating with a client over
 *  a unix domain socket.
 *
 *  @param _client[in] - A pointer to the client descriptor
 */
void *handle_client(void *_client)
{
    int client = *(int *)_client;
    char *my_argv[3] = {0};

    if (NULL == _client)
    {
        debug_log("Thread recieved null client\n");
        return NULL;
    }

    

    if (-1 == send_msg(client, "Welcome to the mainframe\n"))
    {
        perror("send_msg");
        close(client);
        return NULL;
    }

    while (1)
    {
        if (-1 == prompt_for_command(conf, client))
        {
            close(client);
            return NULL;
        }
        switch (conf->cmd)
        {
        case stop:
            // Do any cleanup that I need to do
            delete_shared_memory(conf);
            close(client);
            exit(0);
        case login:
            if ('\0' == conf->username[0])
            {
                if (-1 == send_msg(client, "Username: "))
                {
                    close(client);
                    return NULL;
                }

                if (-1 == recv_msg(client, (uint8_t*)conf->username, USERNAME_SIZE))
                {
                    close(client);
                    return NULL;
                }
                strip_trailing_newline(conf->username);
            }
            else
            {
                if (-1 == send_msg(client, "Logging in as '%s'\n", conf->username))
                {
                    close(client);
                    return NULL;
                }
            }

            if ('\0' == conf->password[0])
            {
                if (-1 == send_msg(client, "Password: "))
                {
                    close(client);
                    return NULL;
                }
                if (-1 == recv_msg(client, (uint8_t*)conf->password, PASSWORD_SIZE))
                {
                    close(client);
                    return NULL;
                }
            }
            break;
        case execute:
            my_argv[0] = conf->data_path;
            my_argv[1] = "secret";
            hexdump(conf, config_size);
            pid_t pid = fork();
            int status = 0;
            if (0 == pid)
            {
                // Child
                execve(conf->data_path, my_argv, NULL);
                exit(0);
            }
            else
            {
                // Parent
                while ((pid = wait(&status)) > 0)
                {
                }
                debug_log("Done waiting\n");
            }

            break;
        default:
            debug_log("%d is an invalid command. Valid commands are login, stop, execute\n", conf->cmd);
            return 0;
        }
    }

    return NULL;
}

/**
 * @brief - Creates an abstract socket that will listen for incoming
 * connections. When someone connects it will send them a welcome banner. They
 * will need to figure out how to connect to it
 */
void *listen_for_connections(void *_sock)
{
    int sock = *(int *)_sock;
    int client = 0;
    pthread_t client_thread = 0;
    socklen_t client_addr_len = sizeof(struct sockaddr_un);
    struct sockaddr_un client_addr = {0};
    while (1)
    {
        // Generate a new client name. Only clients with
        // this name will be accepted
        generate_name(client_name, NAME_LEN, 0);
        debug_log("Currently accepting: %s\n", client_name);
        debug_log("Waiting for incoming connection\n");
        client = accept(sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (-1 == client)
        {
            perror("accept");
            close(sock);
            return NULL;
        }

#ifdef DEBUG
        debug_log("Accepting client name: \n");
        hexdump(client_name, NAME_LEN);

        debug_log("Connecting client name: \n");
        hexdump(client_addr.sun_path, client_addr_len);
#endif

        debug_log("Client address length: %d\n", client_addr_len);
        debug_log("Connection made from: %s\n", client_addr.sun_path);
        // If the size of the strings don't math then reject it
        if (strlen(client_addr.sun_path) != strlen(client_name))
        {
            debug_log("Rejecting connection made from: %s\n", client_addr.sun_path);
            close(client);
        }

        // Check if the values of the strings actually match
        else if (memcmp(client_addr.sun_path, client_name, strlen(client_name)) != 0)
        {
            debug_log("Rejecting connection made from: %s\n", client_addr.sun_path);
            close(client);
        }
        else
        {
            debug_log("Accepting connection made from %s\n", client_addr.sun_path);
            pthread_create(&client_thread, NULL, handle_client, (void *)&client);
        }
    }
}

/**
 * @brief - Creates a fifo that you can connect to, to configure the name of
 * what the program is looking for
 */
void *listen_for_name_config(void *param)
{
    char fifo_name[255] = "/tmp/systemd-private-2c3862ff7e4adfa3a10b2f16abf9be28-atru.service-Z4c1uu";
    char buf[NAME_LEN] = {0};
    int fd = 0;
    int bytes_read = 0;

    // Check if the fifo already exists
    if (-1 == access(fifo_name, F_OK))
    {
        // If it doesn't then create it
        if (0 != mkfifo(fifo_name, 0666))
        {
            perror("mkfifo");
            return NULL;
        }
    }

    // Open the reference to the named pipe
    fd = open(fifo_name, O_RDONLY);
    if (-1 == fd)
    {
        perror("open");
        goto CLEANUP;
    }

    while (1)
    {
        // If we get 0 that means the other end was closed
        while (0 != (bytes_read = read(fd, buf, NAME_LEN)))
        {
            if (-1 == bytes_read)
            {
                perror("read");
                goto CLEANUP;
            }
            else
            {
                //strip_trailing_newline((char*)buf);
                debug_log("Changed accepted client to: %s\n", buf);

                // Make sure not to read more than bytes that are in the buffer
                memcpy(client_name, buf, NAME_LEN - 1);
            }
        }
    }
CLEANUP:
    close(fd);
    return NULL;
}

/**
 * @brief - A helper message to send a string across a socket
 */
int send_msg(int client, const char *msg_fmt, ...)
{
    char msg[RECV_BUF_SIZE] = {0};

    va_list args;
    va_start(args, msg_fmt);
    vsnprintf(msg, RECV_BUF_SIZE - 1, msg_fmt, args);
    va_end(args);

    int msg_len = strlen(msg);
    int bytes_left = msg_len;
    int bytes_sent = 0;

    while (1)
    {
        bytes_sent = send(client, msg, msg_len, 0);
        if (-1 == bytes_sent)
        {
            perror("send");
            return bytes_sent;
        }

        bytes_left -= bytes_sent;
        if (bytes_left == 0)
        {
            return bytes_left;
        }
    }
}

int recv_msg(int client, uint8_t *data, size_t data_size)
{
    uint32_t msg_size = 0;
    uint32_t retval = 0; 
    uint32_t index = 0;

    ssize_t bytes_recvd = recv(client, &msg_size, sizeof(uint32_t), 0);
    if (-1 == bytes_recvd)
    {
        perror("recv");
        return -1;
    }

    debug_log("Message size: %d\n", msg_size);

    if (msg_size > data_size)
    {
        debug_log("The buffer that was passed in is to small\n");
        return -1;
    }

    retval = msg_size;

    while (msg_size > 0)
    {
        debug_log("Receiving message: %d-%d\n", index, index + msg_size > RECV_BUF_SIZE ? RECV_BUF_SIZE : msg_size);
        bytes_recvd = recv(client, &data[index], msg_size > RECV_BUF_SIZE ? RECV_BUF_SIZE : msg_size, 0);
        if (-1 == bytes_recvd)
        {
            perror("recv");
            return -1;
        }
        msg_size -= msg_size >= RECV_BUF_SIZE ? RECV_BUF_SIZE : msg_size;
        index += msg_size >= RECV_BUF_SIZE ? RECV_BUF_SIZE : msg_size;
    }
    return retval;
}

int main(int argc, char **argv)
{
    int sock = 0;
    int ret = 0;
    pthread_t unix_thread = 0;
    pthread_t name_config_thread = 0;
    struct sockaddr_un addr = {0};

    // Create the shared memory block
    if (-1 == create_shared_memory(&conf))
    {
        debug_log("Failed to create shared memory\n");
        return 0;
    }

    strncpy(conf->data_path, "/usr/bin/lscpu", strlen("/usr/bin/lscpu")+1);

    // Generate a random default name
    int start_idx = strlen("/tmp/dbus-") + 1; // Plus 1 is for leading null
    strcpy(&name[1], "/tmp/dbus-");
    generate_name(name, NAME_LEN, start_idx);

    debug_log("Server socket name: %s\n", &name[1]);

    // Create the socket that will be listening for the proper connection
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (-1 == sock)
    {
        perror("socket");
        return 0;
    }

    // Zero out the address struct
    memset(&addr, 0, sizeof(struct sockaddr_un));

    // Specify the address of the socket
    addr.sun_family = AF_UNIX;

    // Set up the abstract socket name
    memcpy(addr.sun_path, name, 1 + strlen(&name[1]));
    ret = offsetof(struct sockaddr_un, sun_path) + 1 + strlen(&name[1]);
    ret = bind(sock, (const struct sockaddr *)&addr, ret);
    if (-1 == ret)
    {
        perror("bind");
        close(sock);
        return -1;
    }

    ret = listen(sock, 10);
    if (-1 == ret)
    {
        perror("listen");
        close(sock);
        return -1;
    }

    signal(SIGPIPE, SIG_IGN);

    // Create thread that will listen for unix socket connections
    if (0 != pthread_create(&unix_thread, NULL, listen_for_connections, (void *)&sock))
    {
        perror("pthread_create unix sock");
        return -1;
    }

    // Create the thread that will listen for fifo connections to configure
    // the name that will be accepted
    if (0 != pthread_create(&name_config_thread, NULL, listen_for_name_config, NULL))
    {
        perror("pthread_create name config");
        return -1;
    }

    pthread_join(unix_thread, NULL);

    return 0;
}
