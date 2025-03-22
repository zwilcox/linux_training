#!/user/bin/python3.7
import socket
import struct

###########################################################
#
#   Lab 4 Task 2 - Multicast Server
#
#       Server that can receive UDP datagrams on
#       Multicast address
#
###########################################################

"""
Phase 1:
man ipv6
/usr/include/linux/in6.h
pydoc3 socket (look in the DATA section near the end)
man setsockopt (what are the arguments to setsockopt?)

Phase 2:
search rfc for ipv6_mreq
rfc3493.txt
IPV6_JOIN_GROUP (It used to be IPV6_ADD_MEMBERSHIP but that was obsoleted by later RFC’s).
It's defined in /usr/linux/x86_64-linux-gnu/bits/in.h
"""


# Configure address info for connection
dst_ip = "FF02::777:777:777"
dst_port = 7777

# Create and configure UDP socket (DGRAM)
my_sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)

# OPTIONAL: Allow multiple copies of this program on one machine
my_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Bind it to the port
my_sock.bind(('', dst_port))

"""
group_ipv6 = socket.inet_pton(socket.AF_INET6, dst_ip)
inet_pton -> Converts an IPv6 address (dst_ip) from its string representation (e.g., "ff02::1") to its binary form.
The socket.AF_INET6 indicates that the address is IPv6.
The result (group_ipv6) is a 16-byte binary representation of the IPv6 address.
"""
group_ipv6 = socket.inet_pton(socket.AF_INET6, dst_ip)

"""
group = group_ipv6 + struct.pack('@I', 0)
Combines the binary IPv6 address (group_ipv6) with a 4-byte integer packed using struct.pack('@I', 0):
@I specifies the packing format:
@ means native byte order, standard size, and alignment.
I means an unsigned 32-bit integer (4 bytes).

In this case, the integer is 0, representing the interface index. This tells the system which network interface to use for multicast traffic.
A value of 0 means to use the default interface.

group is now a 20-byte structure: 16 bytes for the IPv6 address and 4 bytes for the interface index.
"""
group_mreq = group_ipv6 + struct.pack('@I',0)

"""
my_sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP, group_mreq)
Configures the socket (my_sock) to join the specified IPv6 multicast group.
socket.IPPROTO_IPV6 specifies that this is an IPv6-level socket option.
socket.IPV6_JOIN_GROUP is the socket option to join an IPv6 multicast group. -> takes in the ipv6_mreq struct (group_mreq)
The group_mreq argument tells the system which multicast group to join (IPv6 address + interface index).
"""
my_sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP, group_mreq)


# Loop, printing any data we receive
while True:
    data, sender = my_sock.recvfrom(1500)
    print(str(sender) + '  ' + repr(data.decode()))
