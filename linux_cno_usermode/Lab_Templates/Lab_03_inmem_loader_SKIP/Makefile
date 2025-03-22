CC=gcc
LD=ld

CC_ARGS=-pie -fPIE -Werror -Wall -g

LDSCRIPT=linkerscript.ld

all: ldr_hdr.bin nasmline shelltest

ldr_hdr.bin: $(LDSCRIPT) ldr.o ldr_lib.o
	$(LD) --orphan-handling=error -T $^

ldr.o: ldr.S
	$(CC) $(CC_ARGS) -nostdlib -c -o $@ $^

ldr_lib.o: ldr_lib.c
	$(CC) $(CC_ARGS) -nostdlib -c -o $@ $^

nasmline: nasmline.c
	$(CC) $(CC_ARGS) -o $@ $^

shelltest: shelltest.c
	$(CC) $(CC_ARGS) -o $@ $^

clean:
	rm -f ./ldr_hdr.bin
	rm -f ./ldr.o
	rm -f ldr_lib.o
	rm -f nasmline
	rm -f shelltest
