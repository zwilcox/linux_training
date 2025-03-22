// This is a dumb vulnerable service, that is also helpful
// It is an interactive nasm
// You can: give opcodes and get back disasm
// You can: give asm and get back opcodes
// You can: give asm and run it, get back register values

// just use ncat to turn it into a network service
// ncat -l -k --exec "./thisprog"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define OPSTR		"OP "
#define BITSSTR		"BITS "
#define EXITSTR 	"EXIT"
#define HELPSTR		"HELP"
#define OPSTR_l		"op "
#define BITSSTR_l	"bits "
#define EXITSTR_l 	"exit"
#define HELPSTR_l	"help"
#define OPSTRSZ		(sizeof(OPSTR) - 1)
#define BITSSTRSZ	(sizeof(BITSSTR) - 1)
#define EXITSTRSZ	(sizeof(EXITSTR) - 1)
#define HELPSTRSZ	(sizeof(HELPSTR) - 1)

#define TMP_PREFIX	"/tmp/nasmline_"
#define TMP_TEMPLATE	(TMP_PREFIX "XXXXXX")
#define CLEANUP_TMP_CMD ("rm -f " TMP_PREFIX "*")
#define CMDSZ		0x1000
#define NEWLINELEN	1 // 2 for windows
#define NEWSTACKSZ	0x6000
#define NEWSTACKPAD	0x100

#define NASMHEADERFMT	\
			"BITS %d\n"\
			"DEFAULT REL\n"\
			"GLOBAL _start\n"\
			"SECTION .text\n"\
			"\n"\
			"_start:\n"

#define PREEXECODEFMT	"\n"\
			"; save off state\n"\
			"	pushfq\n"\
			"	push rbp\n"\
			"	push r15\n"\
			"	push r14\n"\
			"	push r13\n"\
			"	push r12\n"\
			"	push r11\n"\
			"	push r10\n"\
			"	push r9\n"\
			"	push r8\n"\
			"	push rsi\n"\
			"	push rdi\n"\
			"	push rdx\n"\
			"	push rcx\n"\
			"	push rbx\n"\
			"	push rax\n"\
			"; set new stack\n"\
			"	mov rbp, rsp\n"\
			"	mov rsp, 0x%016llx\n"\
			"; push a bunch of ret stacks so \n"\
			"; they can mess with the stack by\n"\
			"; a bit and we are still okay\n"\
			"	push rbp\n"\
			"	push rbp\n"\
			"	push rbp\n"\
			"	push rbp\n"\
			"	push rbp\n"\
			"	push rbp\n"\
			"	push rbp\n"\
			"	push rbp\n"\
			"	push rbp\n"\
			"	sub rsp, 0xB8\n"\
			"; zero registers\n"\
			"	xor rbp, rbp\n"\
			"	push rbp\n"\
			"	popfq\n"\
			"	xor r15, r15\n"\
			"	xor r14, r14\n"\
			"	xor r13, r13\n"\
			"	xor r12, r12\n"\
			"	xor r11, r11\n"\
			"	xor r10, r10\n"\
			"	xor r9, r9\n"\
			"	xor r8, r8\n"\
			"	xor rsi, rsi\n"\
			"	xor rdi, rdi\n"\
			"	xor rdx, rdx\n"\
			"	xor rcx, rcx\n"\
			"	xor rbx, rbx\n"\
			"	xor rax, rax\n"\
			"; give them a bit of room\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"; START USER CODE\n\n"

#define POSTEXECODEFMT	"\n"\
			"; END USER CODE\n\n"\
			"; give them a bit of room\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"	nop\n"\
			"; put registers into structure\n"\
			"	pushfq\n"\
			"	push rbp\n"\
			"	mov rbp, 0x%016llx\n"\
			"	mov [rbp+0x00], rax\n"\
			"	mov [rbp+0x08], rbx\n"\
			"	mov [rbp+0x10], rcx\n"\
			"	mov [rbp+0x18], rdx\n"\
			"	mov [rbp+0x20], rdi\n"\
			"	mov [rbp+0x28], rsi\n"\
			"	mov [rbp+0x30], r8\n"\
			"	mov [rbp+0x38], r9\n"\
			"	mov [rbp+0x40], r10\n"\
			"	mov [rbp+0x48], r11\n"\
			"	mov [rbp+0x50], r12\n"\
			"	mov [rbp+0x58], r13\n"\
			"	mov [rbp+0x60], r14\n"\
			"	mov [rbp+0x68], r15\n"\
			"	pop rax\n"\
			"	mov [rbp+0x70], rax\n"\
			"	pop rax\n"\
			"	mov [rbp+0x78], rax\n"\
			"; restore stack\n"\
			"	add rsp, 0xD8\n"\
			"	mov rbp, 0x8\n"\
			"	neg rbp\n"\
			"	and rsp, rbp\n"\
			"	pop rsp\n"\
			"; restore registers\n"\
			"	pop rax\n"\
			"	pop rbx\n"\
			"	pop rcx\n"\
			"	pop rdx\n"\
			"	pop rdi\n"\
			"	pop rsi\n"\
			"	pop r8\n"\
			"	pop r9\n"\
			"	pop r10\n"\
			"	pop r11\n"\
			"	pop r12\n"\
			"	pop r13\n"\
			"	pop r14\n"\
			"	pop r15\n"\
			"	pop rbp\n"\
			"	popfq\n"\
			"	ret\n"

