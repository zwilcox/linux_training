CC = gcc
CFLAGS=-g -Wall
 
EXEC:= runner
SRCS:= $(wildcard *.c)
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:= -lpthread

all: $(EXEC)

$(EXEC): $(OBJS)
		$(CC) -o $@ $^ $(LIBS)
		chown root:root $(EXEC)
		chmod u+s $(EXEC)

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f $(EXEC) $(OBJS)

