/* ReLabs.cpp for ACTP Software Reverse Engineering course */

#include <ctype.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* Used to grab user input from the console */
char g_linebuf[1024];

#define _fastcall __attribute__((fastcall))
#define _stdcall __attribute__((stdcall))
#define __forceinline __attribute__((always_inline))
#define INFINITE 0xffffffff
#define MAX_PATH 260
int trimNewline(char *sz);
char *getLine(const char *prompt = "line?") {
  printf("%s> ", prompt);
  fflush(NULL);
  if (!fgets(g_linebuf, sizeof(g_linebuf), stdin)) {
    return NULL;
  }
  trimNewline(g_linebuf);
  return g_linebuf;
}

/********************************************************************
 * Lab A
 ********************************************************************/

int aplus2b(int a, int b, int *c) { return *c = a + 2 * b; }

int promptForNumber(const char *prompt = "number") {
  printf("%s> ", prompt);
  fflush(NULL);
  if (!fgets(g_linebuf, sizeof(g_linebuf), stdin)) exit(0);
  return atoi(g_linebuf);
}

extern "C"
    __attribute__((
        visibility("default")))  // This is like __declspec(dllexport)
    void
    ReLabA() {
  srand((unsigned int)time(NULL));
  int c;
  aplus2b(promptForNumber(), rand() & 0xFF, &c);
  printf("%d\n", c);
}

/********************************************************************
 * Lab B
 ********************************************************************/
// fastcall
// uses lea to do subtracts
// an inefficient fibonacci calculator
unsigned long _fastcall slowfib(unsigned long a) {
  if (a <= 1) {
    return a;
  }
  return slowfib(a - 1) + slowfib(a - 2);
}

extern "C" __attribute__((visibility("default"))) void ReLabB() {
  unsigned long a = (unsigned long)promptForNumber();
  if (a == 0) {
    printf("no\n");
  } else if (a > 30) {
    printf("Too big\n");
  } else {
    printf("%ld %ld\n", a, slowfib(a));
  }
}

/********************************************************************
 * Lab C
 ********************************************************************/
int my_max(int a, int b, int *c = 0) {
  int max = a;
  if (b > a) {
    max = b;
  }
  if (c) {
    *c = max;
  }
  return max;
}

extern "C" __attribute__((visibility("default"))) void ReLabC() {
  int res;
  res = my_max(promptForNumber(), promptForNumber());
  printf("%d\n", res);

  my_max(promptForNumber(), promptForNumber(), &res);
  printf("%d\n", res);
}

/********************************************************************
 * Lab D
 ********************************************************************/
/*
table switch
*/
extern "C" __attribute__((visibility("default"))) const char *getColor(int a) {
  // table switch
  switch (a) {
    case 0:
      return "black";
    case 1:
      return "white";
    case 2:
      return "red";
    case 3:
      return "blue";
    case 4:
      return "green";
    case 5:
      return "orange";
    case 6:
      return "purple";
    case 7:
      return "cyan";
    case 8:
      return "pink";
    default:
      return NULL;
  }
}

extern "C" __attribute__((visibility("default"))) void ReLabD() {
  printf("?? to exit\n");
  do {
    int a = promptForNumber();
    if (a == 99) {
      return;
    }
    const char *p = getColor(a);
    if (!p) {
      printf("no\n");
    } else {
      printf("%d %s\n", a, p);
    }
  } while (1);
}

/********************************************************************
 * Lab E
 ********************************************************************/
/*
Branching switch

*/
extern "C" __attribute__((visibility("default"))) const char *getBgColor(
    int a) {
  // branching switch
  switch (a) {
    case 0:
      return "fg_black";
    case 1000:
      return "fg_white";
    case 2000:
      return "fg_red";
    case 3000:
      return "fg_blue";
    case 4000:
      return "fg_green";
    case 5000:
      return "fg_orange";
    case 6000:
      return "fg_purple";
    case 7000:
      return "fg_cyan";
    case 8000:
      return "fg_pink";
    default:
      return NULL;
  }
}

