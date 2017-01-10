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
#include <time.h>
#include <unistd.h>

#include <kapp.h>
#include <kmsgbox.h>

#include "IndexDlg.h"
#include "KDat.h"
#include "Options.h"
#include "Tape.h"
#include "TapeDrive.h"
#include "TapeManager.h"
#include "kdat.h"

Tape::Tape()
        : _stubbed( FALSE ),
          _name( i18n( "New Tape" ) ),
          _size( Options::instance()->getDefaultTapeSize() ),
          _fptr( 0 )
{
    _id.resize( 1024 );
    gethostname( _id.data(), 1024 );
    time_t tm = time( NULL );
    _ctime = tm;
    _mtime = tm;
    _id.sprintf( "%s:%d", _id.data(), tm );

    write();
}

Tape::Tape( const char* id )
        : _stubbed( TRUE ),
          _id( id ),
          _ctime( -1 ),
          _mtime( -1 ),
          _name( "<unknown>" ),
          _size( -1 ),
          _fptr( 0 )
{
}

Tape::~Tape()
{
    clear();
}

void Tape::format()
{
    // Rewind tape.
    if ( !TapeDrive::instance()->rewind() ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Format Failed" ), i18n( "Rewinding tape failed." ), KMsgBox::EXCLAMATION );
        return;
    }

    // Set block size for tape.
    if ( !TapeDrive::instance()->setBlockSize( Options::instance()->getTapeBlockSize() ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Format Failed" ), i18n( "Cannot set tape block size." ), KMsgBox::EXCLAMATION );
        return;
    }

    // TAPE HEADER
    int iv;

    // KDat magic string.
    if ( TapeDrive::instance()->write( KDAT_MAGIC, KDAT_MAGIC_LENGTH ) < KDAT_MAGIC_LENGTH ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Format Failed" ), i18n( "Writing magic string failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    
    // Tape header version number.
    iv = KDAT_TAPE_HEADER_VERSION;
    if ( TapeDrive::instance()->write( (const char*)&iv, 4 ) < 4 ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Format Failed" ), i18n( "Writing version number failed." ), KMsgBox::EXCLAMATION );
        return;
    }

    // Write tape ID.  Tape ID is machine name + current time.
    iv = _id.length() + 1;
    if ( TapeDrive::instance()->write( (const char*)&iv, 4 ) < 4 ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Format Failed" ), i18n( "Writing tape ID length failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    if ( TapeDrive::instance()->write( _id.data(), iv ) < iv ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Format Failed" ), i18n( "Writing tape ID failed." ), KMsgBox::EXCLAMATION );
        return;
    }

    // Write end of file marker.
    TapeDrive::instance()->close();
    TapeDrive::instance()->open();

    // Write new tape index.
    write();
}

void Tape::read()
{
    if ( !_stubbed ) {
        return;
    }

    _stubbed = FALSE;

    if ( !_fptr ) {
        QString fn = getenv( "HOME" );
        fn += "/.kdat/";
        fn += _id;
        _fptr = fopen( fn.data(), "r+" );
    }

    if ( !_fptr ) {
        // No tape index file was found.
        if ( KMsgBox::yesNo( KDat::instance(), i18n( "KDat: Tape Index" ), i18n( "No index file was found for this tape.\nRecreate the index from tape?" ), KMsgBox::EXCLAMATION ) == 1 ) {
            write();
            
            IndexDlg dlg( this, KDat::instance() );
            dlg.exec();
            TapeManager::instance()->addTape( this );
            return;
        } else {
            return;
        }
    }
    
    char buf[4096];

    fseek( _fptr, 0, SEEK_SET );

    // Read index file version number.
    if ( !fgets( buf, 4096, _fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading version number failed." ), KMsgBox::EXCLAMATION );
        fclose( _fptr );
        return;
    }
    int version = atoi( buf );

    switch ( version ) {
        case 1:
            readVersion1Index( _fptr );
            readAll( version );
            calcRanges();
            fclose( _fptr );
            _fptr = NULL;
            write();
            break;

        case 2:
            readVersion2Index( _fptr );
            readAll( version );
            calcRanges();
            fclose( _fptr );
            _fptr = NULL;
            write();
            break;

        case 3:
            readVersion3Index( _fptr );
            readAll( version );
            calcRanges();
            fclose( _fptr );
            _fptr = NULL;
            write();
            break;

        case 4:
            readVersion4Index( _fptr );
            break;

        default:
        {
            QString msg;
            msg.sprintf( i18n( "The tape index file format is version %d.  The index cannot be read by this version of KDat.  Perhaps the tape index file was created by a newer version of KDat?" ), version );
            KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), msg, KMsgBox::EXCLAMATION );
        }
    }
}

void Tape::readAll( int version )
{
    read();

    QListIterator<Archive> i( _children );
    for ( ; i.current(); ++i ) {
        i.current()->readAll( version );
    }
}

void Tape::write()
{
    QString fn = getenv( "HOME" );
    fn += "/.kdat/";
    fn += _id;
    if ( !_fptr ) {
        _fptr = fopen( fn.data(), "w" );

        if ( !_fptr ) {
            // Suck!
            printf( "Tape::write() -- cannot open '%s' for writing!\n", fn.data() );
            return;
        }
    } else {
        freopen( fn.data(), "w", _fptr );
    }
    
    int zero = 0;

    //===== Write the tape data =====

    fprintf( _fptr, "%d\n", KDAT_INDEX_FILE_VERSION );
    fprintf( _fptr, "%s\n", _id.data() );

    fwrite( &_ctime, sizeof( _ctime ), 1, _fptr );
    fwrite( &_mtime, sizeof( _mtime ), 1, _fptr );
    fwrite( &_size , sizeof( _size  ), 1, _fptr );
    char buf[4096];
    memset( buf, 0, 4096 );
    memcpy( buf, _name.data(), _name.length() > 4095 ? 4095 : _name.length() );
    fwrite( buf, sizeof( char ), 4096, _fptr );
    int ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );

    // Fill in the archive offsets later...
    int archiveTable = ftell( _fptr );
    for ( uint i = 0; i < MAX_NUM_ARCHIVES; i++ ) {
        fwrite( &zero, sizeof( zero ), 1, _fptr );
    }

    //===== Write archives =====
    QListIterator<Archive> i( _children );
    int count = 0;
    for ( ; i.current(); ++i, count++ ) {
        // Fill in the file offset.
        int here = ftell( _fptr );
        fseek( _fptr, archiveTable + 4*count, SEEK_SET );
        fwrite( &here, sizeof( here ), 1, _fptr );
        fseek( _fptr, here, SEEK_SET );

        i.current()->write( _fptr );
    }

    freopen( fn.data(), "r+", _fptr );
}

QString Tape::getID()
{
    read();

    return _id;
}

QString Tape::getName()
{
    read();

    return _name;
}

int Tape::getCTime()
{
    read();
    
    return _ctime;
}

int Tape::getMTime()
{
    read();
    
    return _mtime;
}

int Tape::getSize()
{
    read();

    return _size;
}

const QList<Archive>& Tape::getChildren()
{
    read();

    return _children;
}

void Tape::setName( const char* name )
{
    read();

    if ( !_fptr ) {
        // Create a new empty index file.
        write();
    }

    _name = name;

    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 12, SEEK_CUR );
    memset( buf, 0, 4096 );
    memcpy( buf, _name.data(), _name.length() > 4095 ? 4095 : _name.length() );
    fwrite( buf, sizeof( char ), 4096, _fptr );
    fflush( _fptr );

    TapeManager::instance()->tapeModified( this );
}

