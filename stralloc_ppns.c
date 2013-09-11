#include "stralloc.h"
#include "str.h"

int stralloc_prepends(stralloc *sa, const char *s) {
  return stralloc_prependb(sa, s, str_len(s));
}
