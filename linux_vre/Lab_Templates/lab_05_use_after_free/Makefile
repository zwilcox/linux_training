CC = gcc
CFLAGS=-g -Wall -O0

EXEC:= use_after_free
SRCS:= $(wildcard *.c)
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:= 

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c $(HDRS)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXEC) $(OBJS)

