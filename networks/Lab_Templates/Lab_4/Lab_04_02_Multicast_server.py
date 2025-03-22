#!/usr/bin/python3.7
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

# TODO: Configure address info for connection
dst_ip = ""
dst_port = 0

# Create and configure UDP socket (DGRAM)
my_sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)

# Bind it to the port
my_sock.bind(('', dst_port))

# TODO: Configure socket for multicast
############################################################################################
#   For help getting subscribed to the multicast group look here:
#       search rfc for ipv6_mreq -> rfc3493.txt
#
#       /usr/include/linux/in6.h (struct ipv6_mreq)
#       man setsockopt (what are the arguments to setsockopt?)
###########################################################################################



# Loop, printing any data we receive
while True:
    data, sender = my_sock.recvfrom(1500)
    print(str(sender) + '  ' + repr(data.decode()))
