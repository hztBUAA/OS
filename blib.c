#include <blib.h>

size_t strlen(const char *s) {
	size_t l = 0;
	for(l=0;s[l]!='\0';l++);
	return l;
}

char *strcpy(char *dst, const char *src) {
	int i;
	for(i = 0;src[i]!='\0';i++)
		dst[i]=src[i];
	dst[i]='\0';
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
	char *res = dst;
	while (*src && n--) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return res;
}

char *strcat(char *dst, const char *src) {
	char *res = dst;
	while(*dst){
		dst++;
	}

	while(*src){
		*dst++=*src++;
	}
	return res;
}

int strcmp(const char *s1, const char *s2) {
	while(*s1||*s2){
		if(*s1==*s2){
			s1++;
			s2++;
		}
		else if(*s1<*s2){
			return -1;
		}
		else{
			return 1;
		}

	}
	return 0;


}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n--) {
		if (*s1 != *s2) {
			return *s1 - *s2;
		}
		if (*s1 == 0) {
			break;
		}
		s1++;
		s2++;
	}
	return 0;
}

void *memset(void *s, int c, size_t n) {
	const unsigned char uc = c;
        unsigned char *su;
        for(su = s;0 < n;++su,--n)
           *su = uc;
         return s;
}
void *memcpy(void *out, const void *in, size_t n) {
	char *csrc = (char *)in;
	char *cdest = (char *)out;
	for (int i = 0; i < n; i++) {
		cdest[i] = csrc[i];
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	if(!n)
    {
        return 0;
    }
    // 当比较位数不为0时，且每位数据相等时，移动指针
    while(--n && *(char*)s1 == *(char*)s2)
    {
        s1 = (char*)s1 + 1;    // 转换类型，移动指针
        s2 = (char*)s2 + 1;
    }
    // 返回超过比较位数之后 比较的大小
    return( *((unsigned char *)s1) - *((unsigned char *)s2) ); 
}
