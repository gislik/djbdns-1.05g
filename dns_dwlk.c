#include "dns.h"

inline int dns_domain_walk(char **dp, unsigned int *dlenp) 
{
  unsigned int n;
  if (!**dp) return 0;
  n = 1 + (unsigned int) **dp;
  if (dlenp) *dlenp -= n;
  *dp += n;
  return 1;
}

