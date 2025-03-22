#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <sched.h>

#define FIXEDBUFSZ		0x1000
#define UNAME_STR_NUM		5
#define UNAME_BUF_LEN		(0x100 * UNAME_STR_NUM)

#define UPTIME_PATH		XORLIT("/proc/uptime")
#define	BUILD_PATH		XORLIT("/proc/version")
#define USERS_PATH		XORLIT("/etc/passwd")
#define MELTDOWN_PATH		XORLIT("/sys/devices/system/cpu/vulnerabilities/meltdown")

#define TCP_PATH		XORLIT("/proc/net/tcp")
#define TCP6_PATH		XORLIT("/proc/net/tcp6")
#define UDP_PATH		XORLIT("/proc/net/udp")
#define UDP6_PATH		XORLIT("/proc/net/udp6")

#define XORLIT(lit)	(xorstr(lit, sizeof(lit)))
#define CLEARXORSTR()	(XORLIT(""))

#define XORPRINTF(fmt, ...)	(printf(XORLIT(fmt), ##__VA_ARGS__))
#define XORKEY			{0x41, 0x43, 0x54, 0x50}
// WARNING! You can only have 1 XORSTR in play at a time, it will wipe the last one out
static inline char* xorstr(char* ciphr, size_t clen) {
	static char* xstr = NULL;
	static size_t xstrSz = 0;

	uint8_t key[] = XORKEY;
	size_t i = 0;

	if (xstr != NULL) {
		memset(xstr, '\0', xstrSz);
		free(xstr);
		xstr = NULL;
	}

	if (clen <= 1) {
		return NULL;
	}

	xstr = (char*)malloc(clen);

	memcpy(xstr, ciphr, clen);
	
	for (i = 0; i < (clen-1); i++) {
		xstr[i] = xstr[i] ^ key[i % sizeof(key)];
	}

	return xstr;
}

extern char** environ;

ssize_t readAll(const char* path, char** buf, ssize_t* bufsz) {
	int fd = -1;
	ssize_t amt = -1;
	off_t flen = 0;
	char* newbuf = NULL;
	
	fd = open(path, 0);
	if (fd == -1) {
		XORPRINTF("Unable to open \"%s\". %s\n", path, strerror(errno));
		goto END;
	}
	
	flen = lseek(fd, 0, SEEK_END);
	if (flen == (off_t)-1 || flen == 0) {
		// if we can't seek in the file
		// it is probaly a strange file
		// try to read into a fixed size buf
		flen = FIXEDBUFSZ-1;
	}
	
	lseek(fd, 0, SEEK_SET);

	if (*buf == NULL) {
		*buf = malloc(flen+1);
		*bufsz = flen+1;
	} else if ((*bufsz) < flen+1) {
		newbuf = realloc(*buf, flen+1);
		if (newbuf == NULL) {
			free(*buf);
			*buf = NULL;
		}
		*bufsz = flen+1;
	}
	
	if (*buf == NULL) {
		*bufsz = 0;
		XORPRINTF(
			"Err allocating for reading file \"%s\" of size 0x%lx. %s\n",
			path,
			flen,
			strerror(errno)
		); 
		goto END;
	}
	
	amt = read(fd, *buf, (*bufsz) - 1);
	if (amt == -1) {
		XORPRINTF("Error reading file \"%s\". %s\n", path, strerror(errno));
		goto END;
	}

	// null terminate
	(*buf)[amt] = '\0';

END:
	if (fd != -1) {
		close(fd);
	}
	
	return amt;
}