void Tape::setMTime( int mtime )
{
    read();

    if ( !_fptr ) {
        // Create a new empty index file.
        write();
    }

    _mtime = mtime;

    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 4, SEEK_CUR );
    fwrite( &_mtime, sizeof( _mtime ), 1, _fptr );
    fflush( _fptr );

    TapeManager::instance()->tapeModified( this );
}

void Tape::setSize( int size )
{
    read();

    if ( !_fptr ) {
        // Create a new empty index file.
        write();
    }

    _size = size;
    
    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 8, SEEK_CUR );
    fwrite( &_size, sizeof( _size ), 1, _fptr );
    fflush( _fptr );

    TapeManager::instance()->tapeModified( this );
}

void Tape::addChild( Archive* archive )
{
    read();
    
    archive->calcRanges();

    _children.append( archive );

    char buf[4096];
    fseek( _fptr, 0, SEEK_END );
    int here = ftell( _fptr );
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 12 + 4096, SEEK_CUR );
    int ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    fseek( _fptr, ( _children.count() - 1 ) * 4, SEEK_CUR );
    fwrite( &here, sizeof( here ), 1, _fptr );
    fseek( _fptr, here, SEEK_SET );
    archive->write( _fptr );
    fflush( _fptr );

    setMTime( time( NULL ) );
}

