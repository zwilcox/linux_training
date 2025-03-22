#include <ctype.h>
#include <elf.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 0x1000
#define MAX_ADDR_FMT_LEN 16
#define DUMP_COLS 16

__attribute__((constructor)) void on_load(void);
__attribute__((destructor)) void on_unload(void);
int debug(char *param);
int printf_wrapper(const char *fmt, ...);
ssize_t send_wrapper(int sockfd, const void *buf, size_t len, int flags);
void *get_main_image_addr(void);
void *get_got_addr(void *image_addr);
int hook_got_entry(void *got_addr, void *orig_func_ptr, void *hook_func_ptr);
void hexdump(const unsigned char *buf, unsigned int len);
void *attempt_hooks(void *got_addr);
void create_hooking_thread(void *got_addr);

int debug(char *param) {
  printf("Pfffttt... I ain't the real debuglib\n");
  return 0;
}

int printf_wrapper(const char *fmt, ...) {
  unsigned int maxlen;
  char *new_fmt;
  va_list args;
  int ret;
  char prefix[] = "[Hooked] ";

  maxlen = strlen(fmt) + sizeof(prefix);
  new_fmt = malloc(maxlen);
  if (new_fmt == NULL) {
    return -1;
  }
  snprintf(new_fmt, maxlen, "%s%s", prefix, fmt);
  va_start(args, fmt);
  ret = vprintf(new_fmt, args);
  va_end(args);
  free(new_fmt);
  return ret;
}

ssize_t send_wrapper(int sockfd, const void *buf, size_t len, int flags) {
  int ret;
  ret = send(sockfd, buf, len, flags);
  if (ret != -1) {
    printf("Someone is sending data:\n");
    hexdump(buf, ret);
  }
  return ret;
}

int hook_got_entry(void *got_addr, void *orig_func_ptr, void *hook_func_ptr) {
  uint64_t *got_entry;

  got_entry = (uint64_t *)got_addr;
  while (*got_entry != (uint64_t)orig_func_ptr) {
    // printf("%p: %#016llx\n", got_entry, *got_entry);
    if ((got_entry - (uint64_t *)got_addr) >= 100)
      break;
    got_entry++;
  }
  if (*got_entry == (uint64_t)orig_func_ptr) {
    // memcpy(got_entry, &hook_func_ptr, sizeof(uint64_t)); //also works
    *got_entry = (uint64_t)hook_func_ptr;
    printf("Overwrote GOT entry at %p\n", got_entry);
    return 1;
  } else {
    printf("Couldn't find symbol\n");
  }
  return 0;
}

void *get_got_addr(void *image_addr) {
  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;
  Elf64_Dyn *dyn;
  int i;
  int num_dyn;

  ehdr = (Elf64_Ehdr *)image_addr;
  fprintf(stderr, "ehrd: %p\n", ehdr);
  dyn = NULL;
  phdr = (Elf64_Phdr *)((uint8_t *)image_addr + ehdr->e_phoff);
  fprintf(stderr, "phrd: %p\n", phdr);
  for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
    if (phdr->p_type == PT_DYNAMIC) {
      dyn = (Elf64_Dyn *)((uint8_t *)image_addr + phdr->p_offset);
      num_dyn = phdr->p_filesz / sizeof(Elf64_Dyn);
      break;
    }
  }

  fprintf(stderr, "dyn: %p\n", dyn);
  if (dyn == NULL) {
    return NULL;
  }

  for (i = 0; i < num_dyn; i++, dyn++) {
    if (dyn->d_tag == DT_PLTGOT) {
      fprintf(stderr, "pltgot d_ptr: %p\n", dyn->d_un.d_ptr);
      return (void *)dyn->d_un.d_ptr;
    }
  }

  /* 	for above ^^^ from elf.h
          Dynamic tags (Dyn)
  The .dynamic section contains a series of structures that hold
  relevant dynamic linking information.  The d_tag member controls
  the interpretation of d_un.

          typedef struct {
          Elf64_Sxword    d_tag;
          union {
                  Elf64_Xword d_val;
                  Elf64_Addr  d_ptr;
          } d_un;
          } Elf64_Dyn;
          extern Elf64_Dyn _DYNAMIC[];

  d_tag  This member may have any of the following values:
                  ...
          DT_PLTGOT
              Address of PLT and/or GOT
                  ...
  */

  fprintf(stderr, "Could not find GOT\n");
  return NULL;
}

