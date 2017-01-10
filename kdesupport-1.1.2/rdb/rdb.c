/* This is LGPLed free software.
 * This code implements a layer over Dan Bernstein's great CDB database
 * library.  Of all the libraries I've ever used, CDB is the only one
 * that never failed me.  I hope this library will be the second one.
 * Author: Felix von Leitner <leitner@math.fu-berlin.de>
 * At least rdb_compact_db partly taken from cdb!
 */

#include <config.h>

#include "cdb.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <cdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "hash.h"
#include "rdb.h"

/* lcc has no inline support */
#ifdef __LCC__
#define inline
#endif
/* In case someone compiles this with -ansi -pedantic */
#ifdef __GNUC__
#define inline __inline__
#endif

#ifndef O_SYNC
# define O_SYNC 0x0
#end

inline uint32 cdb_unpack(unsigned char *buf)
{
  uint32 num;
  num = buf[3]; num <<= 8;
  num += buf[2]; num <<= 8;
  num += buf[1]; num <<= 8;
  num += buf[0];
  return num;
}

/* Muß Datenbank mit cdb laden.
 * Muß Log einlesen und in Hash tun.
 * Das Log machen wir am besten auch im CDB-Format, nur ohne Leerzeile
 * am Ende, damit man einfach anfügen kann.
 */
int rdb_open(struct rdb *db, const char *filename, const char *logname,
	     int mode, unsigned int htlen) {
  char *logfile;
  off_t maplen;
  db->cdbfd=open(filename,O_RDONLY);
  if ((db->cdbfd==-1) && (errno!=ENOENT))
    return -2;
  db->eod=0;
  db->key.dsize=0; db->value.dsize=0;
  db->key.dptr=0; db->value.dptr=0;
  if (db->cdbfd!=-1) {
    char buf[4];
    if (read(db->cdbfd,buf,4)==4)
      db->eod = cdb_unpack(buf);
  }
  db->logfd=open(logname,O_RDONLY);
  if ((db->logfd==-1) && (errno!=ENOENT))
    return -2;
  db->db=strdup(filename);
  db->log=strdup(logname);
  db->mode=mode;
  ht_init(&db->ht,htlen);
  if (db->logfd!=-1) {
    maplen=lseek(db->logfd,0,SEEK_END);
    if (maplen != (off_t)-1) {
/*      fprintf(stderr,"Log file found, %ld bytes\n",maplen); */
      logfile=mmap(0,maplen,PROT_READ,MAP_SHARED,db->logfd,0);
      if (logfile != (char *)-1) {
	char *c=logfile,*max=logfile+maplen;
	while (c<max) {
	  int klen=0,vlen=0;
	  char *key=0,*value=0;
	  if (c>=max || *c != '+') return -1; c++;
	  while (c<max && isdigit(*c))
	    klen=klen*10+(*(c++) -'0');
	  if (c>=max || *c != ',' ) return -1; c++;
	  if (*c=='-' && *(c+1)=='1') {	/* special case: deleted key */
	    c+=2; vlen=-1;
	  } else {
	    while (c<max && isdigit(*c))
	      vlen=vlen*10+(*(c++) -'0');
	  }
	  if (c>=max || *c != ':' ) return -1; c++;
	  key=c;
	  if (c+klen>=max) return -1; c+=klen;
	  if (c>=max || *c != '-' ) return -1; c++;
	  if (c>=max || *c != '>' ) return -1; c++;
	  if (vlen>-1) {
	    value=c;
	    if (c+vlen>=max) return -1; c+=vlen;
	  } else
	    value=0;
	  if (c>=max || *c != '\n' ) return -1; c++;
	  if (vlen>-1)
	    ht_insert(&db->ht,klen,key,vlen,value);
	  else {
	    char *temp=(char *)malloc(klen);
	    memmove(temp,key,klen);
	    ht_insert_nocopy(&db->ht,klen,temp,-1,0);
	  }
	  if (c==max) break;
	}
	munmap(logfile,maplen);
      }
    }
    close(db->logfd);
  }
  db->logfd=open(logname,O_WRONLY|O_APPEND|O_SYNC|O_CREAT,mode);
  if (db->logfd == -1)
    return -2;
  return 0;
}

