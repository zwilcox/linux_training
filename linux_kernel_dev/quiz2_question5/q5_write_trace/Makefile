all:
	gcc -o proga proga.c
	gcc -o progb progb.c
	sudo trace-cmd record -p function_graph -g __x64_sys_write -F ./proga
	sudo trace-cmd report > a.txt
	sudo trace-cmd record -p function_graph -g __x64_sys_write -F ./progb
	sudo trace-cmd report > b.txt
	sed -n -e 's/^.*| //w acalls.txt' a.txt
	sed -n -e 's/^.*| //w bcalls.txt' b.txt

clean:
	rm -f proga progb a.txt b.txt trace.dat acalls.txt bcalls.txt
