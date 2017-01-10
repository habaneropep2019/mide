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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mtio.h>
#include <unistd.h>
#include <time.h>

#include <kapp.h>
#include <kmsgbox.h>

#include "KDat.h"
#include "Options.h"
#include "Tape.h"
#include "TapeManager.h"
#include "TapeDrive.h"
#include "kdat.h"

#include "TapeDrive.moc"

TapeDrive* TapeDrive::_instance = 0;

TapeDrive* TapeDrive::instance()
{
    if ( !_instance ) {
        _instance = new TapeDrive();
    }

    return _instance;
}

TapeDrive::TapeDrive()
        : _fd ( -1 ),
          _readOnly( TRUE ),
          _tape( 0 ),
          _writeBuf( 0 ),
          _readBuf( 0 ),
          _writeBufIdx( 0 ),
          _readBufIdx( Options::instance()->getTapeBlockSize() )
{
    setBlockSize( Options::instance()->getTapeBlockSize() );
    _writeBuf = new char[ Options::instance()->getTapeBlockSize() ];
    _readBuf  = new char[ Options::instance()->getTapeBlockSize() ];
}

TapeDrive::~TapeDrive()
{
    if ( Options::instance()->getLockOnMount() ) {
        unlock();
    }

    delete [] _writeBuf;
    delete [] _readBuf;
}

void TapeDrive::flush()
{
    _readBufIdx = Options::instance()->getTapeBlockSize();

    if ( _writeBufIdx <= 0 ) {
        return;
    }

    memset( _writeBuf + _writeBufIdx, 0, Options::instance()->getTapeBlockSize() - _writeBufIdx );
    int ret = ::write( _fd, _writeBuf, Options::instance()->getTapeBlockSize() );
    if ( ret < 0 ) {
        printf( "TapeDrive::flush() -- write failed!\n" );
    }

    _writeBufIdx = 0;
}

bool TapeDrive::load()
{
    if ( _fd < 0 ) {
        return FALSE;
    }

#ifdef MTLOAD
    struct mtop tapeOp;
    tapeOp.mt_op    = MTLOAD;
    tapeOp.mt_count = 1;

    int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
    if ( ret < 0 ) {
        printf( "TapeDrive::lock() -- ioctl( MTLOAD ) failed!\n" );
    }

    return ret >= 0;
#else
    return TRUE;
#endif
}

bool TapeDrive::lock()
{
    if ( _fd < 0 ) {
        return FALSE;
    }

#ifdef MTLOCK
    struct mtop tapeOp;
    tapeOp.mt_op    = MTLOCK;
    tapeOp.mt_count = 1;

    int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
    if ( ret < 0 ) {
        printf( "TapeDrive::lock() -- ioctl( MTLOCK ) failed!\n" );
    }

    return ret >= 0;
#else
    return TRUE;
#endif
}

bool TapeDrive::unlock()
{
    if ( _fd < 0 ) {
        return FALSE;
    }

#ifdef MTUNLOCK
    struct mtop tapeOp;
    tapeOp.mt_op    = MTUNLOCK;
    tapeOp.mt_count = 1;

    int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
    if ( ret < 0 ) {
        printf( "TapeDrive::unlock() -- ioctl( MTUNLOCK ) failed!\n" );
    }

    return ret >= 0;
#else
    return TRUE;
#endif
}

bool TapeDrive::isReadOnly()
{
    return _readOnly;
}

