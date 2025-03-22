CC=gcc
CFLAGS=-g -Wall

EXEC:= my_driver
SRCS:= $(wildcard *.c)
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:= my_lib.so

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS) $(LIBS)
		$(CC) -L. -Wl,-rpath=. -o $@ $^ -l:$(LIBS)

my_driver.o: my_driver.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

my_lib.o: my_lib.c $(HDRS)
		$(CC) -c -fPIC $(CFLAGS) -o $@ $<

$(LIBS): my_lib.o
		$(CC) -shared $(CFLAGS) -o $@ $<

clean:
		rm -f $(EXEC) $(OBJS) *.so
