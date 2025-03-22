
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/prctl.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MSGBUFSZ	0x10
#define HEARTBEATTIME	6

int read_out_messages(int readfd);
int write_periodic_messages(int writefd);

#define HEARTBEATMSG	"HEARTBEAT"
#define NEWCLIENTMSG	"NEWCLIENT"
#define DEBUGAUTHMSG	"DEBUGAUTH"
#define TERMINATEMSG	"TERMINATE"

// program should be setuid to run as root
// make a pipe to talk
// fork and have the child drop to normal user level
// child will periodically write over the pipe telling the root portion to do something
int main(int argc, char** argv) {
	int err = 0;
	int pipefd[2] = {-1,-1};
	pid_t childpid = 0;
	uid_t euid = 0;
	uid_t ruid = 0;
	uid_t suid = 0;
	gid_t rgid = 0;
	gid_t egid = 0;
	gid_t sgid = 0;

	err = getresuid(&ruid, &euid, &suid);
	if (err == -1) {
		printf("Error getting uids. %s\n", strerror(errno));
		err = errno;
		goto END;
	}

	printf(
		"Parent : euid 0x%x ruid 0x%x suid 0x%x\n",
		euid,
		ruid,
		suid
	);

	err = getresgid(&rgid, &egid, &sgid);
	if (err == -1) {
		printf("Err getting gids. %s\n", strerror(errno));
		err = errno;
		goto END;
	}

	printf(
		"Parent : egid 0x%x rgid 0x%x sgid 0x%x\n",
		egid,
		rgid,
		sgid
	);

	if (euid || !ruid) {
		printf("Invalid euid/uid. Must be setuid to run as root!\n");
		err = -1;
		goto END;
	}

	// use pipe to create a channel for communication
	if (pipe2(pipefd, O_DIRECT)) {
		printf("Unable to pipe. %s\n", strerror(errno));
		err = errno;
		goto END;
	}

	childpid = fork();
	if (-1 == childpid) {
		printf("Unable to fork. %s\n", strerror(errno));
		err = errno;
		goto END;
	}
	if (childpid) {
		// be parent
		close(pipefd[1]); // close write end
		read_out_messages(pipefd[0]);
	} else {
		// be child
		// drop user
		setgid(rgid);
		setuid(ruid);
		
		// set dumpable so it can be attached to
		prctl(
			PR_SET_DUMPABLE,
			1,
			0,
			0,
			0
		);

		err = getresuid(&ruid, &euid, &suid);
		if (err == -1) {
			printf("Error getting client uids. %s\n", strerror(errno));
			err = errno;
			goto END;
		}
		
		printf(
			"Client : euid 0x%x ruid 0x%x suid 0x%x\n",
			euid,
			ruid,
			suid
		);

		err = getresgid(&rgid, &egid, &sgid);
		if (err == -1) {
			printf("Err getting gids. %s\n", strerror(errno));
			err = errno;
			goto END;
		}

		printf(
			"Client : egid 0x%x rgid 0x%x sgid 0x%x\n",
			egid,
			rgid,
			sgid
		);

		// cut off stdout and stdin
		fclose(stdin);
		fclose(stdout);

		close(pipefd[0]); // close read end
		printf("Writing client (child) message\n");
		write_periodic_messages(pipefd[1]);
	}

END:
	return err;
}


int read_out_messages(int readfd) {
	char msgbuf[MSGBUFSZ] = {0};
	char authdebug[PATH_MAX] = {0};
	int err = 0;

	// setup authdebug script
	err = snprintf(authdebug, PATH_MAX-1, "/tmp/%lx_authdebug.sh", random() & 0xffffff);
	if (err == -1) {
		printf("Unable to set up authdebug path\n");
		goto END;
	}
	printf("Should have created auth path --- Auth setup err: %d\n", err);
	while (1) {
		err = read(readfd, msgbuf, MSGBUFSZ);
		if (err == -1) {
			goto END;
		}

		if (!strncmp(msgbuf, HEARTBEATMSG, MSGBUFSZ)) {
			printf("Client sent heartbeat\n");
		} else if (!strncmp(msgbuf, NEWCLIENTMSG, MSGBUFSZ)) {
			printf("Client connected\n");
		} else if (!strncmp(msgbuf, DEBUGAUTHMSG, MSGBUFSZ)) {
			printf("Client sent debug auth request:\n");
			printf("\tExecuting auth debug script \"%s\"\n", authdebug);
			if (!fork()) {
				execl(authdebug, authdebug, NULL);
				perror("execl");
				exit(-1);
			}
		} else if (!strncmp(msgbuf, TERMINATEMSG, MSGBUFSZ)) {
			printf("Client sent terminate\n");
			break;
		} else {
			printf("Unknown client request %s\n", msgbuf);
		}
	}

END:

	return err;
}

int write_periodic_messages(int writefd) {
	char msgbuf[MSGBUFSZ] = NEWCLIENTMSG;
	int err = 0;
	
	err = write(writefd, msgbuf, strlen(msgbuf)+1);
	if (err == -1) {
		goto END;
	}

	while (1) {
		sleep(HEARTBEATTIME);
		strncpy(msgbuf, HEARTBEATMSG, MSGBUFSZ);
		err = write(writefd, msgbuf, strlen(msgbuf)+1);
		if (err == -1) {
			goto END;
		}
	}

END:
	return err;
}
