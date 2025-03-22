// Hide Me Option 2
//
// Checks if "password" is an environment variable, then execs the value of
// variable as an executable path, passing the flag as argv[1]
//

// flag must start with "FLAG{" and end with "}"
// flag must only consist of alphanumeric characters and underscores
#define FLAG	"FLAG{default_flag}"

#define ENVVAR	"password"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	(void)argc; (void)argv;

	char* envval = NULL;

	envval = getenv(ENVVAR);
	if (envval != NULL) {
		execl(envval, envval, FLAG, (char*)NULL);
		perror("execl");
	}

	return 0;
}
