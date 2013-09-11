#include "stralloc.h"
#include "byte.h"

int stralloc_prependb(stralloc *sa, const char *s, const unsigned int n) {
	char temp[sa->len];
	if (!sa->s) return stralloc_copyb(sa, s, n);	
	if (!stralloc_readyplus(sa, n+1)) return 0;
	byte_copy(temp, sa->len, sa->s); 
	byte_copy(sa->s+n, sa->len, temp); 
	byte_copy(sa->s, n, s);
	sa->len += n;
	sa->s[sa->len] = 'Z'; /* ``offensive programming'' */
	return 1;
}

