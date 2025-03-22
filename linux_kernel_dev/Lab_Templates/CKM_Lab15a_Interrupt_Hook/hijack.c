#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/desc.h>
#include <asm/pgtable.h>

#define DIVIDE_BY_ZERO_INDEX	0

/* helper functions */
static struct desc_ptr get_idt_desc(void);
static void make_addr_writable(u64 addr);
u64 get_abs_addr(u64 base, u32 rip_offset);

extern void my_handler(void); /* this our hook, defined in int_hook.S */
void my_do_handler(struct pt_regs* regs, long error_code); /* C portion of hook */

/* The following are used by our hook to mimic previous entry */
u64 error_entry_addr;
u64 error_exit_addr;
u64 do_handler_addr;

gate_desc* div_zero_gate; /* address of divide by zero gate in IDT */
gate_desc orig_gate; /* copy of original gate entry */

static int __init mod_init(void) {
	struct desc_ptr idt_desc;
	gate_desc* idt;
	gate_desc my_gate;
	u64 handler_addr;

	/* get information from idtr reg */
	idt_desc = get_idt_desc();

	/* get address of gate to hook and save original */
	idt = /* FIXME */;
	div_zero_gate = /* FIXME */;
	orig_gate = /* FIXME */;

	/* copy the gate and change values as desired */
	my_gate = /* FIXME */;
	//my_gate.bits.type = 0xF;
	//my_gate.bits.dpl = 0x3;

	printk(KERN_ALERT "IDT is at %#016llx\n", (u64)idt);
	/* get the handler address - note: shifting gate offsets might help */
	handler_addr = /* FIXME */; 
	printk(KERN_ALERT "divide_error is at %#016llx\n", handler_addr);

	/* The offset in these calls is the number of bytes from
	 * the beginning of handler_addr to the instruction following
	 * a 5-byte call or jmp.
	 *
	 * The offsets used here will have to be set for a specific
	 * kernel binary. Ideally we'd just programmatically determine
	 * these. */
	error_entry_addr = get_abs_addr(handler_addr, /* FIXME */);
	printk(KERN_ALERT "error_entry at %#016llx\n", error_entry_addr);

	error_exit_addr = get_abs_addr(handler_addr, /* FIXME */);
	printk(KERN_ALERT "error_exit_addr is %#016llx\n", error_exit_addr);

	do_handler_addr = get_abs_addr(handler_addr, /* FIXME */);
	printk(KERN_ALERT "do_handler_addr is %#016llx\n", do_handler_addr);
	
	/* Now we actually change the default gate.
	 * We first have to make the original IDT writable,and then we will
	 * split up the address of our handler to match the gate format */
	make_addr_writable((u64)idt);
	my_gate.offset_high = /* FIXME */;
	my_gate.offset_middle = /* FIXME */;
	my_gate.offset_low = /* FIXME */;

	/* TODO: memcpy our new gate entry over our old one */
	printk(KERN_ALERT "New gate set\n");

	return 0;
}

static void __exit mod_exit(void) {
	/* TODO: memcpy the old gate entry over the new one before you exit */
	printk(KERN_ALERT "Old gate set\n");

	return;
}

struct desc_ptr get_idt_desc(void) {
	struct desc_ptr idt_desc;
	__asm__ (/* TODO: use inline assembly */);

	return idt_desc;
}

void make_addr_writable(u64 addr) {
	unsigned int level;
	pte_t *pte;

	/* TODO: make address writable - lookup_address(), set_pte_atomic(), pte_mkwrite() might help with this */
	
	return;
}

void my_do_handler(struct pt_regs* regs, long error_code) {
	void (*orig_do_handler)(struct pt_regs*, long) = (void*)do_handler_addr;
	printk(KERN_ALERT "Hooked!, %016lx\n", regs->ip);
	/* dangerously assume that the bad divide is a 3-byte idiv */
	regs->ip += 3;

	/* or uncomment this to just invoke the original handler */
	//orig_do_handler(regs, error_code);

	return;
}


/* This calculates the absolute offset in these calls - is the number of bytes from
 * the instruction following a 5-byte call or jmp to the beginnig of handler_addr. */
u64 get_abs_addr(u64 base, u32 rip_offset) {
	u64 addr;
	addr = base + rip_offset;
	addr += *((s32*)(base + (rip_offset-4)));
	
	return addr;
}


module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark O'Neill");
MODULE_DESCRIPTION("IDT Hooking Solution");

