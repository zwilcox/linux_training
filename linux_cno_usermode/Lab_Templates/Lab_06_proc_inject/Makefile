CC=gcc

all: auth

auth: auth.c
	$(CC) -fPIE -pie -o $@ $^

clean:
	rm -f ./auth

