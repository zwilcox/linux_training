import socket
import signal
import struct
import binascii
import argparse
import configparser
import cno_net_helper as cno

IP_PROTOS = {
    1: 'ICMP',
    6: 'TCP',
    17: 'UDP'
}

seen = []

def receiveSignal(signalNumber, fram):
    """
    Catch signal so we can print ports that were found
    """
    print(f"Open ports {sorted(list(seen))}")
    raise SystemExit("Sniffer is done")

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('target', type=str)
    parser.add_argument('-6', '--ipv6', action="store_true")
    parser.add_argument('-i', '--interface', default='eno1')
    return parser.parse_args()

args = get_args()
config = configparser.ConfigParser()
config.read('config.txt')

my_port = int(config['me']['ports'])
if args.ipv6:
    protocol = b"\x86\xdd"
    target_ip = config[args.target]['ipv6']
else:
    protocol = b"\x08\x00"
    target_ip = config[args.target]['ip']

signal.signal(signal.SIGINT, receiveSignal)

class EthernetHdr:
    """
    Parse out Ethernet Header data.
    """

    def _bin_mac_to_str(self, mac):
        hex_mac = binascii.hexlify(mac).decode()
        res = ""
        for idx, num in enumerate(hex_mac):
            if idx > 0 and idx % 2 == 0:
                res += ":"
            res += num
        return res

    def __init__(self, data):
        self.src_raw = data[6:12]
        self.dst_raw = data[:6]
        self.dst = self._bin_mac_to_str(self.dst_raw)
        self.src = self._bin_mac_to_str(self.src_raw)
        self.typ = data[12:14]

    def __str__(self):
        res = f"Ethernet:\n"
        res += f"  dst: {self.dst}\n"
        res += f"  src: {self.src}\n"
        res += f"  type: {self.typ}"
        return res

class Ipv4Hdr:
    """
    Parse out Ipv4Hdr data
    """
    IP_PROTOS = {1: 'ICMP',
                 6: 'TCP',
                 17: 'UDP'}

    def __init__(self, data):
        self.version = (data[0]&0xf0) >> 4
        self.hdr_len = data[0]&0xf
        self.diff = data[1]
        self.tot_len = struct.unpack(">H", data[2:4])[0]
        self.id = struct.unpack(">H", data[4:6])[0]
        self.flags = (data[6]&0xe0) >> 5
        self.fragment_off = struct.unpack(">H", data[6:8])[0] & 0x1fff
        self.ttl = data[8]
        self.proto = data[9]
        self.cksum = struct.unpack(">H", data[10:12])[0]
        self.src_ip_raw = data[12:16]
        self.dst_ip_raw = data[16:20]
        self.src_ip = socket.inet_ntop(socket.AF_INET, data[12:16])
        self.dst_ip = socket.inet_ntop(socket.AF_INET, data[16:20])

    def __str__(self):
        res = f"IPv4:\n"
        res += f"  Version: {self.version}\n"
        res += f"  Header Length: {self.hdr_len * 4} Bytes ({self.hdr_len})\n"
        res += f"  Differentiated Services: {hex(self.diff)}\n"
        res += f"  Total Length: {self.tot_len}\n"
        res += f"  Identification: {hex(self.id)}\n"
        res += f"  Flags: {format(self.flags, '03b')}\n"
        res += f"  Time to Live: {self.ttl}\n"
        res += f"  Protocol: {self.IP_PROTOS.get(self.proto, 'Unknown')} ({self.proto})\n"
        res += f"  Header Checksum: {hex(self.cksum)}\n"
        res += f"  Source Address: {self.src_ip}\n"
        res += f"  Destination Address: {self.dst_ip}"
        return res

