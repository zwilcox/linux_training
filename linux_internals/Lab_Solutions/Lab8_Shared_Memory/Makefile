CC = gcc
CFLAGS=-g -Wall
 
EXEC:= shared_mem
SRCS:= $(wildcard *.c)
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:=

all: $(EXEC)

$(EXEC): $(SRCS) $(HDRS)
		$(CC) -o $@ $^ $(LIBS)

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f  $(OBJS) $(EXEC)