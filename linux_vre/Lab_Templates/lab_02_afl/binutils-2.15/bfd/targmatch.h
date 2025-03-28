#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_alpha_freebsd_vec)

{ "alpha*-*-freebsd*", NULL },{ "alpha*-*-kfreebsd*-gnu",
&bfd_elf64_alpha_freebsd_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_alpha_vec)

{ "alpha*-*-netbsd*", NULL },{ "alpha*-*-openbsd*",
&bfd_elf64_alpha_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_ecoffalpha_little_vec)

{ "alpha*-*-netware*",
&ecoffalpha_little_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_ecoffalpha_little_vec)

{ "alpha*-*-linuxecoff*",
&ecoffalpha_little_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_alpha_vec)

{ "alpha*-*-linux-gnu*", NULL },{ "alpha*-*-elf*",
&bfd_elf64_alpha_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_vms_alpha_vec)

{ "alpha*-*-*vms*",
&vms_alpha_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_ecoffalpha_little_vec)

{ "alpha*-*-*",
&ecoffalpha_little_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_ia64_little_vec)

{ "ia64*-*-freebsd*", NULL },{ "ia64*-*-netbsd*", NULL },{ "ia64*-*-linux-gnu*", NULL },{ "ia64*-*-elf*", NULL },{ "ia64*-*-kfreebsd*-gnu",
&bfd_elf64_ia64_little_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_ia64_hpux_big_vec)

{ "ia64*-*-hpux*",
&bfd_elf32_ia64_hpux_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_sparc_vec)

{ "sparc64-*-freebsd*", NULL },{ "sparc64-*-netbsd*", NULL },{ "sparc64-*-openbsd*", NULL },{ "sparc64-*-kfreebsd*-gnu",
&bfd_elf64_sparc_vec },
#endif


    
#endif /* BFD64 */

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_am33lin_vec)

{ "am33_2.0-*-linux*",
&bfd_elf32_am33lin_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearc_vec)

{ "arc-*-elf*",
&bfd_elf32_littlearc_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_bigarm_vec)

{ "armeb-*-netbsdelf*",
&bfd_elf32_bigarm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "arm-*-netbsdelf*",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_armnetbsd_vec)

{ "arm-*-netbsd*", NULL },{ "arm-*-openbsd*",
&armnetbsd_vec },
#endif




    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "arm-*-nto*", NULL },{ "nto*arm*",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_riscix_vec)

{ "arm-*-riscix*",
&riscix_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_arm_epoc_pe_little_vec)

{ "arm-epoc-pe*",
&arm_epoc_pe_little_vec },
#endif




    
#if !defined (SELECT_VECS) || defined (HAVE_armpe_little_vec)

{ "arm-wince-pe", NULL },{ "arm-*-wince",
&armpe_little_vec },
#endif




    
#if !defined (SELECT_VECS) || defined (HAVE_armpe_little_vec)

{ "arm-*-pe*",
&armpe_little_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_aout_arm_little_vec)

{ "arm-*-aout", NULL },{ "armel-*-aout",
&aout_arm_little_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_aout_arm_big_vec)

{ "armeb-*-aout",
&aout_arm_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_armcoff_little_vec)

{ "arm-*-coff",
&armcoff_little_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "arm-*-rtems*",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_bigarm_vec)

{ "armeb-*-elf", NULL },{ "arm*b-*-linux-gnu*",
&bfd_elf32_bigarm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "arm-*-kaos*", NULL },{ "strongarm-*-kaos*",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "arm-*-elf", NULL },{ "arm-*-freebsd*", NULL },{ "arm*-*-linux-gnu*", NULL },{ "arm*-*-conix*", NULL },
{ "arm*-*-uclinux*", NULL },{ "arm-*-kfreebsd*-gnu", NULL },{ "arm-*-vxworks",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "arm9e-*-elf",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_oabi_vec)

{ "arm-*-oabi",
&bfd_elf32_littlearm_oabi_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_armcoff_little_vec)

{ "thumb-*-coff",
&armcoff_little_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_oabi_vec)

{ "thumb-*-oabi",
&bfd_elf32_littlearm_oabi_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "thumb-*-elf",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_arm_epoc_pe_little_vec)

{ "thumb-epoc-pe*",
&arm_epoc_pe_little_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_armpe_little_vec)

