// wchan.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "wchan.h"

QIntDict<char> Wchan::dict(17);
char *Wchan::sysmap = 0;
bool Wchan::sysmap_inited = FALSE;
int Wchan::sysmap_size = 0;

// return wchan symbol (possibly numeric, and empty string if addr=0)

QString Wchan::name(unsigned addr)
{
    char *p = dict[addr];
    if(!p) {
	if(addr == 0)
	    p = "";
	else if(sysmap || (!sysmap_inited && open_sysmap())) {
	    p = find_sym(addr);
	} else {
	    // no sysmap, use numeric format
	    p = (char *)malloc(12);
	    sprintf(p, "%x", addr);
	}
	dict.insert(addr, p);
    }
    QString s(p);
    return s;
}

// return TRUE if open succeeds
bool Wchan::open_sysmap()
{
    // common places to look for a valid System.map
    static char *paths[] = {
	"/boot/System.map-%s",
	"/boot/System.map",
	"/lib/modules/%s/System.map",
	"/usr/src/linux/System.map",
	0
    };
    sysmap_inited = TRUE;	// don't try again
    for(char **p = paths; *p; p++) {
	char buf[80];
	struct utsname ub;
	uname(&ub);
	sprintf(buf, *p, ub.release);
	if(try_sysmap(buf))
	    return TRUE;
    }
    return FALSE;
}

// try mapping System.map from path, return TRUE if success
bool Wchan::try_sysmap(const char *path)
{
    int fd = open(path, O_RDONLY);
    struct stat sbuf;
    if(fd >= 0 
       && fstat(fd, &sbuf) == 0) {
	sysmap_size = sbuf.st_size;
	unsigned pagesize = getpagesize();
	int mapsize = (sysmap_size + pagesize - 1) & ~(pagesize - 1);
	sysmap = (char *)mmap(0, mapsize, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);
     	if(sysmap != (char *)-1)
	    return TRUE;
    }
    close(fd);
    return FALSE;
}

inline int Wchan::beginning_of_line(int ofs)
{
    // seek backwards to beginning of line
    while(ofs >= 0 && sysmap[ofs] != '\n')
	ofs--;
    return ofs + 1;	
}

char *Wchan::find_sym(unsigned addr)
{
    // use binary search to find symbol; return malloced string
    int l = 0, r = sysmap_size;
    for(;;) {
	unsigned a;
	char buf[80];
	int m = (l + r) / 2;
	m = beginning_of_line(m);
	if(m == l) {
	    // see if there is a line further down
	    while(m < r - 1 && sysmap[m] != '\n')
		m++;
	    if(m < r - 1) {
		m++;
	    } else {
		m = l;
		sscanf(sysmap + m, "%x %*c %s", &a, buf);
		// strip leading sys_ or do_ to reduce field width
		char *p = buf;
		if(strncmp(buf, "do_", 3) == 0)
		    p += 3;
		if(strncmp(buf, "sys_", 4) == 0)
		    p += 4;
		return strdup(p);
	    }
	}
	sscanf(sysmap + m, "%x %*c %s", &a, buf);
	if(addr < a) {
	    r = m;
	} else {
	    l = m;
	}
    }
}
