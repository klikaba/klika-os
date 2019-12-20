#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <types.h>


// inline function to swap two numbers
void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
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

int memcmp (const void *str1, const void *str2, size_t count) {
  register const unsigned char *s1 = (const unsigned char*)str1;
  register const unsigned char *s2 = (const unsigned char*)str2;

  while (count-- > 0) {
    if (*s1++ != *s2++) {
		  return s1[-1] < s2[-1] ? -1 : 1;
    }
  }
  return 0;
}

size_t strlen(const char *str) {
	size_t s;
	for(s=0; *str!='\0'; str++) {
    	s++;
	}
	return s;
}

char* strcpy(char* dest, const char* src) {
	char *tmp = dest;
	while (*dest++ = *src++) ;
	return tmp;
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

int strcmp(const char* s1, const char* s2) {
  while(*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}