#include <string.h>
#include <stdint.h>
#include <types.h>


// inline function to swap two numbers
void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
}

int abs(int n) {
	return n < 0 ? -n : n;
}

// function to reverse buffer[i..j]
char* reverse(char *buffer, int i, int j) {
	while (i < j)
		swap(&buffer[i++], &buffer[j--]);

	return buffer;
}

// Iterative function to implement itoa() function in C
char* itoa(int value, char* buffer, int base) {
	// invalid input
	if (base < 2 || base > 32)
		return buffer;

	// consider absolute value of number
	int n = abs(value);

	int i = 0;
	while (n)
	{
		int r = n % base;

		if (r >= 10) 
			buffer[i++] = 65 + (r - 10);
		else
			buffer[i++] = 48 + r;

		n = n / base;
	}

	// if number is 0
	if (i == 0)
		buffer[i++] = '0';

	// If base is 10 and value is negative, the resulting string 
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10)
		buffer[i++] = '-';

	buffer[i] = '\0'; // null terminate string

	// reverse the string and return it
	return reverse(buffer, 0, i - 1);
}


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

char* strncpy(char* dest, const char* src, size_t num) {
	char* ptr = dest;
	if (dest == NULL)
		return NULL;
	while (*src && num--) {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';

	return ptr;
}

typedef struct { unsigned char dummy [32]; } DT;

void fast_memcpy(unsigned char* dst, unsigned char* src, size_t s) {
	unsigned char* sa = src+s;
	DT *d1 = (DT*)dst - 1;
	DT *s1 = (DT*)src - 1;
	size_t si = s / sizeof(DT);

	si = (si + 7) / 8;
	switch(si % 8) 
	{
	 case 0: do { *++d1 = *++s1;
	 case 7:      *++d1 = *++s1;
	 case 6:      *++d1 = *++s1;
	 case 5:      *++d1 = *++s1;
	 case 4:      *++d1 = *++s1;
	 case 3:      *++d1 = *++s1;
	 case 2:      *++d1 = *++s1;
	 case 1:      *++d1 = *++s1;
           } while(--si > 0);
	}
	dst = (unsigned char*)d1;
	src = (unsigned char*)s1;
	while(src < sa) {
	  *++dst = *++src;
	}
}