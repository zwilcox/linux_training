#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "av.h"
#include "avlibbuf.h"

#define DETECT_STR	"\n"\
			"______  _____  _____  _____  _____  _____  _____ ______ \n"\
			"|  _  \\|  ___||_   _||  ___|/  __ \\|_   _||  ___||  _  \\\n"\
			"| | | || |__    | |  | |__  | /  \\/  | |  | |__  | | | |\n"\
			"| | | ||  __|   | |  |  __| | |      | |  |  __| | | | |\n"\
			"| |/ / | |___   | |  | |___ | \\__/\\  | |  | |___ | |/ / \n"\
			"|___/  \\____/   \\_/  \\____/  \\____/  \\_/  \\____/ |___/  \n"\
			"                                                        \n"

#define RET_NORMAL	0
#define RET_ERR		1
#define RET_DETECT	-1

#define VERBENVLEN	0x40
#define CMDSZ		0x400
#define CMDF_INDIS	"objdump -d %s | grep %s | exit $(if [[ $(wc -l) -gt 0 ]]; then echo 1; else echo 2; fi)"

#define TEMPLATE	"/tmp/antiviruslib_XXXXXX"

#define USAGELINE	"\nUsage: %s [-h][options] <binary to run>\n"

#define HELPLINE	USAGELINE \
			"\t-h : Print this information\n"\
			"\t-p : Don't run Prerun checks\n"\
			"\t-r : Don't run Runtime checks (won't run the executable)\n"\
			"\t-d : Don't run Prerun Disassembly check\n"\
			"\t-t : Don't attach Runtime Tracing\n"\
			"\t-l : Don't insert Runtime Library\n"\
			/*"\t-v #: Verbosity level, with -1 being none, and 4 being all\n"*/\
			"\n"

int preruncheck(char* filepath, int bDoDisass);
int runtimecheck(char* filepath, int bDoTracing, int bDoLibrary);

static char* droplib();
static int traceexec(pid_t pid, int bDoTracing);

// A really dumb little "antivirus"
// that tries to detect everything the user has to do
int main(int argc, char* argv[]) {
	int retval = RET_NORMAL;
	int c = -1;
	int bDoPrerun = 1;
	int bDoRuntime = 1;
	int bDoDisass = 1;
	int bDoTracing = 1;
	int bDoLibrary = 1;

	while((c = getopt(argc, argv, "hprdtlv:")) != -1) {
		switch (c) {
		case 'h':
			DBGF(DBG_CRT, HELPLINE, argv[0]);
			goto END;
		case 'p':
			bDoPrerun = 0;
			break;
		case 'r':
			bDoRuntime = 0;
			break;
		case 'd':
			bDoDisass = 0;
			break;
		case 't':
			bDoTracing = 0;
			break;
		case 'l':
			bDoLibrary = 0;
			break;
		case 'v':
			dbg_lvl = strtol(optarg, NULL, 0);
			break;
		case '?':
		default:
			if (optopt == 'v') {
				DBGF(DBG_ERR, "Verbosity argument requires an argument\n");
			} else {
				DBGF(DBG_ERR, "Unknown option -%c\n", optopt);
			}
			goto END;
		}
	}

	if (optind >= argc) {
		DBGF(
			DBG_CRT,
			USAGELINE,
			(argc == 0) ? "./antivirus" : argv[0]
		);
		goto END;
	}

	// 3 pieces of protection
	// prerun:
	//	strings
	// 	objdump for syscall
	// runtime:
	// 	LD_PRELOAD
	// 	ptrace

	// Prerun
	if (bDoPrerun) {
		retval = preruncheck(argv[optind], bDoDisass);
		if (RET_DETECT == retval) {
			DBGF(DBG_CRT, DETECT_STR);
			goto END;
		} else if (RET_ERR == retval) {
			DBGF(DBG_ERR, "Error during prerun\n");
			goto END;
		}
	}
	
	// Runtime
	if (bDoRuntime) {
		retval = runtimecheck(argv[optind], bDoTracing, bDoLibrary);
		if (RET_DETECT == retval) {
			DBGF(DBG_CRT, DETECT_STR);
			goto END;
		} else if (RET_ERR == retval) {
			DBGF(DBG_ERR, "Error during runtime\n");
			goto END;
		}
	}

END:
	return 0;
}

