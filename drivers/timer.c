#include "timer.h"
#include "debug.h"
#include "common.h"
#include "idt.h"
#include "sched.h"

typedef int bool;

#define true  1
#define false 0
int32_t tick=(1000000+HZ/2)/HZ; //时钟滴答的时间间隔
unsigned long volatile jiffies; //全局变量jiffies

void timer_callback(pt_regs *regs)
{
/*
	static num=0;
	++num;
	if(num<100)
	return;
	else{
	num=0;
	++jiffies;
	timer temp=flash_time();
	printk_color(rc_black, rc_red, "Time: %d/%d/%d  %d:%d:%d\n",temp.year,temp.month,temp.day,temp.hour,temp.min,temp.second);
	}*/
	schedule();
	
}

timer flash_time(){
	timer store=data_timer;
	data_timer.second+=jiffies;
	data_timer.min+=data_timer.second/60;
	data_timer.second=data_timer.second%60;
	data_timer.hour+=data_timer.min/60;
	data_timer.min=data_timer.min%60;
	int temp=data_timer.hour/24;  //the add day
	data_timer.hour=data_timer.hour%24;

	//only deal with the time in one year 
	bool flag=false;	//judge the year is leap year
	if(data_timer.year%100==0&&data_timer.year%400==0)
	flag=true;
	while(temp!=0){
	if(data_timer.month==1||data_timer.month==3||data_timer.month==5||data_timer.month==7||data_timer.month==8||data_timer.month==10||data_timer.month==12)
	{
		if(temp+data_timer.day>31){
			temp=temp-(31-data_timer.day+1);
			data_timer.day=1;
			data_timer.month+=1;
			if(data_timer.month>12)
			data_timer.year+=1;
			if(data_timer.year%100==0&&data_timer.year%400==0)
			flag=true;
			else
			flag=false;	
		}
		else{
			data_timer.day+=temp;
			temp=0;
		}
	}
	if(data_timer.month==4||data_timer.month==6||data_timer.month==9||data_timer.month==11)
	{
		if(temp+data_timer.day>30){
			temp=temp-(30-data_timer.day+1);
			data_timer.day=1;
			data_timer.month+=1;
			if(data_timer.month>12)
			data_timer.year+=1;
			if(data_timer.year%100==0&&data_timer.year%400==0)
			flag=true;
			else
			flag=false;	
		}
		else{
			data_timer.day+=temp;
			temp=0;
		}
	}
	else{
		if(flag){
			if(temp+data_timer.day>28){
			temp=temp-(28-data_timer.day+1);
			data_timer.day=1;	
			data_timer.month+=1;
		}
		else{
			data_timer.day+=temp;
			temp=0;
		}		
		}
	}
	}
	timer t=store;
	store=data_timer;
	data_timer=t;
	return store;
}

void init_timer(int year,int month,int day,int hour,int min,int second)
{
	data_timer.year=year;
	data_timer.month=month;
	data_timer.day=day;
	data_timer.hour=hour;
	data_timer.min=min;
	data_timer.second=second;
	// 注册时间相关的处理函数
	register_interrupt_handler(IRQ0, timer_callback);

	// Intel 8253/8254 PIT芯片 I/O端口地址范围是40h~43h
	// 输入频率为 1193180，frequency 即每秒中断次数1193180 / frequency;
	uint32_t divisor = LATCH;

	// D7 D6 D5 D4 D3 D2 D1 D0
	// 0  0  1  1  0  1  0  0
	// 即就是 36 H
	// 设置 8253/8254 芯片工作在模式 2 下
	outb(0x43, 0x34);

	// 拆分低字节和高字节
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);
	
	// 分别写入低字节和高字节
	outb(0x40, low);
	outb(0x40, hign);

}
