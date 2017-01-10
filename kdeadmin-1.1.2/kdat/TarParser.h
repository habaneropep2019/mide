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

#ifndef _TarParser_h_
#define _TarParser_h_

#include <qobject.h>

/* Standard Archive Format - Standard TAR - USTAR.  */

/* Header block on tape.
   
   We use traditional DP naming conventions here.  A "block" is a big chunk
   of stuff that we do I/O on.  A "record" is a piece of info that we care
   about.  Typically many "record"s fit into a "block".  */

#define RECORDSIZE      512
#define NAMSIZ          100
#define TUNMLEN         32
#define TGNMLEN         32
#define SPARSE_EXT_HDR  21
#define SPARSE_IN_HDR   4

struct sparse
{
    char offset[12];
    char numbytes[12];
};

union record
{
    char charptr[RECORDSIZE];

    struct header
    {
        char arch_name[NAMSIZ];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        char mtime[12];
        char chksum[8];
        char linkflag;
        char arch_linkname[NAMSIZ];
        char magic[8];
        char uname[TUNMLEN];
        char gname[TGNMLEN];
        char devmajor[8];
        char devminor[8];

        /* The following fields were added for GNU and are not standard.  */

        char atime[12];
        char ctime[12];
        char offset[12];
        char longnames[4];
        /* Some compilers would insert the pad themselves, so pad was
           once autoconfigured.  It is simpler to always insert it!  */
        char pad;
        struct sparse sp[SPARSE_IN_HDR];
        char isextended;
        char realsize[12];      /* true size of the sparse file */
#if 0
        char ending_blanks[12]; /* number of nulls at the end of the file,
                                   if any */
#endif
    }
    header;

    struct extended_header
    {
        struct sparse sp[21];
        char isextended;
    }
    ext_hdr;
};

/* The checksum field is filled with this while the checksum is computed.  */
#define CHKBLANKS       "        "      /* 8 blanks, no null */

/* The magic field is filled with this value if uname and gname are valid,
   marking the archive as being in standard POSIX format (though GNU tar
   itself is not POSIX conforming).  */
#define TMAGIC "ustar  "        /* 7 chars and a null */

/* The magic field is filled with this if this is a GNU format dump entry.
   But I suspect this is not true anymore.  */
#define GNUMAGIC "GNUtar "      /* 7 chars and a null */

/* The linkflag defines the type of file.  */
#define LF_OLDNORMAL    '\0'    /* normal disk file, Unix compat */
#define LF_NORMAL       '0'     /* normal disk file */
#define LF_LINK         '1'     /* link to previously dumped file */
#define LF_SYMLINK      '2'     /* symbolic link */
#define LF_CHR          '3'     /* character special file */
#define LF_BLK          '4'     /* block special file */
#define LF_DIR          '5'     /* directory */
#define LF_FIFO         '6'     /* FIFO special file */
#define LF_CONTIG       '7'     /* contiguous file */
/* Further link types may be defined later.  */

/* Note that the standards committee allows only capital A through
   capital Z for user-defined expansion.  This means that defining
   something as, say '8' is a *bad* idea.  */

/* This is a dir entry that contains the names of files that were in the
   dir at the time the dump was made.  */
#define LF_DUMPDIR      'D'

/* Identifies the NEXT file on the tape as having a long linkname.  */
#define LF_LONGLINK     'K'

/* Identifies the NEXT file on the tape as having a long name.  */
#define LF_LONGNAME     'L'

/* This is the continuation of a file that began on another volume.  */
#define LF_MULTIVOL     'M'

/* For storing filenames that didn't fit in 100 characters.  */
#define LF_NAMES        'N'

/* This is for sparse files.  */
#define LF_SPARSE       'S'

/* This file is a tape/volume header.  Ignore it on extraction.  */
#define LF_VOLHDR       'V'

#if 0
/* The following two blocks of #define's are unused in GNU tar.  */

/* Bits used in the mode field - values in octal */
#define  TSUID    04000         /* set UID on execution */
#define  TSGID    02000         /* set GID on execution */
#define  TSVTX    01000         /* save text (sticky bit) */

/* File permissions */
#define  TUREAD   00400         /* read by owner */
#define  TUWRITE  00200         /* write by owner */
#define  TUEXEC   00100         /* execute/search by owner */
#define  TGREAD   00040         /* read by group */
#define  TGWRITE  00020         /* write by group */
#define  TGEXEC   00010         /* execute/search by group */
#define  TOREAD   00004         /* read by other */
#define  TOWRITE  00002         /* write by other */
#define  TOEXEC   00001         /* execute/search by other */

#endif

/* End of Standard Archive Format description.  */

/**
 * @short A parser for GNU tar archives.
 */
class TarParser : public QObject {
    Q_OBJECT
    char _buf[512];
    int  _bufIdx;
    int  _blocksToSkip;
    int  _record;
    bool _longname;
    bool _extended;
    char _archname[8192];
    int  _archnameIdx;

    int  parseOctal( const char* data, int length );
    void parseTarBlock();
public:
    /**
     * Create a new GNU tar archive parser.
     */
    TarParser();
public slots:
    /**
     * This slot receives raw data that represents a GNU tar archive.
     *
     * @param data   An array of data bytes.
     * @param length The length of the data array.
     */
    void slotData( const char* data, int length );
signals:
    /**
     * This signal is emitted whenever a file entry is parsed from the archive.
     *
     * @param name   The name of the the file.
     * @param size   The size of the file, in bytes.
     * @param mtime  The last modification time for the file.
     * @param record The tar record that this file begins on.
     */
    void sigEntry( const char* name, int size, int mtime, int record );
};

#endif