class Ipv6Hdr:
    """
    Parse the Ipv6 hdr of a packet
    """
    IPV6_PROTOS = { 0: 'IPv6 Hop-by-Hop Option',
                 6: 'TCP',
                 17: 'UDP',
                 43: 'Routing Header for Ipv6',
                 44: 'Fragment Header for Ipv6',
                 50: 'Encapsulating Security Payload',
                 51: 'Authentication Header',
                 58: 'ICMPv6',
                 60: 'Destination Options for Ipv6',
                 135: 'Mobility Header',
                 139: 'Host Identity Protocol',
                 140: 'Shim6 Protocol',
                 253: 'Use for experimentation and testing',
                 254: 'Use for experimentation and testing',
                 }

    def __init__(self, data):
        self.version = (data[0]&0xf0) >> 4
        self.traffic_cls = (struct.unpack(">H", data[0:2])[0] & 0x0ff0) >> 4
        self.flow_label = struct.unpack(">L", data[0:4])[0] & 0x000fffff
        self.payload_len = struct.unpack(">H", data[4:6])[0]
        self.next_hdr = data[6]
        self.hop_limit = data[7]
        self.src_ip_raw = data[8:24]
        self.dst_ip_raw = data[24:40]
        self.src_ip = socket.inet_ntop(socket.AF_INET6, data[8:24])
        self.dst_ip = socket.inet_ntop(socket.AF_INET6, data[24:40])

    def __str__(self):
        res = f"IPv6:\n"
        res += f" Version: {self.version}\n"
        res += f" Traffic Class: {hex(self.traffic_cls)}\n"
        res += f" Flow Label: {hex(self.flow_label)}\n"
        res += f" Payload Length: {self.payload_len}\n"
        res += f" Next Header: {self.IPV6_PROTOS.get(self.next_hdr, 'Unknown')} ({self.next_hdr})\n"
        res += f" Hop Limit: {self.hop_limit}\n"
        res += f" Src: {self.src_ip}\n"
        res += f" Dst: {self.dst_ip}"
        return res

class TcpHdr:
    """
    Parse out Tcp header data.
    """
    def __init__(self, data):
        """
        Parse the binary blob into a tcp hdr.
        """
        self.src_port_raw = data[0:2]
        self.dst_port_raw = data[2:4]
        self.src_port = struct.unpack(">H", data[0:2])[0]
        self.dst_port = struct.unpack(">H", data[2:4])[0]
        self.seq_no = struct.unpack(">L", data[4:8])[0]
        self.ack_no = struct.unpack(">L", data[8:12])[0]
        self.hdr_len = (data[12] & 0xf0) >> 4
        self.flags = struct.unpack(">H", data[12:14])[0] & 0x0fff
        self._assign_flags()
        self.window = struct.unpack(">H", data[14:16])[0]
        self.cksum = struct.unpack(">H", data[16:18])[0]
        self.urg_ptr = struct.unpack(">H", data[18:20])[0]

    def _assign_flags(self):
        """
        Add boolean field for each flag.
        """
        if self.flags & 0x1 == 0x1:
            self.FIN = True
        else:
            self.FIN = False

        if self.flags & 0x2 == 0x2:
            self.SYN = True
        else:
            self.SYN = False

        if self.flags & 0x4 == 0x4:
            self.RST = True
        else:
            self.RST = False

        if self.flags & 0x8 == 0x8:
            self.PSH = True
        else:
            self.PSH = False

        if self.flags & 0x10 == 0x10:
            self.ACK = True
        else:
            self.ACK = False

        if self.flags & 0x20 == 0x20:
            self.URG = True
        else:
            self.URG = False
        
        if self.flags & 0x40 == 0x40:
            self.ECN = True
        else:
            self.ECN = False

        if self.flags & 0x80 == 0x80:
            self.CWR = True
        else:
            self.CWR = False

    def _flags_to_str(self):
        """
        Convert flags to string representation
        """
        res = ""
        if self.FIN:
            res += "FIN "
        if self.SYN:
            res += "SYN "
        if self.RST:
            res += "RST "
        if self.PSH:
            res += "PSH "
        if self.ACK:
            res += "ACK "
        if self.URG:
            res += "URG "
        if self.ECN:
            res += "ECN "
        if self.CWR:
            res += "CWR "
        return res

    def __str__(self):
        res = f"TCP:\n"
        res += f"  src port: {self.src_port}\n"
        res += f"  dst_port: {self.dst_port}\n"
        res += f"  flags: {self._flags_to_str()}"
        return res

def handle_packet(data):
    eth_hdr = EthernetHdr(data[0:14])
    if eth_hdr.typ != protocol:
        return
    if args.ipv6:
        ip_hdr = Ipv6Hdr(data[14:54])
        if ip_hdr.next_hdr != 6:
            return
        tcp_hdr = TcpHdr(data[54:74])
    else:
        ip_hdr = Ipv4Hdr(data[14:34])
        if ip_hdr.proto != 6:
            return
        tcp_hdr = TcpHdr(data[34:54])
    if tcp_hdr.dst_port == my_port:
        if tcp_hdr.SYN and tcp_hdr.ACK and not tcp_hdr.src_port in seen:
            print(f"Open port {tcp_hdr.src_port} on {target_ip}")
            seen.append(tcp_hdr.src_port)
        elif tcp_hdr.RST:
            #print(f"Closed port {tcp_hdr.src_port} on {target_ip}")
            pass #putting a pass to limit verbosity

if __name__ == '__main__':
    cno.rawrecv_with_callback(args.interface,handle_packet)
