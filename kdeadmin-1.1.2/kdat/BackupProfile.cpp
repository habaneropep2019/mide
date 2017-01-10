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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "BackupProfile.h"
#include "BackupProfileManager.h"
#include "Util.h"

BackupProfile::BackupProfile()
{
}

BackupProfile::BackupProfile( const char* name )
        : _name( name ),
          _lastSavedName( name )
{
    load();
}

BackupProfile::~BackupProfile()
{
}

void BackupProfile::load()
{
    QString base = getenv( "HOME" );
    base += "/.kdat/";
    
    FILE* fptr = fopen( base + _lastSavedName + ".bp", "r" );
    if ( !fptr ) {
        return;
    }

    char buf[4096];

    fgets( buf, 4096, fptr );
    int version = 0;
    sscanf( buf, "%d", &version );

    fgets( buf, 4096, fptr );
    _name = buf;
    _name.truncate( _name.length() - 1 );

    fgets( buf, 4096, fptr );
    _archiveName = buf;
    _archiveName.truncate( _archiveName.length() - 1 );

    fgets( buf, 4096, fptr );
    _workingDirectory = buf;
    _workingDirectory.truncate( _workingDirectory.length() - 1 );

    _absoluteFiles.clear();
    fgets( buf, 4096, fptr );
    int filecount = 0;
    sscanf( buf, "%d", &filecount );
    for ( int i = 0; i < filecount; i++ ) {
        fgets( buf, 4096, fptr );
        QString filename = buf;
        filename.truncate( filename.length() - 1 );
        _absoluteFiles.append( filename );
    }

    calcRelativeFiles();

    fgets( buf, 4096, fptr );
    int tmpBool = 0;
    sscanf( buf, "%d", &tmpBool );
    _oneFilesystem = tmpBool;

    fgets( buf, 4096, fptr );
    sscanf( buf, "%d", &tmpBool );
    _incremental = tmpBool;

    fgets( buf, 4096, fptr );
    _snapshotFile = buf;
    _snapshotFile.truncate( _snapshotFile.length() - 1 );

    fgets( buf, 4096, fptr );
    sscanf( buf, "%d", &tmpBool );
    _removeSnapshot = tmpBool;

    fclose( fptr );
}

void BackupProfile::save()
{
    QString base = getenv( "HOME" );
    base += "/.kdat/";

    if ( _lastSavedName.length() ) {
        unlink( base + _lastSavedName + ".bp" );
    }

    FILE* fptr = fopen( base + _name + ".bp", "w" );
    if ( !fptr ) {
        return;
    }

    fprintf( fptr, "%d\n", 1 ); // Version
    fprintf( fptr, "%s\n", _name.data() );
    fprintf( fptr, "%s\n", _archiveName.data() );
    fprintf( fptr, "%s\n", _workingDirectory.data() );
    fprintf( fptr, "%d\n", _absoluteFiles.count() );
    QStrListIterator it( _absoluteFiles );
    for ( ; it.current(); ++it ) {
        fprintf( fptr, "%s\n", it.current() );
    }
    fprintf( fptr, "%d\n", _oneFilesystem );
    fprintf( fptr, "%d\n", _incremental );
    fprintf( fptr, "%s\n", _snapshotFile.data() );
    fprintf( fptr, "%d\n", _removeSnapshot );

    fclose( fptr );

    _lastSavedName = _name.copy();
}

const char* BackupProfile::getName()
{
    return _name;
}

const char* BackupProfile::getArchiveName()
{
    return _archiveName;
}

const char* BackupProfile::getWorkingDirectory()
{
    return _workingDirectory;
}

const QStrList& BackupProfile::getRelativeFiles()
{
    return _relativeFiles;
}

const QStrList& BackupProfile::getAbsoluteFiles()
{
    return _absoluteFiles;
}

bool BackupProfile::isOneFilesystem()
{
    return _oneFilesystem;
}

bool BackupProfile::isIncremental()
{
    return _incremental;
}

const char* BackupProfile::getSnapshotFile()
{
    return _snapshotFile;
}

bool BackupProfile::getRemoveSnapshot()
{
    return _removeSnapshot;
}

void BackupProfile::setName( const char* name )
{
    _name = name;
    
    BackupProfileManager::instance()->backupProfileModified( this );
}
    
void BackupProfile::setArchiveName( const char* archiveName )
{
    _archiveName = archiveName;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setWorkingDirectory( const char* workingDirectory )
{
    _workingDirectory = workingDirectory;

    // Regenerate the list of relative files.
    calcRelativeFiles();
    
    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setAbsoluteFiles( const QStrList& files )
{
    _absoluteFiles.clear();
    QStrListIterator it( files );
    for ( ; it.current(); ++it ) {
        _absoluteFiles.append( it.current() );
    }

    // Make sure working directory is still valid.
    for ( it.toFirst(); it.current(); ++it ) {
        if ( strncmp( it.current(), _workingDirectory.data(), _workingDirectory.length() ) ) {
            // Working directory is not a prefix for this file.
            break;
        }
    }

    if ( it.current() ) {
        // Pick another working directory.
        _workingDirectory = Util::longestCommonPath( _absoluteFiles );
    }

    // Regenerate the list of relative files.
    calcRelativeFiles();
    
    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setOneFilesystem( bool oneFilesystem )
{
    _oneFilesystem = oneFilesystem;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setIncremental( bool incremental )
{
    _incremental = incremental;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setSnapshotFile( const char* snapshotFile )
{
    _snapshotFile = snapshotFile;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setRemoveSnapshot( bool removeSnapshot )
{
    _removeSnapshot = removeSnapshot;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::calcRelativeFiles()
{
    _relativeFiles.clear();
    int remove = _workingDirectory.length();
    if ( remove > 1 ) {
        remove++;
    }

    QStrListIterator it( _absoluteFiles );
    for ( it.toFirst(); it.current(); ++it ) {
        QString fn = it.current();
        fn.remove( 0, remove );
        if ( fn.length() == 0 ) {
            fn = ".";
        }
        _relativeFiles.append( fn );
    }
}
