#include "util.h"
// User name is user12345
#define USERNAME_SIZE 50

// Password is S3cretPassw0rd
#define PASSWORD_SIZE 30
#define DATA_PATH_SIZE 260
#define COMMAND_SIZE 8

typedef enum
{
    login,
    stop,
    execute
} CMD;

struct config
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char data_path[DATA_PATH_SIZE];
    size_t data_path_file_size;
    CMD cmd;
};
char config_file_path[260] = {"../secret_config.txt"};

int save_config(struct config *conf);
int read_config(struct config **conf);
uint8_t *get_file_data(char *filename);
void strip_trailing_newline(char *str);
void print_config(struct config *conf);
void encrypt_config(void *data, ssize_t len);
void decrypt_config(void *data, ssize_t len);
