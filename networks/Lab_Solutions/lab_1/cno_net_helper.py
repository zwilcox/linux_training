import os
"""
Helper functions for CNO Dev Course Networks Series
"""
import binascii
import socket
import struct
import platform

PLATFORM = platform.system()
if PLATFORM == "Windows":
    from scapy.all import *

ETH_TYPES = {'ARP': 0x0806,
            'IP': 0x0800,
            'IPv6': 0x86dd}


### COMMON CODE ACROSS PLATFORMS ###
def hex_dumps(frame):
    output = "      0  1  2  3  4  5  6  7 |  8  9  a  b  c  d  e  f\n"
    output += "_" * 54 + "\n"
    line_index = 0
    for i, b in enumerate(frame):
        if i == 0:
            output += "%04x %02x " % (line_index, b)
            line_index += 16
            continue

        if ((i % 8) == 0) and ((i % 16) != 0):
            output += "| "
        if i % 16 == 0:
            output += "\n%04x %02x " % (line_index, b)
            line_index += 16
        else:
            output += "%02x " % b

    return output


def mac_str_to_bin(mac):
    return binascii.unhexlify(mac.replace(':', ''))

def bin_mac_to_str(mac):
    hex_mac = binascii.hexlify(mac).decode()
    res = ""
    for idx, num in enumerate(hex_mac):
        if idx > 0 and idx % 2 == 0:
            res += ":"
        res += num
    return res

def ipv4_str_to_bin(addr):
    return socket.inet_pton(socket.AF_INET, addr)

def ipv6_str_to_bin(addr):
    return socket.inet_pton(socket.AF_INET6, addr)

class err(Exception):
    def __str__(self):
        return "Malformed packet"


