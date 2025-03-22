#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_MAX 1024
#define FLAG "FLAG{THIS_IS_A_FAKE_FLAG}"
#define STRMATCH(x, y) (strncmp(x, y, sizeof(y) - 1))

typedef struct data {
  char str[120];
  int count;
  char type;
} data_t;

char *create_name(char *name);
void print_name(char *name);
data_t *create_flag();
void get_input(char *buf);

int main(int argc, char *argv[]) {
  char buf[BUF_MAX];
  while (1) {
    char *name;
    printf("Enter command> ");
    fflush(stdout);
    get_input(buf);
    if (STRMATCH(buf, "name") == 0) {
      printf("Enter name> ");
      fflush(stdout);
      get_input(buf);
      name = create_name(buf);
      print_name(name);
    } else if (STRMATCH(buf, "delname") == 0) {
      free(name);
    } else if (STRMATCH(buf, "flag") == 0) {
      create_flag();
    } else if (STRMATCH(buf, "print") == 0) {
      print_name(name);
    }
  }
  return EXIT_SUCCESS;
}

void get_input(char *buf) {
  size_t len;
  if (fgets(buf, BUF_MAX, stdin) != NULL) {
    len = strlen(buf);
    buf[len - 1] = '\0';
  } else {
    exit(0);
  }
  return;
}

void print_name(char *name) {
  printf("Welcome, %s\n", name);
  return;
}

char *create_name(char *name) {
  char *str = malloc(strlen(name) + 1);
  strcpy(str, name);
  return str;
}

data_t *create_flag() {
  data_t *data = malloc(sizeof(data_t));
  data->type = 'F';
  data->count = 1;
  strcpy(data->str, FLAG);
  return data;
}
