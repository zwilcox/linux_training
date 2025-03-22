my_module-objs := hijack.o int_hook.o
obj-m += my_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o div divide.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm div
