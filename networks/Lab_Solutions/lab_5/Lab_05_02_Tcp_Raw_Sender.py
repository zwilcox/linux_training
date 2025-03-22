import socket
import struct
import configparser
import cno_net_helper as cno
import cProfile
import argparse
from time import sleep

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("target", type=str, help="The target configuration")
    parser.add_argument("-6", "--ipv6", action="store_true", help="Use IPv6")
    parser.add_argument('-i', '--interface', type=str, help="The interface that yo uwill be sending on", default='eno1')
    parser.add_argument('-p', '--profile', action="store_true", help="A flag to indication that cProfile should be used")
    return parser.parse_args()

args = get_args()
dev = args.interface

# Read in my config file
config = configparser.ConfigParser()
config.read('config.txt')
ports = range(int(config[args.target]['ports'].split('-')[0]), int(config[args.target]['ports'].split('-')[1]))

def send_syn6():
# Build our tcp packet
    dst = cno.mac_str_to_bin(config[args.target]['mac']) # target the victim
    src = cno.mac_str_to_bin(config['me']['mac'])     # My mac address
    typ = struct.pack(">H", cno.ETH_TYPES['IPv6'])    # IPv6 type
    etherhdr = dst + src + typ

    # tcp
    tcp_hdr = struct.pack(">H", int(config['me']['ports']))    # src port (connected host)
    tcp_hdr += struct.pack(">H", 0)        # dst port (victim's side)
    tcp_hdr += struct.pack(">L", 12345678)            # sequence number (from earlier packet)
    tcp_hdr += struct.pack(">L", 87654321)            # acknowledgement number (calculated from packet)
    tcp_hdr += b"\x50\x02"        # header length 20 bytes, then flags (ACK+RST)
    tcp_hdr += b"\x00\x00"        # Window size
    tcp_hdr += b"\x00\x00"        # checksum
    tcp_hdr += b"\x00\x00"        # urgent pointer

    ipv6_hdr = b"\x60\x00\x00\x00"  # Version (4 bit): 6 (0x6)
    ipv6_hdr += struct.pack(">H", len(tcp_hdr))
    ipv6_hdr += b"\x06"             # Next header: 54 (ICMPv6)
    ipv6_hdr += b"\xff"             # Max hop limit: 255
    ipv6_hdr += socket.inet_pton(socket.AF_INET6, config['me']['ipv6'])            # Source IPv6 adresses
    ipv6_hdr += socket.inet_pton(socket.AF_INET6, config[args.target]['ipv6'])     # Destination ICMPv6
    
    # Put our payload together
    payload = etherhdr + ipv6_hdr + tcp_hdr
    
    # Only put the port modification in the for loop so that we have to do less
    # inside the big loop
    for port in ports:
        payload = b''.join([payload[:56], struct.pack(">H", port), payload[58:]])
        cno.rawsend_cksum_ipv6(payload,dev)
        print(f"Sending packet to port {port}")


def send_syn():
    # Build our tcp packet
    dst = cno.mac_str_to_bin(config[args.target]['mac']) # target the victim
    src = cno.mac_str_to_bin(config['me']['mac'])     # My mac address
    typ = b"\x08\x00"                    # IPv4 type
    etherhdr = dst + src + typ

    # ipv4hdr
    ipv4hdr = b"\x45"       # version 4, header length 5 (*4 bytes)
    ipv4hdr += b"\x00"      # type of service
    ipv4hdr += b"\x00\x28"  # total length including internet header and data
    ipv4hdr += b"\xff\xff"  # identification
    ipv4hdr += b"\x00\x00"  # 3 bits flags, 13 bits fragment offset
    ipv4hdr += b"\xff"
    ipv4hdr += b"\x06"      # protocol TCP
    ipv4hdr += b"\x00\x00"  # hdr checksum
    ipv4hdr += socket.inet_pton(socket.AF_INET, config['me']['ip'])      # source IP
    ipv4hdr += socket.inet_pton(socket.AF_INET, config[args.target]['ip'])  # destination IP

    # tcp
    hdr3 = struct.pack(">H", int(config['me']['ports']))    # src port (connected host)
    hdr3 += struct.pack(">H", 0)        # dst port (victim's side)
    hdr3 += struct.pack(">L", 0)            # sequence number (from earlier packet)
    hdr3 += struct.pack(">L", 0)            # acknowledgement number (calculated from packet)
    hdr3 += b"\x50\x02"        # header length 20 bytes, then flags (SYN)
    hdr3 += b"\x00\x00"        # Window size
    hdr3 += b"\x00\x00"        # checksum
    hdr3 += b"\x00\x00"        # urgent pointer
    
    # Put our payload together
    payload = etherhdr + ipv4hdr + hdr3
    
    # Only put the port modification in the for loop so that we have to do less
    # inside the big loop
    for port in ports:
        payload = b''.join([payload[:36], struct.pack(">H", port), payload[38:]])
        print(f"Sending packet to port {port}")
        cno.rawsend_cksum_ipv4(payload,dev)

if args.ipv6:
    send_syn6()
elif args.profile:
    print(cProfile.run("send_syn()"))    
else:    
    sleep(0.5)
    send_syn()
    sleep(0.5)
