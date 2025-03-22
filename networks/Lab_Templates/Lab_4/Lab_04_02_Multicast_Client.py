#!/usr/bin/python3.7
import socket
import time

###########################################################
#
#   Lab 4 Task 2 - Multicast Client
#
#       Client that can send UDP datagrams on
#       Multicast address. This can be used for
#       testing your multicast server
#
###########################################################

# Configure address info for connection
dst_ip = ""
dst_port = 0
my_sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)

for i in range(1, 99999999):
    data = (b"Sending packet #%d\n" % (i))
    my_sock.sendto(data, (dst_ip, dst_port))
    time.sleep(5)

my_sock.close()
