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

#include <qstrlist.h>

#include "Archive.h"
#include "Options.h"
#include "TapeManager.h"

Archive::Archive( Tape* tape, int ctime, const char* name )
        : _stubbed( FALSE ),
          _fptr( 0 ),
          _offset( 0 ),
          _name( name ),
          _tape( tape )
{
    assert( _tape );

    _ctime = ctime;
}

Archive::Archive( Tape* tape, FILE* fptr, int offset )
        : _stubbed( TRUE ),
          _tape( tape )
{
    assert( _tape );

    _fptr   = fptr;
    _offset = offset;
}

Archive::~Archive()
{
}

void Archive::read( int version )
{
    if ( !_stubbed ) {
        return;
    }

    _stubbed = FALSE;

    fseek( _fptr, _offset, SEEK_SET );

    // Archive name (4 bytes + n chars).
    int ival;
    fread( &ival, sizeof( ival ), 1, _fptr );
    _name.resize( ival+1 );
    fread( _name.data(), sizeof( char ), ival, _fptr );
    
    // Archive creation time (4 bytes).
    fread( &ival, sizeof( ival ), 1, _fptr );
    _ctime = ival;
    
    // Archive ending block (4 bytes).
    fread( &ival, sizeof( ival ), 1, _fptr );
    _endBlock = ival;

    if ( version > 3 ) {
        fread( &ival, sizeof( ival ), 1, _fptr );
        int rc = ival;
        int start = 0;
        int end = 0;
        for ( int ii = 0; ii < rc; ii++ ) {
            fread( &ival, sizeof( ival ), 1, _fptr );
            start = ival;
            fread( &ival, sizeof( ival ), 1, _fptr );
            end = ival;
            _ranges.addRange( start, end );
        }
    }
    
    // Number of immediate children (4 bytes).
    fread( &ival, sizeof( ival ), 1, _fptr );
    
    //===== Read files =====
    for ( int count = ival; count > 0; count-- ) {
        fread( &ival, sizeof( ival ), 1, _fptr );
        addChild( new File( 0, _fptr, ival ) );
    }
}

void Archive::readAll( int version )
{
    read( version );

    QListIterator<File> i( getChildren() );
    for ( ; i.current(); ++i ) {
        i.current()->readAll( version );
    }
}

void Archive::write( FILE* fptr )
{
    _fptr   = fptr;
    _offset = ftell( _fptr );

    int zero = 0;
    
    // Archive name (4 bytes + n chars).
    int ival = 4096;
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    char buf[4096];
    memset( buf, 0, 4096 );
    memcpy( buf, _name.data(), _name.length() > 4095 ? 4095 : _name.length() );
    fwrite( buf, sizeof( char ), 4096, _fptr );
    
    // Archive creation time (4 bytes).
    ival = getCTime();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    
    // Archive ending block (4 bytes).
    ival = getEndBlock();
    fwrite( &ival, sizeof( ival ), 1, _fptr );

    // Child range list.
    ival = _ranges.getRanges().count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    QListIterator<Range> it( _ranges.getRanges() );
    for ( ; it.current(); ++it ) {
        ival = it.current()->getStart();
        fwrite( &ival, sizeof( ival ), 1, _fptr );
        ival = it.current()->getEnd();
        fwrite( &ival, sizeof( ival ), 1, _fptr );
    }
    
    // Number of immediate children (4 bytes).
    ival = getChildren().count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    
    // Fill in file offsets later...
    int fileTable = ftell( _fptr );
    for ( ; ival > 0; ival-- ) {
        fwrite( &zero, sizeof( zero ), 1, _fptr );
    }

    //===== Write files =====
    QListIterator<File> i( getChildren() );
    int count = 0;
    for ( ; i.current(); ++i, count++ ) {
        // Fill in the file offset.
        int here = ftell( _fptr );
        fseek( _fptr, fileTable + 4*count, SEEK_SET );
        fwrite( &here, sizeof( here ), 1, _fptr );
        fseek( _fptr, here, SEEK_SET );

        i.current()->write( _fptr );
    }
}

int Archive::getCTime()
{
    read();
    
    return _ctime;
}

int Archive::getEndBlock()
{
    read();

    return _endBlock;
}

QString Archive::getName()
{
    read();
    
    return _name;
}

