#include <errno.h>
#include <linux/audit.h>
#include <linux/bpf.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <linux/unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "bpf-helper.h"

static int install_filter(void) {
	struct sock_filter filter[] = {
		LOAD_SYSCALL_NR,
		SYSCALL(__NR_exit, ALLOW),
		SYSCALL(__NR_exit_group, ALLOW),
		SYSCALL(__NR_open, ALLOW),
		SYSCALL(__NR_read, ALLOW),
		SYSCALL(__NR_write, ALLOW),
		SYSCALL(__NR_lseek, ALLOW),
		SYSCALL(__NR_close, ALLOW),
		SYSCALL(__NR_execve, ALLOW),
		SYSCALL(__NR_getuid, ALLOW),
		SYSCALL(__NR_getpid, ALLOW),
		SYSCALL(__NR_openat, ALLOW),
		SYSCALL(__NR_brk, ALLOW),
		SYSCALL(__NR_mmap, ALLOW),
		SYSCALL(__NR_munmap, ALLOW),
		SYSCALL(__NR_mprotect, ALLOW),
		SYSCALL(__NR_arch_prctl, ALLOW),
		SYSCALL(__NR_access, ALLOW),
		SYSCALL(__NR_fstat, ALLOW),
		DENY,
	};
	struct sock_fprog prog = {
		.len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
		.filter = filter,
	};

	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
		perror("prctl(NO_NEW_PRIVS)");
		return EXIT_FAILURE;
	}
	if (prctl(PR_SET_SECCOMP, 2, &prog)) {
		perror("prctl(PR_SET_SECCOMP)");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
	char* env[] = {NULL};
	install_filter();

	execve("./rooter", argv, env);
	return EXIT_SUCCESS;
}
