CC = gcc
CFLAGS=-g -Wall

SERVER:= server
SRCS:= server.c queue.c
HDRS:= 
OBJS:= $(SRCS:.c=.o)
LIBS:= -lpthread

CLIENT:= client
CSRCS:= client.c
COBJS:= $(CSRCS:.c=.o)

.PHONY: handout

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJS)
		$(CC) -o $@ $^ $(LIBS)

$(CLIENT): $(COBJS)
		$(CC) -o $@ $^ $(LIBS)

handout: $(SRCS)
		unifdef -U__SOLUTION__ server.c > server_handout.c

%.o: %.c $(HDRS)
		$(CC) -D__SOLUTION__ -c $(CFLAGS) -o $@ $<

clean:
		rm -f $(SERVER) $(CLIENT) $(OBJS) $(COBJS)