int rdb_close(struct rdb *db) {
  ht_delete_all(&db->ht);
  if (db->cdbfd>=0)
    if (close(db->cdbfd))
      return -1;
  if (db->logfd>=0)
    if (close(db->logfd))
      return -1;
  free(db->db);
  free(db->log);
  free(db->key.dptr);
  free(db->value.dptr);
  return 0;
}

int rdb_reorganize(struct rdb *db) { /* blatantly taken from cdb */
/* returns -1 on read error, -2 on format violation, -3 on cdbmake error */
  FILE *in=0;
  FILE *out;
  char buf[8];
  uint32 pos,klen,dlen;
  int i;
  char *key,*value;

  {
    char buf[1024];
#ifdef HAVE_SNPRINTF
    snprintf(buf,1023,"cdbmake %s %s.tmp",db->db,db->db);
#else
    sprintf(buf,"cdbmake %s %s.tmp",db->db,db->db);
#endif
    umask(0777-db->mode);
    out=popen(buf,"w");
    if (out==NULL)
      return -3;
  }

  if (db->cdbfd>=0)
    in=fdopen(db->cdbfd,"r");
  if (in) {
    fseek(in,0,SEEK_SET);
    for (i = 0;i < 2048;++i) if (fgetc(in) == EOF) return -1;
    pos = 2048;
    while (pos < db->eod) {
      if (db->eod - pos < 8) return -2;
      pos += 8;
      if (fread(buf,1,8,in) < 8) return -1;
      klen = cdb_unpack(buf);
      dlen = cdb_unpack(buf + 4);
      if (db->eod - pos < klen) return -2;
      pos += klen;
      if (db->eod - pos < dlen) return -2;
      pos += dlen;
      key=alloca(klen);
      if (fread(key,1,klen,in)!=klen) return -1;
      value=alloca(dlen);
      if (fread(value,1,dlen,in)!=dlen) return -1;
      {
	int dummyint;
	char *dummyval;
	if (ht_lookup(&db->ht,klen,key,&dummyint,&dummyval)) {
	  fprintf(out,"+%lu,%lu:",(unsigned long) klen,(unsigned long) dlen);
	  fwrite(key,klen,1,out);
	  fputs("->",out);
	  fwrite(value,dlen,1,out);
	  fputs("\n",out);
	}
      }
    }
  }
  if (!ht_findfirst(&db->ht,(unsigned int *)&klen,(const char **)&key)) {
    do {
      if (ht_lookup(&db->ht,(unsigned int)klen,key,
		            (unsigned int *)&dlen,&value))
	return -2;
      if (dlen!=-1) {
	fprintf(out,"+%lu,%lu:",(unsigned long) klen,(unsigned long) dlen);
	fwrite(key,klen,1,out);
	fputs("->",out);
	fwrite(value,dlen,1,out);
	fputs("\n",out);
      }
    } while (!ht_findnext(&db->ht,(unsigned int *)&klen,(const char **)&key));
  }
  fputs("\n",out);
  if (pclose(out))
    return -4;
  if (close(db->cdbfd))
    return -5;
  if (close(db->logfd))
    return -5;
  db->cdbfd=open(db->db,O_RDONLY);
  db->eod=0;
  db->key.dsize=0; db->value.dsize=0;
  db->key.dptr=0; db->value.dptr=0;
  if (db->cdbfd!=-1) {
    char buf[4];
    if (read(db->cdbfd,buf,4)==4)
      db->eod = cdb_unpack(buf);
  }
  db->logfd=open(db->log,O_TRUNC|O_WRONLY|O_APPEND|O_SYNC|O_CREAT,db->mode);
  if (db->logfd == -1)
    return -6;
  return 0;
}

