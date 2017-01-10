// ttystr.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <dirent.h>
#include "ttystr.h"

const int Ttystr::DEVNAMELEN = 8;
const int Ttystr::NMAJORS = sizeof(major_numbers) / sizeof(dev_t);
const int Ttystr::NMINORS = 256;
const int Ttystr::NDEVS = Ttystr::NMAJORS * Ttystr::NMINORS;
const int Ttystr::DEVTABSIZE = Ttystr::NDEVS * Ttystr::DEVNAMELEN;


char *Ttystr::tab = 0;
dev_t Ttystr::major_numbers[6] = {2, 3, 4, 5, 19, 20};

// return two last digits of tty name ("-" if devnr is zero, "?" if unknown)
// NOTE: this code uses the psdevtab database from the procps1.01 suite, and
// might stop working if a later procps version changes the file format

QString Ttystr::name(int devnr)
{
    if(devnr == 0)
	return QString("-");
    if(!tab) {
	// see if we can use /etc/psdevtab (from the procps suite)
	struct stat sb;
	int fd = open("/etc/psdevtab", O_RDONLY);
	if(fd < 0
	   || fstat(fd, &sb) < 0
	   || sb.st_size != DEVTABSIZE
	   || (tab = (char *)mmap(0, DEVTABSIZE,
				  PROT_READ, MAP_SHARED, fd, 0)) == (caddr_t)-1
	   || close(fd) < 0) {
	    // no psdevtab, and we don't fix it here (run ps as root to do it)
	    // scan /dev instead (takes time, but only do it once)
	    read_devs();
	}
    }
    char *ts = tabstr(devnr);
    QString s("?");
    if(ts && ts[0] != '\0') {
	char buf[DEVNAMELEN + 1];
	strncpy(buf, ts, DEVNAMELEN);
	buf[DEVNAMELEN] = '\0';
	char *p = strpbrk(buf, "yu"); // prefixes "tty" or "cu"
	if(p) {
	    s = "xx";
	    s[0] = p[1]; s[1] = p[2];     // copy two chars
	}
    }
    return s;
}

// scan /dev for tty names
void Ttystr::read_devs()
{
    tab = (char *)calloc(DEVTABSIZE, 1);
    DIR *d = opendir("/dev");
    struct dirent *e;
    struct stat sb;
    char name[80] = "/dev/";
    while((e = readdir(d)) != 0) {
	strncpy(name + 5, e->d_name, sizeof(name) - 6);
	if(lstat(name, &sb) >= 0 && S_ISCHR(sb.st_mode)) {
	    char *s = tabstr(sb.st_rdev);
	    if(s)
		strncpy(s, e->d_name, DEVNAMELEN);
	}
    }
    closedir(d);
}

// return pointer to tty name in table or NULL
char *Ttystr::tabstr(int devnr)
{
    int i = major_index(devnr >> 8); // i = major_index(major(devnr)); 
    int m = devnr & 0xff;            // m = minor(devnr)
    return (i >= 0) ? tab + DEVNAMELEN * (i * NMINORS + m) : 0;
}

// return index of device major number or -1 if none
int Ttystr::major_index(int major)
{
    int i;
    for(i = 0; i < NMAJORS; i++)
	if((int)major_numbers[i] == major)
	    return i;
    return -1;
}









