BITS 64
segment .text

extern regRAX
extern regRBX
extern regRCX
extern regRDX
extern regR8
extern regR9
extern moveToStack
extern labFunc

%define OFF_R15 0x00
%define OFF_R14 0x08
%define OFF_R13 0x10
%define OFF_R12 0x18
%define OFF_RBP 0x20
%define OFF_RBX 0x28
%define OFF_R11 0x30
%define OFF_R10 0x38
%define OFF_R9  0x40
%define OFF_R8  0x48
%define OFF_RAX 0x50
%define OFF_RCX 0x58
%define OFF_RDX 0x60
%define OFF_RSI 0x68
%define OFF_RDI 0x70

extern lab5_2

GLOBAL modify_stack_lab5_2
modify_stack_lab5_2:
	jmp here5_2

there5_2:
	sub rsp, 0x800
	jmp lab5_2

here5_2:
	call there5_2
	ret

extern lab9_1
GLOBAL val_lab9_1
val_lab9_1:
	mov rax, 1
	test rax, rax
	call lab9_1
	setz al
	and rax, 0x01
	ret

extern lab9_2
GLOBAL val_lab9_2
val_lab9_2:
	push r13
	mov r13, rdi

	mov rax, 1
	test rax, rax
	call lab9_2
	seto al
	setc bl
	and rax, 0x01
	and rbx, 0x01
	mov [r13 + OFF_RBX], rbx

	pop r13
	ret

GLOBAL save_n_go
save_n_go:
	; save off all the volatile registers
	push rbx
	push rbp
	push rdi
	push rsi
	push r12
	push r13
	push r14
	push r15
	push rsi
	mov r12, rdi
	mov r13, rsi

	; pull from user_regs_struct
	mov rax, [r13 + OFF_RAX]
	mov rbx, [r13 + OFF_RBX]
	mov rcx, [r13 + OFF_RCX]
	mov rdx, [r13 + OFF_RDX]
	mov rbp, [r13 + OFF_RBP]
	mov r8,  [r13 + OFF_R8]
	mov r9,  [r13 + OFF_R9]
	mov r10, [r13 + OFF_R10]
	mov r11, [r13 + OFF_R11]
	mov r14, [r13 + OFF_R14]
	mov r15, [r13 + OFF_R15]
	mov rsi, [r13 + OFF_RSI]
	mov rdi, [r13 + OFF_RDI]
	mov r13, [r13 + OFF_R13]

	; give some stack space
	sub rsp, 0x100

	call r12

	add rsp, 0x100

	pop r13

	; fill out the user_regs_struct
	mov [r13 + OFF_RAX], rax
	mov [r13 + OFF_RBX], rbx
	mov [r13 + OFF_RCX], rcx
	mov [r13 + OFF_RDX], rdx
	mov [r13 + OFF_RDI], rdi
	mov [r13 + OFF_RSI], rsi
	mov [r13 + OFF_RBP], rbp
	mov [r13 + OFF_R8], r8
	mov [r13 + OFF_R9], r9
	mov [r13 + OFF_R10], r10
	mov [r13 + OFF_R11], r11
	mov [r13 + OFF_R12], r12
	mov [r13 + OFF_R13], r13
	mov [r13 + OFF_R14], r14
	mov [r13 + OFF_R15], r15

	; restore saved registers
	pop r15
	pop r14
	pop r13
	pop r12
	pop rsi
	pop rdi
	pop rbp
	pop rbx
	ret
