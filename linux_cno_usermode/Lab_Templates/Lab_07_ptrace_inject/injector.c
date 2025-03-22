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
#define STACKSIZE		0x100000

#define SYSCALLCODE		0xCC050F
#define CALLCODE		0xCCD0FF
#define SYSCALLCODEMAP	(~0xffffff)
#define CALLCODEMAP		(~0xffffff)

// given a pid, will:
// 	fill out_libc_entry with the entry function in libc in the target process
// 	fill out_dlopen with the address of dlopen in the target process
int getTargetInfo(pid_t pid, void** out_libc_entry, void** out_dlopen);

// used by getTargetInfo to open and map Libc
int openTargetLibc(pid_t pid, FILE** out_file, void** out_addr);

// given a pid to an process we are PTRACED_ATTACHed to
// 	will resume execution (at the set rip register)
//	will wait for the process to execute a break instruction (int3, 0xCC)
//	if successful will fill out out_regs with the register state after the break
//	otherwise will print out the unexpected signal, and return an error (nonzero)
int ptraceContinueWait(pid_t pid, struct user_regs_struct* out_regs);

int main(int argc, char* argv[]) {
	int err = 0;
	int status = 0;
	pid_t pid = 0;
	
	void* targ_libc_entry = NULL;
	void* targ_dlopen = NULL;

	struct user_regs_struct savedregs = {0};
	long savedcode = 0;	

	size_t pathareasz = getpagesize();
	size_t pathlen = 0;

	if (argc < 3) {
		printf("Usage: %s target_pid /path/to/injection.so\n", argv[0]);
		err = -1;
		goto END;
	}

	pid = strtol(argv[1], NULL, 10);
	
	pathlen = strlen(argv[2]);
	if (pathlen == 0 || pathlen > pathareasz) {
		printf("Invalid .so path\n");
		err = -1;
		goto END;
	}
	
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
	

	printf("Found target's dlopen at 0x%016lx\n", (uint64_t)targ_dlopen);
	printf("Found target's libc entry at 0x%016lx\n", (uint64_t)targ_libc_entry);

	/* STUDENT TODOs */
	// 1. Save program state
	// 2. Cause the target to run mmap to allocate area for a stack and dlopen arguments
	// 3. Copy over the dlopen arguments
	// 4. Cause the target to run dlopen (make sure the stack is aligned before the call)
	// 5. Restore program state
	
	/* Added variables */
	struct user_regs_struct reqregs = {0}; // registers structure for overwriting/setting
	long reqcode = 0;
	uint64_t mappedloc = 0x0;

	// TODO: use ptrace to get/save register state

	// TODO: use ptrace to get/save target state

	// TODO: use ptrace to write/map path area and stack

	// TODO: setup registers for mmap system call
	// mmap(addr<rdi>, len<rsi>, prot<rdx>, flags<r10>, fd<r8>, off<r9>)

	// TODO: use ptrace to set registers for syscall

	err = ptraceContinueWait(pid, &reqregs);
	if (err) {
		printf("Unable to complete remote mmap.\n");
		goto END_UNMAP;
	}

	// RAX will hold the return from the mmap system call (ie. the newly mapped location)
	mappedloc = reqregs.rax;

	printf("Mapped memory at 0x%016lx\n", mappedloc);

	// TODO: use ptrace to write path to patharea

	// TODO: use ptrace to write remote call dl_open

	// TODO: use ptrace to setup registers for dl_open call 

	// DEBUG OPTION (uncomment below for debug print)
	// if we wanted to debug here, we could print out the regs
	// then just reset the registers back to previous and detach
	// then we can attach with gdb, and set the registers, then step through 
	/*printf(
		"Regs:\nrip 0x%016llx\nrbp 0x%016llx\nrsp 0x%016llx\nrax 0x%016llx\nrdi 0x%016llx\nrsi 0x%016llx\n",
		reqregs.rip,
		reqregs.rbp,
		reqregs.rsp,
		reqregs.rax,
		reqregs.rdi,
		reqregs.rsi
	);*/

	err = ptraceContinueWait(pid, &reqregs);
	if (err) {
		printf("Unable to complete remote dlopen.\n");
		goto END_UNMAP;
	}

	printf("Dlopen on target returned 0x%llx\n", reqregs.rax);
	
END_UNMAP:	
	// TODO: repeat ptrace steps from above for call to mumap (unmap) --- ie. resetting everything

END_RESTORE:
	// TODO: restore memory using ptrace commands

	// TODO: restore regs using ptrace commands 
		
END_DETACH:	
	// TODO: user ptrace to detach

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

	// assert that the .e_machine and e_ident are compatible
	//TODO
	
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
