#ifndef QUERY_H
#define QUERY_H

#include "qmerge.h"
#include "uint32.h"

#define QUERY_MAXLEVEL 5
#define QUERY_MAXALIAS 16
#define QUERY_MAXNS 16
#define QUERY_MAXLOOP 200
#define QUERY_CACHEPREFIXLEN 2

struct query {
  unsigned int loop;
  unsigned int level;
  char *name[QUERY_MAXLEVEL];
  char *control[QUERY_MAXLEVEL]; /* pointing inside name */
  char *ns[QUERY_MAXLEVEL][QUERY_MAXNS];
  char servers[QUERY_MAXLEVEL][64];
  int isrecursive[QUERY_MAXLEVEL];
  char *alias[QUERY_MAXALIAS];
  uint32 aliasttl[QUERY_MAXALIAS];
  char localip[4];
  char type[2];
  char class[2];
  struct qmerge *qm;
  char cacheprefix[QUERY_CACHEPREFIXLEN];
} ;

extern int query_start(struct query *,char *,char *,char *,char *,char *);
extern void query_io(struct query *,iopause_fd *,struct taia *);
extern int query_get(struct query *,iopause_fd *,struct taia *);

extern void query_forwardonly(void);

#endif
