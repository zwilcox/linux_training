#!/user/bin/python3.7
import socket
import select

# socket setup -> shows an example using select() -> https://docs.python.org/3.8/library/select.html#select.select
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(("", 8888))
sock.listen(socket.SOMAXCONN)
readfds = [sock]
writefds = []
eventfds = []

while(True):
    (read_socks, write_socks, event_socks) = select.select(readfds, writefds, eventfds)
    for s in read_socks:
        if s == sock:
            (conn, addr) = s.accept()
            readfds.append(conn)
        else:
            msg = s.recv(1024)
            if msg == b'':
                s.close()
                readfds.remove(s)
            else:
                s.send(b"I got you!\n")
                s.shutdown(socket.SHUT_WR)
                print(msg)
