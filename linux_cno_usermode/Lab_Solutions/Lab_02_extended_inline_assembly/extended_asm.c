#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void printcpuinfo(int option);

#define CACHELINE(x) (((x & 0x0000ff00) >> 8) * 8)
#define BRAND(x) (x & 0x000000ff)
#define MODEL(x) ((x & 0x000000f0) >> 4)
#define EXTENDED_MODEL(x) ((x & 0x000f0000) >> 16)
#define FAMILY_ID(x) ((x & 0x00000f00) >> 8)

int main(int argc, char *argv[]) {
  long option;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s input\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  option = strtol(argv[1], NULL, 16);

  printcpuinfo(option);

  return 0;
}

void printcpuinfo(int option) {
  uint32_t eax;
  uint32_t ebx;
  uint32_t ecx;
  uint32_t edx;
  __asm__(
		/*Note that we don't need a "mov val, %%eax" here, as the
		 * input binding takes care of it */
		"cpuid"
			: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
			 /* the line above binds the "a" register (EAX) to the local C var named eax,
			  * the "b" register (EBX) to the local C var named ebx, etc. */
			: "a" (option) /* bind input to the "a" register (EAX) */
			: /* no clobbered registers. Note output and input registers are implictly
			   * considered clobbered so we don't list them here */);
  printf("Basic CPU information associated with input: %d\n", option);
  if (option == 0x00) {
    printf("%.4s%.4s%.4s\n", (char *)&ebx, (char *)&edx, (char *)&ecx);
  } else if (option == 0x01) {
    printf("Family:\t\t\t\t%u\nModel:\t\t\t\t%u\nExtended Model\t\t\t%u\n",
           FAMILY_ID(eax), MODEL(eax), EXTENDED_MODEL(eax));
    printf("Brand index:\t\t\t%u\nCache line size (bytes):\t%u\n", BRAND(ebx),
           CACHELINE(ebx));
  }
  return;
}