void *get_main_image_addr(void) {
  FILE *stream = NULL;
  unsigned char buf[BUF_SIZE];
  void *main_image_addr = NULL;
  char *eoi_char = NULL;

  stream = fopen("/proc/self/maps", "rb");
  if (stream == NULL) {
    fprintf(stderr, "Failure opening maps file\n");
    return NULL;
  }

  if (fread(buf, 1, BUF_SIZE, stream) <= 0) {
    fprintf(stderr, "Failed to read file\n");
    fclose(stream);
    return NULL;
  }
  fclose(stream);

  main_image_addr = (void *)strtol(buf, &eoi_char, 16);
  if (eoi_char == NULL || *eoi_char != '-') {
    fprintf(stderr, "Couldn't find main image address\n");
    return NULL;
  }
  return main_image_addr;
}

void on_load(void) {
  void *main_image_addr;
  void *got_addr;

  printf("Injected lib running...\n");
  main_image_addr = get_main_image_addr();
  if (main_image_addr == NULL) {
    return;
  }
  printf("Found main image base address at %p\n", main_image_addr);

  got_addr = get_got_addr(main_image_addr);
  if (got_addr == NULL) {
    return;
  }
  printf("Found GOT at %p\n", got_addr);

  create_hooking_thread(got_addr);
  return;
}

void on_unload(void) {
  printf("Unloading\n");
  return;
}

void create_hooking_thread(void *got_addr) {
  pthread_t tid;
  pthread_attr_t tattr;
  pthread_attr_init(&tattr);
  pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
  pthread_create(&tid, &tattr, attempt_hooks, got_addr);
  return;
}

void *attempt_hooks(void *got_addr) {
  int hooked_send = 0;
  int hooked_printf = 0;
  while (hooked_send == 0 || hooked_printf == 0) {
    printf("Attempting to hook GOT entries...\n");
    if (hooked_printf == 0) {
      hooked_printf = hook_got_entry(got_addr, printf, printf_wrapper);
    }
    if (hooked_send == 0) {
      hooked_send = hook_got_entry(got_addr, send, send_wrapper);
    }
    sleep(1);
  }
  return NULL;
}

void hexdump(const unsigned char *buf, unsigned int len) {
  char addr_buf[MAX_ADDR_FMT_LEN + 1];
  unsigned int i;
  unsigned int j;
  unsigned int power;
  unsigned int limit;
  unsigned int pad;
  char c;
  power = 1;
  for (i = 0; i < 64; i++) {
    if (len >= power) {
      power = power << 2;
      continue;
    }
    break;
  }
  snprintf(addr_buf, MAX_ADDR_FMT_LEN, "%%0%ullx:", i);
  for (i = 0; i < len; i++) {
    if (i % DUMP_COLS == 0) {
      printf(addr_buf, i);
    }
    if (i % 2 == 0)
      printf(" ");
    printf("%02x", buf[i]);
    if (i % DUMP_COLS == DUMP_COLS - 1 || i == (len - 1)) {
      limit = (i % DUMP_COLS);
      pad = DUMP_COLS - limit;
      if (i == (len - 1)) {
        for (j = 0; j < pad - 1; j++) {
          if (j % 2 != 0) {
            printf(" ");
          }
          printf("  ");
        }
      }
      printf("  ");
      for (j = 0; j <= limit; j++) {
        c = buf[i - limit + j];
        if (isalnum(c)) {
          printf("%c", c);
        } else {
          printf(".");
        }
      }
      printf("\n");
    }
  }
  printf("\n");
}
