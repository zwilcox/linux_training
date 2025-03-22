#!/user/bin/python3.7
import socket
import time
import struct

###########################################################
#
#   Lab 4 Task 2 - Multicast Client
#
#       Client that can send UDP datagrams on
#       Multicast address
#
###########################################################

# Configure address info for connection
dst_ip = "FF02::777:777:777"
dst_port = 7777
# Create and configure UDP socket (DGRAM)

my_sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)

for i in range(1, 99999999):
    data = (b"Client packet #%d\n" % (i))
    print(my_sock.sendto(data, (dst_ip, dst_port)))
    time.sleep(10)

my_sock.close()
