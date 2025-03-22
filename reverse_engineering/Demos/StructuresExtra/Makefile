CC=gcc
CFLAGS=-Wall -masm=intel -O1
HDRS:= $(wildcard *.h)
EXECS:= structures_extra solcheck
all: $(EXECS)

$(EXECS): % : %.o
	$(CC) $(CFLAGS) -o $@ $^
	strip -K main $@

%.o : %.c $(HDRS)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECS)
	rm -f *.o
