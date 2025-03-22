CC=gcc

all: whosthebest injector badso.so

whosthebest: whosthebest.c
	$(CC) -Wall -Werror -fPIE -pie -o $@ $^
	sudo chown root:root $@
	sudo chmod +s $@

injector: injector.c
	$(CC) -Wall -Werror -fPIE -pie -o $@ $^

badso.o: badso.c
	$(CC) -c -fPIC -o $@ $^

badso.so: badso.o
	$(CC) -shared -o $@ $^
	rm -f ./badso.o

clean:
	rm -f ./injector
	rm -f ./badso.o
	rm -f ./badso.so
	rm -f ./whosthebest