def cksum_ipv6(frame, dev="ens33"):
    """
    Perform IP, ICMP, UDP, and/or TCP checksum
    calculations and send raw frame.
    """

    class err(Exception):
        def __str__(self):
            return "Malformed packet"

    def checksum(data):
        # if len(data) % 2:
        #    raise err
        sum = 0
        for i in range(0, len(data)//2):
            sum += struct.unpack(">H", data[i*2:(i+1)*2])[0]
        sum = (((sum >> 16) + sum) & 0xffff) ^ 0xffff
        return sum

    try:
        type = struct.unpack(">H", frame[12:14])[0]
    except:
        raise err
    if type != 0x0800 and type != 0x86dd:
        raise Exception("Unable to checksum non-IP ethertype 0x%04x" % type)

    try:
        ippkt = frame[14:]
        ippayload = struct.unpack(">H", ippkt[4:6])[0]
        if ippayload == 0:
            # calculate payload:
            ippayload = len(ippkt[40:])
            ippkt = ippkt[:4] + struct.pack(">H", ippayload) + ippkt[6:]
        ipnexthdr = ippkt[6]

        # Upper-Layer Packet Length is 32-bit in pseudo header vice 16 bit in IPv6 header
        # If upper layer protocol includes length field, use that, if not, take payload length from ipv6 header, minus length of any extension headers

    except:
        raise err

    if ipnexthdr == 58:
        try:
            # Calculate payload without extension headers:
            ip_pseudohdr = ippkt[8:24] + ippkt[24:40] + b"\x00\x00" + \
                struct.pack(">H", ippayload) + b"\x00\x00\x00" + ippkt[6:7]
            icmpsum = checksum(ip_pseudohdr+ippkt[40:])
            ippkt = ippkt[:40+2] + struct.pack(">H", icmpsum) + ippkt[40+4:]
        except:
            raise err
    elif ipnexthdr == 17:
        # UDP
        try:
            udp_payload = struct.unpack(">H", ippkt[40+4:40+6])
            ip_pseudohdr = ippkt[8:24] + ippkt[24:40] + b"\x00\x00" + \
                str(struct.pack(">H", udp_payload[0])
                    ) + b"\x00\x00\x00" + ippkt[6]
            sum = checksum(ip_pseudohdr + ippkt[40:])
            sumoff = 40 + {6: 16, 17: 6}[ipnexthdr]
            udp_len = len(ippkt[40:])-1
            ippkt = ippkt[:sumoff] + struct.pack(">H", sum) + ippkt[sumoff+2:]
            # length auto-calc:
            ippkt = ippkt[:44] + struct.pack(">H", udp_len) + ippkt[46:]
        except:
            raise err
    elif ipnexthdr == 6:
        # TCP
        try:
            ip_pseudohdr = ippkt[8:24] + ippkt[24:40] + b"\x00\x00" + struct.pack(">H", ippayload) + b"\x00\x00\x00" + ippkt[6:7]
            sum = checksum(ip_pseudohdr + ippkt[40:])
            sumoff = 40 + {6: 16, 17: 6}[ipnexthdr]
            ippkt = ippkt[:sumoff] + struct.pack(">H", sum) + ippkt[sumoff+2:]
        except:
            raise err
    else:
        raise Exception("Unable to checksum IP proto 0x%02x" % ipnexthdr)

    return frame[:14] + ippkt



def cksum_ipv4(frame):
    """
    A slightly optimized version of the cno net helper checksum algorithm
    """

    def checksum(data):
        length = len(data)
        sum = 0
        for i in range(0, length//2):
            sum += struct.unpack(">H", data[i*2:(i+1)*2])[0]
        sum = (((sum >> 16) + sum) & 0xffff) ^ 0xffff
        return sum
        
    ippkt = frame[14:]
    iphlen = (ippkt[0] & 0x0f) * 4
    ipsum = checksum(ippkt[:iphlen])
    ippkt = b''.join([ippkt[:10], struct.pack(">H", ipsum), ippkt[12:]])
    iptotlen = struct.unpack(">H", ippkt[2:4])[0]

    pseudohdr = b''.join([ippkt[12:20], b"\x00", ippkt[9:10], struct.pack(">H", iptotlen - iphlen)])
    sum = checksum(b''.join([pseudohdr, ippkt[iphlen:iptotlen]]))
    sumoff = iphlen + 16
    ippkt = b''.join([ippkt[:sumoff], struct.pack(">H", sum), ippkt[sumoff+2:]])

    return b''.join([frame[:14], ippkt])


class Windows:
    PYPCAP_DEVICES = ""
    def __init__(self):
        self.setup()
        self.list_devices()

    def setup(self):
        if not Windows.PYPCAP_DEVICES:
            Windows.PYPCAP_DEVICES = IFACES.show(print_result=False)

    def list_devices(self):
        print("\n  List of available devices to send/recieve from:\n  Use the name from the 'Name' column as your dev argument to the raw_send family of functions\n  You can also find this information from 'ipconfig /all' under the Description column.\n")
        print(Windows.PYPCAP_DEVICES)
    
    def send_raw_packet(self,frame,dev):
        socket = conf.L2socket(iface=dev)
        socket.send(frame)

    def recv_raw_with_callback(self,dev,callback):
        socket = conf.L2socket(iface=dev)
        #let the callback handle the packet parsing
        while True:
            try:
                data = socket.recv_raw()[1]
                if data:
                    callback(data)
            except KeyboardInterrupt: #ctrl-c
                socket.close()
                break
        print("Stopped recieving...")
class Linux:
    
    def __init__(self):
        self.setup()
    
    def setup(self):
        if os.geteuid() != 0:
            print("WARNING:\
                You are not root, you probably should be.\
                ...especially if this is somebody else's box ;-)\
                ")
            exit(-1)

    def send_raw_packet(self,frame, dev):
        sock = socket.socket(socket.PF_PACKET, socket.SOCK_RAW)
        sock.bind((dev, 0))
        sock.send(frame)
        sock.close()
    
    def recv_raw_with_callback(self,dev,callback):
        s = socket.socket( socket.AF_PACKET, socket.SOCK_RAW, socket.ntohs(0x0003))
        s.bind((dev, 0))
        continue_recieving = True
        while True:
            try:
                data = s.recv(4096)
                if data:
                    callback(data)
            except KeyboardInterrupt: #ctrl-c
                s.close()
                break
        print("Stopped recieving...")

#putting these here to get initialization out of the way on first import
if PLATFORM == "Windows":
    NETWORKS_CLASS = Windows()
else:
    NETWORKS_CLASS = Linux() 

# For Linux, dev will be the device name found by 'ip addr show' (i.e. ens160)
# For Windows, dev will be the string corresponding to the InterfaceDescription from ipconfig /all

### SENDING FUNCTIONS ###
def rawsend(frame,dev):
     NETWORKS_CLASS.send_raw_packet(frame,dev.strip())

def rawsend_cksum_ipv4(frame,dev):
    frame_with_cksum = cksum_ipv4(frame)
    NETWORKS_CLASS.send_raw_packet(frame_with_cksum,dev.strip())

def rawsend_cksum_ipv6(frame,dev):
    frame_with_cksum = cksum_ipv6(frame)
    NETWORKS_CLASS.send_raw_packet(frame_with_cksum,dev.strip())

### RECIEVING FUNCTIONS ###
def rawrecv_with_callback(dev,callback):
    NETWORKS_CLASS.recv_raw_with_callback(dev.strip(),callback)