{ "thumb-*-pe*",
&armpe_little_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "strongarm-*-elf",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_armcoff_little_vec)

{ "strongarm-*-coff",
&armcoff_little_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlearm_vec)

{ "xscale-*-elf",
&bfd_elf32_littlearm_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_armcoff_little_vec)

{ "xscale-*-coff",
&armcoff_little_vec },
#endif



    

#if !defined (SELECT_VECS) || defined (HAVE_a29kcoff_big_vec)

{ "a29k-*-ebmon*", NULL },{ "a29k-*-udi*", NULL },{ "a29k-*-coff*", NULL },{ "a29k-*-sym1*", NULL },
{ "a29k-*-vxworks*", NULL },{ "a29k-*-sysv*", NULL },{ "a29k-*rtems*",
&a29kcoff_big_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_sunos_big_vec)

{ "a29k-*-aout*", NULL },{ "a29k-*-bsd*", NULL },{ "a29k-*-vsta*",
&sunos_big_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_avr_vec)

{ "avr-*-*",
&bfd_elf32_avr_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_tic30_aout_vec)

{ "c30-*-*aout*", NULL },{ "tic30-*-*aout*",
&tic30_aout_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_tic30_coff_vec)

{ "c30-*-*coff*", NULL },{ "tic30-*-*coff*",
&tic30_coff_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_tic4x_coff1_vec)

{ "c4x-*-*coff*", NULL },{ "tic4x-*-*coff*", NULL },{ "tic4x-*-rtems*",
&tic4x_coff1_vec },
#endif



    

#if !defined (SELECT_VECS) || defined (HAVE_tic54x_coff1_vec)

{ "c54x*-*-*coff*", NULL },{ "tic54x-*-*coff*",
&tic54x_coff1_vec },
#endif



    

#if !defined (SELECT_VECS) || defined (HAVE_cris_aout_vec)

{ "cris-*-*",
&cris_aout_vec },
#endif



    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_d10v_vec)

{ "d10v-*-*",
&bfd_elf32_d10v_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_dlx_big_vec)

{ "dlx-*-elf*",
&bfd_elf32_dlx_big_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_d30v_vec)

{ "d30v-*-*",
&bfd_elf32_d30v_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_fr30_vec)

{ "fr30-*-elf",
&bfd_elf32_fr30_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_frv_vec)

{ "frv-*-elf",
&bfd_elf32_frv_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_frvfdpic_vec)

{ "frv-*-*linux*",
&bfd_elf32_frvfdpic_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_h8300_vec)

{ "h8300*-*-elf",
&bfd_elf32_h8300_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_h8300coff_vec)

{ "h8300*-*-*",
&h8300coff_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_h8500coff_vec)

{ "h8500-*-*",
&h8500coff_vec },
#endif


    

#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_hppa_linux_vec)

{ "hppa*64*-*-linux-gnu*",
&bfd_elf64_hppa_linux_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_hppa_vec)

{ "hppa*64*-*-hpux11*",
&bfd_elf64_hppa_vec },
#endif



    
#endif

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_hppa_linux_vec)

{ "hppa*-*-linux-gnu*", NULL },{ "hppa*-*-netbsd*",
&bfd_elf32_hppa_linux_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_hppa_vec)

{ "hppa*-*-*elf*", NULL },{ "hppa*-*-lites*", NULL },{ "hppa*-*-sysv4*", NULL },{ "hppa*-*-rtems*", NULL },{ "hppa*-*-openbsd*",
&bfd_elf32_hppa_vec },
#endif


    

#if defined (HOST_HPPAHPUX) || defined (HOST_HPPABSD) || defined (HOST_HPPAOSF) || defined (HOST_HPPAMPEIX)
#if !defined (SELECT_VECS) || defined (HAVE_som_vec)

{ "hppa*-*-bsd*",
&som_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_som_vec)

{ "hppa*-*-hpux*", NULL },{ "hppa*-*-hiux*", NULL },{ "hppa*-*-mpeix*",
&som_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_som_vec)

{ "hppa*-*-osf*",
&som_vec },
#endif


    
#endif /* defined (HOST_HPPAHPUX) || defined (HOST_HPPABSD) || defined (HOST_HPPAOSF) */

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i370_vec)

