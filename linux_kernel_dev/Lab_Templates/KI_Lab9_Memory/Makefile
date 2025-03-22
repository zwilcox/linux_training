CC=gcc
CFLAGS=-Wall -Werror -Wextra

obj-m += mapdriver.o

all: mapdriver mapuser

mapdriver: mapdriver.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

mapuser: mapuser.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f ./mapuser
