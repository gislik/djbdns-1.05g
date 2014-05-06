#include "alloc.h"
#include "dns.h"
#include "stralloc.h"
#include "str.h"

int dns_domain_prependb2(char **out, const char *d, const char *s1, unsigned int n1, const char *s2, unsigned int n2) {
  static stralloc sa;
  if (!stralloc_copyb(&sa, s1, n1)) return 0;
  if (s2 && n2) if (!stralloc_copyb(&sa, s2, n2)) return 0;
  if (!dns_domain_todot_cat(&sa, d)) return 0;
  if (!dns_domain_fromdot(out, sa.s, sa.len)) return 0;
  return 1;
}

inline int dns_domain_prependb(char **out, const char *d, const char *s, unsigned int n) {
  return dns_domain_prependb2(out, d, s, n, 0, 0);
}

inline int dns_domain_prepends(char **out, const char *d, const char *s) {
  return dns_domain_prependb(out, d, s, str_len(s));
}

inline int dns_domain_prepends2(char **out, const char *d, const char *s1, const char *s2) {
  return dns_domain_prependb2(out, d, s1, str_len(s1), s2, str_len(s2));
}
