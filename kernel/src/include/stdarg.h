#ifndef __STDARG_H
#define __STDARG_H

typedef __builtin_va_list va_list;
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

/*
#ifndef __STDARG_H
#define __STDARG_H

typedef unsigned char *va_list;

#define STACKITEM	int
#define VA_SIZE(TYPE)\
	 ((sizeof(TYPE) + sizeof(STACKITEM) - 1)\
        & ~(sizeof(STACKITEM) - 1))

#define va_start(AP, LASTARG)	\
	  (AP=((va_list)&(LASTARG) + VA_SIZE(LASTARG)))

#define va_end(AP)

#define va_arg(AP, TYPE)\
	 (AP += VA_SIZE(TYPE), *((TYPE *)(AP - VA_SIZE(TYPE))))

#endif
*/

#endif
