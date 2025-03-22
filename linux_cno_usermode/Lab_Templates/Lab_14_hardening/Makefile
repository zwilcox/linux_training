CC=gcc
LD=ld

CC_ARGS=-pie -fPIE -fPIC -Werror -Wall -Wextra

all: infoFinder

infoFinder: infoFinder.c
	$(CC) $(CC_ARGS) -o $@ $^
	strip -s $@
clean:
	rm -f ./infoFinder

