CC=gcc
CFLAGS=-Wall -Werror -Wextra

obj-m += xordriver.o

.PHONY: all clean

all: xordriver xoruser

xordriver: xordriver.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

xoruser: xoruser.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f ./xoruser
	rm -f mydev