void Tape::removeChild( Archive* archive )
{
    read();

    while ( _children.last() != archive ) {
        _children.removeLast();
    }
    _children.removeLast();

    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 12 + 4096, SEEK_CUR );
    int ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    int here;
    if ( ival > 0 ) {
        fseek( _fptr, _children.count() * 4, SEEK_CUR );
        fread( &here, sizeof( here ), 1, _fptr );
    } else {
        fseek( _fptr, MAX_NUM_ARCHIVES * 4, SEEK_CUR );
        here = ftell( _fptr );
    }
    QString fn = getenv( "HOME" );
    fn += "/.kdat/";
    fn += _id;
    truncate( fn.data(), here );
    freopen( fn.data(), "r+", _fptr );
    fflush( _fptr );

    setMTime( time( NULL ) );
}

void Tape::clear()
{
    if ( _children.count() < 1 ) {
        return;
    }

    while ( _children.first() ) {
        delete _children.first();
        _children.removeFirst();
    }

    char buf[4096];
    fseek( _fptr, 0, SEEK_SET );
    fgets( buf, 4096, _fptr );
    fgets( buf, 4096, _fptr );
    fseek( _fptr, 12 + 4096, SEEK_CUR );
    int ival = _children.count();
    fwrite( &ival, sizeof( ival ), 1, _fptr );
    fseek( _fptr, MAX_NUM_ARCHIVES * 4, SEEK_CUR );
    int here = ftell( _fptr );
    QString fn = getenv( "HOME" );
    fn += "/.kdat/";
    fn += _id;
    truncate( fn.data(), here );
    freopen( fn.data(), "r+", _fptr );
    fflush( _fptr );

    setMTime( time( NULL ) );
}

void Tape::readVersion1Index( FILE* fptr )
{
    fseek( fptr, 0, SEEK_CUR );

    char buf[4096];

    // Read the tapeID.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading tape ID failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    QString tapeID = buf;
    tapeID.truncate( tapeID.length() - 1 );
    if ( tapeID != _id ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Tape ID on tape does not match tape ID in index file." ), KMsgBox::EXCLAMATION );
        return;
    }

    // Read creation time.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading creation time failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    _ctime = atoi( buf );

    // Read modification time.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading modification time failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    _mtime = atoi( buf );

    // Read tape name.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading tape name failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    _name = buf;
    _name.truncate( _name.length() - 1 );

    // Read tape size.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading tape size failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    _size = atoi( buf );

    // Read number of archives.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading archive count failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    int numArchives = atoi( buf );

    for ( ; numArchives; numArchives-- ) {
        // Read archive name.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading archive name failed." ), KMsgBox::EXCLAMATION );
            return;
        }
        QString archiveName = buf;
        archiveName.truncate( archiveName.length() - 1 );

        // Read archive time stamp.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading archive time stamp failed." ), KMsgBox::EXCLAMATION );
            return;
        }
        int archiveTimeStamp = atoi( buf );

        // Read archive start block.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading archive start block failed." ), KMsgBox::EXCLAMATION );
            return;
        }
        int archiveStartBlock = atoi( buf );

        // Read archive end block.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading archive end block failed." ), KMsgBox::EXCLAMATION );
            return;
        }
        int archiveEndBlock = atoi( buf );

        // Create the archive.
        Archive* archive = new Archive( this, archiveTimeStamp, archiveName );
        archive->setEndBlock( archiveEndBlock - archiveStartBlock );
        
        _children.append( archive );
        
        // Read the number of files.
        if ( !fgets( buf, 4096, fptr ) ) {
            KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading archive file count failed." ), KMsgBox::EXCLAMATION );
            return;
        }
        int archiveNumFiles = atoi( buf );

        QString tmpFileName;
        int tmpFileSize = -1;
        int tmpFileMTime = -1;
        int tmpFileStartRecord = -1;
        for ( ; archiveNumFiles; archiveNumFiles-- ) {
            // Read file name.
            if ( !fgets( buf, 4096, fptr ) ) {
                KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading file name failed." ), KMsgBox::EXCLAMATION );
                return;
            }
            QString fileName = buf;
            fileName.truncate( fileName.length() - 1 );
            
            // Read file size.
            if ( !fgets( buf, 4096, fptr ) ) {
                KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading file size failed." ), KMsgBox::EXCLAMATION );
                return;
            }
            int fileSize = atoi( buf );
            
            // Read file modification time.
            if ( !fgets( buf, 4096, fptr ) ) {
                KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading file modification time failed." ), KMsgBox::EXCLAMATION );
                return;
            }
            int fileModTime = atoi( buf );
            
            // Read file record number.
            if ( !fgets( buf, 4096, fptr ) ) {
                KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading file record number failed." ), KMsgBox::EXCLAMATION );
                return;
            }
            int fileRecord = atoi( buf );

            if ( tmpFileName.length() > 0 ) {
                archive->addFile( tmpFileSize, tmpFileMTime, tmpFileStartRecord, fileRecord, tmpFileName );
            }

            tmpFileName = fileName.copy();
            tmpFileSize = fileSize;
            tmpFileMTime = fileModTime;
            tmpFileStartRecord = fileRecord;
        }

        if ( tmpFileName.length() > 0 ) {
            archive->addFile( tmpFileSize, tmpFileMTime, tmpFileStartRecord, archive->getEndBlock() * ( Options::instance()->getTapeBlockSize() / 512 ), tmpFileName );
        }
    }
}

