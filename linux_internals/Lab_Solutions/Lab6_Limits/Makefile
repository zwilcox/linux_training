CC = gcc
CFLAGS=-g -Wall
 
EXEC:= limit
SRCS:= $(wildcard *.c)
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:=

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS)
		$(CC) -o $@ $^ $(LIBS)

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f $(EXEC) $(OBJS)

