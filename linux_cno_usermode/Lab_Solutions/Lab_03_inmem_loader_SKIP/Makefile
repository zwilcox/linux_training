CC=gcc
LD=ld

CC_ARGS=-pie -fPIE -Werror -Wall

LOADER_LDSCRIPT=loaderscript.ld

all: ldr_hdr.bin nasmline testlib.so shelltest shellpack

ldr_hdr.bin: $(LOADER_LDSCRIPT) ldr.o ldr_lib.o
	$(LD) --orphan-handling=error -T $(LOADER_LDSCRIPT)

ldr.o: ldr.S
	$(CC) $(CC_ARGS) -nostdlib -c -o $@ $^

ldr_lib.o: ldr_lib.c
	$(CC) $(CC_ARGS) -nostdlib -c -o $@ $^

nasmline: nasmline.c
	$(CC) $(CC_ARGS) -o $@ $^

shelltest: shelltest.c
	$(CC) $(CC_ARGS) -o $@ $^

testlib.so: testlib.o
	$(CC) $(CC_ARGS) -shared -o testlib.so testlib.o

testlib.o: testlib.c
	$(CC) $(CC_ARGS) -c -o $@ $^

shellpack: testlib.so ldr_hdr.bin
	cp ldr_hdr.bin shellpack
	#du -b testlib.so | python3 -c "import sys; import struct; sz = input(); sys.stdout.buffer.write(struct.pack('<I', int(sz.split()[0])))" >> shellpack
	echo "$$(printf "%08x\n" $$(stat -c "%s" testlib.so))" | rev | dd conv=swab 2>/dev/null | xxd -r -ps >> shellpack
	cat $< >> shellpack

clean:
	rm -f ./ldr_hdr.bin
	rm -f ./ldr.o
	rm -f ldr_lib.o
	rm -f nasmline
	rm -f payload.txt
	rm -f testlib.o
	rm -f testlib.so
	rm -f shelltest
	rm -f shellpack
