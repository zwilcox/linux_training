obj-m := keylogger.o

all: printer
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm printer

printer:
	gcc -o printer printer.c

	
