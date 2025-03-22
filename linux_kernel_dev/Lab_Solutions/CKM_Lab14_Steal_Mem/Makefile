obj-m += steal.o

CC=gcc
CFLAGS=-Wall -Wextra -Werror -Wconversion

all: target steal

steal: steal.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

target: target.c
	$(CC) -pie -fPIE $(CFLAGS) -o $@ $^

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f ./target
