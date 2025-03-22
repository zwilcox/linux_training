#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include "av.h"


int dbg_lvl = DBG_DEFAULT;
const char* const badstr[] = {"lscpu", "proc/cpuinfo", "cpuinfo", "sys/devices/system/cpu/", "sys/devices/system/cpu/cpu0/*", "sys/devices/system/node/node0/cpumap", "cpumap", "proc/sys/kernel/osrelease", "osrelease", "proc/sysinfo", "sysinfo", "usr/lib64/gconv/gconv-modules.cache", "gconv", "lspci", "sys/bus/pci/devices", "usr/share/hwdata/pci.ids", "usr/share/hwdata", "usr/share/hwdata/pci.ids.d/", "lsusb", "run/udev/control", "udev", "sys/subsystem", "subsystem", "sys/bus/usb/devices", "sys/bus", "lsblk", "sys/dev/block", "proc/self/mountinfo", "run/mount/utab", "dev", "mount", "proc/self/mountinfo", "proc", "proc/crypto", "proc/kallsyms", "proc/key-users", "proc/uptime", "proc/version", "uname", "localtime", "gettimeofday", "settimeofday", "time", "setdate", "getdate", "whoami", "netstat", "proc/net/tcp", "net/tcp", "tcp", "net/udp", "udp", "proc/net/tcp6", "proc/net/", "proc/net/snmp", "sys/kernel/debug/bluetooth/", "bluetooth", "/etc/passwd", "passwd", "/etc/shadow", "shadow", "sys/devices/system", "system", "sys/devices/system/cpu/vulnerabilities/meltdown", "vunlnerabilities", "meltdown", "spectre", "nmap", "zmap", "nc", "ncat", "netcat", "lsof", "proc/pid/fd", "pid/fd", "syscall", "LD_PRELOAD", "LD_LIBRARY_PATH"};
const size_t badstr_count = (sizeof(badstr) / sizeof(badstr[0]));


int checkBufBadStr(const void* buf, size_t bufsz) {
	size_t bufleft = 0;
	size_t i = 0;
	size_t stri = 0;

	for (i = 0; i < bufsz; i++) {
		bufleft = bufsz - i;
		// just do a dumb search for bad strings
		for (stri = 0; stri < badstr_count; stri++) {
			if (strlen(badstr[stri]) > bufleft) {
				// no room for this str, don't try it
				continue;
			}
			if (0 == strncmp(badstr[stri], ((char*)buf) + i, bufleft)) {
				// Got a match
				setLvlFromEnv();
				DBGF(DBG_INF, "Found bad string \"%s\" in target\n", badstr[stri]);
				return -1;
			}
		}	
	}
	
	return 0;
}

void setLvlFromEnv() {
	char* e = NULL;	
	static int tried = 0;

	if (tried) {
		return;
	}
	
	tried = 1;

	if (dbg_lvl != DBG_DEFAULT) {
		// probably already set
		return;
	}

	e = getenv(VERB_ENV);
	if (e) {
		dbg_lvl = strtol(e, NULL, 0);
	}
}
