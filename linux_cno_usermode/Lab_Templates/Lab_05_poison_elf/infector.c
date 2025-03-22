#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

static void* map_path(char* path, size_t* out_size);

int main(int argc, char* argv[]) {
	/* 
	
		1) Map the target and payload into memory
		2) Find a suitable gap (if any) in the target by
			iterating through its program headers
		3) Replace the entry point in the target to be
			the start of the gap you found. Save the original.
		4) Inject the bytes from the payload into the gap
		5) Search the payload for the sequence of 8 0x41s
			and replace it with offset to the original entry
		6) Make it work for both DYN and EXEC ELFs
	
	*/
}

void* map_path(char* path, size_t* out_size) {
	int fd;
	struct stat st;
	void* data = NULL;
	
	fd = open(path, O_APPEND | O_RDWR);
	if (fd < 0) {
		perror("open");
		return NULL;
	}

	if (fstat(fd, &st) == -1) {
		perror("stat");
		close(fd);
		return NULL;
	}
	
	data = mmap(0, st.st_size,
		PROT_READ | PROT_WRITE, MAP_SHARED,
		fd,	0);
	close(fd);
	if (data == MAP_FAILED) {
		perror("mmap");
		return NULL;
	}

	*out_size = st.st_size;
	return data;
}


