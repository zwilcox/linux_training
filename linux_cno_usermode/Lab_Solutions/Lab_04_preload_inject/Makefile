CC=gcc
LD=ld
LDSCRIPT=linkerscript.ld

all: nochill.so patience

simplesum: simplesum.c
	$(CC) -o $@ $^

patience.o: patience.c
	$(CC) -g -fPIE -pie -c -o $@ $^ 

patience: patience.o simplesum
	$(CC) -T $(LDSCRIPT) -o $@ $< -lpthread
	objcopy --dump-section .text=out.bin $@
	cat out.bin | ./simplesum > hash.o
	rm out.bin
	objcopy --update-section hash=hash.o $@
	rm hash.o
	strip -s $@

nochill.o: nochill.c
	$(CC) -c -fPIC -o $@ $^ 

nochill.so: nochill.o
	$(CC) -shared -o $@ $^

clean:
	rm -f *.o nochill.so nochill.o patience simplesum hash.S patience_pre
