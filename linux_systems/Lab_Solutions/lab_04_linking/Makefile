CC= gcc
CFLAGS=-g -Wall

EXEC_STATIC:= demo_static
EXEC_DYNAMIC:= demo_dynamic
EXEC_EXPLICIT:= demo_explicit
SRCS:= main.c
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.c=.o)

ESRCS:= main_explicit.c
EOBJS:= $(ESRCS:.c=.o)

STATIC_LIB:= libactp.a
DYNAMIC_LIB:= libactp.so
COMMON_LIBS:= -ldl
LSRCS:= actp_factorial.c
LOBJS:= $(LSRCS:.c=.o)
PICLOBJS:= $(LSRCS:.c=_pic.o)


all: $(STATIC_LIB) $(DYNAMIC_LIB) $(EXEC_STATIC) $(EXEC_DYNAMIC) $(EXEC_EXPLICIT)

$(EXEC_STATIC): $(OBJS) $(STATIC_LIB)
		$(CC) -L. -o $@ $^ -l:$(STATIC_LIB) $(COMMON_LIBS)

$(EXEC_DYNAMIC): $(OBJS) $(DYNAMIC_LIB)
		$(CC) -L. -Wl,-rpath=. -o $@ $^ -l:$(DYNAMIC_LIB) $(COMMON_LIBS)

$(EXEC_EXPLICIT): $(EOBJS)
		$(CC) -Wl,-rpath=. -o $@ $^ $(COMMON_LIBS)

$(DYNAMIC_LIB): $(PICLOBJS)
		$(CC) -shared -Wl,-soname,$@ -o $@ $^

$(STATIC_LIB): $(LOBJS)
		ar rcs $@ $^

%_pic.o: %.c $(HDRS)
		$(CC) -c -fPIC $(CFLAGS) -o $@ $<

%.o: %.c $(HDRS)
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f $(EXEC_STATIC) $(STATIC_LIB) $(EXEC_DYNAMIC) $(DYNAMIC_LIB) $(OBJS) $(LOBJS) $(PICLOBJS) $(EXEC_EXPLICIT) $(EOBJS)

