#include "console.h"
#include "debug.h"
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "mypmm.h"

int kern_entry()
{
	init_debug();
	init_gdt();
	init_idt();

	console_clear();

	printk_color(rc_black, rc_green, "Hello, OS kernel!%d\n",112);

	printk("kernel in memory start: 0x%08X\n", kern_start);
	printk("kernel in memory end:   0x%08X\n", kern_end);
	printk("kernel in memory used:   %d KB\n\n", (kern_end - kern_start) / 1024);
	
	show_memory_map();
	init_pmm();

	show_size();
	uint32_t temp1=pmm_alloc_page(4097),temp2=pmm_alloc_default();
	printk("alloc addr :%08x\n",temp1);
	printk("alloc addr :%08x\n",temp2);
	pmm_free(temp1);
	printk("alloc addr :%08x\n",pmm_alloc_default());
	asm volatile ("sti");

	return 0;
}

