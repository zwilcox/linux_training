#!/usr/bin/python3

import sys
import struct
import subprocess
import codecs

MAINRETOFF = 0x0 ### Fill this out
WINOFF = ### Fill this out
IMGNAME = "auth"

def main():
    if len(sys.argv) < 2:
        print("Usage: "+ sys.argv[0] +" auth_pid")
        exit(-1)

    pid = sys.argv[1]

    pmap = ""
    with open("/proc/"+ sys.argv[1] +"/maps", "r") as fp:
        pmap = fp.read()

    # find image offset in maps
    # find stack in maps
    # replace return on stack with desired target

if __name__ == '__main__':
    main()
