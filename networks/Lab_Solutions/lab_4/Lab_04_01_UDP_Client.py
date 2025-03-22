import socket

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.sendto(b"Hello, UDP",("172.16.1.100", 8888))
print(s.recvfrom(1024))
s.close()
