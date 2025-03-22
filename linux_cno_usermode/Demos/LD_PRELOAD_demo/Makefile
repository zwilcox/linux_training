CC = gcc
EXEC = target
LIB = preloadthis.so
CFLAGS = -Wall -Werror

.PHONY:	all clean

all: $(EXEC) $(LIB)

$(EXEC): target.c
	$(CC) $(CFLAGS) -o $@ $^

$(LIB): preloadthis.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $< -ldl

clean:
	rm -f $(EXEC) $(LIB)
