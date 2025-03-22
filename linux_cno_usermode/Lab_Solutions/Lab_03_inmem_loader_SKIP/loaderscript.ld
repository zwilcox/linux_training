
/* What format we want, we could do an elf here if we wanted */
OUTPUT_FORMAT(binary)

/* output file */
OUTPUT(ldr_hdr.bin)

/* order of file */
SECTIONS
{
	.text 0x0 : {
		ldr.o (.ldr_preface)
		ldr_lib.o (.text)
		ldr_lib.o (.bss)
		ldr_lib.o (.data*)
		ldr_lib.o (.rodata*)
		ldr.o (.ldr_end)
	}

	/* throw away extra sections we don't need */
	/DISCARD/ : {
		*(.debug*)
		*(.note*)
		ldr.o (.text)
		ldr.o (.data)
		ldr.o (.bss)
		ldr_lib.o (.comment)
		ldr_lib.o (.eh_frame)
	}
}
