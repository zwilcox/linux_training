CC = gcc
EXEC = srv
LIB = libdebug.so

.PHONY:	template solution clean

template: $(EXEC)
solution: $(EXEC) $(LIB)

$(EXEC): server.c
	$(CC) -L. -Wl,-rpath=. -Wl,--export-dynamic -o $@ $^ -ldl -lpthread
	strip $@
	chmod u+s $@

$(LIB): injectlib.o
	$(CC) -shared -o $@ $< -ldl

injectlib.o: injectlib.c
	$(CC) -c -fPIC -o $@ $<

clean:
	rm -f $(EXEC) $(LIB) *.o
