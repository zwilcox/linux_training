CC=gcc
SSL=./openssl-1.0.1f
CFLAGS=-Wall -Werror -Wextra -I$(SSL)/include
LIBS=-pthread -ldl -L$(SSL)/lib -l:libssl.a -l:libcrypto.a

all: server cert.pem key.pem

server: server.c cert.pem key.pem
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
	strip $@ -K main

key.pem:
	openssl req -batch -newkey rsa:2048 -nodes -keyout $@

cert.pem: key.pem
	# generate the certificate and key
	openssl req -batch -newkey rsa:2048 -key $< -x509 -days 365 -out $@
clean:
	rm -f cert.pem
	rm -f key.pem
	rm -f server


