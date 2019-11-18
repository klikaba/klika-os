#ifndef __ASERT_H
#define __ASERT_H

#include <kernel.h>

#define __ASSERT_VOID_CAST (void)

#define assert(expr)							\
	((expr)													\
	 ? __ASSERT_VOID_CAST (0)				\
	 : __assert_fail (#expr, __FILE__, __LINE__, __func__))

static inline void __assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function) {
	HALT_AND_CATCH_FIRE("%s  %s:%i @ %s", __assertion, __file, __line, __function);
}

#endif