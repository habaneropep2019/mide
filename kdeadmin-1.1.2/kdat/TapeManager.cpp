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

#include <stdlib.h>
#include <unistd.h>

#include <qdir.h>
#include <qregexp.h>

#include "TapeManager.h"

#include "TapeManager.moc"

TapeManager::TapeManager()
        : _mountedTape( 0 )
{
    _tapes.setAutoDelete( TRUE );

    // Get a list of all available tape indexes.
    QString dn = getenv( "HOME" );
    dn += "/.kdat/";
    QDir dir( dn );
    const QFileInfoList* list = dir.entryInfoList( QDir::Files, QDir::Name );
    if ( list ) {
        QRegExp re( ".*:[0-9]+" );
        QFileInfoListIterator it( *list );
        for ( ; it.current(); ++it ) {
            if ( re.match( it.current()->fileName() ) > -1 ) {
                _tapeIDs.append( it.current()->fileName() );
            }
        }
    }
}

TapeManager::~TapeManager()
{
}

TapeManager* TapeManager::_instance = 0;

TapeManager* TapeManager::instance()
{
    if ( _instance == 0 ) {
        _instance = new TapeManager();
    }

    return _instance;
}

const QStrList& TapeManager::getTapeIDs()
{
    return _tapeIDs;
}

Tape* TapeManager::findTape( const char* id )
{
    Tape* tape = _tapes[ id ];

    if ( !tape ) {
        tape = new Tape( id );
        _tapes.insert( tape->getID(), tape );
    }

    return tape;
}

void TapeManager::addTape( Tape* tape )
{
    Tape* old = _tapes[ tape->getID() ];
    if ( old ) {
        removeTape( old );
    }

    _tapeIDs.append( tape->getID() );
    _tapes.insert( tape->getID(), tape );

    emit sigTapeAdded( tape );
}

void TapeManager::removeTape( Tape* tape )
{
    emit sigTapeRemoved( tape );

    // Remove the index file.
    QString fn = getenv( "HOME" );
    fn += "/.kdat/";
    fn += tape->getID();
    unlink( fn.data() );

    _tapeIDs.remove( tape->getID() );
    _tapes.remove( tape->getID() );
}

void TapeManager::tapeModified( Tape* tape )
{
    emit sigTapeModified( tape );
}

void TapeManager::mountTape( Tape* tape )
{
    _mountedTape = tape;
    emit sigTapeMounted();
}

void TapeManager::unmountTape()
{
    emit sigTapeUnmounted();
    _mountedTape = 0;
}

Tape* TapeManager::getMountedTape()
{
    return _mountedTape;
}