extern "C" __attribute__((visibility("default"))) void ReLabE() {
  printf("?? to exit\n");
  do {
    int a = promptForNumber();
    if (a == 99) {
      return;
    }
    const char *p = getBgColor(a);
    if (!p) {
      printf("no\n");
    } else {
      printf("%d %s\n", a, p);
    }
  } while (1);
}

/********************************************************************
 * Lab F
 ********************************************************************/
int my_strlen(const char *sz) {
  int i;
  for (i = 0; sz[i]; i++)
    ;
  return i;
}

// returns length
int trimNewline(char *sz) {
  // trim any newline, return length of string
  int cb;

  if (!sz) {
    printf("Invalid parameter!\n");
    return -1;
  }

  cb = my_strlen(sz);

  if (cb > 1 && sz[cb - 1] == '\n') {
    sz[--cb] = '\0';
  }

  return cb;
}

extern "C" __attribute__((visibility("default"))) void ReLabF() {
  volatile uint32_t secretPhrase[] = {0x60562f10, 0xf98ba986, 0x6ac8e0ad,
                                      0xacc8a9ad, 0x00000000};

  secretPhrase[0] ^= 0x12345678;
  secretPhrase[1] ^= 0x90ABCDEF;
  secretPhrase[2] ^= 0x0BADC0DE;
  secretPhrase[3] ^= 0xDEADC0DE;

  do {
    if (!fgets(g_linebuf, sizeof(g_linebuf), stdin)) {
      return;
    }
    trimNewline(g_linebuf);

    if (0 == strcasecmp(g_linebuf, (char *)&secretPhrase)) {
      return;
    }

  } while (1);
}

/********************************************************************
 * Lab G
 ********************************************************************/
/**
 * @brief - The cmdline file will have the arguments in null terminated strings
 * In light of that you need to read the whole file and then replace the null
 * bytes with spaces to make it look like a normally formatted string
 *
 * @param str[in, out] - The string of data with possible null bytes
 * @param bytes_read[in] - The length of the string in bytes
 */
