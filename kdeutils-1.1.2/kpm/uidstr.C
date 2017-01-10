// uidstr.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include <pwd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uidstr.h"

QIntDict<char> Uidstr::dict(101);

// return user name (possibly numeric)

QString Uidstr::name(int uid)
{
    char *p = dict[uid];
    if(!p) {
	struct passwd *pw = getpwuid(uid);
	if(!pw) {
	    p = (char *)malloc(10);
	    sprintf(p, "%d", uid);
	} else
	    p = strdup(pw->pw_name);
	dict.insert(uid, p);
    }
    QString s(p);
    return s;
}
