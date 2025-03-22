#include <sys/time.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include "av.h"

#define _GNU_SOURCE
#define __USE_GNU
#include <dlfcn.h>

//TODO
// dlsym (can be done by using _dl_sym as the real dlsym, and hoping for no threading, haha)
// setenv/clearenv?
// execve
// execl ...

// don't let them call these
//
extern int gettimeofday(struct timeval* tv, struct timezone* tz) {
	(void)tv; (void)tz;

	_Exit(EXIT_DETECT);
}

extern int uname(struct utsname* buf) {
	(void)buf;

	_Exit(EXIT_DETECT);
}

extern long syscall(long number, ...) {
	(void)number;

	_Exit(EXIT_DETECT);
}

extern void* dlopen(const char* filename, int flags) {
	(void)filename; (void)flags;

	_Exit(EXIT_DETECT);
}

extern void* dlmopen(Lmid_t lmid, const char* filename, int flags) {
	(void)lmid; (void)filename; (void)flags;

	_Exit(EXIT_DETECT);
}

extern int system(const char* command) {
	(void)command;

	_Exit(EXIT_DETECT);
}

extern long ptrace(int request, pid_t pid, void* addr, void* data) {
	(void)request; (void)pid; (void)addr; (void)data;

	_Exit(EXIT_DETECT);
}


// filter on arguments
//
extern char* getenv(const char *name) {
	char*(*orig_getenv)(const char*);

	if (!strcmp(name, "LD_PRELOAD")) {
		_Exit(EXIT_DETECT);
	}

	orig_getenv = dlsym(RTLD_NEXT, "getenv");

	return orig_getenv(name);
}

extern FILE* fopen(const char* pathname, const char* mode) {
	FILE*(*orig_fopen)(const char* pathname, const char* mode);

	if (checkBufBadStr(pathname, strlen(pathname))) {
		_Exit(EXIT_DETECT);
	}

	orig_fopen = dlsym(RTLD_NEXT, "fopen");
	
	return orig_fopen(pathname, mode);
}

extern int open(const char* pathname, int flags, mode_t mode) {
	int(*orig_open)(const char*,int,mode_t);

	if (checkBufBadStr(pathname, strlen(pathname))) {
 		_Exit(EXIT_DETECT);
	}

	orig_open = dlsym(RTLD_NEXT, "open");
	
	return orig_open(pathname, flags, mode);
}

extern int openat(int dirfd, const char* pathname, int flags, mode_t mode) {
	int(*orig_openat)(int,const char*, int, mode_t);

	if (checkBufBadStr(pathname, strlen(pathname))) {
		_Exit(EXIT_DETECT);
	}
	
	orig_openat = dlsym(RTLD_NEXT, "openat");
	return orig_openat(dirfd, pathname, flags, mode);
}
