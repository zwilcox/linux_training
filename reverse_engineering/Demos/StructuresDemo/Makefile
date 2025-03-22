CC=gcc
CFLAGS=-Wall -masm=intel -01
HDRS:= $(wildcard *.h)
EXECS:= structures solcheck
all: $(EXECS)

$(EXECS): % : %.o
	$(CC) $(CFLAGS) -o $@ $^

%.o : %.c $(HDRS)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECS)
	rm -f *.o