Tape* Archive::getTape()
{
    return _tape;
}

const QList<File>& Archive::getChildren()
{
    read();

    return _children;
}

const QList<Range>& Archive::getRanges()
{
    read();

    return _ranges.getRanges();
}

void Archive::setEndBlock( int endBlock )
{
    read();

    _endBlock = endBlock;

    if ( _fptr ) {
        fseek( _fptr, _offset + 4 + 4096 + 4, SEEK_SET );
        fwrite( &_endBlock, sizeof( _endBlock ), 1, _fptr );
    }

    TapeManager::instance()->tapeModified( _tape );
}

void Archive::setName( const char* name )
{
    read();

    _name = name;

    if ( _fptr ) {
        char buf[4096];
        fseek( _fptr, _offset + 4, SEEK_SET );
        memset( buf, 0, 4096 );
        memcpy( buf, _name.data(), _name.length() > 4095 ? 4095 : _name.length() );
        fwrite( buf, sizeof( char ), 4096, _fptr );
        fflush( _fptr );
    }

    TapeManager::instance()->tapeModified( _tape );
}

void Archive::addChild( File* file )
{
    read();
    
    _children.append( file );
}

File* Archive::addFile( int size, int mtime, int startRecord, int endRecord, const char* name )
{
    read();

    QStrList path;

    QString fn( name );
    int idx = 0;
    while ( ( idx = fn.find( '/' ) ) > -1 ) {
        path.append( fn.left( idx + 1 ) );
        fn.remove( 0, idx + 1 );
    }

    if ( fn.length() == 0 ) {
        fn = path.last();
        path.removeLast();
    }

    File* file = 0;
    if ( path.count() == 0 ) {
        // Top level file/directory.
        file = new File( 0, size, mtime, startRecord, endRecord, fn );
        
        addChild( file );
        return file;
    }

    QString dir = path.first();
    path.removeFirst();
    QListIterator<File> i( getChildren() );
    File* parent = 0;
    for ( ; i.current() ; ++i ) {
        if ( i.current()->getName() == dir ) {
            parent = i.current();
            break;
        }
    }

    if ( parent == 0 ) {
        parent = new File( 0, 0, 0, startRecord, endRecord, dir );
        addChild( parent );
    }

    QStrListIterator j( path );
    for ( ; j.current(); ++j ) {
        QString dir = j.current();
        File* pparent = parent;
        QListIterator<File> i( pparent->getChildren() );
        for ( parent = 0; i.current() ; ++i ) {
            if ( i.current()->getName() == dir ) {
                parent = i.current();
                break;
            }
        }
        
        if ( parent == 0 ) {
            parent = new File( pparent, 0, 0, 0, 0, dir );
            pparent->addChild( parent );
        }
    }
    
    file = new File( parent, size, mtime, startRecord, endRecord, fn );
    parent->addChild( file );

    return file;
}

void Archive::calcRanges()
{
    assert( !_stubbed );

    _ranges.clear();

    QListIterator<File> it( getChildren() );
    for ( ; it.current(); ++it ) {
        it.current()->calcRanges();
        QListIterator<Range> it2( it.current()->getRanges() );
        for ( ; it2.current(); ++it2 ) {
            _ranges.addRange( it2.current()->getStart(), it2.current()->getEnd() );
        }
    }
    
    //%%% This is a kludge to cope with a bug that I haven't found yet.
    //%%% If there is more than one range, then all of the ranges are merged
    //%%% into one big contiguous range.
    if ( _ranges.getRanges().count() > 1 ) {
        debug( "Archive::calcRanges() -- extra ranges detected, fixing..." );
        QListIterator<Range> iter( _ranges.getRanges() );
        for ( ; iter.current(); ++iter ) {
            debug( "Archive::calcRanges() -- range = %d to %d", iter.current()->getStart(), iter.current()->getEnd() );
        }
        int start = _ranges.getRanges().getFirst()->getStart();
        int end = _ranges.getRanges().getLast()->getEnd();
        _ranges.clear();
        _ranges.addRange( start, end );
    }

    assert( _ranges.getRanges().count() <= 1 );

    _endBlock = _ranges.getRanges().getFirst()->getEnd() / ( Options::instance()->getTapeBlockSize() / 512 );
}
