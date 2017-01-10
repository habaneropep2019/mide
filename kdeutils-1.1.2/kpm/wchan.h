// wchan.h
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef WCHAN_H
#define WCHAN_H

#include <qintdict.h>
#include <qstring.h>

class Wchan {
public:
    static QString name(unsigned addr);

private:
    static bool open_sysmap();
    static bool try_sysmap(const char *path);
    static char *find_sym(unsigned addr);
    static inline int beginning_of_line(int ofs);

    static QIntDict<char> dict;
    static char *sysmap;	// pointer to mmap()ed System.map
    static bool sysmap_inited;	// if an attempt to read sysmap has been made
    static int sysmap_size;
};

#endif	// WCHAN_H