{ "i370-*-*", 
&bfd_elf32_i370_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386coff_vec)

{ "i[3-7]86-*-sco3.2v5*coff",
&i386coff_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-sysv4*", NULL },{ "i[3-7]86-*-unixware*", NULL },{ "i[3-7]86-*-solaris2*", NULL },
{ "i[3-7]86-*-elf", NULL },{ "i[3-7]86-*-sco3.2v5*", NULL },
{ "i[3-7]86-*-dgux*", NULL },{ "i[3-7]86-*-sysv5*",
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-kaos*",
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-nto*",
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-aros*",
&bfd_elf32_i386_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-chorus*",
&bfd_elf32_i386_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_go32coff_vec)

{ "*-*-msdosdjgpp*", NULL },{ "*-*-go32*", NULL },{ "*-go32-rtems*",
&go32coff_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386coff_vec)

{ "i[3-7]86-*-sysv*", NULL },{ "i[3-7]86-*-isc*", NULL },{ "i[3-7]86-*-sco*", NULL },{ "i[3-7]86-*-coff", NULL },
{ "i[3-7]86-*-aix*",
&i386coff_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_i386coff_vec)

{ "i[3-7]86*-*-rtemscoff*",
&i386coff_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-rtemself*", NULL },{ "i[3-7]86-*-rtems*",
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_mach_o_le_vec)

{ "i[3-7]86-*-darwin*", NULL },{ "i[3-7]86-*-macos10*", NULL },{ "i[3-7]86-*-rhapsody*",
&mach_o_le_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_i386dynix_vec)

{ "i[3-7]86-sequent-bsd*",
&i386dynix_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386bsd_vec)

{ "i[3-7]86-*-bsd*",
&i386bsd_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386freebsd_vec)

{ "i[3-7]86-*-freebsdaout*", NULL },{ "i[3-7]86-*-freebsd[12].*", NULL },
{ "i[3-7]86-*-freebsd[12]",
&i386freebsd_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_freebsd_vec)

{ "i[3-7]86-*-freebsd*", NULL },{ "i[3-7]86-*-kfreebsd*-gnu",
&bfd_elf32_i386_freebsd_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-netbsdelf*", NULL },{ "i[3-7]86-*-netbsd*-gnu*", NULL },{ "i[3-7]86-*-knetbsd*-gnu",
&bfd_elf32_i386_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_i386pe_vec)

{ "i[3-7]86-*-netbsdpe*",
&i386pe_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386netbsd_vec)

{ "i[3-7]86-*-netbsdaout*", NULL },{ "i[3-7]86-*-netbsd*", NULL },
{ "i[3-7]86-*-openbsd[0-2].*", NULL },{ "i[3-7]86-*-openbsd3.[0-3]",
&i386netbsd_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-openbsd*", 
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-netware*",
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386linux_vec)

{ "i[3-7]86-*-linux*aout*",
&i386linux_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-linux-gnu*",
&bfd_elf32_i386_vec },
#endif



    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_x86_64_vec)

{ "x86_64-*-freebsd*", NULL },{ "x86_64-*-kfreebsd*-gnu",
&bfd_elf64_x86_64_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_x86_64_vec)

{ "x86_64-*-netbsd*", NULL },{ "x86_64-*-openbsd*",
&bfd_elf64_x86_64_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_x86_64_vec)

{ "x86_64-*-linux-gnu*",
&bfd_elf64_x86_64_vec },
#endif


    
#endif
#if !defined (SELECT_VECS) || defined (HAVE_i386lynx_coff_vec)

{ "i[3-7]86-*-lynxos*",
&i386lynx_coff_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-gnu*",
&bfd_elf32_i386_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_i386mach3_vec)

{ "i[3-7]86-*-mach*", NULL },{ "i[3-7]86-*-osf1mk*",
&i386mach3_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_i386os9k_vec)

{ "i[3-7]86-*-os9k",
&i386os9k_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_i386aout_vec)

{ "i[3-7]86-*-msdos*",
&i386aout_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-moss*",
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386pe_vec)

{ "i[3-7]86-*-beospe*",
&i386pe_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-beoself*", NULL },{ "i[3-7]86-*-beos*",
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386pei_vec)

{ "i[3-7]86-*-interix*",
&i386pei_vec },
#endif




    
#if !defined (SELECT_VECS) || defined (HAVE_i386pe_vec)

