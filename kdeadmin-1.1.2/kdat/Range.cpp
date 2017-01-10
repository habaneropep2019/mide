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

#include <assert.h>

#include "Range.h"

Range::Range( int start, int end )
        : _start( start ),
          _end( end )
{
}

int Range::getStart()
{
    return _start;
}

int Range::getEnd()
{
    return _end;
}

void Range::setStart( int start )
{
    _start = start;
}

void Range::setEnd( int end )
{
    _end = end;
}

RangeList::RangeList()
{
}

RangeList::~RangeList()
{
    while ( _ranges.first() ) {
        delete _ranges.first();
        _ranges.removeFirst();
    }
}

const QList<Range>& RangeList::getRanges() const
{
    return _ranges;
}

void RangeList::addRange( int start, int end )
{
    assert( end >= start );

    if ( start == end ) {
        return;
    }

    // Remove all of the ranges that are totally contained by the new range.
    for ( _ranges.first(); _ranges.current(); ) {
        if ( ( _ranges.current()->getStart() >= start ) && ( _ranges.current()->getEnd() <= end ) ) {
            _ranges.remove();
        } else if ( ( start >= _ranges.current()->getStart() ) && ( end <= _ranges.current()->getEnd() ) ) {
            // The new range is totally contained in the current range.
            return;
        } else {
            _ranges.next();
        }
    }

    // Find the correct insertion point for the new range.
    int idx = 0;
    for ( _ranges.first(); _ranges.current(); _ranges.next(), idx++ ) {
        if ( _ranges.current()->getStart() > start ) {
            break;
        }
    }

    if ( !_ranges.current() ) {
        // Append new range to end of list.
        if ( ( _ranges.last() ) && ( _ranges.last()->getEnd() >= start ) ) {
            // Ranges overlap, merge them.
            _ranges.last()->setEnd( end );
        } else {
            // Append a new range.
            _ranges.append( new Range( start, end ) );
        }
    } else {
        // Insert new range before current range.
        if ( _ranges.current()->getStart() <= end ) {
            // Ranges overlap, merge them.
            _ranges.current()->setStart( start );
            end = _ranges.current()->getEnd();
            
            // Check previous range for overlap.
            if ( ( _ranges.prev() ) && ( _ranges.current()->getEnd() >= start ) ) {
                // New range overlapped both the before and after ranges.
                _ranges.current()->setEnd( end );
                _ranges.next();
                _ranges.remove();
            }
        } else {
            // Check previous range for overlap.
            if ( ( _ranges.prev() ) && ( _ranges.current()->getEnd() >= start ) ) {
                // New range overlapped the before range.
                _ranges.current()->setEnd( end );
            } else {
                _ranges.insert( idx, new Range( start, end ) );
            }
        }
    }
}

void RangeList::clear()
{
    _ranges.clear();
}
