#include "alloc.h"
#include "dns.h"
#include "stralloc.h"

int dns_domain_prepend(char **out, const char *a, const char *b, unsigned int n) {
  static stralloc sa;
  if (!stralloc_copyb(&sa, b, n)) return 0;
  if (!dns_domain_todot_cat(&sa, a)) return 0;
  if (!dns_domain_fromdot(out, sa.s, sa.len)) return 0;
  return 1;
}
