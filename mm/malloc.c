#include "mypmm.h"
#include "vmm.h"
#include "debug.h"
#include "malloc.h"

static uint32_t start_addr = START;

void *kmalloc(uint32_t len)
{
	uint32_t page=pmm_alloc_page(len);
	//将申请到的物理内存对应到START起始的虚拟内存地址
	uint32_t va=page-(uint32_t)kern_end+start_addr+PAGE_OFFSET;
	map(pgd_kern, va, page, PAGE_PRESENT | PAGE_WRITE);
	return (void*)va;
}

void kfree(void *p){
	uint32_t va=(uint32_t)p;
	
	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_kern[pgd_idx] & PAGE_MASK);

	if (!pte) {
		unmap(pgd_kern,va);
		return;
	}

	// 转换到内核线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);

	uint32_t phy_addr=(uint32_t)(pte[pte_idx] & PAGE_MASK);
	phy_addr=(phy_addr+(va & 0x0FFF));
	
	pmm_free(phy_addr);
	unmap(pgd_kern,va);
}


void test_malloc(){
		printk_color(rc_black, rc_magenta, "Test kmalloc() && kfree() now ...\n\n");

	void *addr1 = kmalloc(50);
	printk("kmalloc    50 byte in 0x%X\n", addr1);
	void *addr2 = kmalloc(500);
	printk("kmalloc   500 byte in 0x%X\n", addr2);
	void *addr3 = kmalloc(5000);
	printk("kmalloc  5000 byte in 0x%X\n", addr3);
	void *addr4 = kmalloc(50000);
	printk("kmalloc 50000 byte in 0x%X\n\n", addr4);

	printk("free mem in 0x%X\n", addr1);
	kfree(addr1);
	printk("free mem in 0x%X\n", addr2);
	kfree(addr2);
	printk("free mem in 0x%X\n", addr3);
	kfree(addr3);
	printk("free mem in 0x%X\n\n", addr4);
	kfree(addr4);

	addr1 = kmalloc(50);
	printk("kmalloc    50 byte in 0x%X\n", addr1);
	addr2 = kmalloc(500);
	printk("kmalloc   500 byte in 0x%X\n", addr2);
	addr3 = kmalloc(5000);
	printk("kmalloc  5000 byte in 0x%X\n", addr3);
	addr4 = kmalloc(50000);
	printk("kmalloc 50000 byte in 0x%X\n\n", addr4);
}
