CC=gcc

all: example_lazy example_now example_exec_lazy example_exec

example_lazy: example.c
	$(CC) -g -pie -fPIE -zlazy -znorelro -o $@ $^

example_now: example.c
	$(CC) -g -pie -fPIE -znow -zrelro -o $@ $^

example_exec_lazy: example.c
	$(CC) -g -zlazy -znorelro -o $@ $^

example_exec: example.c
	$(CC) -g -o $@ $^

clean:
	rm -f ./example_lazy ./example_now ./example_exec_lazy ./example_exec
