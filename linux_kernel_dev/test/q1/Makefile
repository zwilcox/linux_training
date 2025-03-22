all:
	gcc -o proga proga.c
	gcc -o progb progb.c
	touch /home/student/myfile
	touch /tmp/myfile
	sudo trace-cmd ??
	sudo trace-cmd report > a.txt
	sudo trace-cmd ??
	sudo trace-cmd report > b.txt
	sed -n -e 's/^.*| //w acalls.txt' a.txt
	sed -n -e 's/^.*| //w bcalls.txt' b.txt
	
clean:
	rm -f proga progb /tmp/myfile /home/student/myfile a.txt b.txt trace.dat
