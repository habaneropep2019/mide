// KDat - a tar-based DAT archiver
// Copyright (C) 1998  Sean Vyain, svyain@mail.tds.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <qstrlist.h>

#include "Util.h"

QString Util::bytesToString( uint bytes )
{
    QString str;

    str.setNum( bytes );
    for ( int i = str.length() - 3; i > 0; i -= 3 ) {
        str.insert( i, ',' );
    }

    return str;
}

QString Util::kbytesToString( uint kbytes )
{
    QString str;

    str.setNum( kbytes );
    str += "k";
    for ( int i = str.length() - 4; i > 0; i -= 3 ) {
        str.insert( i, ',' );
    }

    return str;
}

QString Util::longestCommonPath( const QStrList& files )
{
    QStrListIterator i( files );

    uint minLen = strlen( i.toFirst() );
    for ( ; i.current(); ++i ) {
        if ( minLen > strlen( i.current() ) ) {
            minLen = strlen( i.current() );
        }
    }
    uint j;
    for ( j = 0; j < minLen; j++ ) {
        const char* first = i.toFirst();
        for ( ; i.current(); ++i ) {
            if ( strncmp( first, i.current(), j ) ) {
                // Prefixes are NOT the same.
                break;
            }
        }
        if ( i.current() ) {
            // The common prefix is 0 to j-1, inclusive.
            break;
        }
    }
    QString prefix = i.toFirst();
    int idx = prefix.findRev( '/', j );
    if ( idx > -1 ) {
        prefix = prefix.left( prefix.findRev( '/', j ) );
    } else {
        prefix = "";
    }

    return prefix;
}
