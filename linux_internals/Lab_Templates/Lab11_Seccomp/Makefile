CC=gcc
CFLAGS=-g -Wall

EXEC:= demo
SRCS:= main.c

all: $(EXEC)


$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(EXEC)
