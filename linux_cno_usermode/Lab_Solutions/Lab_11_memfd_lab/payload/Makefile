CC = gcc
CFLAGS=-g -Wall -fPIC
 
EXEC:= payload.so
SRCS:= $(wildcard *.c)
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:=

all: $(EXEC)

$(EXEC): $(OBJS)
		$(CC) -shared -Wl,-soname,$@ -o $@ $^ $(LIBS)

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f $(EXEC) $(OBJS)