void replace_char(char *str, size_t len, char token, char replace) {
  size_t i = 0;
  for (; i < len; i++) {
    if (str[i] == token) {
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
size_t get_process_name(char *path, char *output) {
  char name[1024] = {0};
  int i = 0;
  char *index = NULL;
  size_t bytes_read = 0;
  FILE *fp = NULL;

  fp = fopen(path, "rb");
  if (NULL == fp) {
    return bytes_read;
  }

  bytes_read = fread(name, sizeof(char), 1024, fp);
  if (bytes_read == 0) {
    goto CLEANUP;
  } else {
    index = name + 6;
    while (*index != '\n') {
      *output = *index;
      index++;
      output++;
    }
    output[i] = '\0';

    // memcpy(output, name+8, bytes_read-8);
    // replace_char(output, bytes_read, '\0', ' ');
    // replace_char(output, bytes_read, '\n', '\0');
  }

CLEANUP:
  fclose(fp);
  return bytes_read;
}

char *getProcName(unsigned long pid) {
  char path[1024] = {0};
  char *result = (char *)malloc(1024);
  size_t bytes_read = 0;

  // Check if cmd line file exists
  snprintf(path, sizeof(path), "/proc/%ld/status", pid);
  if (access(path, F_OK) == -1) {
    printf("Failed to access %s\n", path);
    return NULL;
  }

  // If get_cmdline returns a non-zero number then we are done
  bytes_read = get_process_name(path, result);
  if (0 != bytes_read) {
    return result;
  } else {
    return NULL;
  }
}

unsigned long *getProcList(unsigned long *pLength) {
  DIR *dir = NULL;
  struct dirent *entry = {0};
  unsigned long cbAlloc = 10 * sizeof(unsigned long);
  unsigned long *pids = (unsigned long *)malloc(cbAlloc);
  *pLength = 0;

  printf("Opendir\n");
  if (!(dir = opendir("/proc"))) {
    perror("opendir");
    return NULL;
  }

  while ((entry = readdir(dir)) != NULL) {
    // Check to see if I need more memory
    if (*pLength == (cbAlloc / sizeof(unsigned long))) {
      cbAlloc *= 2;
      pids = (unsigned long *)realloc(pids, cbAlloc);
      if (!pids) {
        printf("Memory allocation failure for %ld bytes!\n", cbAlloc);
        return NULL;
      }
    }

    if (entry->d_type == DT_DIR) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }

      if (atoi(entry->d_name) == 0) {
        continue;
      }

      pids[*pLength] = (unsigned long)atoi(entry->d_name);
      *pLength = *pLength + 1;
      printf("%s - %d\n", entry->d_name, atoi(entry->d_name));
    }
  }
  closedir(dir);
  return pids;
}

// calls two subfuncs, to get list of pids and print names for each process
// good one for enums (maybe)
extern "C" __attribute__((visibility("default"))) void ReLabG() {
  unsigned long num_pids;
  unsigned long *pids = getProcList(&num_pids);

  if (pids) {
    for (unsigned long i = 0; i < num_pids; i++) {
      if (!pids[i]) continue;

      char *sz = getProcName(pids[i]);
      if (sz) {
        printf("%4ld %s\n", pids[i], sz);
        free(sz);
      }
    }
    free(pids);
  }
}

// For lab h we just need something that uses structs and enums

int configSysInfo(struct sysinfo *sys_struct) {
  sysinfo(sys_struct);
  sys_struct->procs += 1;

  return 1;
}

extern "C" __attribute__((visibility("default"))) int createProcAndWait(
    int sleep_time) {
  struct sysinfo sys_struct = {0};

  if (!configSysInfo(&sys_struct)) return 0;

  pid_t pid = 0;
  if ((pid = fork()) == 0) {  // child process
    printf("You have %.1fGB of available RAM left\n",
           ((double)sys_struct.freeram * sys_struct.mem_unit) /
               (1024 * 1024 * 1024));  // 1024^3 bytes is a GB
    sleep(sleep_time);
    exit(1);
  }

  int status = 0;
  if (pid != 0) {  // wait for child to finish
    wait(&status);
    return status;  // should always be 1
  } else {
    return 0;  // error
  }
}

extern "C" __attribute__((visibility("default"))) void ReLabH() {
  unsigned long seconds = promptForNumber("seconds");
  if (seconds == 0) {
    seconds = 100;
  }

  if (!createProcAndWait(seconds)) printf("failed\n");
}

/********************************************************************
 * Lab I
 ********************************************************************/
typedef struct _TEST_STRUCT {
  char *pBuf;
  unsigned long dwBufUsed;
  unsigned long dwBufMax;
  int bFillMax;
} TEST_STRUCT, *PTEST_STRUCT;

void FillStruct(PTEST_STRUCT pTest) {
  char letter = 'a';
  unsigned long i;
  unsigned long dwFillLen;

  if (pTest->bFillMax == 1) {
    dwFillLen = pTest->dwBufMax;
  } else {
    dwFillLen = pTest->dwBufUsed;
  }

  for (i = 0; i < dwFillLen; i++) {
    pTest->pBuf[i] = letter++;
  }
  pTest->pBuf[i] = '\0';
}

extern "C" __attribute__((visibility("default"))) void ReLabI() {
  char *line = NULL;
  unsigned long dwLen;
  unsigned long maxLength;
  unsigned long number;
  TEST_STRUCT test = {0};

  maxLength = promptForNumber("max length");
  if (maxLength == 0) {
    printf("Must have non-zero max length.\n");
    goto cleanup;
  }

  line = strdup(getLine());
  dwLen = my_strlen(line);
  if (dwLen > maxLength) {
    printf("line too long.\n");
    goto cleanup;
  }

  number = promptForNumber("0 or 1?");
  if (number != 0 && number != 1) {
    printf("invalid response.\n");
    goto cleanup;
  }

  test.pBuf = (char *)malloc(maxLength + 1);
  strcpy(test.pBuf, line);

  test.dwBufUsed = dwLen;
  test.dwBufMax = maxLength;
  test.bFillMax = (int)number;

  FillStruct(&test);

  printf("%s\n", test.pBuf);

cleanup:
  if (line) {
    free(line);
  }
  if (test.pBuf) {
    free(test.pBuf);
  }

  return;
}

/********************************************************************
 * Lab J
 ********************************************************************/
void printIndent(int level) {
  for (int i = 0; i < level; i++) printf("   ");
}

/* Note VS2010 seems to ignore any and all attempts to force inline
 * memcpy() or memset() to use rep movs and repo stos.  To emulate
 * the old behavior from VS6, at least for the 32-bit case, I've
 * implemented them here. */
void __forceinline my_memcpy(void *dst, void *src, size_t count) {
#ifdef _M_IX86
  __asm {
        mov ecx, count
        mov eax, ecx
        shr ecx, 2
        mov esi, src
        mov edi, dst
        rep movsd
        mov ecx, eax
        and ecx, 3
        rep movsb
  }
#else
  memcpy(dst, src, count);
#endif
}

void __forceinline my_memset(void *dst, unsigned char src, size_t count) {
#ifdef _M_IX86
  __asm {
        xor eax, eax
        mov ecx, count
        mov ebx, ecx
        shr ecx, 2
        mov al, src
        mov edi, dst
        rep stosd
        mov ecx, ebx
        and ecx, 3
        rep stosb
  }
#else
  memset(dst, src, count);
#endif
}

int recurseListDirSimple(const char *name, unsigned long &total,
                         int level = 0) {
  DIR *dir;
  struct dirent *entry;

  if (!(dir = opendir(name))) {
    return 0;
  }
  total = 0;
  int count = 0;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      char path[1024];
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }
      unsigned long subtotal = 0;
      snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
      printf("%*s[%s]\n", level, "", path);
      count += recurseListDirSimple(path, subtotal, level + 2);
      total += subtotal;
    } else {
      char path[1024];
      struct stat st = {0};
      snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
      stat(path, &st);
      count++;
      total += st.st_size;
      printf("%*s- %s\n", level, "", entry->d_name);
    }
  }
  closedir(dir);
  printf("%*s>>>> total %ld in %d files\n\n", level, "", total, count);
  return count;
}

