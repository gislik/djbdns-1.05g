#include "alloc.h"
#include "dns.h"
#include "stralloc.h"
#include "str.h"

int dns_domain_prependb(char **out, const char *d, const char *s, unsigned int n) {
  static stralloc sa;
  if (!stralloc_copyb(&sa, s, n)) return 0;
  if (!dns_domain_todot_cat(&sa, s)) return 0;
  if (!dns_domain_fromdot(out, sa.s, sa.len)) return 0;
  return 1;
}

inline int dns_domain_prepends(char **out, const char *d, const char *s) {
  return dns_domain_prependb(out, d, s, str_len(s));
}