int preruncheck(char* filepath, int bDoDisass) {
	int retval = RET_NORMAL;
	int fd = -1;
	off_t fsz = (off_t)-1;
	void* mapped = (void*)-1;
	char cmd[CMDSZ] = {0};
	int sysret = 0;

	DBGF(DBG_TRC, "Checking file \"%s\"\n", filepath);
	
	// map file and search for strings
	fd = open(filepath, 0);
	if (fd == -1) {
		DBGF(DBG_ERR, "Error opening file. %s\n", strerror(errno));
		retval = RET_ERR;
		goto END;
	}

	fsz = lseek(fd, 0, SEEK_END);
	if (fsz == -1) {
		DBGF(DBG_ERR, "Unable to seek in file. %s\n", strerror(errno));
		retval = RET_ERR;
		goto END;
	}

	mapped = mmap(NULL, (size_t)fsz, PROT_READ, MAP_PRIVATE, fd, 0);
	if (mapped == (void*)-1) {
		DBGF(DBG_ERR, "Unable to map file. %s\n", strerror(errno));
		retval = RET_ERR;
		goto END;
	}

	if (-1 == checkBufBadStr(mapped, fsz)) {
		DBGF(DBG_INF, "Found bad string in prerun check\n");
		retval = RET_DETECT;
		goto END;
	}

	// also search for blatent syscall in code
	if (bDoDisass) {
		sysret = snprintf(cmd, CMDSZ-1, CMDF_INDIS, filepath, "syscall");
		if (sysret <= 0 || sysret >= (CMDSZ-1)) {
			DBGF(DBG_ERR, "Unable to create command string!\n");
			retval = RET_ERR;
			goto END;
		}
		sysret = system(cmd);
		if (!WIFEXITED(sysret)) {
			DBGF(DBG_ERR, "Command errored out, got 0x%x\n", sysret);
			retval = RET_ERR;
			goto END;
		}
		if (WEXITSTATUS(sysret) == 1) {
			DBGF(DBG_INF, "Found a syscall in the disassembly\n");
			retval = RET_DETECT;
			goto END;
		} else if (WEXITSTATUS(sysret) != 2) {
			DBGF(DBG_ERR, "Command errored out, got %d\n", sysret);
			retval = RET_ERR;
			goto END;
		}
	}

	DBGF(DBG_TRC, "Finished prerunchecks with no detections\n");
	
END:
	if (mapped != (void*)-1) {
		munmap(mapped, (size_t)fsz);
	}

	if (fd != -1) {
		close(fd);
	}

	return retval;
}

int runtimecheck(char* filepath, int bDoTracing, int bDoLibrary) {
	int retval = 0;
	pid_t target = 0;
	char* argv[2] = {0};
	char* libloc = NULL;
	char verbenv[VERBENVLEN];
	char* pwd = NULL;

	if (bDoLibrary) {
		libloc = droplib();
		if (libloc == NULL) {
			retval = RET_ERR;
			goto END;
		}
	}

	target = fork();
	if (target == -1) {
		DBGF(DBG_ERR, "Unable to fork! %s\n", strerror(errno));
		retval = RET_ERR;
		goto END;
	}

	if (target == 0) {
		// child
		// set the gid to the pid so any children can be killed
		setpgid(0, 0);

		// save pwd
		pwd = getenv("PWD");
		
		// clear environment for target
		clearenv();

		// restore pwd for target
		setenv("PWD", pwd, 0);

		//Drop lib in

		if (bDoLibrary) {
			setenv("LD_PRELOAD", libloc, 1);
			if (dbg_lvl > DBG_CRT) {
				snprintf(verbenv, VERBENVLEN, "%d", dbg_lvl);
				setenv(VERB_ENV, verbenv, 1);
			}
		}

		argv[0] = filepath;
		argv[1] = NULL;

		// be traced
		if (bDoTracing) {
			if (ptrace(PTRACE_TRACEME, 0, 0, 0)) {
				DBGF(DBG_ERR, "Unable to ask to be traced! %s\n", strerror(errno));
			}

			// send a signal so the tracer can be attached
			kill(getpid(), SIGSTOP);
		}

		execv(filepath, argv);
		DBGF(DBG_ERR, "execve returned! %s\n", strerror(errno));
		exit(-1);
	} else {
		// parent
		
		retval = traceexec(target, bDoTracing);
	}

	DBGF(DBG_TRC, "Target finished\n");

END:
	if (libloc != NULL) {
		unlink(libloc);
		free(libloc);
	}
	return retval;
}


static char* droplib() {
	int fd = -1;
	char* str;

	str = (char*)malloc(sizeof(TEMPLATE));
	if (str == NULL) {
		DBGF(DBG_ERR, "Unable to allocate temp path\n");
		return NULL;
	}

	memcpy(str, TEMPLATE, sizeof(TEMPLATE));

	fd = mkstemp(str);
	if (fd == -1) {
		DBGF(DBG_ERR, "Unable to create temporary file for lib. %s\n", strerror(errno));
		free(str);
		return NULL;
	}

	if (write(fd, antiviruslib_so, antiviruslib_so_len) < antiviruslib_so_len) {
		DBGF(DBG_ERR, "Unable to write lib to temp file!\n");
		free(str);
		close(fd);
		return NULL;
	}

	close(fd);

	return str;
}