int main(int argc, char* argv[]) {
	int errval = 0;
	ssize_t amt = -1;
	struct utsname unameinfo = {0};
	struct timeval tv = {0};
	struct timezone tz = {0};
	char* buf = NULL;
	ssize_t bufsz = 0;
	char* envent = NULL;
	int i =0;
	char* environptr[1] = {NULL};
	int cloneflags = 0;

	// unused
	(void)argc;

	// Try to avoid being LD_PRELOADed by execing ourselves 
	// to check if we have been execed already, look to see if LD_PRELOAD is in the env
	envent = environ[0];
	while (envent != NULL) {
		// check if there is LD_PRELOAD
		// NOTE this could be defeated by the av by clearning the env before main
		// but we could check /proc/self/maps, but then they could ... cat/mouse
		
		if (argc > 1 || strstr(envent, XORLIT("LD_PRELOAD"))) {
			// Execve ourselves with no environment
			XORPRINTF("\n+++\nWe were LD_PRELOADed, evading\n+++\n\n");

			// clone and exec (can't fork)
			// specify no signal to catch on termination
			cloneflags = CLONE_UNTRACED | CLONE_PARENT;
			__asm__(
				"mov %[flgs], %%rdi\n\t"
				"xor %%rsi, %%rsi\n\t"
				"xor %%rdx, %%rdx\n\t"
				"xor %%r10, %%r10\n\t"
				"xor %%r8, %%r8\n\t"
				"mov $0x38, %%rax\n\t"
				"jmp overbad_0\n\t"
				".byte 0xe9\n"
				"overbad_0:\n\t"
				"syscall\n\t"
				: "=a" (errval)
				: [flgs] "m" (cloneflags)
				: "rdi", "rsi", "rdx", "r10", "r8", "r11"
			);

			if (errval == -1) {
				XORPRINTF("Error trying to clone. %s\n", strerror(errno));
				goto END;
			}

			if (errval != 0) {
				exit(0);
			}
			
			//XORPRINTF("Child trying to exec\n");
			
			__asm__(
				"mov %[fname], %%rdi\n\t"
				"mov %[argvp], %%rsi\n\t"
				"mov %[envp], %%rdx\n\t"
				"mov $0x3B, %%rax\n\t"
				"jmp overbad_1\n\t"
				".byte 0xe9\n"
				"overbad_1:\n\t"
				"syscall\n\t"
				: "=a" (errval)
				: [fname] "m" (argv[0]), [argvp] "m" (argv), [envp] "m" (environptr)
				: "rdi", "rsi", "rdx", "r11"
			);
		}

		i++;
		envent = environ[i];
	}

	// gather information about the system
	// get uname info (uname)
	// try the hide the syscall with unaligned instruction stuff
	__asm__(
		"mov $0x3f, %%rax\n\t"
		"lea %[uiaddr], %%rdi\n\t"
		"jmp overbad_2\n\t"
		".byte 0xe9\n"
		"overbad_2:\n\t"
		"syscall\n\t"
		: "=a" (errval)
		: [uiaddr] "m" (unameinfo)
		: "rdi", "rsi", "r11"
	);
	if (errval) {
		XORPRINTF("Unable to get information. Err 0x%x\n", errval);
		goto END;
	} else {
		XORPRINTF(
			"Uname info: %s %s %s %s %s\n\n",
			unameinfo.sysname,
			unameinfo.nodename,
			unameinfo.release,
			unameinfo.version,
			unameinfo.machine
		);
	}
	
	// get uptime (/proc/uptime)
	amt = readAll(UPTIME_PATH, &buf, &bufsz);
	if (amt == -1) {
		XORPRINTF("Unable to read uptime\n");
		goto END;
	} else {
		XORPRINTF("Uptime: %s\n", buf);
	}
	
	// get build info (/proc/version)
	amt = readAll(BUILD_PATH, &buf, &bufsz);
	if (amt == -1) {
		XORPRINTF("Unable to read build info\n");
		goto END;
	} else {
		XORPRINTF("Build info: %s\n", buf);
	}

	// get users (/etc/passwd)
	amt = readAll(USERS_PATH, &buf, &bufsz);
	if (amt == -1) {
		XORPRINTF("Unable to read user info\n");
		goto END;
	} else {
		XORPRINTF("User info: %s\n", buf);
	}

	// check if vulnerable to meltdown (/sys/devices/system/cpu/vulnerabilities/meltdown)
	amt = readAll(MELTDOWN_PATH, &buf, &bufsz);
	if (amt == -1) {
		XORPRINTF("Unable to read meltdown info\n");
		goto END;
	} else {
		XORPRINTF("Meltdown info: %s\n", buf);
	}
		
	// get open sockets (/proc/net/<proto>)
	amt = readAll(TCP_PATH, &buf, &bufsz);
	if (amt == -1) {
		XORPRINTF("Unable to read tcp info\n");
		goto END;
	} else {
		XORPRINTF("tcp info:\n%s\n", buf);
	}
	amt = readAll(TCP6_PATH, &buf, &bufsz);
	if (amt == -1) {
		XORPRINTF("Unable to read tcp6 info\n");
		goto END;
	} else {
		XORPRINTF("tcp6 info:\n%s\n", buf);
	}
	amt = readAll(UDP_PATH, &buf, &bufsz);
	if (amt == -1) {
		XORPRINTF("Unable to read udp info\n");
		goto END;
	} else {
		XORPRINTF("udp info:\n%s\n", buf);
	}
	amt = readAll(UDP6_PATH, &buf, &bufsz);
	if (amt == -1) {
		XORPRINTF("Unable to read udp6 info\n");
		goto END;
	} else {
		XORPRINTF("udp6 info:\n%s\n", buf);
	}

	// gettimeofday (in memory, or gettimeofday)
	__asm__(
		"mov $0x60, %%rax\n\t"
		"lea %[tvaddr], %%rdi\n\t"
		"lea %[tzaddr], %%rsi\n\t"
		"jmp overbad_3\n\t"
		".byte 0xe9\n"
		"overbad_3:\n\t"
		"syscall\n\t"
		: "=a" (errval)
		: [tvaddr] "m" (tv), [tzaddr] "m" (tz)
		: "rdi", "rsi", "r11"
	);

	if (errval) { 
		XORPRINTF("Unable to get time of day and timezone! Ret 0x%x\n", errval);
		goto END;
	} else {
		XORPRINTF(
			"Time: sec : 0x%lx, usec : 0x%lx, minwest : 0x%x, dsttype : 0x%x\n",
			tv.tv_sec,
			tv.tv_usec,
			tz.tz_minuteswest,
			tz.tz_dsttime
		);
	}

	XORPRINTF("\n===\nCompleted info grab successfully\n===\n\n");

END:
	CLEARXORSTR();

	if (buf != NULL) {
		free(buf);
	}
	
	return 0;
}
