// Hide Me Option 3
//
// If it sees the sequence of SIGUSR1 followed by SIGUSR2 followed by
// "password" passed to stdin, it writes the flag out to stdout
//

// flag must start with "FLAG{" and end with "}"
// flag must only consist of alphanumeric characters and underscores
#define FLAG	"FLAG{default_flag}"

#define PASS	"password"
#define PASSSZ	sizeof(PASS)

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

volatile int usr1_recv = 0;
volatile int usr2_recv = 0;

static void sighandler(int s) {
	if (!usr1_recv && s == SIGUSR1) {
		usr1_recv = 1;
	} else if (usr1_recv && !usr2_recv && s == SIGUSR2) {
		usr2_recv = 1;
	} else {
		usr1_recv = 0;
		usr2_recv = 0;
	}
}

int main(int argc, char* argv[]) {
	(void)argc; (void)argv;

	struct sigaction sa = {0};
	char passbuf[PASSSZ] = {0};

	sa.sa_handler = sighandler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGUSR1);
	sigaddset(&sa.sa_mask, SIGUSR2);
	
	if (sigaction(SIGUSR1, &sa, NULL)) {
		perror("sigaction SIGUSR1");
		goto END;
	}
	if (sigaction(SIGUSR2, &sa, NULL)) {
		perror("sigaction SIGUSR2");
		goto END;
	}

	while (!usr2_recv) {
		fflush(stdin);
	}

	if (!fgets(passbuf, PASSSZ, stdin)) {
		goto END;
	}

	if (strcmp(PASS, passbuf)) {
		goto END;
	}

	fputs(FLAG, stdout);

END:
	return 0;
}
