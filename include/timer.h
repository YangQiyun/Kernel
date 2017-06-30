#ifndef INCLUDE_TIMER_H_
#define INCLUDE_TIMER_H_

#include "types.h"

#define CLOCK_TICK_RATE 1193180  //Underlying HZ ,8254输入时钟脉冲的频率

#ifndef HZ  //时钟滴答的频率
#define HZ 100
#endif

#define LATCH ((CLOCK_TICK_RATE+HZ/2)/HZ) //8254计数器中的值,被除数表达式中的HZ／2也是用来将LATCH向上圆整成一个整数

typedef struct recordtime{
	int year;
	int month;
	int day;
	int hour;
	int min;
	int second;
}timer;

void init_timer(int year,int month,int day,int hour,int min,int second);

timer flash_time();

timer data_timer;

#endif 	// INCLUDE_TIMER_H_
