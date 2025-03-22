#!/usr/bin/python3
from socket import *
import struct

'''Use/modify this script for the kohttpd webservers'''

ADDR = '127.0.0.1'
PORT = ??? 
HEADER = b"???"

s = socket(AF_INET, SOCK_STREAM)
s.connect((ADDR, PORT))

# this will be the same for all remaining labs
x = b'GET /asdf.vbs HTTP/1.1\r\n'

# this will get modified
x += HEADER + b':' 
x += b'\r\n'

# another \r\n after last header (don't remove)
x += b'\r\n'

s.send(x)

print(s.recv(1024))
