#include "alloc.h"
#include "stralloc.h"

void stralloc_free(stralloc *sa) {
  // if(sa->s)
  //   alloc_free(sa->s);
  // sa->a = 0;
  // sa->s = 0;
  sa->len = 0;
}