void Tape::readVersion2Index( FILE* fptr )
{
    fseek( fptr, 0, SEEK_CUR );

    char buf[4096];

    // Read the tapeID.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading tape ID failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    QString tapeID = buf;
    tapeID.truncate( tapeID.length() - 1 );
    if ( tapeID != _id ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Tape ID on tape does not match tape ID in index file." ), KMsgBox::EXCLAMATION );
        return;
    }
    
    // Read creation time.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading creation time failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    _ctime = atoi( buf );
    
    // Read modification time.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading modification time failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    _mtime = atoi( buf );
    
    // Read tape name.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading tape name failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    _name = buf;
    _name.truncate( _name.length() - 1 );
    
    // Read tape size.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading tape size failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    _size = atoi( buf );
    
    // Read number of archives.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading archive count failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    int numArchives = atoi( buf );

    int ival;
    for ( ; numArchives; numArchives-- ) {
        fread( &ival, sizeof( ival ), 1, fptr );
        _children.append( new Archive( this, fptr, ival ) );
    }
}

void Tape::readVersion3Index( FILE* fptr )
{
    fseek( fptr, 0, SEEK_CUR );

    char buf[4096];

    // Read the tapeID.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading tape ID failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    QString tapeID = buf;
    tapeID.truncate( tapeID.length() - 1 );
    if ( tapeID != _id ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Tape ID on tape does not match tape ID in index file." ), KMsgBox::EXCLAMATION );
        return;
    }
    
    // Read creation time.
    fread( &_ctime, sizeof( _ctime ), 1, fptr );
    
    // Read modification time.
    fread( &_mtime, sizeof( _mtime ), 1, fptr );

    // Read tape size.
    fread( &_size, sizeof( _size ), 1, fptr );
    
    // Read tape name.
    _name.resize( 4096 );
    fread( _name.data(), sizeof( char ), 4096, fptr );
    
    // Read number of archives.
    int numArchives;
    fread( &numArchives, sizeof( numArchives ), 1, fptr );

    int ival;
    for ( ; numArchives; numArchives-- ) {
        fread( &ival, sizeof( ival ), 1, fptr );
        _children.append( new Archive( this, fptr, ival ) );
    }
}

void Tape::readVersion4Index( FILE* fptr )
{
    fseek( fptr, 0, SEEK_CUR );

    char buf[4096];

    // Read the tapeID.
    if ( !fgets( buf, 4096, fptr ) ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Reading tape ID failed." ), KMsgBox::EXCLAMATION );
        return;
    }
    QString tapeID = buf;
    tapeID.truncate( tapeID.length() - 1 );
    if ( tapeID != _id ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Index File Error" ), i18n( "Tape ID on tape does not match tape ID in index file." ), KMsgBox::EXCLAMATION );
        return;
    }
    
    // Read creation time.
    fread( &_ctime, sizeof( _ctime ), 1, fptr );
    
    // Read modification time.
    fread( &_mtime, sizeof( _mtime ), 1, fptr );

    // Read tape size.
    fread( &_size, sizeof( _size ), 1, fptr );
    
    // Read tape name.
    _name.resize( 4096 );
    fread( _name.data(), sizeof( char ), 4096, fptr );
    
    // Read number of archives.
    int numArchives;
    fread( &numArchives, sizeof( numArchives ), 1, fptr );

    int ival;
    for ( ; numArchives; numArchives-- ) {
        fread( &ival, sizeof( ival ), 1, fptr );
        _children.append( new Archive( this, fptr, ival ) );
    }
}

void Tape::calcRanges()
{
    QListIterator<Archive> it( getChildren() );
    for ( ; it.current(); ++it ) {
        it.current()->calcRanges();
    }
}
