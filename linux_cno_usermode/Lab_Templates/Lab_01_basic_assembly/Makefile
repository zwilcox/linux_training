CC=gcc

CC_ARGS=-g -Wall -Werror

all: base

student.o: student.S
	$(CC) $(CC_ARGS) -c -o $@ $^

base: base.c student.o
	$(CC) $(CC_ARGS) -o $@ $^

clean:
	rm -f ./student.o
	rm -f base
