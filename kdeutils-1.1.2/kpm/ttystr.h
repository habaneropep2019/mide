// ttystr.h
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef TTYSTR_H
#define TTYSTR_H

#include <qstring.h>

class Ttystr {
public:
    static QString name(int uid);

private:
    static void read_devs();
    static int major_index(int major);
    static char *tabstr(int devnr);

    static char *tab;
    
    static const int DEVNAMELEN;
    static dev_t major_numbers[6];	// {2, 3, 4, 5, 19, 20}
    static const int NMAJORS;
    static const int NMINORS;
    static const int NDEVS;
    static const int DEVTABSIZE;
};

#endif	// TTYSTR_H