extern "C" __attribute__((visibility("default"))) void ReLabJ() {
  unsigned long liTotal;
  recurseListDirSimple(getLine(), liTotal);
}

/********************************************************************
 * Lab K
 ********************************************************************/
/*list files

RE FEATURES:
        large integers (adc, structure)
        floating point
        argument reuse as local
        recursion
        inlined strcpy, memcpy, strlen idiom (all rep instructions)
        win32 api with structures  (FindFirstFile)
        loop
        or eax, 0xFFFFFFFF idiom
        ASCII character compares (reason to use 'r')

*/
int recurseListDirComplex(const char *name, uint64_t &total, int level = 0) {
  DIR *dir;
  struct dirent *entry;

  if (!(dir = opendir(name))) {
    return 0;
  }
  total = 0;
  int count = 0;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      char path[1024];
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }
      uint64_t subtotal = 0;
      snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
      printf("%*s[%s]\n", level, "", path);
      count += recurseListDirComplex(path, subtotal, level + 2);
      total += subtotal;
    } else {
      char path[1024];
      struct stat st = {0};
      snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
      stat(path, &st);
      count++;
      total += st.st_size;
      printf("%*s- %s\n", level, "", entry->d_name);
    }
  }
  closedir(dir);
  printf("%*s>>>> total %lldMB in %d files\n\n", level, "",
         total / (1024 * 1024), count);
  return count;
}

