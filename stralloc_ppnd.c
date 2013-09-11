#include "stralloc.h"
#include "byte.h"

int stralloc_prepend(stralloc *sato, const stralloc *safrom) {
  return stralloc_prependb(sato, safrom->s, safrom->len);
}
