#ifndef AV_H_
#define AV_H_

#include <stdlib.h>
#include <stdio.h>

// shared resources between antivirus and antiviruslib
#define EXIT_DETECT	0xf3


#define DBG_ALL		4

#define	DBG_TRC		3
#define DBG_INF		2
#define DBG_ERR		1
#define DBG_CRT		0

#define DBG_NON		-1

#define DBG_DEFAULT	DBG_CRT

#define VERB_ENV	"AV_V"

extern int dbg_lvl;
#define DBGF(lvl, fmt, ...)	do{ if (lvl <= dbg_lvl) fprintf(stderr, "[ANTIVIRUS]:" fmt, ##__VA_ARGS__); }while(0)

extern const char* const badstr[];
extern const size_t badstr_count;

int checkBufBadStr(const void* buf, size_t bufsz);
void setLvlFromEnv();

#endif