extern "C" __attribute__((visibility("default"))) void ReLabK() {
  uint64_t liTotal;
  recurseListDirComplex(getLine(), liTotal);
}

/********************************************************************
 * Lab L
 ********************************************************************/
#define M_READ_ONLY 1
#define M_READ_WRITE 0
struct FileMappingData {
  FILE *m_file;
  void *mapping;
  uint64_t size;

  // Constructor
  FileMappingData() : m_file(NULL), mapping(NULL), size(0) {}

  // Destructor
  ~FileMappingData() {
    if (NULL != mapping) {
      munmap(mapping, size);
    }

    if (NULL != m_file) {
      fclose(m_file);
    }
  }
  int mapFile(const char *szFile, int bReadOnly);
};

int FileMappingData::mapFile(const char *szFile, int bReadOnly) {
  m_file = fopen(szFile, bReadOnly ? "r" : "r+");
  if (NULL == m_file) {
    perror("fopen");
    return 0;
  }

  fseek(m_file, 0L, SEEK_END);
  size = ftell(m_file);
  rewind(m_file);

  mapping = mmap(NULL, size, bReadOnly ? PROT_READ : PROT_READ | PROT_WRITE,
                 MAP_PRIVATE, fileno(m_file), 0);
  if (MAP_FAILED == mapping) {
    fclose(m_file);
    perror("mmap");
    return 0;
  }

  return 1;
}

// map a file
extern "C" __attribute__((visibility("default"))) void mapFileAndPrintSize(
    const char *szFile) {
  FileMappingData fmd;
  if (!fmd.mapFile(szFile, M_READ_ONLY)) {
    printf("Failed!\n");
  } else {
    printf("Size %llu, at %p\n", fmd.size, fmd.mapping);
  }
}

extern "C" __attribute__((visibility("default"))) void ReLabL() {
  mapFileAndPrintSize(getLine());
}

/********************************************************************
 * Lab M
 ********************************************************************/
void xorEncode(void *buffer, unsigned long length, unsigned long seed = 0) {
  srand(seed);
  for (unsigned long i = 0; i < length; i++) {
    ((char *)buffer)[i] ^= rand() % 256;
  }
}

int encodeFile(const char *szFileIn, const char *szFileOut) {
  FileMappingData fmd;
  if (!fmd.mapFile(szFileIn, M_READ_WRITE)) {
    return 0;
  }

  FILE *f = fopen(szFileOut, "w");
  if (NULL == f) {
    perror("fopen");
    return 0;
  }

  unsigned long len = fmd.size;

  xorEncode(fmd.mapping, len);

  unsigned long soFar = 0;
  do {
    unsigned long justWritten = 0;
    justWritten = fwrite(((uint8_t *)fmd.mapping) + soFar, 1, len - soFar, f);
    if (0 == justWritten) {
      fclose(f);
      remove(szFileOut);
      return 0;
    }
    soFar += justWritten;
  } while (soFar < len);

  fclose(f);
  return 1;
}

extern "C" __attribute__((visibility("default"))) void ReLabM() {
  const char *a = "/tmp/out.bin";
  const char *b = "/tmp/out";
  remove(a);
  remove(b);
  encodeFile(getLine(), a);
  encodeFile(a, b);
}

int main(int argc, char *argv[]) {
  typedef void (*RELAB)();

  RELAB fp[] = {ReLabA, ReLabB, ReLabC, ReLabD, ReLabE, ReLabF, ReLabG,
                ReLabH, ReLabI, ReLabJ, ReLabK, ReLabL, ReLabM, NULL};

  char *p = getLine("Letter?");

  char cc = toupper(p[0]);
  if (cc < 'A' || cc > 'M') {
    printf("nope\n");
    return -1;
  }

  fp[(cc - 'A')]();

  printf("DONE\n");

  return 0;
}
