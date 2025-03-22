CC=gcc
CFLAGS=-nostdlib -Wall -Werror -Wextra -g

all: demo

demo: basic_vuln.S
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f ./demo

