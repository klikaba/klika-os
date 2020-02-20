#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static unsigned long int next = 1;

int rand() {
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

int rand_range(int lower, int upper) {
  return (rand() % (upper - lower + 1)) + lower;
}

void srand(unsigned int seed) {
  next = seed;
}


int __sprintf_index;

int sprintf_helper(unsigned c, void *ptr) {
  ((char*)ptr)[__sprintf_index++] = c;
  return 0;
}

void sprintf(char* buf, const char* fmt, ...) {
  __sprintf_index = 0;
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, sprintf_helper, buf);
  va_end(args);
}

#define   PR_LJ 0x01
#define   PR_CA 0x02
#define   PR_SG 0x04
#define   PR_64 0x08
#define   PR_32 0x10
#define   PR_WS 0x20
#define   PR_LZ 0x40
#define   PR_FP 0x80

#define   PR_BUFLEN 32

int _printf(const char *fmt, va_list args, fnptr_t fn, void *ptr)
{
unsigned state, flags, radix, actual_wd, count, given_wd;
char *where, buf[PR_BUFLEN];
long num;

  state = flags = count = given_wd = 0;
  for(; *fmt; fmt++)
  {
    switch(state)
    {
    case 0:
      if(*fmt != '%') /* not %... */
      {
        fn(*fmt, ptr);  /* ...just echo it */
        count++;
        break;
      }
      state++;
      fmt++;
      /* FALL THROUGH */
    case 1:
      if(*fmt == '%') /* %% */
      {
        fn(*fmt, ptr);
        count++;
        state = flags = given_wd = 0;
        break;
      }
      if(*fmt == '-')
      {
        if(flags & PR_LJ)/* %-- is illegal */
          state = flags = given_wd = 0;
        else
          flags |= PR_LJ;
        break;
      }
      state++;
      if(*fmt == '0')
      {
        flags |= PR_LZ;
        fmt++;
      }
      /* FALL THROUGH */
    case 2:
      if(*fmt >= '0' && *fmt <= '9')
      {
        given_wd = 10 * given_wd +
          (*fmt - '0');
        break;
      }
      state++;
      /* FALL THROUGH */
    case 3:
      if(*fmt == 'F')
      {
        flags |= PR_FP;
        break;
      }
      if(*fmt == 'N')
        break;
      if(*fmt == 'l')
      {
        flags |= PR_64;
        break;
      }
      if(*fmt == 'h')
      {
        flags |= PR_32;
        break;
      }
      state++;
      /* FALL THROUGH */
    case 4:
      where = buf + PR_BUFLEN - 1;
      *where = '\0';
      switch(*fmt)
      {
      case 'X':
        flags |= PR_CA;
        /* FALL THROUGH */
      case 'x':
      case 'p':
        flags |= PR_64;
        __attribute__ ((fallthrough));
      case 'n':
        radix = 16;
        goto DO_NUM;
      case 'b':
        radix = 2;
        goto DO_NUM;
      case 'd':
      case 'i':
        flags |= PR_SG;
        /* FALL THROUGH */
      case 'u':
        radix = 10;
        goto DO_NUM;
      case 'o':
        radix = 8;
DO_NUM:       if(flags & PR_64)
            num = va_arg(args, int64_t);
          else if(flags & PR_32)
          {
            if(flags & PR_SG)
              num = va_arg(args, int32_t);
            else
              num = va_arg(args, uint32_t);
          }
          else
          {
            if(flags & PR_SG)
              num = va_arg(args, int32_t);
            else
              num = va_arg(args, uint32_t);
          }
          if(flags & PR_SG)
          {
            if(num < 0)
            {
              flags |= PR_WS;
              num = -num;
            }
          }
          do
          {
            uint64_t temp;

            temp = (uint64_t)num % radix;
            where--;
            if(temp < 10)
              *where = temp + '0';
            else if(flags & PR_CA)
              *where = temp - 10 + 'A';
            else
              *where = temp - 10 + 'a';
            num = (uint64_t)num / radix;
          }
          while(num != 0);
        goto EMIT;
      case 'c':
        flags &= ~PR_LZ;
        where--;
        *where = (char)va_arg(args, int);
        actual_wd = 1;
        goto EMIT2;
      case 's':
        flags &= ~PR_LZ;
        where = va_arg(args, char *);
EMIT:
        actual_wd = strlen(where);
        if(flags & PR_WS)
          actual_wd++;
        if((flags & (PR_WS | PR_LZ)) ==
          (PR_WS | PR_LZ))
        {
          fn('-', ptr);
          count++;
        }
EMIT2:        if((flags & PR_LJ) == 0)
        {
          while(given_wd > actual_wd)
          {
            fn(flags & PR_LZ ? '0' :
              ' ', ptr);
            count++;
            given_wd--;
          }
        }
        if((flags & (PR_WS | PR_LZ)) == PR_WS)
        {
          fn('-', ptr);
          count++;
        }
        while(*where != '\0')
        {
          fn(*where++, ptr);
          count++;
        }
        if(given_wd < actual_wd)
          given_wd = 0;
        else given_wd -= actual_wd;
        for(; given_wd; given_wd--)
        {
          fn(' ', ptr);
          count++;
        }
        break;
      default:
        break;
      }
    __attribute__ ((fallthrough));
    default:
      state = flags = given_wd = 0;
      break;
    }
  }
  return count;
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
