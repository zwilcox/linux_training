CC = gcc
CFLAGS=-g -Wall

SRCS:= question3.c
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
DYNAMIC_LIB:= question3.so

.PHONY: all

all: $(DYNAMIC_LIB)

$(DYNAMIC_LIB): $(OBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $^

%.o: %.c $(HDRS)
	$(CC) -c -fPIC $(CFLAGS) -o $@ $<

clean:
	rm -f $(DYNAMIC_LIB) $(OBJS)

