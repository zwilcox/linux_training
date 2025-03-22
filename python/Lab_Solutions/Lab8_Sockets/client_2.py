import socket
import pickle
import struct


class Client:
    def __init__(self, ip, port):
        if not isinstance(ip, str) or not isinstance(port, int):
            raise TypeError("IP must be str and port int")

        if port not in range(0, 65536):
            raise ValueError("Not a valid port")

        self._ip = ip
        self._port = port
        self._socket = None

        self._connect()

    def _connect(self) -> None:
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self._socket.connect((self._ip, self._port))
        except ConnectionError as e:
            raise ConnectionError(f"Unable to connect: {e}")

    def send_msg(self, msg: bytes) -> None:
        try:
            self._socket.sendall(msg)

        except Exception as e:
            raise ConnectionError(f"Error Sending: {e}")

    def recv_msg(self):
        try:
            return self._socket.recv(1024)
        except ConnectionError as e:
            raise ConnectionError(f"Error Receiving: {e}")


endianess = None


def unpickle(msg: bytes) -> bytes:
    return pickle.loads(msg)


def pickle_bytes(msg: bytes) -> bytes:
    return pickle.dumps(msg)


def pack(val: int) -> bytes:
    global endianess
    return struct.pack(f"{endianess}L", val)


def unpack(msg: bytes) -> tuple:
    global endianess
    endianess = chr(msg[0])
    return struct.unpack(f"{endianess}LL", msg[1:])


def sum_tuple(inp: tuple) -> int:
    return sum(inp)


def main():
    global endianess
    client = Client("172.16.1.100", 5555)
    msg = client.recv_msg()
    print(f"msg: {msg}\n")

    str_inp = unpickle(msg)
    print(f"str_inp: {str_inp}\n")

    unp_tup = unpack(str_inp)
    print(f"Unp_tup: {unp_tup}\n")

    val = sum_tuple(unp_tup)
    print(f"Val: {val}\n")

    pack_val = pack(val)
    print(f"pack_val: {pack_val}\n")
    test = struct.unpack(f"{endianess}L", pack_val)
    print(f"Unpacked: {test}")

    out_msg = pickle_bytes(pack_val)
    print(f"out_msg: {out_msg}\n")

    client.send_msg(out_msg)
    print(client.recv_msg())


if __name__ == "__main__":
    main()
