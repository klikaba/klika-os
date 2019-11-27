#ifndef __STDLIB_H
#define __STDLIB_H

typedef struct {
  int quot;
  int rem;
} div_t;

typedef struct {
  long int quot;
  long int rem;
} ldiv_t;

int rand();
void srand(unsigned int seed);

div_t div(int numer, int denom);
ldiv_t ldiv(long int numer, long int denom);

int abs(int n);
long int labs(long int n);
long long int llabs(long long int n);

#endif