#ifndef __WORDSIZE_H
#define __WORDSIZE_H

#if defined __x86_64__ && !defined __ILP32__
#define __WORDSIZE	64
#else
#define __WORDSIZE	32
#endif

#endif