CC=gcc
CFLAGS=-g -Wall

EXEC:= crack 
SRCS:= main.c

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) -lcrypt -o $@ $^

clean:
	rm $(EXEC)
