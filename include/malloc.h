#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"


//定义起始地址
#define START 0xE0000000

//内存申请
void *kmalloc(uint32_t len);

//内存释放
void kfree(void *p);

//测试内核的申请释放
void test_malloc();
#endif