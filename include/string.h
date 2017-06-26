
#ifndef INCLUDE_STRING_H_
#define INCLUDE_STRING_H_

#include "types.h"
#define NULL 0

static inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len)
{
	for (; len != 0; --len) {
		*dest++ = *src++;
	}
}

static inline void memset(void *dest, uint8_t val, uint32_t len)
{
	uint8_t *dst = (uint8_t *)dest;

	for ( ; len != 0; --len) {
		*dst++ = val;
	}
}


static inline char *strstr(const char *str1,const char *str2){
	//while()
}

static inline char *strchr(const char *str,char ch){
	while(*str++!=ch)
		NULL;
	if(*--str==0)
		return NULL;
	else
		return --str;
}

static inline char *strcpy(char *dest,const char *src)
{
	char *temp=dest;
	while((*dest++=*temp++)!='\0')
		NULL;
	return temp;
}

static inline int strlen(char *s)
{
	int size=0;
	while(*s++!=0)
		++size;
	return size;
}

static inline char *strcat(char *des,const char *src)
{
	char *temp=des;
	while(*des!='\0')
		++des;
	while((*des++=*src++)!='\0')
		NULL;
	return temp;
}
static inline  int strcmp(const char *str1,const char *str2)
{
	
	while(*str1&&*str2&&*str1==*str2){
		++str1;
		++str2;
	}
	return (*str1-*str2);
	
}

#endif 	// INCLUDE_STRING_H_


