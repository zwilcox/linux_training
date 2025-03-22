CC=gcc
LD=ld

SECT=malleable

CC_ARGS=-pie -fPIE -fPIC -Werror -Wall -Wextra

all: badwhere

badwhere.i: badwhere.c
	$(CC) -E -o $@ $^

badwhere_xorstrd.i: badwhere.i
	python3 ./xorstring_preprocess.py $@ $^

badwhere: badwhere_xorstrd.i badwhere.S
	$(CC) $(CC_ARGS) -o $@ $^
	python3 ./bswapsection.py $@ ${SECT}
	#strip -s $@

clean:
	rm -f ./badwhere_xorstrd.i
	rm -f ./badwhere.i
	rm -f ./badwhere