{ "i[3-7]86-*-mingw32*", NULL },{ "i[3-7]86-*-cygwin*", NULL },{ "i[3-7]86-*-winnt", NULL },{ "i[3-7]86-*-pe",
&i386pe_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_i386coff_vec)

{ "i[3-7]86-none-*",
&i386coff_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_i386aout_vec)

{ "i[3-7]86-*-aout*", NULL },{ "i[3-7]86*-*-vsta*",
&i386aout_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-vxworks",
&bfd_elf32_i386_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i386_vec)

{ "i[3-7]86-*-chaos",
&bfd_elf32_i386_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_i860coff_vec)

{ "i860-*-mach3*", NULL },{ "i860-*-osf1*", NULL },{ "i860-*-coff*",
&i860coff_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i860_little_vec)

{ "i860-stardent-sysv4*", NULL },{ "i860-stardent-elf*",
&bfd_elf32_i860_little_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i860_vec)

{ "i860-*-sysv4*", NULL },{ "i860-*-elf*",
&bfd_elf32_i860_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_b_out_vec_little_host)

{ "i960-*-vxworks4*", NULL },{ "i960-*-vxworks5.0",
&b_out_vec_little_host },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_icoff_little_vec)

{ "i960-*-vxworks5.*", NULL },{ "i960-*-coff*", NULL },{ "i960-*-sysv*", NULL },{ "i960-*-rtems*",
&icoff_little_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_b_out_vec_little_host)

{ "i960-*-vxworks*", NULL },{ "i960-*-aout*", NULL },{ "i960-*-bout*", NULL },{ "i960-*-nindy*",
&b_out_vec_little_host },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_i960_vec)

{ "i960-*-elf*",
&bfd_elf32_i960_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_ip2k_vec)

{ "ip2k-*-elf",
&bfd_elf32_ip2k_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_iq2000_vec)

{ "iq2000-*-elf",
&bfd_elf32_iq2000_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m32rlelin_vec)

{ "m32r*le-*-linux*",
&bfd_elf32_m32rlelin_vec },
#endif


    
                                                                                
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m32rlin_vec)

{ "m32r*-*-linux*",
&bfd_elf32_m32rlin_vec },
#endif


    
                                                                                
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m32rle_vec)

{ "m32r*le-*-*",
&bfd_elf32_m32rle_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m32r_vec)

{ "m32r-*-*",
&bfd_elf32_m32r_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68hc11_vec)

{ "m68hc11-*-*", NULL },{ "m6811-*-*",
&bfd_elf32_m68hc11_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68hc12_vec)

{ "m68hc12-*-*", NULL },{ "m6812-*-*",
&bfd_elf32_m68hc12_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_apollocoff_vec)

{ "m68*-apollo-*",
&apollocoff_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_m68kcoffun_vec)

{ "m68*-bull-sysv*",
&m68kcoffun_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_m68ksysvcoff_vec)

{ "m68*-motorola-sysv*",
&m68ksysvcoff_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_hp300bsd_vec)

{ "m68*-hp-bsd*",
&hp300bsd_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_aout0_big_vec)

{ "m68*-*-aout*",
&aout0_big_vec },
#endif






    
#if !defined (SELECT_VECS) || defined (HAVE_m68kcoff_vec)

{ "m68*-*-rtemscoff*",
&m68kcoff_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68k_vec)

{ "m68*-*-elf*", NULL },{ "m68*-*-sysv4*", NULL },{ "m68*-*-uclinux*",
&bfd_elf32_m68k_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68k_vec)

{ "m68*-*-rtems*",
&bfd_elf32_m68k_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_m68kcoff_vec)

{ "m68*-*-coff*", NULL },{ "m68*-*-sysv*",
&m68kcoff_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_hp300hpux_vec)

{ "m68*-*-hpux*",
&hp300hpux_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_m68klinux_vec)

{ "m68*-*-linux*aout*",
&m68klinux_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68k_vec)

{ "m68*-*-linux-gnu*",
&bfd_elf32_m68k_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68k_vec)

{ "m68*-*-gnu*",
&bfd_elf32_m68k_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_m68klynx_coff_vec)

{ "m68*-*-lynxos*",
&m68klynx_coff_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_m68k4knetbsd_vec)

