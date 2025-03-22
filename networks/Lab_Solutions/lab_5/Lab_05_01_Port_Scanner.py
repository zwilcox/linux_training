#!/usr/bin/env python3
import argparse
import socket
import cProfile


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--ip", required=True,
                        type=str, help="The target IP address")
    parser.add_argument("-6", "--ipv6", action="store_true", help="Use IPv6")
    parser.add_argument('-sp', '--start-port', type=int, help="Start Port", default=0)
    parser.add_argument('-ep', '--end-port', type=int, help="End port", default=65535)
    return parser.parse_args()


def scan_tcp(addr, use_ipv6, start_port, end_port):
    port_state = {}
    for i in range(start_port, end_port+1):
        if use_ipv6:
            s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        else:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(0.01)
        try:
            s.connect((addr, i))
            port_state[i] = "open"
            s.close()
        except socket.gaierror as e:
            raise RuntimeError(
                f"Could not get address info for: {addr}"
            ) from e
        except socket.timeout as e:
            port_state[i] = "closed"
        except socket.error as e:
            if e.errno == 111:  # connection refused
                port_state[i] = "closed"
    return port_state


if __name__ == "__main__":
    args = get_args()
    #cProfile.run("scan_tcp(args.ip, args.ipv6, args.start_port, args.end_port)")
    print("Scanning")
    state = scan_tcp(args.ip, args.ipv6, args.start_port, args.end_port)
    print("Done Scanning")
    for port in state:
        if state[port] == "open":
            print(f"tcp/{port}\t{state[port]}")
