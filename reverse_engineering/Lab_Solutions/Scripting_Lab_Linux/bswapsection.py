#!/usr/bin/python3

import sys
import struct

def main(fn, sn):
    # open the file, read it in, find the section we want, and bswap every 8 bytes
    elf = bytearray()
    with open(fn, "rb") as fp:
        elf = bytearray(fp.read())

    # parse to the sections, find section of name sn
    if elf[0:4] != b"\x7fELF":
        print("Not an elf")
        exit(-1)

    shoff = struct.unpack("<Q", elf[0x28:0x30])[0]
    shent = struct.unpack("<H", elf[0x3A:0x3C])[0]
    shnum = struct.unpack("<H", elf[0x3C:0x3E])[0]
    shnameidx = struct.unpack("<H", elf[0x3E:0x40])[0]

    sh = elf[shoff:shoff + (shnum * shent)]

    # get names first
    nameoff = struct.unpack("<Q", sh[(shnameidx * shent) + 0x18: (shnameidx * shent) + 0x20])[0]

    secstart = 0
    secend   = 0

    for i in range(shnum):
        namei = struct.unpack("<I", sh[(i * shent) : (i * shent) + 0x4])[0]
        startn = nameoff + namei
        endn = startn
        # find end of string
        while elf[endn] != 0:
            endn += 1

        if endn == startn:
            continue
        
        if bytes(sn, "ascii") == elf[startn:endn]:
            # found the section to do the thing to
            secstart = struct.unpack("<Q", sh[(i*shent) + 0x18: (i*shent) + 0x20])[0]
            secsz = struct.unpack("<Q", sh[(i*shent) + 0x20: (i*shent) + 0x28])[0]
            secend = secstart + secsz
            break

    if secstart == 0 or secend == 0:
        print("Unable to find section")
        exit(-1)

    print("modifying from", hex(secstart), hex(secend))

    for o in range(secstart, secend, 8):
        if (o+8) > secend:
            break
        
        b0 = elf[o + 0]
        b1 = elf[o + 1]
        b2 = elf[o + 2]
        b3 = elf[o + 3]
        b4 = elf[o + 4]
        b5 = elf[o + 5]
        b6 = elf[o + 6]
        b7 = elf[o + 7]

        elf[o + 0] = b7
        elf[o + 1] = b6
        elf[o + 2] = b5
        elf[o + 3] = b4
        elf[o + 4] = b3
        elf[o + 5] = b2
        elf[o + 6] = b1
        elf[o + 7] = b0

    # export the file
    with open(fn, "wb") as fp:
        fp.write(elf)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Requires filename argument")
    filename = sys.argv[1]
    sectionname = "malleable"
    main(filename, sectionname) 
