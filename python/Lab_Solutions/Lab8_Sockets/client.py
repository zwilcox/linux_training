import pickle
import socket
import struct

# 172.16.1.100
server_host = "localhost"
server_port = 5555
buf_size = 128  # arbitrarily large buffer

# IPv4, TCP by default
sock = socket.socket()

try:
    sock.connect((server_host, server_port))
    pickled_data = sock.recv(buf_size)
    print(f"Pickled data: {pickled_data}")
    data = pickle.loads(pickled_data)
    print(f"Data: {data}")

    # the reason why they do [:1] vs [0] is that [0] on a bytes object returns the integer,
    # so it wouldn't be b'<' or b'>' anymore
    endianness = data[:1]
    endianness_decoded = endianness.decode()
    print(f'Endianness: "{endianness_decoded}"')
    num1, num2 = struct.unpack(endianness + b"LL", data[1:9])  # Returns a tuple

    print(f"Received {num1} and {num2}")
    total = num1 + num2

    print(f"Sending: {total}")

    sum_packed = struct.pack(endianness_decoded + "L", total)
    sock.sendall(pickle.dumps(sum_packed))
    print(pickle.dumps(sum_packed))

    # Receive secret message
    msg = sock.recv(buf_size)
    decoded_msg = msg.decode("utf-8")
    print(f"Received message: {decoded_msg}")

finally:
    sock.close()
