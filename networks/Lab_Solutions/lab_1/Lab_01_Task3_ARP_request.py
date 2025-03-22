#!/usr/bin/python3.7
import socket
import struct
import cno_net_helper as cno  # cno_net_helper.py should be the same folder


# FOR LINUX - comment out the Windows line below
dev = "eno1"  # Device to send packet out
# FOR WINDOWS - comment out the Linux line above
#dev = "Intel(R) Ethernet Connection (5) I219-LM"  # Device to send packet out

def make_eth_hdr(dst_mac, src_mac, typ):
    dst = cno.mac_str_to_bin(dst_mac)
    src = cno.mac_str_to_bin(src_mac)
    return dst + src + struct.pack('>H', typ)


def make_arp_req(dst_ip, src_mac, src_ip):
    arpreq = b"\x00\x01"	# HW type
    arpreq += b"\x08\x00" 	# Protocol type
    arpreq += b"\x06"  		# HW addr len
    arpreq += b"\x04"  		# Proto addr len
    arpreq += b"\x00\x01"	# Op Code
    arpreq += cno.mac_str_to_bin(src_mac)
    arpreq += socket.inet_pton(socket.AF_INET, src_ip)
    arpreq += b"\x00" * 6  	# all zeroes
    arpreq += socket.inet_pton(socket.AF_INET, dst_ip)
    return arpreq


def send_arp_req(dst_ip, dst_mac, src_ip, src_mac):
    etherhdr = make_eth_hdr(dst_mac, src_mac, cno.ETH_TYPES['ARP'])
    arpreq = make_arp_req(dst_ip, src_mac, src_ip)
    frame = etherhdr + arpreq
    print(cno.hex_dumps(frame))
    cno.rawsend(etherhdr + arpreq, dev)


if __name__ == "__main__":

    send_arp_req("172.16.1.155", "FF:FF:FF:FF:FF:FF",
                 "172.16.1.100", "a4:bb:6d:e1:8c:80")
