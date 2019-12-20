#ifndef __STRING_H
#define __STRING_H

#include <stdint.h>
#include <types.h>

char* reverse(char *buffer, int i, int j);
char* itoa(int value, char* buffer, int base);

void *memset(void* dest, register int value, register uint64_t len);
void *memcpy(void *dest, void *src, register uint64_t len);
void fast_memcpy(void* dst, void* src, size_t s);
int memcmp (const void *str1, const void *str2, size_t count);

size_t strlen(const char *str);
char* strcpy(char *dest, const char *src);
char* strncpy(char* dest, const char* src, size_t num);
int strcmp(const char* s1, const char* s2);
#endif