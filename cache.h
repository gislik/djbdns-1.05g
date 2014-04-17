#ifndef CACHE_H
#define CACHE_H

#include "uint32.h"
#include "uint64.h"

extern uint64 cache_motion;
extern int cache_init(unsigned int);
extern void cache_set(const char *,unsigned int,const char *,unsigned int,uint32);
extern char *cache_get(const char *,unsigned int,unsigned int *,uint32 *);
#ifdef DUMPCACHE
extern int cache_dump(void);
extern int cache_slurp(const char *);
#endif
extern void cache_prefix_set(char *);
extern void cache_prefix_reset();
extern char *cache_prefix_get();
#endif