{ "m68*-hp*-netbsd*",
&m68k4knetbsd_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68k_vec)

{ "m68*-*-netbsdelf*", 
&bfd_elf32_m68k_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_m68knetbsd_vec)

{ "m68*-*-netbsdaout*", NULL },{ "m68*-*-netbsd*",
&m68knetbsd_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_m68knetbsd_vec)

{ "m68*-*-openbsd*",
&m68knetbsd_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_sunos_big_vec)

{ "m68*-*-sunos*", NULL },{ "m68*-*-os68k*", NULL },{ "m68*-*-vxworks*", NULL },{ "m68*-netx-*", NULL },
{ "m68*-*-bsd*", NULL },{ "m68*-*-vsta*",
&sunos_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_sunos_big_vec)

{ "m68*-ericsson-*",
&sunos_big_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68k_vec)

{ "m68*-cbm-*",
&bfd_elf32_m68k_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_m68kaux_coff_vec)

{ "m68*-apple-aux*",
&m68kaux_coff_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m68k_vec)

{ "m68*-*-psos*",
&bfd_elf32_m68k_vec },
#endif



    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_m88k_vec)

{ "m88*-harris-cxux*", NULL },{ "m88*-*-dgux*", NULL },{ "m88*-*-sysv4*",
&bfd_elf32_m88k_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_m88kmach3_vec)

{ "m88*-*-mach3*",
&m88kmach3_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_m88kbcs_vec)

{ "m88*-*-*",
&m88kbcs_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_mcore_big_vec)

{ "mcore-*-elf",
&bfd_elf32_mcore_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_mcore_pe_big_vec)

{ "mcore-*-pe",
&mcore_pe_big_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_ecoff_big_vec)

{ "mips*-big-*",
&ecoff_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlemips_vec)

{ "mips*el-*-netbsd*",
&bfd_elf32_littlemips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_bigmips_vec)

{ "mips*-*-netbsd*",
&bfd_elf32_bigmips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_ecoff_little_vec)

{ "mips*-dec-*", NULL },{ "mips*el-*-ecoff*",
&ecoff_little_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_ecoff_big_vec)

{ "mips*-*-ecoff*",
&ecoff_big_vec },
#endif


    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_nbigmips_vec)

{ "mips*-*-irix6*",
&bfd_elf32_nbigmips_vec },
#endif


    
#endif
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_bigmips_vec)

{ "mips*-*-irix5*",
&bfd_elf32_bigmips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_ecoff_big_vec)

{ "mips*-sgi-*", NULL },{ "mips*-*-bsd*",
&ecoff_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_ecoff_biglittle_vec)

{ "mips*-*-lnews*",
&ecoff_biglittle_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_tradbigmips_vec)

{ "mips*-*-sysv4*",
&bfd_elf32_tradbigmips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_ecoff_big_vec)

{ "mips*-*-sysv*", NULL },{ "mips*-*-riscos*",
&ecoff_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlemips_vec)

{ "mips*el-*-elf*", NULL },{ "mips*el-*-rtems*", NULL },{ "mips*el-*-vxworks*", NULL },{ "mips*-*-chorus*",
&bfd_elf32_littlemips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_bigmips_vec)

{ "mips*-*-elf*", NULL },{ "mips*-*-rtems*", NULL },{ "mips*-*-vxworks", NULL },{ "mips*-*-windiss",
&bfd_elf32_bigmips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_bigmips_vec)

{ "mips*-*-none",
&bfd_elf32_bigmips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_littlemips_vec)

{ "mips*el-*-openbsd*",
&bfd_elf32_littlemips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_bigmips_vec)

{ "mips*-*-openbsd*",
&bfd_elf32_bigmips_vec },
#endif


    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_ntradlittlemips_vec)

{ "mips64*el-*-linux*",
&bfd_elf32_ntradlittlemips_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_ntradbigmips_vec)

{ "mips64*-*-linux*",
&bfd_elf32_ntradbigmips_vec },
#endif


    
#endif
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_tradlittlemips_vec)

{ "mips*el-*-linux*",
&bfd_elf32_tradlittlemips_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_tradbigmips_vec)

{ "mips*-*-linux*",
&bfd_elf32_tradbigmips_vec },
#endif



    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_mmix_vec)

