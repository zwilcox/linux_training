CC=gcc
CFLAGS=-g -Wall

POST_BUILD:= make_suid.sh
EXEC:= ping
SRCS:= main.c

all: $(POST_BUILD) $(EXEC)

$(POST_BUILD): $(EXEC)
	./$@ $^

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(EXEC)
