#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void print_usage(char *name);
int parse_elf(unsigned char *data, unsigned int datalen);
void print_program_header_type(Elf64_Word type);
void print_section_header_type(Elf64_Word type);
const char *get_data_type(Elf64_Half type);
const char *get_object_file_type(Elf64_Half type);
const char *get_machine_type(Elf64_Half type);
const char *get_osabi_type(Elf64_Half type);
void print_program_header_perms(Elf64_Word perms);

int main(int argc, char *argv[]) {
  FILE *stream;
  struct stat st;
  unsigned char *data;
  unsigned int datalen;

  if (argc != 2) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (stat(argv[1], &st) == -1) {
    perror("stat");
    return EXIT_FAILURE;
  }
  datalen = st.st_size;
  data = malloc(datalen);
  if (data == NULL) {
    fprintf(stderr, "Failed to malloc\n");
    return EXIT_FAILURE;
  }

  stream = fopen(argv[1], "rb");
  if (stream == NULL) {
    perror("fopen");
    free(data);
    return EXIT_FAILURE;
  }

  if (fread(data, 1, datalen, stream) != datalen) {
    fprintf(stderr, "Failed to read file data\n");
    free(data);
    return EXIT_FAILURE;
  }

  parse_elf(data, datalen);

  fclose(stream);
  free(data);

  return EXIT_SUCCESS;
}

int parse_elf(unsigned char *data, unsigned int datalen) {
  Elf64_Ehdr *ehdr;
  Elf64_Phdr *phdr;
  Elf64_Shdr *shdr;
  unsigned long int name_tbl_offset;
  unsigned int i;
  ehdr = (Elf64_Ehdr *)data;
  if (strncmp((char *)ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
    printf("Not a valid ELF file\n");
    return 1;
  }

  printf("Elf Header:\n");
  printf("  Magic:   ");
  for (int i = 0; i < 16; i++) {
    printf("%02x ", data[i]);
  }
  printf("\n");
  printf("  %-35s ELF64\n", "Class:");
  printf("  %-35s %s\n", "Data:", get_data_type(ehdr->e_ident[EI_DATA]));
  printf("  %-35s %d\n", "Version:", ehdr->e_ident[EI_VERSION]);
  printf("  %-35s %s\n",
         "OS/ABI:", get_osabi_type(ehdr->e_ident[EI_ABIVERSION]));
  printf("  %-35s %d\n", "ABI Version:", ehdr->e_ident[EI_ABIVERSION]);
  printf("  %-35s %s\n", "Type:", get_object_file_type(ehdr->e_type));
  printf("  %-35s %s\n", "Machine:", get_machine_type(ehdr->e_machine));
  printf("  %-35s 0x%x\n", "Version:", ehdr->e_version);
  printf("  %-35s 0x%lx\n", "Entry point address:", ehdr->e_entry);
  printf("  %-35s %ld (bytes into file)\n",
         "Start of program headers:", ehdr->e_phoff);
  printf("  %-35s %ld (bytes into file)\n",
         "Start of section headers:", ehdr->e_shoff);
  printf("  %-35s 0x%x\n", "Flags:", ehdr->e_flags);
  printf("  %-35s %d (bytes)\n", "Size of this header:", ehdr->e_ehsize);
  printf("  %-35s %d (bytes)\n", "Size of program headers:", ehdr->e_phentsize);
  printf("  %-35s %d\n", "Number of program headers:", ehdr->e_phnum);
  printf("  %-35s %d (bytes)\n", "Size of section headers:", ehdr->e_shentsize);
  printf("  %-35s %d\n", "Number of section headers:", ehdr->e_shnum);
  printf("  %-35s %d\n",
         "Section header string table index:", ehdr->e_shstrndx);

  printf("\nSection headers:\n");
  shdr = (Elf64_Shdr *)(data + ehdr->e_shoff);
  name_tbl_offset = (shdr + ehdr->e_shstrndx)->sh_offset;
  printf("[Nr] %22s %10s %18s %8s %8s  %s %s %s %s\n", "Name", "Type",
         "Address", "Off", "Size", "ES", "Lk", "Inf", "Al");
  for (i = 0; i < ehdr->e_shnum; i++, shdr++) {
    printf("[%02d] ", i);
    printf("%22s ", (char *)data + name_tbl_offset + shdr->sh_name);
    print_section_header_type(shdr->sh_type);
    printf(" ");
    printf("0x%016lx ", shdr->sh_addr);
    printf("0x%06lx ", shdr->sh_offset);
    printf("0x%06lx  ", shdr->sh_size);
    printf("%2lx  ", shdr->sh_entsize);
    printf("%2u ", shdr->sh_link);
    printf("%2u ", shdr->sh_info);
    printf("%02lu\n", shdr->sh_addralign);
  }

  printf("\nProgram headers:\n");
  printf("%16s %8s %18s %18s %8s %8s %3s %6s\n", "Type", "Offset", "VirtAddr",
         "PhysAddr", "FileSiz", "MemSiz", "Flg", "Align");
  phdr = (Elf64_Phdr *)(data + ehdr->e_phoff);
  for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
    print_program_header_type(phdr->p_type);
    printf(" ");
    printf("0x%06lx ", phdr->p_offset);
    printf("0x%016lx ", phdr->p_vaddr);
    printf("0x%016lx ", phdr->p_paddr);
    printf("0x%06lx ", phdr->p_filesz);
    printf("0x%06lx ", phdr->p_memsz);
    print_program_header_perms(phdr->p_flags);
    printf(" ");
    printf("0x%04lx\n", phdr->p_align);
  }

  return 0;
}

