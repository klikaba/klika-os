#include <stdlib.h>

static unsigned long int next = 1;

int rand() {
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

div_t div(int numer, int denom) {
  div_t result;

  result.quot = numer / denom;
  result.rem = numer % denom;
  if ((result.rem ^ numer) < 0) {
		if (result.rem < 0) {
	    result.rem += denom;
	    result.quot -= 1;
		} else {
	    result.rem -= denom;
	    result.quot += 1;
		}
  }
  return result;
}


ldiv_t ldiv(long int numer, long int denom) {
  ldiv_t result;

  result.quot = numer / denom;
  result.rem = numer % denom;
  if ((result.rem ^ numer) < 0) {
    if (result.rem < 0) {
      result.rem += denom;
      result.quot -= 1;
    } else {
      result.rem -= denom;
      result.quot += 1;
    }
  }
  return result;
}


int abs(int n) {
  return n < 0 ? -n : n;
}

long int labs(long int n) {
  return n < 0 ? -n : n;
}

long long int llabs(long long int n) {
  return n < 0 ? -n : n;
}
