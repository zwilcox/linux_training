obj-m := roothelper.o

all: rooter sandboxer
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod roothelper.ko
	sudo ln -s /dev/roothelper roothelper
	sudo chmod 0666 roothelper
	sudo chown student:student roothelper

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f rooter roothelper sandboxer
	sudo rmmod -f roothelper

rooter: rooter.c hexdump.c hexdump.h
	gcc -o rooter rooter.c hexdump.c

sandboxer: sandboxer.c bpf-helper.c bpf-helper.h
	gcc -o sandboxer sandboxer.c bpf-helper.c
