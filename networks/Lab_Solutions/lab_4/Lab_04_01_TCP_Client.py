import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("172.16.1.100", 8888))
s.send(b"Hello, my name is packet\n")
s.shutdown(socket.SHUT_WR)
print(s.recv(1024))
s.close
