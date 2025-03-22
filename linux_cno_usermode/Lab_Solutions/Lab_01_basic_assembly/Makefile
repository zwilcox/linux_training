CC=gcc

CC_ARGS=-g -Wall -Werror

all: base

student_att.o: student_att.S
	as -o $@ $^

base: base.c student_att.o
	$(CC) $(CC_ARGS) -o $@ $^

clean:
	rm -f ./student_att.o
	rm -f base
