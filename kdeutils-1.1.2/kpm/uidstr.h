// uidstr.h
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef UIDSTR_H
#define UIDSTR_H

#include <qintdict.h>
#include <qstring.h>

class Uidstr {
public:
    static QString name(int uid);

private:
    static QIntDict<char> dict;
    
};

#endif	// UIDSTR_H
