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

#include "BackupProfileManager.h"

#include "BackupProfileManager.moc"

BackupProfileManager::BackupProfileManager()
{
    _backupProfiles.setAutoDelete( TRUE );

    // Get a list of all available backup profiles.
    QString dn = getenv( "HOME" );
    dn += "/.kdat/";
    QDir dir( dn );
    const QFileInfoList* list = dir.entryInfoList( QDir::Files, QDir::Name );
    if ( list ) {
        QRegExp re( "*.bp", TRUE, TRUE );
        QFileInfoListIterator it( *list );
        for ( ; it.current(); ++it ) {
            if ( re.match( it.current()->fileName() ) > -1 ) {
                QString fn = it.current()->fileName();
                _backupProfileNames.append( fn.left( fn.length() - 3 ) );
            }
        }
    }
}

BackupProfileManager::~BackupProfileManager()
{
}

BackupProfileManager* BackupProfileManager::_instance = 0;

BackupProfileManager* BackupProfileManager::instance()
{
    if ( _instance == 0 ) {
        _instance = new BackupProfileManager();
    }

    return _instance;
}

const QStrList& BackupProfileManager::getBackupProfileNames()
{
    return _backupProfileNames;
}

BackupProfile* BackupProfileManager::findBackupProfile( const char* name )
{
    BackupProfile* backupProfile = _backupProfiles[ name ];

    if ( !backupProfile ) {
        backupProfile = new BackupProfile( name );
        _backupProfiles.insert( backupProfile->getName(), backupProfile );
    }

    return backupProfile;
}

void BackupProfileManager::addBackupProfile( BackupProfile* backupProfile )
{
    BackupProfile* old = _backupProfiles[ backupProfile->getName() ];
    if ( old ) {
        removeBackupProfile( old );
    }

    _backupProfileNames.append( backupProfile->getName() );
    _backupProfiles.insert( backupProfile->getName(), backupProfile );

    emit sigBackupProfileAdded( backupProfile );
}

void BackupProfileManager::removeBackupProfile( BackupProfile* backupProfile )
{
    emit sigBackupProfileRemoved( backupProfile );

    // Remove the index file.
    QString fn = getenv( "HOME" );
    fn += "/.kdat/";
    fn += backupProfile->getName();
    fn += ".bp";
    unlink( fn.data() );

    _backupProfileNames.remove( backupProfile->getName() );
    _backupProfiles.remove( backupProfile->getName() );
}

void BackupProfileManager::backupProfileModified( BackupProfile* backupProfile )
{
    emit sigBackupProfileModified( backupProfile );
}
