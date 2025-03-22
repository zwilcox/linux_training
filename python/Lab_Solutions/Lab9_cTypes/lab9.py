import os
from ctypes import CDLL

so_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "./libLab9.so")
so = CDLL(so_path)

so.MyMessageBox(b"Message", b"Title")
so.MyMessageBox(
    b"This is my message and it is longer than the first", b"This is my title"
)
so.MyMessageBox(b"", b"")
