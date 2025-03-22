import socket
import ssl
import argparse
from pathlib import Path

def create_ssl_server(port, cert, key):
    addr = ('', port)
    
    if socket.has_dualstack_ipv6():
        server_sock = socket.create_server(addr, family=socket.AF_INET6, dualstack_ipv6=True)
    else:
        server_sock = socket.create_server(addr, reuse_port=True)
    wrapper = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
    wrapper.load_cert_chain(str(cert), keyfile=str(key))
    with wrapper.wrap_socket(server_sock, server_side=True) as ssock:
        while True:
            ssl_client, fromaddr = ssock.accept()
            print(f"Accepted connection from: {fromaddr}")
            try:
                data = ssl_client.read()
                while data:
                    print(f"Received: {data}")
                    ssl_client.send(b'Thanks')
                    break
            finally:
                ssl_client.shutdown(socket.SHUT_RDWR)
                ssl_client.close()

def create_ssl_client(addr, hostname='pyctf.class.net', server_crt='server.crt'):
    # Generate a default context. Python will choose reasonable defaults
    context = ssl.create_default_context()
    
    # Add our self signed cert
    context.load_verify_locations(server_crt)

    with socket.create_connection(addr) as sock:
        with context.wrap_socket(sock, server_hostname=hostname) as ssl_sock:
            ssl_sock.send(b'GET /login HTTP/1.1\r\nHost: pyctf.class.net\r\n\r\n') # double return needed for request
            for item in ssl_sock.recv(8192).split(b'\r\n'):
                print(item)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('port', type=int, help='The port you want to use to connect or listen')
    parser.add_argument('--server', action="store_true", help='A flag that makes this run as a server')
    parser.add_argument('--ip', type=str, default='127.0.0.1', help='The ip or hostname of what you want to connect to. Can be ipv4 or ipv6')
    parser.add_argument('--hostname', type=str, default='pyctf.class.net', help='The hostname to validate when connecting to a server')
    parser.add_argument('--server-crt', type=Path, default=Path('server.crt'), help='A path to the servers certificate')
    parser.add_argument('--server-key', type=Path, default=Path('server.key'), help='A path to the servers key')

    args = parser.parse_args()
    if args.server:
        create_ssl_server(args.port, cert=args.server_crt, key=args.server_key)
    else:
        create_ssl_client((args.ip, args.port), hostname=args.hostname, server_crt=args.server_crt)

