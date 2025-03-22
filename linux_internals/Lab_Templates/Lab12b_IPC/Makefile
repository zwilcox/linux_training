CC = gcc
CFLAGS=-g -Wall
 
EXEC:= 12_ipc
POST_BUILD:= make_suid.sh
SRCS:= $(wildcard *.c)
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:= -pthread

all: $(POST_BUILD) $(EXEC)

$(EXEC): $(OBJS) $(HDRS)
		$(CC) -o $@ $^ $(LIBS)

$(POST_BUILD): $(EXEC)
	./$@ $^

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $< $(LIBS)

clean:
		rm -f $(EXEC) $(OBJS)