#define BANNER		""\
			"   _  __               __   _         \n"\
			"  / |/ /__ ____ __ _  / /  (_)__  ___ \n"\
			" /    / _ `(_-</  ' \\/ /__/ / _ \\/ -_)\n"\
			"/_/|_/\\_,_/___/_/_/_/____/_/_//_/\\__/ \n"\
			"                                      \n"

#define	PROMPT		""\
			"Commands:\n"\
			"\t\"EXIT\"               - Quit the program\n"\
			"\t\"BITS <16, 32, 64>\"  - Change bitness of operations\n"\
			"\t\"OP <ASM, DIS, EXE>\" - Change operation\n"\
			"\t\t\"ASM\" : Assembles nasm syntax assembly to hex\n"\
			"\t\t\"DIS\" : Disassembles hex to nasm syntax assembly\n"\
			"\t\t\"EXE\" : Executes assembly snippets, outputting resulting regs\n"\
			"Otherwise just input the arguments to the operation\n"\
			"Use two consecutive newlines to indicate end of input\n"\
			"\n"

typedef enum _operation {ASSEMB, DISASS, EXECUT} operation;
typedef struct _regs {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t rbp;
	uint64_t eflags;
} regs;

static void handleExecute(char* firstline, uint8_t bitness);
static void handleAssemble(char* firstline, uint8_t bitness);
static void handleDisassemble(char* firstline, uint8_t bitness);

static int startNasmFile(uint8_t bitness, char** filepath);
static int readInputToFile(int fd, char* firstline);
static int getTmpFile(char** filepath);

int main(int argc, char* argv[]) {
	operation op = ASSEMB;
	uint8_t bitness = 64;
	
	ssize_t amt = 0;
	char* lineptr = NULL;
	size_t linebuflen = 0;

	// cleanup previous temp files we died too soon to clean
	system(CLEANUP_TMP_CMD);

	// Print welcome
	printf(BANNER);
	// Control loop
	printf(PROMPT);
	while (1) {
		// Print current mode
		printf(
			"\n\n\t%sing %d bit:\n",
			(op == ASSEMB) ?
				"Assembl" :
				(op == DISASS) ?
					"Disassembl" :
					"Execut",
			bitness
		);

		// We have this strategic fflush
		// which makes a better experience
		// when servicizing this with ncat
		fflush(stdout);
		
		// Get input
		amt = getline(&lineptr, &linebuflen, stdin);
		if (amt == -1) {
			printf("Unable to get user input. %s\n", strerror(errno));
			goto END;
		}

		if (amt == 0) {
			printf("End of input\n");
			goto END;
		}

		if (amt <= NEWLINELEN) {
			continue;
		}

		// check for command
		if (
			(strlen(lineptr) > OPSTRSZ) && 
			(
				(0 == memcmp(lineptr, OPSTR, OPSTRSZ)) ||
				(0 == memcmp(lineptr, OPSTR_l, OPSTRSZ))
			)
		) {
			// parse op
			if (strlen(lineptr) <= OPSTRSZ + 1) {
				printf("Please specifiy the operation");
				continue;
			}

			if (*(lineptr+OPSTRSZ) == 'A' || *(lineptr+OPSTRSZ) == 'a') {
				op = ASSEMB;
			} else if (*(lineptr+OPSTRSZ) == 'D' || *(lineptr+OPSTRSZ) == 'd') {
				op = DISASS;
			} else if (*(lineptr+OPSTRSZ) == 'E' || *(lineptr+OPSTRSZ) == 'e') {
				op = EXECUT;
			} else {
				printf("Unknown operation\n");
			}
		} else if (
			(strlen(lineptr) > BITSSTRSZ) && 
			(
				(0 == memcmp(lineptr, BITSSTR, BITSSTRSZ)) ||
				(0 == memcmp(lineptr, BITSSTR_l, BITSSTRSZ))
			)
		) {
			// parse bits
			if (strlen(lineptr) <= BITSSTRSZ + 2) {
				printf("Please specifiy a bitness");
				continue;
			}

			if (memcmp(lineptr+BITSSTRSZ, "16", 2) == 0) {
				bitness = 16;
			} else if (memcmp(lineptr+BITSSTRSZ, "32", 2) == 0) {
				bitness = 32;
			} else if (memcmp(lineptr+BITSSTRSZ, "64", 2) == 0) {
				bitness = 64;
			} else {
				printf("Invalid bitness\n");
			}
			
		} else if (
			(strlen(lineptr) > EXITSTRSZ) &&
			(
				(0 == memcmp(lineptr, EXITSTR, EXITSTRSZ)) ||
				(0 == memcmp(lineptr, EXITSTR_l, EXITSTRSZ))
			)
		) {
			goto END;
		} else if (
			(strlen(lineptr) > HELPSTRSZ) &&
			(
				(0 == memcmp(lineptr, HELPSTR, HELPSTRSZ)) ||
				(0 == memcmp(lineptr, HELPSTR_l, HELPSTRSZ))
			)
		) {
			printf(PROMPT);
		} else {
			// pass line to operation
			switch (op) {
			case ASSEMB:
				handleAssemble(lineptr, bitness);
				break;
			case DISASS:
				handleDisassemble(lineptr, bitness);
				break;
			case EXECUT:
				handleExecute(lineptr, bitness);
				break;
			}
		}
	}

END:
	if (lineptr != NULL) {
		free(lineptr);
	}

	printf("Done.\n");
	fflush(NULL);

	return 0;
}


