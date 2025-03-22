CC = gcc
CFLAGS=-g -Wall

SERVER:= server
SRCS:= server.c
HDRS:= 
OBJS:= $(SRCS:.c=.o)
LIBS:=

CLIENT:= client
CSRCS:= client.c
COBJS:= $(CSRCS:.c=.o)

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJS)
		$(CC) -o $@ $^ $(LIBS)

$(CLIENT): $(COBJS)
		$(CC) -o $@ $^

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f $(SERVER) $(CLIENT) $(OBJS) $(COBJS)

