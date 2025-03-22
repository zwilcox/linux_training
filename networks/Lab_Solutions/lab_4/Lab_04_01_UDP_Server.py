#!/user/bin/python3.7
import socket

# socket setup
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", 8888))

while(True):
    msg, addr = sock.recvfrom(1024)
    print(addr, msg)
    sock.sendto(b"This is my response...", (addr))
