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
#include <values.h>

#include "File.h"

File::File( File* parent, int size, int mtime, int startRecord, int endRecord, const char* name )
        : _stubbed( FALSE ),
          _name( name ),
          _parent( parent )
{
    assert( endRecord >= startRecord );

    _union._data._size        = size;
    _union._data._mtime       = mtime;
    _union._data._startRecord = startRecord;
    _union._data._endRecord   = endRecord;
}

File::File( File* parent, FILE* fptr, int offset )
        : _stubbed( TRUE ),
          _parent( parent )
{
    _union._stub._fptr   = fptr;
    _union._stub._offset = offset;
}

File::~File()
{
    while ( _children.first() ) {
        delete _children.first();
        _children.removeFirst();
    }
}

void File::read( int version )
{
    if ( !_stubbed ) {
        return;
    }

    _stubbed = FALSE;

    FILE* fptr = _union._stub._fptr;

    fseek( fptr, _union._stub._offset, SEEK_SET );
    
    // File name (4 bytes + n chars).
    int ival;
    fread( &ival, sizeof( ival ), 1, fptr );
    _name.resize( ival+1 );
    fread( _name.data(), sizeof( char ), ival, fptr );

    // File size (4 bytes).
    fread( &ival, sizeof( ival ), 1, fptr );
    _union._data._size = ival;

    // File modification time (4 bytes).
    fread( &ival, sizeof( ival ), 1, fptr );
    _union._data._mtime = ival;

    // Start record number.
    fread( &ival, sizeof( ival ), 1, fptr );
    _union._data._startRecord = ival;
    
    // End record number.
    fread( &ival, sizeof( ival ), 1, fptr );
    _union._data._endRecord = ival;

    //%%% This is a kludge to cope with some screwed up tape indexes.
    //%%% Hopefully the file with the zero end record is *always* at
    //%%% the end of the archive.
    if ( ( _union._data._endRecord <= 0 ) && ( _union._data._startRecord != _union._data._endRecord ) ) {
        _union._data._endRecord = MAXINT;
    }

    if ( version > 3 ) {
        fread( &ival, sizeof( ival ), 1, fptr );
        int rc = ival;
        int start = 0;
        int end = 0;
        for ( int ii = 0; ii < rc; ii++ ) {
            fread( &ival, sizeof( ival ), 1, fptr );
            start = ival;
            fread( &ival, sizeof( ival ), 1, fptr );
            end = ival;
            _ranges.addRange( start, end );
        }
    }

    //===== Read files =====
    fread( &ival, sizeof( ival ), 1, fptr );
    for ( int count = ival; count > 0; count-- ) {
        fread( &ival, sizeof( ival ), 1, fptr );
        addChild( new File( this, fptr, ival ) );
    }
}

void File::readAll( int version )
{
    read( version );

    QListIterator<File> i( getChildren() );
    for ( ; i.current(); ++i ) {
        i.current()->readAll( version );
    }
}

void File::write( FILE* fptr )
{
    int zero = 0;

    // File name (4 bytes + n chars).
    int ival = getName().length();
    fwrite( &ival, sizeof( ival ), 1, fptr );
    fwrite( getName().data(), sizeof( char ), ival, fptr );

    // File size (4 bytes).
    ival = getSize();
    fwrite( &ival, sizeof( ival ), 1, fptr );

    // File modification time (4 bytes).
    ival = getMTime();
    fwrite( &ival, sizeof( ival ), 1, fptr );

    // Start record number.
    ival = getStartRecord();
    fwrite( &ival, sizeof( ival ), 1, fptr );

    // End record number.
    ival = getEndRecord();
    fwrite( &ival, sizeof( ival ), 1, fptr );

    // Child range list.
    ival = _ranges.getRanges().count();
    fwrite( &ival, sizeof( ival ), 1, fptr );
    QListIterator<Range> it( _ranges.getRanges() );
    for ( ; it.current(); ++it ) {
        ival = it.current()->getStart();
        fwrite( &ival, sizeof( ival ), 1, fptr );
        ival = it.current()->getEnd();
        fwrite( &ival, sizeof( ival ), 1, fptr );
    }

    // Number of immediate children (4 bytes).
    ival = getChildren().count();
    fwrite( &ival, sizeof( ival ), 1, fptr );

    // Fill in file offsets later...
    int fileTable = ftell( fptr );
    for ( ; ival > 0; ival-- ) {
        fwrite( &zero, sizeof( zero ), 1, fptr );
    }

    //===== Write files =====
    ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, fptr );

    QListIterator<File> i( _children );
    int count = 0;
    for ( ; i.current(); ++i, count++ ) {
        // Fill in the file offset.
        int here = ftell( fptr );
        fseek( fptr, fileTable + 4*count, SEEK_SET );
        fwrite( &here, sizeof( here ), 1, fptr );
        fseek( fptr, here, SEEK_SET );

        i.current()->write( fptr );
    }
}

bool File::isDirectory()
{
    read();

    return _name[ _name.length() - 1 ] == '/';
}

int File::getSize()
{
    read();

    return _union._data._size;
}

int File::getMTime()
{
    read();

    return _union._data._mtime;
}

int File::getStartRecord()
{
    read();

    return _union._data._startRecord;
}

int File::getEndRecord()
{
    read();

    return _union._data._endRecord;
}

QString File::getName()
{
    read();

    return _name;
}

QString File::getFullPathName()
{
    QString tmp = _name.copy();
    for ( File* parent = getParent(); parent; parent = parent->getParent() ) {
        tmp.prepend( parent->getName() );
    }

    return tmp;
}

File* File::getParent()
{
    return _parent;
}

const QList<File>& File::getChildren()
{
    read();

    return _children;
}

const QList<Range>& File::getRanges()
{
    read();

    return _ranges.getRanges();
}

void File::addChild( File* file )
{
    read();

    _children.append( file );
}

void File::calcRanges()
{
    assert( !_stubbed );

    _ranges.clear();
    _ranges.addRange( getStartRecord(), getEndRecord() );

    QListIterator<File> it( getChildren() );
    for ( ; it.current(); ++it ) {
        it.current()->calcRanges();
        QListIterator<Range> it2( it.current()->getRanges() );
        for ( ; it2.current(); ++it2 ) {
            _ranges.addRange( it2.current()->getStart(), it2.current()->getEnd() );
        }
    }
}
