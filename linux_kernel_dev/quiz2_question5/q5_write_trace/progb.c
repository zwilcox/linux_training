#include <sys/types.h>
#include <unistd.h>

int main() {
	char buf[32];
	int pipefd[2];
	pipe(pipefd);
	write(pipefd[1], buf, 32);
	close(pipefd[0]);
	close(pipefd[1]);
}