bool TapeDrive::isTapePresent()
{
    open();
    if ( _fd < 0 ) {
        return FALSE;
    }

    // Get tape status.
    struct mtget tapeStatus;
    int ret = ioctl( _fd, MTIOCGET, &tapeStatus );
    if ( ret < 0 ) {
        return FALSE;
    }

    // Check for the presence of a tape.
//    if ( !GMT_DR_OPEN( tapeStatus.mt_gstat ) ) {
    if ( GMT_ONLINE( tapeStatus.mt_gstat ) ) {
        // Lock the tape drive door.
        //struct mtop tapeOp;
        //tapeOp.mt_op    = MTLOCK;
        //tapeOp.mt_count = 1;
        //int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
        //if ( ret < 0 ) {
        //    printf( "TapeDrive::isTapePresent() -- ioctl( MTLOCK ) failed!\n" );
        //}
        
        if ( _readOnly ) {
            emit sigStatus( i18n( "Tape mounted readonly." ) );
        } else {
            emit sigStatus( i18n( "Tape mounted read/write." ) );
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

void TapeDrive::eject()
{
    if ( _fd < 0 ) {
        return;
    }

    flush();
    
    struct mtop tapeOp;
    tapeOp.mt_op    = MTOFFL;
    tapeOp.mt_count = 1;
    int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
    if ( ret < 0 ) {
        printf( "TapeDrive::ejectTape() -- ioctl( MTOFFL ) failed!\n" );
    }
}

Tape* TapeDrive::readHeader()
{
    _tape = NULL;
    
    // Rewind tape.
    emit sigStatus( "Rewinding tape..." );
    if ( !rewind() ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Error" ), i18n( "Rewinding tape failed." ), KMsgBox::EXCLAMATION );
        return NULL;
    }

    // KDat magic string.
    emit sigStatus( i18n( "Reading magic string..." ) );
    char magic[9];
    if ( read( magic, 9 ) < 9 ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Error" ), i18n( "Reading magic string failed." ), KMsgBox::EXCLAMATION );
        return NULL;
    }
    if ( strncmp( "KDatMAGIC", magic, 9 ) ) {
        // Bad magic.
        return NULL;
    }

    // Read version number.
    emit sigStatus( i18n( "Reading version number..." ) );
    int version;
    if ( read( (char*)&version, 4 ) < 4 ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Error" ), i18n( "Reading version number failed." ), KMsgBox::EXCLAMATION );
        return NULL;
    }

    if ( version > KDAT_TAPE_HEADER_VERSION ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Warning" ), i18n( "Tape was formatted by a more recent version of KDat, consider upgrading." ), KMsgBox::INFORMATION );
    }

    // Read tape ID.
    emit sigStatus( i18n( "Reading tape ID..." ) );
    int len;
    if ( read( (char*)&len, 4 ) < 4 ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Error" ), i18n( "Reading tape ID length failed." ), KMsgBox::EXCLAMATION );
        return NULL;
    }
    char* tapeID = new char[len];
    if ( read( tapeID, len ) < len ) {
        KMsgBox::message( KDat::instance(), i18n( "KDat: Error" ), i18n( "Reading tape ID failed." ), KMsgBox::EXCLAMATION );
        delete [] tapeID;
        return NULL;
    }

    _tape = TapeManager::instance()->findTape( tapeID );
    delete [] tapeID;
    return _tape;
}

bool TapeDrive::rewind()
{
    if ( _fd < 0 ) {
        return FALSE;
    }

    flush();

    struct mtop tapeOp;
    tapeOp.mt_op    = MTREW;
    tapeOp.mt_count = 1;
    int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
    if ( ret < 0 ) {
        printf( "TapeDrive::rewind() -- ioctl( MTREW ) failed!\n" );
    }

    return ret >= 0;
}

int TapeDrive::getFile()
{
    if ( _fd < 0 ) {
        return -1;
    }

    struct mtget tapePos;
    
    if ( ioctl( _fd, MTIOCGET, &tapePos ) < 0 ) {
        printf( "TapeDrive::getFile() -- ioctl( MTIOCGET ) failed!\n" );
        return -1;
    }

    return tapePos.mt_fileno;
}

int TapeDrive::getBlock()
{
    if ( _fd < 0 ) {
        return -1;
    }

    struct mtget tapePos;
    
    if ( ioctl( _fd, MTIOCGET, &tapePos ) < 0 ) {
        printf( "TapeDrive::getBlock() -- ioctl( MTIOCGET ) failed!\n" );
        return -1;
    }

    //%%% I don't think this makes sense anymore because the tape buffer size == the tape block size.
    // Need to subtract off the blocks that the application has not "read" yet.
    //int unread = ( Options::instance()->getTapeBlockSize() - _readBufIdx ) / Options::instance()->getTapeBlockSize();

    //return tapePos.mt_blkno - unread;

    return tapePos.mt_blkno;
}

bool TapeDrive::nextFile( int count )
{
    if ( _fd < 0 ) {
        return FALSE;
    }

    flush();
    
    struct mtop tapeOp;
    tapeOp.mt_op    = MTFSF;
    tapeOp.mt_count = count;
    int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
    if ( ret < 0 ) {
        printf( "TapeDrive::nextFile() -- ioctl( MTFSF ) failed!\n" );
    }
    return ret >= 0;
}

bool TapeDrive::prevFile( int count )
{
    if ( _fd < 0 ) {
        return FALSE;
    }

    flush();
    
    struct mtop tapeOp;
    tapeOp.mt_op    = MTBSF;
    tapeOp.mt_count = count;
    int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
    if ( ret < 0 ) {
        printf( "TapeDrive::prevFile() -- ioctl( MTBSF ) failed!\n" );
    }
    return ret >= 0;
}

bool TapeDrive::nextRecord( int count )
{
    if ( _fd < 0 ) {
        return FALSE;
    }

    flush();
    
    struct mtop tapeOp;
    tapeOp.mt_op    = MTFSR;
    tapeOp.mt_count = count;

    bool status = ( ioctl( _fd, MTIOCTOP, &tapeOp ) >= 0 );

    if ( !status ) {
        // Try reading count * TapeBlockSize bytes.
        char buf[Options::instance()->getTapeBlockSize()];
        int bytes = count * Options::instance()->getTapeBlockSize();
        int ret;
        while ( bytes > 0 ) {
            ret = read( buf, bytes > Options::instance()->getTapeBlockSize() ? Options::instance()->getTapeBlockSize() : bytes );
            if ( ret <= 0 ) {
                status = FALSE;
                break;
            }
            bytes -= ret;
        }
        status = TRUE;
    }
    return status;
}

bool TapeDrive::prevRecord( int count )
{
    if ( _fd < 0 ) {
        return FALSE;
    }

    flush();

    struct mtop tapeOp;
    tapeOp.mt_op    = MTBSR;
    tapeOp.mt_count = count;
    int ret = ioctl( _fd, MTIOCTOP, &tapeOp );
    if ( ret < 0 ) {
        printf( "TapeDrive::prevRecord() -- ioctl( MTBSR ) failed!\n" );
    }
    return ret >= 0;
}

void TapeDrive::close()
{
    if ( _fd < 0 ) {
        return;
    }

    flush();

    ::close( _fd );
}

void TapeDrive::open()
{
    close();

    // Open the tape device.
    _fd = ::open( Options::instance()->getTapeDevice(), O_RDWR );
    if ( _fd < 0 ) {
        _fd = ::open( Options::instance()->getTapeDevice(), O_RDONLY );
        if ( _fd < 0 ) {
            return;
        } else {
            _readOnly = TRUE;
        }
    } else {
        _readOnly = FALSE;
    }

    // Set the tape block size after the device is opened.
    setBlockSize( Options::instance()->getTapeBlockSize() );
}

int TapeDrive::read( char* buf, int len )
{
    if ( _fd < 0 ) {
        return -1;
    }

    //printf( "TapeDrive::read() -- _readBufIdx = %d\n", _readBufIdx );

    int remain = Options::instance()->getTapeBlockSize() - _readBufIdx;
    if ( len <= remain ) {
        memcpy( buf, _readBuf + _readBufIdx, len );
        _readBufIdx += len;
    } else {
        memcpy( buf, _readBuf + _readBufIdx, remain );
        _readBufIdx = Options::instance()->getTapeBlockSize();

        int need = Options::instance()->getTapeBlockSize();
        int count = 0;
        while ( need > 0 ) {
            int ret = ::read( _fd, _readBuf + count, Options::instance()->getTapeBlockSize() );
            if ( ret <= 0 ) return ret;
            need -= ret;
            count += ret;
        }

        memcpy( buf + remain, _readBuf, len - remain );
        _readBufIdx = len - remain;
    }

    //int ret = ::read( _fd, buf, len );
    return len;
}

int TapeDrive::write( const char* buf, int len )
{
    if ( _fd < 0 ) {
        return -1;
    }

    int bufIdx = 0;
    while ( len + _writeBufIdx - bufIdx > Options::instance()->getTapeBlockSize() ) {
        memcpy( _writeBuf + _writeBufIdx, buf + bufIdx, Options::instance()->getTapeBlockSize() - _writeBufIdx );
        int ret = ::write( _fd, _writeBuf, Options::instance()->getTapeBlockSize() );
        if ( ret < 0 ) {
            printf( "TapeDrive::write() -- write failed!\n" );
            return ret;
        }
        bufIdx += Options::instance()->getTapeBlockSize() - _writeBufIdx;
        _writeBufIdx = 0;
    }

    if ( bufIdx < len ) {
        memcpy( _writeBuf + _writeBufIdx, buf + bufIdx, len - bufIdx );
        _writeBufIdx += len - bufIdx;
    }

    return len;
}

bool TapeDrive::seek( int file, int tarBlock )
{
//    printf( "TapeDrive::seek() -- file = %d, block = %d\n", file, tarBlock );

    if ( _fd < 0 ) {
        printf( "bailing1\n" );
        return FALSE;
    }

    flush();

    // Go to the desired archive.
    emit sigStatus( i18n( "Skipping to archive..." ) );

    int curFile = getFile();
//    printf( "TapeDrive::seek() -- curFile = %d\n", curFile );
    if ( curFile < 0 ) {
        emit sigStatus( i18n( "Rewinding tape..." ) );
        rewind();
        curFile = 0;
    }

    int fileDiff = file - curFile;
    if ( fileDiff > 0 ) {
        nextFile( fileDiff );
    } else if ( fileDiff < 0 ) {
        prevFile( -fileDiff + 1 );
        nextFile( 1 );
    }

    int tapeBlock = tarBlock / ( Options::instance()->getTapeBlockSize() / 512 );
//    printf( "TapeDrive::seek() -- desired tapeBlock = %d\n", tapeBlock );

    // Go to the desired record within the archive.
    emit sigStatus( i18n( "Skipping to block..." ) );
    int curBlock = getBlock();
//    printf( "TapeDrive::seek() -- curBlock = %d\n", curBlock );
    if ( curBlock < 0 ) {
        emit sigStatus( i18n( "Rewinding tape..." ) );
        rewind();
        nextFile( file );
        if ( ( curBlock = getBlock() ) < 0 ) {
            printf( "bailing2\n" );
            return FALSE;
        }
    }

    if ( tapeBlock > curBlock ) {
        if ( !nextRecord( tapeBlock - curBlock ) ) {
            printf( "bailing3\n" );
            return FALSE;
        }
    } else if ( tapeBlock < curBlock ) {
        if ( tapeBlock == 0 ) {
            if ( !prevFile( 1 ) ) {
                printf( "bailing6\n" );
                return FALSE;
            }
            if ( !nextFile( 1 ) ) {
                printf( "bailing7\n" );
                return FALSE;
            }
        } else {
            if ( !prevRecord( curBlock - tapeBlock + 1 ) ) {
                printf( "bailing4\n" );
                return FALSE;
            }
            if ( !nextRecord( 1 ) ) {
                printf( "bailing5\n" );
                return FALSE;
            }
        }
    }

//    printf( "TapeDrive::seek() -- now: file = %d, block = %d\n", getFile(), getBlock() );

    // If tapeBlockSize > 512, we may need to skip some tar blocks.
//    printf ( "TapeDrive::seek() -- skipping %d tar blocks.\n", tarBlock - tapeBlock * ( Options::instance()->getTapeBlockSize() / 512 ) );
    char buf[ Options::instance()->getTapeBlockSize() ];
    read( buf, 512 * ( tarBlock - tapeBlock * ( Options::instance()->getTapeBlockSize() / 512 ) ) );

    return TRUE;
}

bool TapeDrive::pastEOF()
{
    if ( _fd < 0 ) {
        return FALSE;
    }

    struct mtget tapeStatus;
    if ( ioctl( _fd, MTIOCGET, &tapeStatus ) < 0 ) {
        printf( "TapeDrive::pastEOF() -- ioctl( MTIOCGET ) failed!\n" );
        return FALSE;
    }

    return GMT_EOF( tapeStatus.mt_gstat );
}

bool TapeDrive::setBlockSize( int blockSize )
{
    if ( _fd < 0 ) {
        return FALSE;
    }

#ifdef MTSETBLK
    flush();

    int ret = 0;
    if ( Options::instance()->getVariableBlockSize() ) {
        struct mtop tapeOp;
        tapeOp.mt_op    = MTSETBLK;
        tapeOp.mt_count = blockSize;
        ret = ioctl( _fd, MTIOCTOP, &tapeOp );
        if ( ret < 0 ) {
            printf( "TapeDrive::setBlockSize() -- ioctl( MTSETBLK ) failed!\n" );
        }
    }

    _readBufIdx = blockSize;
    _writeBufIdx = 0;
    delete [] _readBuf;
    _readBuf = new char[ blockSize ];
    delete [] _writeBuf;
    _writeBuf = new char[ blockSize ];

    return ret >= 0;
#else
    // some systems (e.g. HP-UX) encode block size into device file names
    // so setting the block size by software does not make sense
    return TRUE;
#endif
}
