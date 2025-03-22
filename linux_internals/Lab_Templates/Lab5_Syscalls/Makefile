CC=gcc
CFLAGS=-g -Wall
 
EXEC:= syscall
SIMPLE_EXEC:= simple_syscall
SRCS:= $(filter-out $(wildcard *simple*.c),$(wildcard *.c))
HDRS:= $(wildcard *.h)
ASM:= $(wildcard *.s)
OBJS:= $(SRCS:.c=.o)
ASM_OBJS:= syscall_hard_asm.o
LIBS:=

all: $(ASM) $(EXEC) $(SIMPLE_EXEC)

$(ASM_OBJS):
	as -o $@ $(ASM)

$(EXEC): $(OBJS) $(ASM_OBJS)
	$(CC) -o  $@ $^ $(LIBS)

$(SIMPLE_EXEC): simple_syscall.o
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f $(EXEC) $(SIMPLE_EXEC) *.o
