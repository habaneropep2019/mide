#include <stdio.h>
#include <unistd.h>
#include "cdbmake.h"

extern char *malloc();

void diesys(why) char *why; { perror(why); exit(111); }

void writeerror() { diesys("cdbmake: fatal: unable to write"); }

void nomem() { fputs("cdbmake: fatal: out of memory\n",stderr); exit(111); }

void overflow() { fputs("cdbmake: fatal: database too large\n",stderr); exit(111); }

void usage() { fputs("cdbmake: usage: cdbmake fn fntemp\n",stderr); exit(1); }

void format() { fputs("cdbmake: fatal: bad input format\n",stderr); exit(1); }

void readerror()
{
  if (ferror(stdin)) diesys("cdbmake: fatal: unable to read");
  format();
}

int get()
{
  int c;
  c = getchar();
  if (c == EOF) readerror();
  return c;
}

uint32 safeadd(u,v) uint32 u; uint32 v;
{
  u += v;
  if (u < v) overflow();
  return u;
}

struct cdbmake cdbm;
uint32 pos;
char packbuf[8];

int main(argc,argv)
int argc;
char **argv;
{
  char *fntemp;
  char *fn;
  FILE *fi;
  uint32 len;
  uint32 u;
  uint32 h;
  int i;
  int c;
  unsigned long keylen;
  unsigned long datalen;

  fn = argv[1];
  if (!fn) usage();
  fntemp = argv[2];
  if (!fntemp) usage();

  cdbmake_init(&cdbm);

  fi = fopen(fntemp,"w");
  if (!fi) diesys("cdbmake: fatal: unable to open temporary file");

  for (i = 0;i < sizeof(cdbm.final);++i)
    if (putc(' ',fi) == EOF)
      writeerror();

  pos = sizeof(cdbm.final);

  while (get() != '\n') {
    keylen = 0;
    while ((c = get()) != ',') keylen = keylen * 10 + (c - '0');
    datalen = 0;
    while ((c = get()) != ':') datalen = datalen * 10 + (c - '0');
    cdbmake_pack(packbuf,(uint32) keylen);
    cdbmake_pack(packbuf + 4,(uint32) datalen);

    if (fwrite(packbuf,1,8,fi) < 8) writeerror();

    h = CDBMAKE_HASHSTART;
    for (i = 0;i < keylen;++i) {
      c = get();
      h = cdbmake_hashadd(h,c);
      if (putc(c,fi) == EOF) writeerror();
    }
    if (get() != '-') format();
    if (get() != '>') format();
    for (i = 0;i < datalen;++i) {
      c = get();
      if (putc(c,fi) == EOF) writeerror();
    }
    if (get() != '\n') format();

    if (!cdbmake_add(&cdbm,h,pos,malloc)) nomem();
    pos = safeadd(pos,(uint32) 8);
    pos = safeadd(pos,(uint32) keylen);
    pos = safeadd(pos,(uint32) datalen);
  }

  if (!cdbmake_split(&cdbm,malloc)) nomem();

  for (i = 0;i < 256;++i) {
    len = cdbmake_throw(&cdbm,pos,i);
    for (u = 0;u < len;++u) {
      cdbmake_pack(packbuf,cdbm.hash[u].h);
      cdbmake_pack(packbuf + 4,cdbm.hash[u].p);
      if (fwrite(packbuf,1,8,fi) < 8) writeerror();
      pos = safeadd(pos,(uint32) 8);
    }
  }

  if (fflush(fi) == EOF) writeerror();
  rewind(fi);

  if (fwrite(cdbm.final,1,sizeof(cdbm.final),fi) < sizeof(cdbm.final)) writeerror();
  if (fflush(fi) == EOF) writeerror();

  if (fsync(fileno(fi)) == -1) diesys("cdbmake: fatal: unable to fsync");
  if (close(fileno(fi)) == -1) diesys("cdbmake: fatal: unable to close");

  if (rename(fntemp,fn)) diesys("cdbmake: fatal: unable to rename");
  exit(0);
}
