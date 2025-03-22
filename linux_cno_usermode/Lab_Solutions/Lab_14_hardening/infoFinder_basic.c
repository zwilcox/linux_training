#include <stdint.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>

#define FIXEDBUFSZ		0x1000
#define UNAME_STR_NUM		5
#define UNAME_BUF_LEN		(0x100 * UNAME_STR_NUM)

#define UPTIME_PATH		"/proc/uptime"
#define	BUILD_PATH		"/proc/version"
#define USERS_PATH		"/etc/passwd"
#define MELTDOWN_PATH		"/sys/devices/system/cpu/vulnerabilities/meltdown"

#define NET_PATH		"/proc/net/"
#define TCP_PATH		NET_PATH "tcp"
#define TCP6_PATH		NET_PATH "tcp6"
#define UDP_PATH		NET_PATH "udp"
#define UDP6_PATH		NET_PATH "udp6"

ssize_t readAll(const char* path, char** buf, ssize_t* bufsz) {
	int fd = -1;
	ssize_t amt = -1;
	off_t flen = 0;
	char* newbuf = NULL;
	
	fd = open(path, 0);
	if (fd == -1) {
		printf("Unable to open \"%s\". %s\n", path, strerror(errno));
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
		printf(
			"Err allocating for reading file \"%s\" of size 0x%lx. %s\n",
			path,
			flen,
			strerror(errno)
		); 
		goto END;
	}
	
	amt = read(fd, *buf, (*bufsz) - 1);
	if (amt == -1) {
		printf("Error reading file \"%s\". %s\n", path, strerror(errno));
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

int main() {
	ssize_t amt = -1;
	struct utsname unameinfo = {0};
	struct timeval tv = {0};
	struct timezone tz = {0};
	char* buf = NULL;
	ssize_t bufsz = 0;

	// gather information about the system
	// get uname info (uname)
	if (uname(&unameinfo)) {
		printf("Unable to get uname information\n");
		goto END;
	} else {
		printf("Uname info: %s %s %s %s %s\n\n", unameinfo.sysname, unameinfo.nodename, unameinfo.release, unameinfo.version, unameinfo.machine);
	}
	
	// get uptime (/proc/uptime)
	amt = readAll(UPTIME_PATH, &buf, &bufsz);
	if (amt == -1) {
		printf("Unable to read uptime\n");
		goto END;
	} else {
		printf("Uptime: %s\n", buf);
	}
	
	// get build info (/proc/version)
	amt = readAll(BUILD_PATH, &buf, &bufsz);
	if (amt == -1) {
		printf("Unable to read build info\n");
		goto END;
	} else {
		printf("Build info: %s\n", buf);
	}

	// get users (/etc/passwd)
	amt = readAll(USERS_PATH, &buf, &bufsz);
	if (amt == -1) {
		printf("Unable to read user info\n");
		goto END;
	} else {
		printf("User info: %s\n", buf);
	}

	// check if vulnerable to meltdown (/sys/devices/system/cpu/vulnerabilities/meltdown)
	amt = readAll(MELTDOWN_PATH, &buf, &bufsz);
	if (amt == -1) {
		printf("Unable to read meltdown info\n");
		goto END;
	} else {
		printf("Meltdown info: %s\n", buf);
	}
		
	// get open sockets (/proc/net/<proto>)
	amt = readAll(TCP_PATH, &buf, &bufsz);
	if (amt == -1) {
		printf("Unable to read tcp info\n");
		goto END;
	} else {
		printf("tcp info:\n%s\n", buf);
	}
	amt = readAll(TCP6_PATH, &buf, &bufsz);
	if (amt == -1) {
		printf("Unable to read tcp6 info\n");
		goto END;
	} else {
		printf("tcp6 info:\n%s\n", buf);
	}
	amt = readAll(UDP_PATH, &buf, &bufsz);
	if (amt == -1) {
		printf("Unable to read udp info\n");
		goto END;
	} else {
		printf("udp info:\n%s\n", buf);
	}
	amt = readAll(UDP6_PATH, &buf, &bufsz);
	if (amt == -1) {
		printf("Unable to read udp6 info\n");
		goto END;
	} else {
		printf("udp6 info:\n%s\n", buf);
	}

	// gettimeofday (in memory, or gettimeofday)
	if (gettimeofday(&tv, &tz)) {
		printf("Unable to get time of day and timezone! %s\n", strerror(errno));
		goto END;
	} else {
		printf(
			"Time: sec : 0x%lx, usec : 0x%lx, minwest : 0x%x, dsttype : 0x%x\n",
			tv.tv_sec,
			tv.tv_usec,
			tz.tz_minuteswest,
			tz.tz_dsttime
		);
	}

	printf("\n===\nCompleted info grab successfully\n===\n\n");

END:
	if (buf != NULL) {
		free(buf);
	}
	
	return 0;
}