void handleExecute(char* firstline, uint8_t bitness) {
	int fd = -1;
	int binfd = -1;
	char* path = NULL;
	char* binpath = NULL;
	size_t binpathsz = 0;
	char cmd[CMDSZ] = {0};
	void* binbuf = (void*)-1;
	size_t binbufsz = 0;
	void* newstack = (void*)-1;
	regs regVal = {0};

	if (bitness != 64) {
		printf("Cannot run %d bit code on this 64 bit machine\n", bitness);
		goto END;
	}

	// prep the nasm file beginning
	fd = startNasmFile(bitness, &path);
	if (fd == -1) {
		goto END;
	}

	// map a new stack for them
	newstack = mmap(NULL, NEWSTACKSZ, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_GROWSDOWN, -1, 0);
	if (newstack == (void*)-1) {
		printf("Unable to map new stack for exec. %s\n", strerror(errno));
		goto END;
	}

	// add the state save code
	dprintf(
		fd,
		PREEXECODEFMT,
		(long long unsigned int)(((char*)newstack) + NEWSTACKSZ - NEWSTACKPAD)
	);

	// keep gathering asm string in a file until we are ready for it
	if (readInputToFile(fd, firstline)) {
		goto END;
	}

	// add the restore code
	dprintf(fd, POSTEXECODEFMT, (long long unsigned int)&regVal);

	binpathsz = strlen(path) + sizeof(".bin") + 1;
	binpath = malloc(binpathsz);
	if (binpath == NULL) {
		printf("Unable to allocate path for binary\n");
		goto END;
	}

	snprintf(binpath, binpathsz - 1, "%s.bin", path);

	// then pass that file to nasm
	snprintf(
		cmd,
		sizeof(cmd) - 1,
		"nasm -f bin %s -o %s",
		path,
		binpath
	);
	system(cmd);

	// then map that file
	binfd = open(binpath, O_RDWR, 0);
	if (binfd == -1) {
		printf("Unable to open assembled file. %s\n", strerror(errno));
		goto END;
	}

	binbufsz = lseek(binfd, 0, SEEK_END);
	if (binbufsz == (size_t)-1) {
		printf("Unable to seek in assembled file. %s\n", strerror(errno));
		goto END;
	}

	binbuf = mmap(NULL, binbufsz, PROT_EXEC|PROT_READ|PROT_WRITE, MAP_PRIVATE, binfd, 0);
	if (binbuf == (void*)-1) {
		printf("Unable to map assembled file. %s\n", strerror(errno));
		goto END;
	}
	
	// save state
	// then run their code
	// then get their registers end state
	((void(*)())binbuf)();

	// then read out the register values
	printf("\nRegister State after execution:\n");
	printf("\tRAX    : 0x%016llX\n", (long long unsigned int)regVal.rax);
	printf("\tRBX    : 0x%016llX\n", (long long unsigned int)regVal.rbx);
	printf("\tRCX    : 0x%016llX\n", (long long unsigned int)regVal.rcx);
	printf("\tRDX    : 0x%016llX\n", (long long unsigned int)regVal.rdx);
	printf("\tRDI    : 0x%016llX\n", (long long unsigned int)regVal.rdi);
	printf("\tRSI    : 0x%016llX\n", (long long unsigned int)regVal.rsi);
	printf("\tR8     : 0x%016llX\n", (long long unsigned int)regVal.r8);
	printf("\tR9     : 0x%016llX\n", (long long unsigned int)regVal.r9);
	printf("\tR10    : 0x%016llX\n", (long long unsigned int)regVal.r10);
	printf("\tR11    : 0x%016llX\n", (long long unsigned int)regVal.r11);
	printf("\tR12    : 0x%016llX\n", (long long unsigned int)regVal.r12);
	printf("\tR13    : 0x%016llX\n", (long long unsigned int)regVal.r13);
	printf("\tR14    : 0x%016llX\n", (long long unsigned int)regVal.r14);
	printf("\tR15    : 0x%016llX\n", (long long unsigned int)regVal.r15);
	printf("\tRBP    : 0x%016llX\n", (long long unsigned int)regVal.rbp);
	printf("\tEFLAGS : 0x%016llX\n", (long long unsigned int)regVal.eflags);

END:
	if (newstack != (void*)-1) {
		munmap(newstack, NEWSTACKSZ);
	}

	if (binbuf != (void*)-1) {
		munmap(binbuf, binbufsz);
	}

	if (fd != -1) {
		close(fd);
	}
	if (path != NULL) {
		unlink(path);
		free(path);
	}
	if (binfd != -1) {
		close(binfd);
	}
	if (binpath != NULL) {
		unlink(binpath);
		free(binpath);
	}
}

