#include <sys/socket.h>
#include <unistd.h>

int main() {
	char buf[32];
	int sock = socket(AF_INET, SOCK_STREAM, 0); 
	write(sock, buf, 32);
	close(sock);
}
