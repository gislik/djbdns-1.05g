#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include "open.h"
#include "error.h"
#include "str.h"
#include "byte.h"
#include "error.h"
#include "direntry.h"
#include "ip4.h"
#include "dns.h"
#include "openreadclose.h"
#include "roots.h"

static stralloc data;
static stralloc recursive;

static int roots_find(char *q)
{
  int i;
  int j;

  i = 0;
  while (i < data.len) {
    j = dns_domain_length(data.s + i);
    if (dns_domain_equal(data.s + i,q)) return i + j;
    i += j;
    i += 64;
  }
  return -1;
}

static int roots_search(char *q)
{
  int r;

  for (;;) {
    r = roots_find(q);
    if (r >= 0) 
      return r;
    if (!dns_domain_walk(&q, 0)) return -1; /* user misconfiguration */
  }
}

int roots(char servers[64], int *isrecursive, char *q)
{
  int r;
  r = roots_find(q);
  if (r == -1) return 0;
  byte_copy(servers,64,data.s + r);
  *isrecursive = !!recursive.s[r];
  return 1;
}

int roots_find2(char *q, char *prefix) 
{
  int r;
  static char *p = 0;
  if (!q) return 0;
  if (!dns_domain_prepends2(&p, q, prefix, "$")) return 0;
  r = roots_find(p);
  if (r >= 0) {
    if (byte_equal(data.s + r, 4, "\0\0\0\0")) return -r;
    return r;
  }
  return 0;
}

int roots_search2(char *q, char *prefix)
{
  int r;
  for (;;) {
    r = roots_find2(q, prefix);
    if (r) return r;
    if (!dns_domain_walk(&q, 0)) return 0;
  }

}

int roots2(char servers[64], int *isrecursive, char *q,char *prefix) 
{
  int r;
  for (;;) {
    r = roots_find2(q, prefix);
    if (r > 0) {
      if (servers) byte_copy(servers,64,data.s + r);
      if (isrecursive) *isrecursive = !!recursive.s[r];
      return 1;
    } else if (r < 0) {
      if (isrecursive) *isrecursive = !!recursive.s[r];
      return -1;
    }
    if (!dns_domain_walk(&q, 0)) return 0;
  }
}

int roots_same2(char *q1,char *q2, char *prefix)
{
  int r1, r2;
  do {
    r1 = roots_search2(q1, prefix);
    if (!dns_domain_walk(&q1, 0)) r1 = 0;
  } while (r1 < 0);
  do {
    r2 = roots_search2(q2, prefix);
    if (!dns_domain_walk(&q2, 0)) r2 = 0;
  } while (r2 < 0);
  if (r1 == r2) return 1;
  return roots_search(q1) == roots_search(q2);
}

static int init2(DIR *dir)
{
  direntry *d;
  const char *fqdn;
  static char *q;
  static stralloc text;
  char servers[64];
  char recbuf[64];
  int serverslen;
  int i;
  int j;

  for (;;) {
    errno = 0;
    d = readdir(dir);
    if (!d) {
      if (errno) return 0;
      return 1;
    }

    if (d->d_name[0] != '.') {
      if (openreadclose(d->d_name,&text,32) != 1) return 0;
      if (!stralloc_append(&text,"\n")) return 0;
      fqdn = d->d_name;
      {
        struct stat st;
        if (stat(fqdn,&st) == -1) return 0;
        if (st.st_mode & 01000)
        {
	  int a ;
	  for (a=0 ; a<64 ; a++) recbuf[a]=0xFF ;
        }
	else byte_zero(recbuf,64) ;
      }
      if (str_equal(fqdn,"@")) fqdn = ".";
      if (!dns_domain_fromdot(&q,fqdn,str_len(fqdn))) return 0;
      serverslen = 0;
      j = 0;
      for (i = 0;i < text.len;++i)
	if (text.s[i] == '\n') {
	  if (serverslen <= 60)
	    if (ip4_scan(text.s + j,servers + serverslen))
	      serverslen += 4;
	  j = i + 1;
	}
      byte_zero(servers + serverslen,64 - serverslen);

      if (!stralloc_catb(&data,q,dns_domain_length(q))) return 0;
      if (!stralloc_catb(&recursive,q,dns_domain_length(q))) return 0;
      if (!stralloc_catb(&data,servers,64)) return 0;
      if (!stralloc_catb(&recursive,recbuf,64)) return 0;
    }
  }
}

static int init1(void)
{
  DIR *dir;
  int r;

  if (chdir("servers") == -1) return 0;
  dir = opendir(".");
  if (!dir) return 0;
  r = init2(dir);
  closedir(dir);
  return r;
}

int roots_init(void)
{
  int fddir;
  int r;

  if (!stralloc_copys(&data,"")) return 0;
  if (!stralloc_copys(&recursive,"")) return 0;

  fddir = open_read(".");
  if (fddir == -1) return 0;
  r = init1();
  if (fchdir(fddir) == -1) r = 0;
  close(fddir);
  return r;
}