{ "mmix-*-*",
&bfd_elf64_mmix_vec },
#endif


    
#endif
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_mn10200_vec)

{ "mn10200-*-*",
&bfd_elf32_mn10200_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_mn10300_vec)

{ "mn10300-*-*",
&bfd_elf32_mn10300_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_msp430_vec)

{ "msp430-*-*",
&bfd_elf32_msp430_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_pc532machaout_vec)

{ "ns32k-pc532-mach*", NULL },{ "ns32k-pc532-ux*",
&pc532machaout_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_pc532netbsd_vec)

{ "ns32k-*-netbsd*", NULL },{ "ns32k-*-lites*", NULL },{ "ns32k-*-openbsd*",
&pc532netbsd_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_openrisc_vec)

{ "openrisc-*-elf",
&bfd_elf32_openrisc_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_or32coff_big_vec)

{ "or32-*-coff",
&or32coff_big_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_or32_big_vec)

{ "or32-*-elf", NULL },{ "or32-*-rtems*",
&bfd_elf32_or32_big_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_pdp11_aout_vec)

{ "pdp11-*-*",
&pdp11_aout_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_pj_vec)

{ "pj-*-*",
&bfd_elf32_pj_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_pjl_vec)

{ "pjl-*-*",
&bfd_elf32_pjl_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_rs6000coff_vec)

{ "powerpc-*-aix5.[01]",
&rs6000coff_vec },
#endif



    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_aix5coff64_vec)

{ "powerpc64-*-aix5.[01]",
&aix5coff64_vec },
#endif



    
#endif

#if !defined (SELECT_VECS) || defined (HAVE_rs6000coff_vec)

{ "powerpc-*-aix5*",
&rs6000coff_vec },
#endif



    
#ifdef BFD64

#if !defined (SELECT_VECS) || defined (HAVE_aix5coff64_vec)

{ "powerpc64-*-aix5*",
&aix5coff64_vec },
#endif



    
#endif

#if !defined (SELECT_VECS) || defined (HAVE_rs6000coff_vec)

{ "powerpc-*-aix*", NULL },{ "powerpc-*-beos*", NULL },{ "rs6000-*-*",
&rs6000coff_vec },
#endif


    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_rs6000coff64_vec)

{ "powerpc64-*-aix*",
&rs6000coff64_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_powerpc_vec)

{ "powerpc64-*-elf*", NULL },{ "powerpc-*-elf64*", NULL },{ "powerpc64-*-linux*", NULL },
{ "powerpc64-*-*bsd*",
&bfd_elf64_powerpc_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_powerpcle_vec)

{ "powerpc64le-*-elf*", NULL },{ "powerpcle-*-elf64*",
&bfd_elf64_powerpcle_vec },
#endif


    
#endif
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_powerpc_vec)

{ "powerpc-*-*bsd*", NULL },{ "powerpc-*-elf*", NULL },{ "powerpc-*-sysv4*", NULL },{ "powerpc-*-eabi*", NULL },
{ "powerpc-*-solaris2*", NULL },{ "powerpc-*-linux-gnu*", NULL },{ "powerpc-*-rtems*", NULL },
{ "powerpc-*-chorus*", NULL },{ "powerpc-*-vxworks*", NULL },{ "powerpc-*-windiss*",
&bfd_elf32_powerpc_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_powerpc_vec)

{ "powerpc-*-kaos*",
&bfd_elf32_powerpc_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_mach_o_be_vec)

{ "powerpc-*-darwin*", NULL },{ "powerpc-*-macos10*", NULL },{ "powerpc-*-rhapsody*",
&mach_o_be_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_pmac_xcoff_vec)

{ "powerpc-*-macos*", NULL },{ "powerpc-*-mpw*",
&pmac_xcoff_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_powerpc_vec)

{ "powerpc-*-netware*",
&bfd_elf32_powerpc_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_powerpc_vec)

{ "powerpc-*-nto*",
&bfd_elf32_powerpc_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_powerpcle_vec)

{ "powerpcle-*-nto*",
&bfd_elf32_powerpcle_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_powerpcle_vec)

