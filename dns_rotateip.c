#include "byte.h"

void dns_rotateip(char *s, unsigned int n)
{
  char tmp[4];

  if (n > 4) { 
    byte_copy(tmp,4,s);
    byte_copy(s,n-4,s+4);
    byte_copy(s+n-4,4,tmp);
  }
}
