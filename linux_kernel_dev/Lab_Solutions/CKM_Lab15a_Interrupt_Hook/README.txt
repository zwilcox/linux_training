The binary for the kernel is vmlinux, so you can view the disassembly in ghidra/objdump etc.
The following is the disassembly for divide_error from objdump of our compiled kernel:

ffffffff81a00c70 <divide_error>:
ffffffff81a00c70:       90                      nop
ffffffff81a00c71:       90                      nop
ffffffff81a00c72:       90                      nop
ffffffff81a00c73:       6a ff                   pushq  $0xffffffffffffffff
ffffffff81a00c75:       e8 76 06 00 00          callq  ffffffff81a012f0 <error_entry>
ffffffff81a00c7a:       48 89 e7                mov    %rsp,%rdi
ffffffff81a00c7d:       31 f6                   xor    %esi,%esi
ffffffff81a00c7f:       e8 1c 73 62 ff          callq  ffffffff81027fa0 <do_divide_error>
ffffffff81a00c84:       e9 77 07 00 00          jmpq   ffffffff81a01400 <error_exit>
ffffffff81a00c89:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)

The solution's int_hook.S which contains our handler, is mimicing this. Notice the calls/jumps to error_entry, do_divide_error and error_exit.

divide_error + 10 is the value of rip at the point of the call to error_entry, so get_abs_addr would calc the absolute address of error_entry.

similarly, +20 is do_divide_error's addr, and +25 is error_exit's addr.

The int_hook.S uses these addresses of error_entry and error_exit to keep their functionality.

do_divide_error is the C handler for divide_error, so the sol saves of this addr to potentially call original, but it calls my_do_handler(), which can call original from there.

error_entry does a few things, one of which is saving the registers to the stack with PUSH_AND_CLEAR_REGS which we see as pt_regs
so the stack will have all the registers saved after that function.

mov %rsp, %rdi will set the first argument to the c handler as the pointer to the saved reg state.

The second arg is the error code, there is no error code for this one so it's just xored to set it to zero.

error_exit will restore register state then iret. Based on if you came from kernel or userland it will aslo call swapgs.

Since it will iret, probably could just use a nop after the jump like divide_error does instead of ret in our int_hook.S