int rdb_store(struct rdb *db,datum key,datum content,int flags) {
  datum old;
  char *c,*d;
  if (flags)
    ht_insert(&db->ht,key.dsize,key.dptr,content.dsize,content.dptr);
  else {
    if (ht_lookup(&db->ht,key.dsize,key.dptr,&old.dsize,&old.dptr))
      if (old.dsize!=-1)
	return -1;
    ht_insert(&db->ht,key.dsize,key.dptr,content.dsize,content.dptr);
  }
  c=(char *)alloca(key.dsize+content.dsize+100);
  sprintf(c,"+%lu,%lu:",(unsigned long)key.dsize,(unsigned long)content.dsize);
  d=c+strlen(c);
  memmove(d,key.dptr,key.dsize); d+=key.dsize;
  *(d++)='-'; *(d++)='>';
  memmove(d,content.dptr,content.dsize); d+=content.dsize;
  *(d++)='\n';
  write(db->logfd,c,d-c);
  return 0;
}

datum rdb_fetch(struct rdb *db, datum key) {
  datum value;
  int j;
  if (!ht_lookup(&db->ht,key.dsize,key.dptr,&value.dsize,&value.dptr))
    return value;
  if (key.dsize==db->key.dsize && !memcmp(key.dptr,db->key.dptr,key.dsize))
    return db->value;
  j=cdb_seek(db->cdbfd,key.dptr,key.dsize,&value.dsize);
  if (j==1) {
    value.dptr=(char *)malloc(value.dsize);
    if (read(db->cdbfd,value.dptr,value.dsize)==value.dsize) {
      char *k=(char *)malloc(key.dsize);
      memmove(k,key.dptr,key.dsize);
      ht_insert_nocopy(&db->ht,key.dsize,k,value.dsize,value.dptr);
      return value;
    }
  }
  value.dsize=0; value.dptr=0;
  return value;
}

int rdb_delete(struct rdb *db, datum key) {
  char *c,*d;
  int res;
  if ((res=ht_delete_mark(&db->ht,key.dsize,key.dptr))) {
    int j,dummy;
    if (res==-2)
      return -2;
    j=cdb_seek(db->cdbfd,key.dptr,key.dsize,&dummy);
    if (j!=1)
      return -1;
  }
  c=(char *)alloca(key.dsize+100);
  sprintf(c,"+%lu,-1:",(unsigned long)key.dsize);
  d=c+strlen(c);
  memmove(d,key.dptr,key.dsize); d+=key.dsize;
  *(d++)='-'; *(d++)='>'; *(d++)='\n';
  write(db->logfd,c,d-c);
  return 0;
}

/* This function is ugly.
 * First we look in the hash tree for the first entry that is not marked
 * as deleted (length of value <0).
 * If that does not find anything, we read the first element from the
 * CDB file.
 */
datum rdb_firstkey(struct rdb *db) {
  datum key;
  free(db->key.dptr); db->key.dptr=0; db->key.dsize=-1;
  if (!ht_findfirst(&db->ht,&key.dsize,(const char **)&key.dptr)) {
    do {
      int dlen;
      char *value;
      if (ht_lookup(&db->ht,(unsigned int)key.dsize,key.dptr,
			    (unsigned int *)&dlen,&value)) {
	key.dsize=-1;
	key.dptr=0;
	return key;
      }
      if (dlen!=-1)
	return key;
    } while (!ht_findnext(&db->ht,&key.dsize,(const char **)&key.dptr));
  }
  key.dsize=-1; key.dptr=0;	/* ptr=0 is the error return */
  if (db->cdbfd!=-1) {
    uint32 pos;
    char buf[8];
    lseek(db->cdbfd,2048,SEEK_SET);
    pos = 2048;
    if (db->eod - pos < 8) return key;
    pos += 8;
    if (read(db->cdbfd,buf,8) < 8) return key;
    db->key.dsize = cdb_unpack(buf);
    db->value.dsize = cdb_unpack(buf + 4);
    if (db->eod - pos < db->key.dsize) return key;
    pos += db->key.dsize;
    if (db->eod - pos < db->value.dsize) return key;
    pos += db->value.dsize;
    free(db->key.dptr);
    db->key.dptr=(char *)malloc(db->key.dsize);
    free(db->value.dptr);
    db->value.dptr=(char *)malloc(db->value.dsize);
    if (!db->key.dptr || !db->value.dptr)
      return key;
    if (read(db->cdbfd,db->key.dptr,db->key.dsize)!=db->key.dsize)
      return key;
    if (read(db->cdbfd,db->value.dptr,db->value.dsize)!=db->value.dsize)
      return key;
    return db->key;
  }
  return key;
}