{ "powerpcle-*-elf*", NULL },{ "powerpcle-*-sysv4*", NULL },{ "powerpcle-*-eabi*", NULL },
{ "powerpcle-*-solaris2*", NULL },{ "powerpcle-*-linux-gnu*", NULL },{ "powerpcle-*-vxworks*", NULL },
{ "powerpcle-*-rtems*",
&bfd_elf32_powerpcle_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_powerpcle_pe_vec)

{ "powerpcle-*-pe", NULL },{ "powerpcle-*-winnt*", NULL },{ "powerpcle-*-cygwin*",
&bfd_powerpcle_pe_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_s390_vec)

{ "s390-*-linux*",
&bfd_elf32_s390_vec },
#endif


    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_s390_vec)

{ "s390x-*-linux*",
&bfd_elf64_s390_vec },
#endif


    
#endif

#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh64l_vec)

{ "sh64l*-*-elf*",
&bfd_elf32_sh64l_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh64_vec)

{ "sh64-*-elf*",
&bfd_elf32_sh64_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh64blin_vec)

{ "sh64eb-*-linux*",
&bfd_elf32_sh64blin_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh64lin_vec)

{ "sh64-*-linux*",
&bfd_elf32_sh64lin_vec },
#endif


    
#endif /* BFD64 */

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_shblin_vec)

{ "sh-*-linux*",
&bfd_elf32_shblin_vec },
#endif


#ifdef BFD64

#endif
    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_shblin_vec)

{ "sh*eb-*-linux*",
&bfd_elf32_shblin_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_shlin_vec)

{ "sh*-*-linux*",
&bfd_elf32_shlin_vec },
#endif


    

#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh64lnbsd_vec)

{ "sh5le-*-netbsd*",
&bfd_elf32_sh64lnbsd_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh64nbsd_vec)

{ "sh5-*-netbsd*",
&bfd_elf32_sh64nbsd_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_sh64lnbsd_vec)

{ "sh64le-*-netbsd*",
&bfd_elf64_sh64lnbsd_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_sh64nbsd_vec)

{ "sh64-*-netbsd*",
&bfd_elf64_sh64nbsd_vec },
#endif


    
#endif

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_shlnbsd_vec)

{ "sh*l*-*-netbsdelf*",
&bfd_elf32_shlnbsd_vec },
#endif


#ifdef BFD64

#endif
    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_shnbsd_vec)

{ "sh-*-netbsdelf*",
&bfd_elf32_shnbsd_vec },
#endif


#ifdef BFD64

#endif
    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_shnbsd_vec)

{ "sh*-*-netbsdelf*",
&bfd_elf32_shnbsd_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_shl_vec)

{ "shl*-*-elf*", NULL },{ "sh[1234]l*-*-elf*", NULL },{ "sh3el*-*-elf*", NULL },{ "shl*-*-kaos*",
&bfd_elf32_shl_vec },
#endif


#ifdef BFD64

#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_shcoff_vec)

{ "sh-*-rtemscoff*",
&shcoff_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh_vec)

{ "sh-*-elf*", NULL },{ "sh[1234]*-elf*", NULL },{ "sh-*-rtems*", NULL },{ "sh-*-kaos*",
&bfd_elf32_sh_vec },
#endif


#ifdef BFD64

#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh_vec)

{ "sh-*-nto*",
&bfd_elf32_sh_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_shlpe_vec)

{ "sh-*-pe",
&shlpe_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sh_vec)

{ "sh-*-vxworks",
&bfd_elf32_sh_vec },
#endif






    
#if !defined (SELECT_VECS) || defined (HAVE_shcoff_vec)

{ "sh-*-*",
&shcoff_vec },
#endif



    

#if !defined (SELECT_VECS) || defined (HAVE_sunos_big_vec)

{ "sparclet-*-aout*",
&sunos_big_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_sunos_big_vec)

{ "sparc86x-*-aout*",
&sunos_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparclite-*-elf*", NULL },{ "sparc86x-*-elf*",
&bfd_elf32_sparc_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc*-*-chorus*",
&bfd_elf32_sparc_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_sparclinux_vec)

{ "sparc-*-linux*aout*",
&sparclinux_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc-*-linux-gnu*",
&bfd_elf32_sparc_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_sparclynx_coff_vec)

{ "sparc-*-lynxos*",
&sparclynx_coff_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc-*-netbsdelf*",
&bfd_elf32_sparc_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_sparcnetbsd_vec)

