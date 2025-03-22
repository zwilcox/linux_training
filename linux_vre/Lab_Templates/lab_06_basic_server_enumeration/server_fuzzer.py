#!/usr/bin/python3
from socket import *
import struct

'''Use/modify this script for the kohttpd webservers'''

ADDR = 'localhost'
PORT = 31337
#HEADER = b"Debug"

s = socket(AF_INET, SOCK_STREAM)
s.connect((ADDR, PORT))

# this will be the same for all remaining labs
x = b'GET / HTTP/1.1 Host: 127.0.0.1\r\n'

# this will get modified
x += b"Debug" + b':'
x += b'\r\n'

# another \r\n after last header (don't remove)
x += b'\r\n'

s.send(x)

print(s.recv(1024))
