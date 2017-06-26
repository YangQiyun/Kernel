
#include "console.h"
#include "string.h"
#include "vargs.h"
#include "debug.h"
#include "ctype.h"

#define ZEROPAD	1		/* pad with zero 输出前补0填充满宽度 */
#define SIGN	2		/* unsigned/signed long 有符号数，默认为无符号 */
#define PLUS	4		/* show plus 在数字前显示+-号表示正负 */
#define SPACE	8		/* space if plus 输出为正时用空格，为负时用负号 */
#define LEFT	16		/* left justified 左对齐，右边填充空格 */	
#define SMALL	32		/* Must be 32 == 0x20 字母为小写 */
#define SPECIAL	64		/* 0x 即在type为o x X时前面加上0 0x 0X */

#define	NUM_TYPE int    

#define do_div(n,base) ({ \
		int __res; \
		__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base)); \
		__res; })


void printk(const char* format,...){

	static char buff[1024];
	va_list args;
	int i;

	va_start(args, format);
	i = vsprintf(buff, format, args);
	va_end(args);

	buff[i] = '\0';

	console_write(buff);
}

void printk_color(real_color_t back, real_color_t fore, const char *format, ...)
{
	// 避免频繁创建临时变量，内核的栈很宝贵
	static char buff[1024];
	va_list args;
	int i;

	va_start(args, format);
	i = vsprintf(buff, format, args);
	va_end(args);

	buff[i] = '\0';

	console_write_color(buff, back, fore);
}
//字符数字转换成int数字
static int skip_atoi(const char **s)
{
	int i=0;
	while(isdigit(**s))
		i=i*10+(int)*((*s)++)-48;
	return i;
}

//将需要输入的字符串传入指定的位置
static char* string(char* str,char* s,int field_width,int precision,int flags){
	int len;
	if(0==s)
		s="<NULL>";
	len=strlen(s);
	
	if(precision<0)
		precision=len;
	else if(precision<len)
		len=precision;
		
	if(!(flags&LEFT)){
	
		while(len<field_width--)
			*str++=' ';
}
	while(0<len--){
		--field_width;
		*str++=*s++;
	}
	while(0<field_width--)
		*str++=' ';
	return str;
}

//将需要输入的数字按照指定的规格传入指定的位置
static char *number(char *str, int num, int base, int size, int precision, int type)
{
	char c, sign, tmp[36];
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i;

	if (type & SMALL) {
		digits ="0123456789abcdefghijklmnopqrstuvwxyz";
	}
	if (type & LEFT) {
		type &= ~ZEROPAD;
	}
	if (base < 2 || base > 36) {
		return 0;
	}

	c = (type & ZEROPAD) ? '0' : ' ' ;

	if (type & SIGN && num < 0) {
		sign = '-';
		num = -num;
	} else {
		sign = (type&PLUS) ? '+' : ((type&SPACE) ? ' ' : 0);
	}

	if (sign) {
	      size--;
	}
	if (type & SPECIAL) {
		if (base == 16) {
			size -= 2;
		} else if (base == 8) {
			size--;
		}
	}
	i = 0;
	if (num == 0) {
		tmp[i++] = '0';
	} else {
		while (num != 0) {
			tmp[i++] = digits[do_div(num,base)];
		}
	}

	if (i > precision) {
		precision = i;
	}
	size -= precision;

	if (!(type&(ZEROPAD+LEFT))) {
		while (size-- > 0) {
			*str++ = ' ';
		}
	}
	if (sign) {
		*str++ = sign;
	}
	if (type & SPECIAL) {
		if (base == 8) {
			*str++ = '0';
		} else if (base == 16) {
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type&LEFT)) {
		while (size-- > 0) {
			*str++ = c;
		}
	}
	while (i < precision--) {
		*str++ = '0';
	}
	while (i-- > 0) {
		*str++ = tmp[i];
	}
	while (size-- > 0) {
		*str++ = ' ';
	}

	return str;
}

