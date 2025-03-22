import socket
import struct

_MSG_ECHO = 0
_MSG_HEXI = 1
_MSG_ADDR = 2
_MSG_DELR = 3
_MSG_CONS = 4
_MSG_MAX = 5

class Client:
        def __init__(self, host, port):
                self.host = host
                self.port = port
                self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        def connect(self):
                self.sock.connect((self.host, self.port))
        def run(self):
                ''' Feel free to modify the message contents
                and header to find exploits in the provided
                server code'''
                message = "Hello World %p %p %p %p %p %p %p %p"
                payload = message.encode()
                header = struct.pack("<hh", _MSG_CONS, len(payload))
                self.sock.sendall(header)
                self.sock.sendall(payload)
                response = self.sock.recv(1024)
                print("Response: %s", response.decode())
        def close(self):
                self.sock.close()

def main():
        client = Client("localhost", 6969)
        client.connect()
        client.run()
        client.close()

if __name__ == "__main__":
        main()
