#include <string.h>
#include <stdint.h>
#include <types.h>

void *memset(void* dest, register int value, register uint64_t len) {
	register unsigned char *ptr = (unsigned char*)dest;
	while(len-- > 0) {
		*ptr++ = value;
	}
	return dest;
}

void *memcpy(void *dest, void *src, register uint64_t len) {
	register unsigned char *bdest = (unsigned char*)dest;
	register unsigned char *bsrc = (unsigned char*)src;
	while(len-- > 0) {
		*bdest++ = *bsrc++;
	}
	return dest;
}

size_t strlen(const char *str) {
	size_t s;
	for(s=0; *str!='\0'; str++) {
    	s++;
	}
	return s;
}