void handleAssemble(char* firstline, uint8_t bitness) {
	int fd = -1;
	char* path = NULL;
	char cmd[CMDSZ] = {0};

	// prep the nasm file beginning
	fd = startNasmFile(bitness, &path);
	if (fd == -1) {
		goto END;
	}
	
	// keep gathering asm string in a file until we are ready for it
	if (readInputToFile(fd, firstline)) {
		goto END;
	}

	// then pass that file to nasm
	// then pass the result to xxd
	snprintf(
		cmd,
		sizeof(cmd) - 1,
		"nasm -f bin %1$s -o %1$s.bin && "
		"xxd %1$s.bin; "
		"rm -f %1$s.bin",
		path
	);
	system(cmd);
	
END:
	if (fd != -1) {
		close(fd);
	}
	if (path != NULL) {
		unlink(path);
		free(path);
	}
}

void handleDisassemble(char* firstline, uint8_t bitness) {
	int fd = -1;
	char* path = NULL;
	char cmd[CMDSZ] = {0};

	fd = getTmpFile(&path);
	if (fd == -1) {
		goto END;
	}

	// keep gathering hex string in a file until we are ready for it
	if (readInputToFile(fd, firstline)) {
		goto END;
	}

	// then pass that file to xxd to make a file that is rawbytes
	// then pass that file to ndisasm
	// then read the result out
	snprintf(
		cmd,
		sizeof(cmd) - 1,
		"xxd -ps -r %1$s > %1$s.bin && "
		"ndisasm -b %2$d %1$s.bin; "
		"rm -f %1$s.bin",
		path,
		bitness
	);
	system(cmd);

END:
	if (fd != -1) {
		close(fd);
	}
	if (path != NULL) {
		unlink(path);
		free(path);
	}
}

int startNasmFile(uint8_t bitness, char** filepath) {
	int fd = -1;

	// create file using mkstemp
	fd = getTmpFile(filepath);
	if (fd == -1) {
		goto END;
	}
	
	// make nasm header
	if (dprintf(fd, NASMHEADERFMT, bitness) <= 0) {
		printf("Unable to write nasm header\n");
		close(fd);
		fd = -1;
		goto END;
	}

END:
	return fd;
}

int readInputToFile(int fd, char* firstline) {
	int err = 0;
	ssize_t amt = 0;
	char* lineptr = NULL;
	size_t linebuflen = 0;

	write(fd, firstline, strlen(firstline));
	
	do {
		amt = getline(&lineptr, &linebuflen, stdin);
		if (amt < 0) {
			// Got an error
			printf("Error reading input. %s\n", strerror(errno));
			err = -1;
			goto END;
		}
		
		write(fd, lineptr, amt);
	} while (amt > NEWLINELEN); // read until we get a line that is just a \n

END:
	if (lineptr != NULL) {
		free(lineptr);
	}

	return err;
}

int getTmpFile(char** filepath) {
	char* path = NULL;
	int fd = -1;

	if (filepath == NULL) {
		printf("NULL filepath to getTmpFile!\n");
		return -1;
	}

	path = malloc(sizeof(TMP_TEMPLATE));
	if (path == NULL) {
		printf("Unable to allocate space for temp path!\n");
		return -1;
	}

	memcpy(path, TMP_TEMPLATE, sizeof(TMP_TEMPLATE));

	fd = mkstemp(path);
	if (fd == -1) {
		printf("Unable to make temp file! %s\n", strerror(errno));
		free(path);
		path = NULL;
	}

	*filepath = path;	

	return fd;
}

