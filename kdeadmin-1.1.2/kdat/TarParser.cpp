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

#include "TarParser.h"

#include "TarParser.moc"

TarParser::TarParser()
        : _bufIdx( 0 ),
          _blocksToSkip( 0 ),
          _record( 0 ),
          _longname( FALSE ),
          _extended( FALSE ),
          _archnameIdx( 0 )
{
}

void TarParser::slotData( const char* data, int length )
{
    for ( int i = 0; i < length; i++ ) {
        if ( _bufIdx >= 512 ) {
            if ( _blocksToSkip > 0 ) {
                if ( _extended ) {
                    record* rec = (record*)_buf;
                    _extended = rec->ext_hdr.isextended;
                } else {
                    _blocksToSkip--;
                    if ( _longname ) {
                        memcpy( _archname + _archnameIdx, _buf, 512 );
                        _archnameIdx += 512;
                    }
                }
            } else {
                parseTarBlock();
            }
            _bufIdx = 0;
            _record++;
        }
        _buf[_bufIdx++] = data[i];
    }
}

int TarParser::parseOctal( const char* buf, int length )
{
    int val = 0;
    
    for ( int i = 0; i < length; i++ ) {
        val *= 8;
        if ( ( buf[i] >= '1' ) && ( buf[i] <= '7' ) ) {
            val += buf[i] - '0';
        }
    }

    return val;
}

void TarParser::parseTarBlock()
{
    record* rec = (record*)_buf;

#if 0
    printf( "----- parsing tar block -----\n" );
    printf( "arch_name     = '%s'\n", rec->header.arch_name );
    printf( "mode          = '%s'\n", rec->header.mode );
    printf( "uid           = %d\n", parseOctal( rec->header.uid, 6 ) );
    printf( "gid           = %d\n", parseOctal( rec->header.gid, 6 ) );
    printf( "size          = %d\n", parseOctal( rec->header.size, 11 ) );
    printf( "mtime         = %d\n", parseOctal( rec->header.mtime, 11 ) );
    printf( "chksum        = '%s'\n", rec->header.chksum );
    printf( "linkflag      = '%c'\n", rec->header.linkflag );
    printf( "arch_linkname = '%s'\n", rec->header.arch_linkname );
    printf( "magic         = '%s'\n", rec->header.magic );
    printf( "uname         = '%s'\n", rec->header.uname );
    printf( "gname         = '%s'\n", rec->header.gname );
    printf( "devmajor      = %d\n", parseOctal( rec->header.devmajor, 8 ) );
    printf( "devminor      = %d\n", parseOctal( rec->header.devminor, 8 ) );
    printf( "atime         = %d\n", parseOctal( rec->header.atime, 11 ) );
    printf( "ctime         = %d\n", parseOctal( rec->header.ctime, 11 ) );
    printf( "offset        = %d\n", parseOctal( rec->header.offset, 11 ) );
    printf( "longnames     = '%s'\n", rec->header.longnames );
    printf( "isextended    = %d\n", rec->header.isextended );
    printf( "realsize      = %d\n", parseOctal( rec->header.realsize, 11 ) );
#endif

    if ( rec->header.magic[0] != 0 ) {
        _blocksToSkip = ( parseOctal( rec->header.size, 11 ) + 511 ) / 512;
        _extended = rec->header.isextended;

        if ( rec->header.linkflag == LF_LONGNAME ) {
            // The actual file name is stored in the next _blocksToSkip blocks of the tar-file.
            _longname = TRUE;
            _archnameIdx = 0;
        } else {
            if ( _longname ) {
                _longname = FALSE;
                emit sigEntry( _archname, parseOctal( rec->header.size, 11 ), parseOctal( rec->header.mtime, 11 ), _record );
            } else {
                emit sigEntry( rec->header.arch_name, parseOctal( rec->header.size, 11 ), parseOctal( rec->header.mtime, 11 ), _record );
            }
        }
    }
}
