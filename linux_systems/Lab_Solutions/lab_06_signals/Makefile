CC = gcc
CFLAGS=-g -Wall -Wno-missing-braces

EXEC:= demo
SRCS:= $(filter-out $(wildcard child_*.c),$(wildcard *.c))
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:= -lncurses

CHILD:= toofast
CSRCS:= $(wildcard child_*.c)
COBJS:= $(CSRCS:.c=.o)

all: $(EXEC) $(CHILD)

$(EXEC): $(OBJS)
		$(CC) -o $@ $^ $(LIBS)

$(CHILD): $(COBJS)
		$(CC) -o $@ $^

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f $(EXEC) $(CHILD) $(OBJS) $(COBJS)