datum rdb_nextkey(struct rdb *db,datum key) {
  /* Even more messy.  First find out if we are still traversing the
   * hash tree or already in the CDB file. */
  if (db->key.dptr==0) {
    /* still in the hash tree */
    while (!ht_findnext(&db->ht,&key.dsize,(const char **)&key.dptr)) {
      int dlen;
      char *value;
      if (ht_lookup(&db->ht,(unsigned int)key.dsize,key.dptr,
			    (unsigned int *)&dlen,&value)) {
	key.dsize=-1;
	key.dptr=0;
	return key;
      }
      if (dlen!=-1)
	return key;
    };
  }
  key.dsize=-1; key.dptr=0;	/* ptr=0 is the error return */
  if (db->cdbfd!=-1) {
    uint32 pos;
    datum value;
    char buf[8];
    int j;
    if (db->key.dptr) {
      j=cdb_seek(db->cdbfd,db->key.dptr,db->key.dsize,&value.dsize);
      if (j!=1) return key;
      pos=(uint32)lseek(db->cdbfd,value.dsize,SEEK_CUR);
    } else {
      pos=2048;
      lseek(db->cdbfd,2048,SEEK_SET);
    }
    free(db->key.dptr);
    free(db->value.dptr);
    db->key.dptr=db->value.dptr=0;
    db->key.dsize=db->value.dsize=-1;
    while (db->eod > pos) {
      if (db->eod - pos < 8) return key;
      pos += 8;
      if (read(db->cdbfd,buf,8) < 8) return key;
      db->key.dsize = cdb_unpack(buf);
      db->value.dsize = cdb_unpack(buf + 4);
      if (db->eod - pos < db->key.dsize) return key;
      pos += db->key.dsize;
      if (db->eod - pos < db->value.dsize) return key;
      pos += db->value.dsize;
      db->key.dptr=(char *)malloc(db->key.dsize);
      db->value.dptr=(char *)malloc(db->value.dsize);
      if (!db->key.dptr || !db->value.dptr)
	return key;
      if (read(db->cdbfd,db->key.dptr,db->key.dsize)!=db->key.dsize)
	return key;
      if (read(db->cdbfd,db->value.dptr,db->value.dsize)!=db->value.dsize)
	return key;
      if (ht_lookup(&db->ht,(unsigned int)db->key.dsize,db->key.dptr,
			    (unsigned int *)&value.dsize,&value.dptr))
	return db->key;
      if (db->key.dsize!=-1)
	continue;
      return db->key;
    }
  }
  return key;
}

#ifdef MAIN
int main() {
  struct rdb db;
  datum a,b;
  printf("rdb_open: %d\n",rdb_open(&db,"db","log",S_IRUSR|S_IWUSR,1021));
  printf("rdb_reorganize: %d\n",rdb_reorganize(&db));
  a=rdb_firstkey(&db);
  while (a.dptr) {
    printf("Found key `%.*s'\n",a.dsize,a.dptr);
    a=rdb_nextkey(&db,a);
  }
  a.dsize=3; a.dptr="foo";
  b.dsize=3; b.dptr="bar";
/*  rdb_delete(&db,a); */
/*  rdb_store(&db,a,b,1); */
  b.dsize=5; b.dptr="Three";
  a=rdb_fetch(&db,b);
  printf("Three -> `%.*s'\n",a.dsize,a.dptr);
  rdb_close(&db);
  return 0;
}
#endif
