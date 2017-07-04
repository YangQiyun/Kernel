#include "multiboot.h"
#include "common.h"
#include "debug.h"
#include "mypmm.h"

// 分配器管理的内存页的连续块数，这个数字实际是由先打印出来的mem管理大小决定的，如果剩余的页数凑不到新的2的幂次个个数会被舍去，为了
//方便实现是这么决定的，但是有一种方案是linux里面采用的数组集合，可以拥有一个分配器的数组管理，最大效率利用内存


#define LEFT_LEAF(index) ((index) * 2 + 1)
#define RIGHT_LEAF(index) ((index) * 2 + 2)
#define PARENT(index) (((index) + 1) / 2 - 1)
//计算申请内存所需的页数
#define PAGE_DEAL(size) (((size+(PMM_PAGE_SIZE-1))&~(PMM_PAGE_SIZE-1))/PMM_PAGE_SIZE)

#define IS_POWER_OF_2(x) (!((x)&((x)-1)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))



struct buddy {
	uint32_t size;
	uint32_t longest[1];
};

//直接使用剩余的没法分配进分配器的页空间作为分配器的管理内存
static struct buddy self[PMM_PAGE_SIZE*10];//该值是用来存储节点多少,16384
static uint32_t fixsize(uint32_t size) {//将底部的所有的0全都用1替换，最后再加1
	for(uint32_t temp=1;temp!=268435456;temp*=2)
	size |= size >> temp;
	return size + 1;
}

void buddy_new(struct buddy *self_first,uint32_t size) {
	uint32_t node_size;
	uint32_t i;

	if (size < 1 || !IS_POWER_OF_2(size))
		return NULL;

	self_first->size = size;
	node_size = size * 2;

	for (i = 0; i < 2 * size - 1; ++i) {
		if (IS_POWER_OF_2(i + 1)) 
			node_size /= 2;
		self_first->longest[i] = node_size;
	}
}

void show_memory_map()//打印处multiboot为我们找到的BIOS提供的内存分布的缓冲区的整个范围
{
	uint32_t mmap_addr = glb_mboot_ptr->mmap_addr;
	uint32_t mmap_length = glb_mboot_ptr->mmap_length;

	printk("Memory map:\n");

	mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
	for (mmap = (mmap_entry_t *)mmap_addr; (uint32_t)mmap < mmap_addr + mmap_length; mmap++) {
		printk_color(rc_black,rc_light_brown,"base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
			(uint32_t)mmap->base_addr_high, (uint32_t)mmap->base_addr_low,
			(uint32_t)mmap->length_high, (uint32_t)mmap->length_low,
			(uint32_t)mmap->type);
	}
}

void init_pmm()
{

	mmap_entry_t *mmap_start_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr;
	mmap_entry_t *mmap_end_addr = (mmap_entry_t *)glb_mboot_ptr->mmap_addr + glb_mboot_ptr->mmap_length;
	
	mmap_entry_t *map_entry;

	for (map_entry = mmap_start_addr; map_entry < mmap_end_addr; map_entry++) {//循环multi提供的内存块，进行遍历可用的块

		// 如果是可用内存 ( 按照协议，1 表示可用内存，其它数字指保留区域 )
		if (map_entry->type == 1 && map_entry->base_addr_low == 0x100000) {

			// 把内核结束位置到结束位置的内存段革除
			uint32_t page_addr =mem_start_addr= map_entry->base_addr_low + (uint32_t)(kern_end - kern_start);
			uint32_t length = map_entry->base_addr_low + map_entry->length_low;//该块的限界
			
			//获得连续内存区域内页的块数
			uint32_t temp=page_addr;
			
			
			while (temp < length && temp <= PMM_MAX_SIZE ) {//地址小于该块的限界&支持的管理的内存界限内
				temp += PMM_PAGE_SIZE;
				mem_buff_count++;
			}
					
			printk("%ddsfsdfsdfs\n",mem_buff_count);
	
			mem_buff_count=fixsize(mem_buff_count)/2;
			buddy_new(self,mem_buff_count);
		}
	}
}

void show_size(){
uint32_t a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0;
for (uint32_t i = 0; i < 2 * mem_buff_count - 1; ++i) {
		if(self->longest[i]==1)
		++a;
if(self->longest[i]==2)
		++b;
if(self->longest[i]==4)
		++c;
if(self->longest[i]==8)
		++d;
if(self->longest[i]==16)
		++e;
if(self->longest[i]==32)
		++f;
if(self->longest[i]==64)
		++g;
if(self->longest[i]==128)
		++h;
		
	}
printk("1: %d\n2: %d\n4: %d\n8: %d\n16: %d\n32: %d\n64: %d\n128: %d\n", a,b,c,d,e,f,g,h);
printk("mem_buff_count: %d\n", mem_buff_count);
}


uint32_t buddy_alloc(struct buddy* self,uint32_t size) {
	size=PAGE_DEAL(size);
	uint32_t index = 0;
	uint32_t node_size;
	uint32_t offset = 0;//标记该分配的区域属于可用内存起始地址后的第几个页面

	if (self == NULL)
		return -1;

	if (size <= 0)
		size = 1;
	else if (!IS_POWER_OF_2(size))
		size = fixsize(size);

	if (self->longest[index] < size)//分配器中已经没有有足够的空间可以分配
		return -1;

	for (node_size = self->size; node_size != size; node_size /= 2) {
		if (self->longest[LEFT_LEAF(index)] >= size)
			index = LEFT_LEAF(index);
		else
			index = RIGHT_LEAF(index);
	}	

	self->longest[index] = 0;
	offset = (index + 1) * node_size - self->size;

	while (index) {
		index = PARENT(index);
		self->longest[index] =
			MAX(self->longest[LEFT_LEAF(index)], self->longest[RIGHT_LEAF(index)]);
	}

	return offset;
}

uint32_t pmm_alloc_default(){
	return buddy_alloc( self,4096)*PMM_PAGE_SIZE+mem_start_addr;
}
uint32_t pmm_alloc_page(uint32_t size){
	return buddy_alloc( self,size)*PMM_PAGE_SIZE+mem_start_addr;
}

void pmm_free(uint32_t addr){
	uint32_t offset=(addr-mem_start_addr)/PMM_PAGE_SIZE;
	buddy_free(self, offset);
}

void buddy_free(struct buddy* self_, uint32_t offset){
	uint32_t node_size, index = 0;
	uint32_t left_longest, right_longest;
	//若出现这些条件则会报错
	if (!(self_ && offset >= 0 && offset < self_->size)){
	 	printk("error in mypmm");
		return;
	}

	node_size = 1;
	index = offset + self_->size - 1;

	for (; self_->longest[index]; index = PARENT(index)) {//根据内存地址索引的位置向上检索以该内存地址为开始的节点是否为0
		node_size *= 2;
		if (index == 0)
			return;
	}

	self_->longest[index] = node_size;

	while (index) {//向上更新树节点的信息
		index = PARENT(index);
		node_size *= 2;

		left_longest = self_->longest[LEFT_LEAF(index)];
		right_longest = self_->longest[RIGHT_LEAF(index)];

		if (left_longest + right_longest == node_size)
			self_->longest[index] = node_size;
		else
			self_->longest[index] = MAX(left_longest, right_longest);
	}
}
