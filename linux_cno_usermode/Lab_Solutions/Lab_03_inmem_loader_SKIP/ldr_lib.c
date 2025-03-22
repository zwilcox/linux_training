#include "str_funcs.h"
#include <stdint.h>
#include <elf.h>
#include <sys/types.h>
#include <dlfcn.h>

#define PATHBUFSZ		0x80
#define MAPSBUFSZ		0x2000
#define NULL			(void*)0x0

#define MEMFD_CREATE_SYM	"memfd_create"
#define SNPRINTF_SYM		"snprintf"
#define DLOPEN_SYM		"__libc_dlopen_mode"

// These have to be static const [], because we no global pointers will be correct
static const char file_path_fmt[] = "/proc/self/fd/%d";
static const char maps_path[] = "/proc/self/maps";

// extern functions from the assembly, giving us some primitives before we find libc
extern int OPEN(const char* pathname, int flags, mode_t mode);
extern int CLOSE(int fd);
extern ssize_t READ(int fd, void* buf, size_t count);
extern ssize_t WRITE(int fd, void* buf, size_t count);

// static functions
static void* getBaseLibc(char* maps_output, size_t maps_sz);
static void* getElfSym(const char* symname, void* elfBase);

// function that gets called by our assembly piece
int ldBuf(uint32_t buflen, void* buf) {
	int (*memfd_create_ptr)(const char* name, unsigned int flags) = NULL;
	int (*snprintf_ptr)(char* str, size_t size, const char* format, ...) = NULL;
	void* (*dlopen_ptr)(const char* filename, int flags) = NULL;

	int err = 0;
	int maps_fd = -1;
	int mem_fd = -1;
	char maps_buf[MAPSBUFSZ];
	char filepathbuf[PATHBUFSZ];
	ssize_t amt = -1;
	void* baseLibc = 0;

	maps_fd = OPEN(maps_path, 0, 0);
	if (maps_fd < 0) {
		err = 1;
		goto END;
	}

	// MAPSBUFSZ could be huge
	// ideally we would actually read a portion at a time
	// but this works for now, as long as the target isn't too complex
	amt = READ(maps_fd, maps_buf, MAPSBUFSZ-1);
	if (amt <= 0) {
		err = 2;
		goto END;
	}
	// null terminate the maps_buf
	maps_buf[amt] = '\0';
	
	// get base of libc
	baseLibc = getBaseLibc(maps_buf, amt);
	if (baseLibc == NULL) {
		err = 3;
		goto END;
	}

	// lookup the symbols we need
	memfd_create_ptr = getElfSym(MEMFD_CREATE_SYM, baseLibc);
	if (memfd_create_ptr == NULL) {
		err = 4;
		goto END;
	}

	snprintf_ptr = getElfSym(SNPRINTF_SYM, baseLibc);
	if (snprintf_ptr == NULL) {
		err = 5;
		goto END;
	}

	dlopen_ptr = getElfSym(DLOPEN_SYM, baseLibc);
	if (dlopen_ptr == NULL) {
		err = 6;
		goto END;
	}

	// load our buffer into a memfd
	mem_fd = memfd_create_ptr("", 0);
	if (mem_fd == -1) {
		err = 7;
		goto END;
	}

	amt = WRITE(mem_fd, buf, buflen);
	if (amt != buflen) {
		err = 8;
		goto END;
	}
	
	// call dlopen on the fake file
	amt = snprintf_ptr(filepathbuf, PATHBUFSZ, file_path_fmt, mem_fd);
	if (amt >= PATHBUFSZ) {
		err = 9;
		goto END;
	}

	if (dlopen_ptr(filepathbuf, RTLD_LAZY) == NULL) {
		err = 10;
		goto END;
	}

END:
	if (mem_fd != -1) {
		CLOSE(mem_fd);
	}
	if (maps_fd != -1) {
		CLOSE(maps_fd);
	}
	return err;
}

void* getBaseLibc(char* maps_output, size_t maps_sz) {
	char* firstlibc = NULL;
	char* cursor = NULL;
	uint64_t libcbase = 0;
	
	// find first mention of "libc[-.]" in the buffer
	firstlibc = strstr(maps_output, "libc-");
	if (firstlibc == NULL) {
		firstlibc = strstr(maps_output, "libc.");
		if (firstlibc == NULL) {
			return NULL;
		}
	}
	// then find '\n' before that (or beginning of buffer)
	cursor = firstlibc;
	while (cursor > maps_output) {
		if (*cursor == '\n') {
			cursor++;
			break;
		}
		cursor--;
	}
	
	// then turn the characters after that until a '-' into a number
	libcbase = strtoull(cursor, NULL, 16);
	
	return (void*)libcbase;
}

void* getElfSym(const char* symname, void* elfBase) {
	uint8_t* base = (uint8_t*)elfBase;
	Elf64_Ehdr* hdr = NULL;
	Elf64_Phdr* phdr = NULL;
	Elf64_Half phnum = 0;
	Elf64_Dyn* dyn = NULL;
	uint64_t dynnum = 0;
	Elf64_Sym* sym = NULL;
	char* strtab = NULL;
	uint32_t maxstroff = 0;
	uint64_t i = 0;	

	hdr = (Elf64_Ehdr*)base;
	phnum = hdr->e_phnum;
	
	for (i = 0; i < phnum; i++) {
		// find the phdr for Dyn
		phdr = (Elf64_Phdr*)(base + hdr->e_phoff + (sizeof(Elf64_Phdr)*i));
		if (phdr->p_type == PT_DYNAMIC) {
			dynnum = phdr->p_memsz / sizeof(Elf64_Dyn);
			dyn = (Elf64_Dyn*)(base + phdr->p_vaddr);
			break;
		}
	}
	
	if (dyn == NULL) {
		return NULL;
	}

	for (i = 0; i < dynnum; i++) {
		if (dyn->d_tag == DT_STRTAB) {
			strtab = (char*)dyn->d_un.d_ptr;
		}
		else if (dyn->d_tag == DT_SYMTAB) {
			sym = (Elf64_Sym*)dyn->d_un.d_ptr;
		}
		else if (dyn->d_tag == DT_STRSZ) {
			maxstroff = dyn->d_un.d_val;
		}

		if (strtab != NULL && sym != NULL && maxstroff != 0) {
			break;
		}

		dyn = (Elf64_Dyn*)(((char*)dyn) + sizeof(Elf64_Dyn));
	}

	if (strtab == NULL || sym == NULL) {
		return NULL;
	}

	if (maxstroff == 0) {
		maxstroff = (uint32_t)-1;
	}

	do {
		if (0 == strcmp(symname, strtab + sym->st_name)) {
			// found symbol
			return (void*)(base + sym->st_value);
		}
		
		sym = (Elf64_Sym*)(((char*)sym) + sizeof(Elf64_Sym));		

		// I actually don't know how to know how many symbols there are
		// or when to stop?
	} while (sym->st_name != 0 && sym->st_name < maxstroff);

	return NULL;
}

