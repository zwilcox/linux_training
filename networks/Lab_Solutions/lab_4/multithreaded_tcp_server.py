import argparse
import queue
import socket
import threading

SIGNAL = True  # yeah this is bad. come at me

def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--count", type=int, default=5,
                        help="Number of worker threads to spawn")
    return parser.parse_args()


def create_socket(queue_size):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(("", 8889))
    s.listen(queue_size)
    return s


def handle_client(thread_id, client_queue):
    while SIGNAL:
        try:
            sock = client_queue.get(timeout=1)
            buf = sock.recv(100)
            memes = [b"globglogabgalab", b"keyboard cat",
                     b"15 minutes", b"advice mallard"]
            if buf.strip() not in memes:
                bytes_sent = sock.send(b"that isn't a meme")
                print("{} thinks {} is a meme".format(sock, buf.strip()))
            else:
                sock.send(b"you're a meme master")
            sock.close()
            client_queue.task_done()
        except:
            pass


def create_workers(total, client_queue):
    threads = []
    for i in range(total):
        t = threading.Thread(target=handle_client, args=(i+1, client_queue))
        t.start()
        threads.append(t)
    return threads


def accept_clients(sock, client_queue):
    global SIGNAL
    try:
        while True:
            client_sock, client_info = sock.accept()
            client_queue.put(client_sock)
    except KeyboardInterrupt:
        SIGNAL = False
        print("RIParooni")


def main():
    args = get_args()
    client_queue = queue.Queue(5)
    s = create_socket(args.count)
    threads = create_workers(args.count, client_queue)
    accept_clients(s, client_queue)
    client_queue.join()
    s.close()
    for thread in threads:
        thread.join()


if __name__ == "__main__":
    main()
