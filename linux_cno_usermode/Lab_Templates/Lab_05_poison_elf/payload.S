.intel_syntax noprefix


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
