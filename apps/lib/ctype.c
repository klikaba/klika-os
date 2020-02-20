#include <ctype.h>

int isspace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

int isprint(char c) {
  return ((c >= ' ' && c <= '~') ? 1 : 0);
}
