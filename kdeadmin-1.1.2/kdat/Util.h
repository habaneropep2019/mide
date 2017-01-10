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

#ifndef _Util_h_
#define _Util_h_

#include <qstring.h>

/**
 * @short A collection of common useful methods.
 */
class Util {
public:
    /**
     * Format the given number of bytes into a comma-separated string of
     * digits.
     */
    static QString bytesToString( uint bytes );

    /**
     * Format the given number of kilobytes into a comma-separated string of
     * digits, followed by a 'k'.
     */
    static QString kbytesToString( uint kbytes );

    /**
     * Find the longest common path prefix for a list of files.
     */
    static QString longestCommonPath( const QStrList& files );
};

#endif