int traceexec(pid_t pid, int bDoTracing) {
	int wstatus = 0;
	int retval = RET_NORMAL;
	int err = 0;
	int firstexec = 1;

	DBGF(DBG_TRC, "Target pid is %d\n", pid);
	
	if (!bDoTracing) {
		// if we aren't tracing, just do a normal wait
		do {
			err = waitpid(pid, &wstatus, 0);
		} while(err != -1 && !WIFEXITED(wstatus));
		
		if (err == -1) {
			DBGF(DBG_INF, "Wait error. %s\n", strerror(errno));
			retval = RET_ERR;
			goto END;
		} else if (WEXITSTATUS(wstatus) == EXIT_DETECT) {
			DBGF(DBG_INF, "Target exited due to preloaded library detect\n");
			retval = RET_DETECT;
		}
		goto END;
	}

	/*if (ptrace(PTRACE_ATTACH, pid, NULL, NULL)) {
		DBGF(DBG_INF, "Unable to attach to target. %s\n", strerror(errno));
		retval = RET_ERR;
		goto END;
	}*/

	// wait until the attach / traceme works
	do {
		err = waitpid(pid, &wstatus, 0);
	} while (err == -1 && errno == EINTR);

	if (err == -1) {
		DBGF(DBG_ERR, "Wait error. %s\n", strerror(errno));
		retval = RET_ERR;
		goto END_DETACH;
	}
	err = 0;

	// attached
	DBGF(DBG_TRC, "Attached to target\n");

	// traceexec so we can test when they exec something
	// exitkill so they can't just kill us
	//if (ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXEC | PTRACE_O_EXITKILL | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK)) {
	if (ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEEXEC | PTRACE_O_EXITKILL | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE)) {
		DBGF(DBG_ERR, "Unable to set trace option. %s\n", strerror(errno));
		retval = RET_ERR;
		goto END_DETACH;
	}

	// wait in a loop for exit or exec
	while (1) {
		if (ptrace(PTRACE_CONT, pid, 0, 0)) {
			DBGF(DBG_ERR, "Unable to continue target. %s\n", strerror(errno));
			retval = RET_ERR;
			goto END_DETACH;
		}

		// wait for signal
		do {
			err = waitpid(pid, &wstatus, 0);
		} while (err == -1 && errno == EINTR);

		if (err == -1) {
			DBGF(DBG_ERR, "Unable to wait on target. %s\n", strerror(errno));
			retval = RET_ERR;
			goto END_DETACH;
		}

		// check what kind of signal we got
		if ((wstatus >> 8) == (SIGTRAP | (PTRACE_EVENT_EXEC<<8))) {
			// Got an exec to handle!
			if (!firstexec) {
				DBGF(DBG_INF, "Target trying to exec\n");
				retval = RET_DETECT;

				// kill -pid to kill the whole group
				// because we did setpgid(0, 0) earlier in the child
				if (!kill(-pid, SIGKILL)) {
					DBGF(
						DBG_ERR,
						"Unable to kill target that tried to exec! %s\n",
						strerror(errno)
					);
				}
				break;
			} else {
				DBGF(DBG_TRC, "Starting target\n");
			}

			// let the first exec through
			// because it is us starting the program
			firstexec = 0;
		} else if (
			((wstatus >> 8) == (SIGTRAP | (PTRACE_EVENT_FORK<<8))) ||
			((wstatus >> 8) == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))) ||
			((wstatus >> 8) == (SIGTRAP | (PTRACE_EVENT_CLONE<<8)))
		) {
			DBGF(DBG_INF, "Target trying to fork\n");
			retval = RET_DETECT;

			if (!kill(-pid, SIGKILL)) {
				DBGF(DBG_ERR, "Unable to kill target that tried to fork! %s\n", strerror(errno));
			}
			break;
		} else if (WIFEXITED(wstatus)) {
			// got an exit
			DBGF(DBG_TRC, "Target exited\n");
			if (WEXITSTATUS(wstatus) == EXIT_DETECT) {
				DBGF(DBG_INF, "Target exited due to preloaded library detect\n");
				retval = RET_DETECT;
			}
			break;
		} else {
			DBGF(DBG_TRC, "Got unexpected signal from target with status 0x%x\n", wstatus);
		}
	}

	DBGF(DBG_TRC, "Traced target successfully\n");

END_DETACH:
	if (ptrace(PTRACE_DETACH, pid, 0, 0)) {
		DBGF(DBG_INF, "Could not properly detach from target. %s\n", strerror(errno));
	}	

	DBGF(DBG_TRC, "Done tracing target\n");

END:
	return retval;
}