void print_program_header_perms(Elf64_Word perms) {
  if (PF_R & perms) {
    printf("R");
  } else {
    printf(" ");
  }
  if (PF_W & perms) {
    printf("W");
  } else {
    printf(" ");
  }
  if (PF_X & perms) {
    printf("E");
  } else {
    printf(" ");
  }
  return;
}

void print_program_header_type(Elf64_Word type) {
  char fmt[] = "%16s";
  switch (type) {
  case PT_NULL:
    printf(fmt, "PT_NULL");
    break;
  case PT_LOAD:
    printf(fmt, "PT_LOAD");
    break;
  case PT_DYNAMIC:
    printf(fmt, "PT_DYNAMIC");
    break;
  case PT_INTERP:
    printf(fmt, "PT_INTERP");
    break;
  case PT_NOTE:
    printf(fmt, "PT_NOTE");
    break;
  case PT_SHLIB:
    printf(fmt, "PT_SHLIB");
    break;
  case PT_PHDR:
    printf(fmt, "PT_PHDR");
    break;
  case PT_TLS:
    printf(fmt, "PT_TLS");
    break;
  case PT_NUM:
    printf(fmt, "PT_NUM");
    break;
  case PT_LOOS:
    printf(fmt, "PT_LOOS");
    break;
  case PT_GNU_EH_FRAME:
    printf(fmt, "PT_GNU_EH_FRAME");
    break;
  case PT_GNU_STACK:
    printf(fmt, "PT_GNU_STACK");
    break;
  case PT_GNU_RELRO:
    printf(fmt, "PT_GNU_RELRO");
    break;
  case PT_LOSUNW:
    printf(fmt, "PT_LOSUNW");
    break;
  case PT_SUNWSTACK:
    printf(fmt, "PT_SUNWSTACK");
    break;
  case PT_HISUNW:
    printf(fmt, "PT_HISUNW");
    break;
  case PT_LOPROC:
    printf(fmt, "PT_LOPROC");
    break;
  case PT_HIPROC:
    printf(fmt, "PT_HIPROC");
    break;
  default:
    printf(fmt, "Unknown");
    break;
  }
  return;
}

void print_section_header_type(Elf64_Word type) {
  char fmt[] = "%10s";
  switch (type) {
  case SHT_NULL:
    printf(fmt, "NULL");
    break;
  case SHT_PROGBITS:
    printf(fmt, "PROGBITS");
    break;
  case SHT_SYMTAB:
    printf(fmt, "SYMTAB");
    break;
  case SHT_STRTAB:
    printf(fmt, "STRTAB");
    break;
  case SHT_RELA:
    printf(fmt, "RELA");
    break;
  case SHT_HASH:
    printf(fmt, "HASH");
    break;
  case SHT_DYNAMIC:
    printf(fmt, "DYNAMIC");
    break;
  case SHT_NOTE:
    printf(fmt, "NOTE");
    break;
  case SHT_NOBITS:
    printf(fmt, "NOBITS");
    break;
  case SHT_REL:
    printf(fmt, "REL");
    break;
  case SHT_SHLIB:
    printf(fmt, "SHLIB");
    break;
  case SHT_DYNSYM:
    printf(fmt, "DYNSYM");
    break;
  case SHT_INIT_ARRAY:
    printf(fmt, "INIT_ARRAY");
    break;
  case SHT_FINI_ARRAY:
    printf(fmt, "FINI_ARRAY");
    break;
  default:
    printf(fmt, "Unknown");
    break;
  }
  return;
}

const char *get_data_type(Elf64_Half type) {
  switch (type) {
  case ELFDATANONE:
    return "Invalid data encoding";
  case ELFDATA2LSB:
    return "2's complement, little endian";
  case ELFDATA2MSB:
    return "2's complement, big endian";
  default:
    return "Unknown";
  }
}

const char *get_osabi_type(Elf64_Half type) {
  switch (type) {
  case ELFOSABI_ARM:
    return "ARM ABI";
  case ELFOSABI_GNU:
    return "GNU ABI";
  case ELFOSABI_NONE:
    return "UNIX System V ABI";
  default:
    return "Unknown";
  }
}

const char *get_object_file_type(Elf64_Half type) {
  switch (type) {
  case ET_NONE:
    return "NONE";
  case ET_REL:
    return "REL";
  case ET_EXEC:
    return "EXEC";
  case ET_DYN:
    return "DYN (Shared object file)";
  case ET_CORE:
    return "CORE";
  default:
    return "Unknown";
  }
  return "Unknown";
}

const char *get_machine_type(Elf64_Half type) {
  switch (type) {
  case EM_X86_64:
    return "Advanced Micro Devices X86-64";
  case EM_SPARC:
    return "SUN SPARC";
  case EM_860:
    return "Intel 80860";
  case EM_ARM:
    return "ARM";
  case EM_386:
    return "Intel 80386";
  default:
    return "Unknown";
  }
}

/* TODO: Need to actually check for -h and -S flag and even -l */
void print_usage(char *name) { printf("Usage: %s file\n", name); }
