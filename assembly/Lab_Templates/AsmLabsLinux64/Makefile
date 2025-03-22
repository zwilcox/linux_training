# Dependencies are make, nasm, g++, libstdc++.i686, glibc-devel.i686
CC=g++
CFLAGS=-g -Wall -masm=intel

EXEC:= assembly
SRCS:= $(wildcard *.cpp)
HDRS:= $(wildcard *.h)
OBJS:= $(SRCS:.cpp=.o)
LIBS:= -ldl

all: $(EXEC) student.o validate.o validation.o AsmLabs64.o

$(EXEC): student.o validate.o $(OBJS) $(HDRS)
	$(CC) -o  $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

student.o: student.nasm
	nasm -f elf64 student.nasm -o student.o

validate.o: validate.nasm
	nasm -f elf64 validate.nasm -o validate.o
clean:
	rm $(EXEC) $(OBJS) student.o validate.o
