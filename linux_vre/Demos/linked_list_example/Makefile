CC=gcc
CFLAGS=-g 

all: example

example: unlink_example.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f ./example

