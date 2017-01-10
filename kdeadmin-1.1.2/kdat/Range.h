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

#ifndef _Range_h_
#define _Range_h_

#include <qlist.h>

/**
 * @short This class represents a range of tar records.
 */
class Range {
    int _start;
    int _end;
public:
    /**
     * Create a new range.
     *
     * @param start The first tar record in the range.
     * @param end   The last tar record in the range.
     */
    Range( int start = 0, int end = 0 );

    /**
     * Get the first tar record in this range.
     *
     * @return The starting tar record.
     */
    int getStart();

    /**
     * Get the last tar record in this range.
     *
     * @return The ending tar record.
     */
    int getEnd();

    /**
     * Set the first tar record in this range.
     *
     * @param start The starting tar record.
     */
    void setStart( int start );

    /**
     * Set the last tar record in this range.
     *
     * @param end The ending tar record.
     */
    void setEnd( int end );
};

/**
 * A simple list of Ranges.
 */
class RangeList {
    QList<Range> _ranges;
public:
    /**
     * Create an empty list of ranges.
     */
    RangeList();

    /**
     * Destroy each range in the list.
     */
    ~RangeList();

    /**
     * Get the simplified list of ranges.
     *
     * @return The list of ranges.
     */
    const QList<Range>& getRanges() const;

    /**
     * Intelligently add the given range to the list of ranges.  If possible,
     * the new range is merged with one (or two) of the existing ranges in
     * the list.  Otherwise, a new range is added to the list.
     *
     * @param start The starting tar record.
     * @param end   The ending tar record.
     */
    void addRange( int start, int end );

    /**
     * Erase the list of ranges.
     */
    void clear();
};

#endif
