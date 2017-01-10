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

#ifndef _Archive_h_
#define _Archive_h_

#include <stdio.h>

#include <qlist.h>
#include <qstring.h>

#include "File.h"

class Tape;

/**
 * @short This class represents a single tar archive.
 */
class Archive {
    bool _stubbed;

    int         _endBlock;
    int         _ctime;
    FILE*       _fptr;
    int         _offset;
    QString     _name;
    QList<File> _children;
    RangeList   _ranges;
    Tape*       _tape;
public:
    /**
     * Create a new archive.
     *
     * @param tape       The tape containing this archive.
     * @param ctime      The create time of the archive.
     * @param name       The name given to this archive by the user.
     */
    Archive( Tape* tape, int ctime, const char* name );

    /**
     * Create a new stubbed instance of an archive.  The file pointer and
     * offset specify where the actual instance data can be found.  The real
     * data is read on demand when one of the accessor functions is called.
     *
     * @param tape   The tape containing this archive.
     * @param fptr   The open index file containing this archive entry.  The file
     *               must be left open so that the archive entry information can
     *               be read at a later time.
     * @param offset The offset that will be seeked to when reading the archive
     *               entry information.
     */
    Archive( Tape* tape, FILE* fptr, int offset );

    /**
     * Destroy the archive entry and all of its children.
     */
    ~Archive();

    /**
     * Insure that all of the data fields for this archive entry have been read
     * in.  If the archive entry is a stub then the actual data is read from the
     * index file.  If the archive entry is not a stub then no action is taken.
     *
     * @param version The version of the old tape index.
     */
    void read( int version = KDAT_INDEX_FILE_VERSION );

    /**
     * Recursively read the instance data for this archive entry and all of it's
     * children.  This method is used when converting from an older index format.
     *
     * @param version The version of the old tape index.
     */
    void readAll( int version );

    /**
     * Write out the archive entry to the open file.  Entries for each of its
     * children will also be written.
     */
    void write( FILE* fptr );

    /**
     * Get the creation time for this archive.
     *
     * @return The creation time in seconds since the Epoch.
     */
    int getCTime();

    /**
     * Get the last tape block of this archive.
     *
     * @return The last tape block used by this archive.
     */
    int getEndBlock();

    /**
     * Get the name of this archive.
     *
     * @return The name of this archive.
     */
    QString getName();

    /**
     * Get the tape that contains this archive.
     *
     * @return A pointer to the tape containing this archive.
     */
    Tape* getTape();
    
    /**
     * Get the list of top-level files in this archive.
     *
     * @return A list of the immediate children of this archive.
     */
    const QList<File>& getChildren();

    /**
     * Get the list of ranges of this file and all of its children.
     *
     * @return A list of ranges.
     */
    const QList<Range>& getRanges();

    /**
     * Set the ending tape block for this archive.
     *
     * @param endBlock The last tape block used by this archive.
     */
    void setEndBlock( int endBlock );

    /**
     * Set the name of this archive.
     *
     * @param name The new archive name.
     */
    void setName( const char* name );

    /**
     * Add a new top level file as a child of this archive.
     *
     * @param file The file to add.
     */
    void addChild( File* file );

    /**
     * Create a new file entry, and add it to the archive.  Based on the
     * full path name of the file, an appropriate parent is found.  The parent
     * may be this archive or another file entry.  File entries will be created
     * on demand if some or all of the file's path does not yet exist in this
     * archive.
     *
     * @param size        The size, in bytes, of the file.
     * @param mtime       The last modification time for the file, in seconds since
     *                    the Epoch.
     * @param startRecord The first tar record number of this file.
     * @param endRecord   The last tar record number of this file.
     * @param name        The full path name for the file.
     *
     * @return A pointer to the newly created file entry.
     */
    File* addFile( int size, int mtime, int startRecord, int endRecord, const char* name );

    /**
     * Recursively calculate the list of ranges for all of the archive's children.
     */
    void calcRanges();
};

#endif
