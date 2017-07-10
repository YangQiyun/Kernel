
#ifndef INCLUDE_PMM_H
#define INCLUDE_PMM_H

#include "multiboot.h"

//内核线程栈大小位linux的8KB
#define STACK_SIZE 8192

// 支持的最大物理内存(512MB)
#define PMM_MAX_SIZE 0x20000000

// 物理内存页框大小 ,一页为4k
#define PMM_PAGE_SIZE 0x1000


struct buddy;
//struct buddy *self;//2^14
uint16_t mem_buff_count;//分配器内的页数
uint32_t mem_start_addr;//可用内存起始地址
void buddy_new(struct buddy *self_first,uint32_t size );

// 内核文件在内存中的起始和结束位置
// 在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];

//输出分配器各个size大小拥有的个数
void show_size();
// 输出 BIOS 提供的物理内存布局
void show_memory_map();

// 初始化物理内存管理
void init_pmm();

// 返回一个内存页的物理地址
uint32_t pmm_alloc_default();
uint32_t pmm_alloc_page(uint32_t size);

//释放内存地址索引所指向的分配页块
void buddy_free(struct buddy* self_,uint32_t offset);
void pmm_free(uint32_t addr);

// 释放申请的内存地址索引
uint32_t buddy_alloc(struct buddy* self, uint32_t size);

#endif 	// INCLUDE_PMM_H
