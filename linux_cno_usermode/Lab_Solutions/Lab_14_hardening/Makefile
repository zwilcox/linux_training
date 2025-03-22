CC=gcc
LD=ld

CC_ARGS=-pie -fPIE -fPIC -Werror -Wall -Wextra

all: infoFinder antivirus

infoFinder.i: infoFinder.c
	$(CC) -E -o $@ $^

infoFinder_xorstrd.i: infoFinder.i
	./xorstring_preprocess.py $@ $^

infoFinder: infoFinder_xorstrd.i
	$(CC) $(CC_ARGS) -o $@ $^
	strip -s $@

antiviruslib.so: antiviruslib.c av.c
	$(CC) $(CC_ARGS) -ldl -shared -o $@ $^

avlibbuf.c: antiviruslib.so
	echo "#include \"avlibbuf.h\"" > $@
	xxd -i $^ >> $@

antivirus: antivirus.c av.c avlibbuf.c
	$(CC) $(CC_ARGS) -o $@ $^
	upx $@

clean:
	rm -f ./infoFinder_xorstrd.i
	rm -f ./infoFinder.i
	rm -f ./infoFinder
	rm -f ./antivirus
	rm -f ./avlibbuf.c
	rm -f ./antiviruslib.so

