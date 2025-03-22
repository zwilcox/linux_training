CC = gcc
CFLAGS=-O3 -Wall
 
EXEC:= cno_hook
SRCS:= write_flag.c
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)
LIBS:= -ldl

DYNAMIC_LIB:= mywrite.so
LSRCS:= lib_write.c
PICLOBJS:= $(LSRCS:.c=_pic.o)

all: $(EXEC) $(DYNAMIC_LIB)

$(EXEC): $(OBJS)
		$(CC) -o $@ $^ $(LIBS)
		strip $@

$(DYNAMIC_LIB): $(PICLOBJS)
		$(CC) -shared -Wl,-soname,$@ -o $@ $^ $(LIBS)

%_pic.o: %.c $(HDRS)
		$(CC) -c -fPIC $(CFLAGS) -o $@ $<

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f $(EXEC) $(OBJS) $(PICLOBJS) $(DYNAMIC_LIB)