int vsprintf(char *buf,const char* fmt,va_list args)
{
	unsigned NUM_TYPE num;//需要转换为实际的字符串的数字
	int base;//数字的进制位数
	char *str;//作为一个转存到buf的游标
	
int *ip;
	int flags;  //输出的格式形式
	
	int field_width; //对于需要被输出的参数的实际指定范围
	int precision;   //digit需要被精确的位数
	
	int qualifier;  /* 'h', 'l', or 'L' for integer fields */
	
	str=buf; //将游标指向buf的首位
	
	for(;*fmt;++fmt){//遍历fmt字符串，停止位为字符串规定的最后一个字符为0的标志
		if(*fmt!='%'){
		    *str++=*fmt;
			continue;
		}
		
		/*process flags 输出的格式标准*/
		flags=0;
		int pro_flag;
		do{
			pro_flag=0;
			++fmt;
			switch(*fmt){
				case '-':
					flags|=LEFT;
					pro_flag=1;
					break;
				case '+':
					flags|=PLUS;
					pro_flag=1;
					break;
				case ' ':
					flags|=SPACE;
					pro_flag=1;
					break;
				case '#':
					flags|=SPECIAL;
					pro_flag=1;
					break;
				case '0':
					flags|=ZEROPAD;
					pro_flag=1;
					break;
			}
		}while(pro_flag!=0);
		
		/*get field width 获取数字输出域的范围大小*/
		field_width=-1;
		
		/*此处为了防止简单的宏定义判断是否为数字的字符会导致的参数求值两次等的问题，使一个或多个转换表的集合#define _XXXMASK 0x...
         *#define isXXX(c) (_Ctytable[c] & _XXXMASK)  
	     *借用了ctype的定义
	     */
		if(isdigit(*fmt))
		    field_width=skip_atoi(&fmt); 
		else if(*fmt=='*'){//表示精度的参数放置在参数列表中
			++fmt;
			field_width=va_arg(args,int);
			if(field_width<0){
				field_width=-field_width;
				flags|=LEFT;
			}
				
		}
		
		
		
		/*get the precision 获取精度*/
		precision=-1;
		if(*fmt=='.'){
			++fmt;
			if(isdigit(*fmt))
				precision=skip_atoi(&fmt);
			else if(*fmt=='*'){
				++fmt;
				precision=va_arg(args,int);
			}
			if(precision<0)
				precision=0;
		}
		
		/*get the conversation qualifier*/
		qualifier=-1;
		/*
		 *h 代表int 
		 *l 代表long/float
		 *L 代表long long
		 */
		if(*fmt=='h'||*fmt=='l'||*fmt=='L'){//01101100  108
			qualifier=*fmt;
			++fmt;
		}
		
		/*default base  默认为十进制数*/
		base=10; 
		
		/*对操作的类型进行判断*/
		switch(*fmt){
			case 'c':
				if(!(flags&LEFT))
					while(--field_width>0)
						*str++=' ';
				*str++=(unsigned char)va_arg(args,int);   //对于单个字符需求取的是4个字节的参数大小？
				while(--field_width>0)
					*str++=' ';
				continue;
			case 's':
				str=string(str,va_arg(args,char*),field_width,precision,flags);
				continue;
			case 'o':
			base=8;
			break;

		case 'p':
			if (field_width == -1) {
				field_width = 8;
				flags |= ZEROPAD;
			}
			str = number(str, (unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;

		case 'x':
			flags |= SMALL;
		case 'X':
			base=16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;
		case 'b':
			base=2;
			break;

		case 'n':
			ip = va_arg(args, int *);
			*ip = (str - buf);
			break;

		default:
			if (*fmt != '%')
				*str++ = '%';
			if (*fmt) {
				*str++ = *fmt;
			} else {
				--fmt;
			}
			continue;
		}
		if(qualifier=='l'){
			num=va_arg(args,unsigned long);
			if(flags&SIGN)
				num=(signed long)num;
		}else if(qualifier=='h'){
			num=(unsigned short)va_arg(args,int);
			if(flags&SIGN)
				num=(signed short)num;
		}else if(qualifier=='L'){
		       num=(unsigned long long)va_arg(args,unsigned long long);
			if(flags&SIGN)
				num=(signed long long)num;
		}else{
		        num=va_arg(args,unsigned int);
			if(flags&SIGN)
				num=(signed int)num;
		}
		str = number(str, num, base,field_width, precision, flags);
	}
	*str = '\0';

	return (str -buf);
}
