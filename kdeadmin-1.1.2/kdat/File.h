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

#ifndef _File_h_
#define _File_h_

#include <stdio.h>

#include <qlist.h>
#include <qstring.h>

#include "Range.h"
#include "kdat.h"

/**
 * @short This class represents a single file or directory in a tar file.
 */
class File {
    bool _stubbed;
    union {
        struct {
            int _size;
            int _mtime;
            int _startRecord;
            int _endRecord;
        } _data;
        struct {
            FILE* _fptr;
            int   _offset;
        } _stub;
    } _union;
    QString     _name;
    File*       _parent;
    QList<File> _children;
    RangeList   _ranges;
public:
    /**
     * Create a new file entry.
     *
     * @param parent      The directory file entry that contains this file.
     * @param size        The size of the file in bytes.
     * @param mtime       The last modification time of the file in seconds since
     *                    the Epoch.
     * @param startRecord The first tar record number in the file.
     * @param endRecord   The last tar record number in the file.
     * @param name        The file name.  If the file name ends with a '/' then it
     *                    is assumed to be a directory name.  Only the last part of
     *                    the file's path name should be passed in.  The rest of the
     *                    path is determined by this file entry's ancestors.
     */
    File( File* parent, int size, int mtime, int startRecord, int endRecord, const char* name );
    
    /**
     * Create a new stubbed instance of a file entry.  The file pointer and
     * offset specify where the actual instance data can be found.  The real
     * data is read on demand when one of the accessor functions is called.
     *
     * @param parent The directory file entry that contains this file.
     * @param fptr   The open index file containing this file entry.  The file
     *               must be left open so that the file entry information can
     *               be read at a later time.
     * @param offset The offset that will be seeked to when reading the file
     *               entry information.
     */
    File( File* parent, FILE* fptr, int offset );

    /**
     * Destroy the file entry and all of its children.
     */
    ~File();
    
    /**
     * Insure that all of the data fields for this file entry have been read
     * in.  If the file entry is a stub then the actual data is read from the
     * index file.  If the file entry is not a stub then no action is taken.
     *
     * @param version The version of the old tape index.
     */
    void read( int version = KDAT_INDEX_FILE_VERSION );

    /**
     * Recursively read the instance for this file entry and all of it's
     * children.  This method is used when converting from an older index format.
     *
     * @param version The version of the old tape index.
     */
    void readAll( int version );
    
    /**
     * Write out the file entry to the open file.  Entries for each of its
     * children will also be written.
     */
    void write( FILE* fptr );

    /**
     * Determine whether this file entry represents a directory.  If the file
     * name ends in a '/' then it is assumed that it is a directory.
     *
     * @return TRUE if the file represents a directory, or FALSE if it is an
     *         ordinary file.
     */
    bool isDirectory();

    /**
     * Get the size of the file.
     *
     * @return The size, in bytes, of the file.
     */
    int getSize();

    /**
     * Get the last modification time for the file.
     *
     * @ return The last time the file was modified, in seconds since the Epoch.
     */
    int getMTime();

    /**
     * Get the tar record number for the file.  This is the number of 512-byte
     * tar blocks that must be read before getting to this file.
     *
     * @return The tar record number for the file.
     */
    int getStartRecord();

    /**
     * Get the tar record number that is just past the end of the file.  This
     * number minus the start record gives the number of 512-byte tar blocks
     * that this file occupies in the archive.
     *
     * @return The last tar record number for the file.
     */
    int getEndRecord();

    /**
     * Get the name of this file.  Only the last component of the full path
     * name is returned.
     *
     * @return The file's name.
     */
    QString getName();

    /**
     * Get the full path name of the file.
     *
     * @return The full path to the file.
     */
    QString getFullPathName();

    /**
     * Get the file entry's parent.  A NULL parent indicates that this is one
     * of (possibly) many top level directories within the tar archive.
     *
     * @return A pointer to the file entry that contains this file entry.
     */
    File* getParent();

    /**
     * Get the children of this file entry.  A normal file will never have any
     * children.  A directory may or may not have children.
     *
     * @return A list of the immediate children of this file entry.
     */
    const QList<File>& getChildren();

    /**
     * Get the list of ranges of this file and all of its children.
     *
     * @return A list of ranges.
     */
    const QList<Range>& getRanges();

    /**
     * Add a new file entry as a child of this file entry.
     *
     * @param file The file to add.
     */
    void addChild( File* file );

    /**
     * Recursively calculate the list of ranges for all of the file's children.
     */
    void calcRanges();
};

#endif
