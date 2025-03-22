#include <unistd.h>
#include <fcntl.h>

int main() {
	char buffer[1024];
	int fd = open("/home/student/myfile", O_RDONLY);
	read(fd, buffer, 1024);
	close(fd);
}