{ "sparc-*-netbsdaout*", NULL },{ "sparc-*-netbsd*",
&sparcnetbsd_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_sparcnetbsd_vec)

{ "sparc-*-openbsd[0-2].*", NULL },{ "sparc-*-openbsd3.[0-1]",
&sparcnetbsd_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc-*-openbsd*",
&bfd_elf32_sparc_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc-*-elf*", NULL },{ "sparc-*-solaris2.[0-6]", NULL },{ "sparc-*-solaris2.[0-6].*",
&bfd_elf32_sparc_vec },
#endif


    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc-*-solaris2*", NULL },{ "sparcv9-*-solaris2*", NULL },{ "sparc64-*-solaris2*",
&bfd_elf32_sparc_vec },
#endif


    
#endif
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc-*-sysv4*",
&bfd_elf32_sparc_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc-*-netware*",
&bfd_elf32_sparc_vec },
#endif


    
#ifdef BFD64
#if !defined (SELECT_VECS) || defined (HAVE_sunos_big_vec)

{ "sparc64-*-aout*",
&sunos_big_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_sparc_vec)

{ "sparc64-*-linux-gnu*",
&bfd_elf64_sparc_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf64_sparc_vec)

{ "sparc64-*-elf*",
&bfd_elf64_sparc_vec },
#endif


    
#endif /* BFD64 */
#if !defined (SELECT_VECS) || defined (HAVE_sparccoff_vec)

{ "sparc*-*-coff*",
&sparccoff_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_sunos_big_vec)

{ "sparc*-*-rtemsaout*",
&sunos_big_vec },
#endif



    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_sparc_vec)

{ "sparc*-*-rtems*", NULL },{ "sparc*-*-rtemself*",
&bfd_elf32_sparc_vec },
#endif


    
#if !defined (SELECT_VECS) || defined (HAVE_sunos_big_vec)

{ "sparc*-*-*", NULL },{ "sparc*-*-rtems*",
&sunos_big_vec },
#endif


    

#if HAVE_host_aout_vec
#if !defined (SELECT_VECS) || defined (HAVE_host_aout_vec)

{ "tahoe-*-*",
&host_aout_vec },
#endif


    
#endif

#if !defined (SELECT_VECS) || defined (HAVE_tic80coff_vec)

{ "tic80*-*-*",
&tic80coff_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_v850_vec)

{ "v850-*-*",
&bfd_elf32_v850_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_v850_vec)

{ "v850e-*-*",
&bfd_elf32_v850_vec },
#endif

    
#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_v850_vec)

{ "v850ea-*-*",
&bfd_elf32_v850_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_vax_vec)

{ "vax-*-netbsdelf*",
&bfd_elf32_vax_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_vaxnetbsd_vec)

{ "vax-*-netbsdaout*", NULL },{ "vax-*-netbsd*",
&vaxnetbsd_vec },
#endif



    

#if !defined (SELECT_VECS) || defined (HAVE_vaxbsd_vec)

{ "vax-*-bsd*", NULL },{ "vax-*-ultrix*",
&vaxbsd_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_vaxnetbsd_vec)

{ "vax-*-openbsd*",
&vaxnetbsd_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_vax_vec)

{ "vax-*-linux-gnu*",
&bfd_elf32_vax_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_vms_vax_vec)

{ "vax*-*-*vms*",
&vms_vax_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_we32kcoff_vec)

{ "we32k-*-*",
&we32kcoff_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_w65_vec)

{ "w65-*-*",
&w65_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_xstormy16_vec)

{ "xstormy16-*-elf",
&bfd_elf32_xstormy16_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_bfd_elf32_xtensa_le_vec)

{ "xtensa-*-*",
&bfd_elf32_xtensa_le_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_z8kcoff_vec)

{ "z8k*-*-*",
&z8kcoff_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_ieee_vec)

{ "*-*-ieee*",
&ieee_vec },
#endif

    

#if !defined (SELECT_VECS) || defined (HAVE_a_out_adobe_vec)

{ "*-adobe-*",
&a_out_adobe_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_newsos3_vec)

{ "*-sony-*",
&newsos3_vec },
#endif


    

#if !defined (SELECT_VECS) || defined (HAVE_m68kcoff_vec)

{ "*-tandem-*",
&m68kcoff_vec },
#endif


    
