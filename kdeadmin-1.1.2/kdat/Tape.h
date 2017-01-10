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

#ifndef _Tape_h_
#define _Tape_h_

#include <qlist.h>
#include <qstring.h>

#include "Archive.h"

/**
 * @short This class represents a single tape index.
 */
class Tape {
    bool           _stubbed;
    QString        _id;
    int            _ctime;
    int            _mtime;
    QString        _name;
    int            _size;
    QList<Archive> _children;

    FILE* _fptr;

    void readVersion1Index( FILE* fptr );
    void readVersion2Index( FILE* fptr );
    void readVersion3Index( FILE* fptr );
    void readVersion4Index( FILE* fptr );

    void calcRanges();

    void read();
    void readAll( int version );
    void write();
public:
    /**
     * Create a new tape index, and automatically generate a unique tape ID.
     */
    Tape();

    /**
     * Create a new tape index for the given tape index ID.
     *
     * @param id The unique tape index identifier.
     */
    Tape( const char* id );

    /**
     * Destroy the tape index.
     */
    ~Tape();

    /**
     * Writes a KDat header containing the tape ID, at the beginning of the
     * tape.  All data on the tape will be lost.
     */
    void format();

    /**
     * Get the unique ID for the tape.
     *
     * @return The tape id.
     */
    QString getID();

    /**
     * Get the user-specified name for the tape.
     *
     * @return The name of the tape.
     */
    QString getName();

    /**
     * Get the date and time that the tape was formatted.
     *
     * @return The tape format time, in seconds since the Epoch.
     */
    int getCTime();
    
    /**
     * Get the last time that the tape was modified.
     *
     * @return The tape modification time, in seconds since the Epoch.
     */
    int getMTime();

    /**
     * Get the total tape capacity.
     *
     * @return The tape capacity in kilobytes.
     */
    int getSize();

    /**
     * Get the list of archives on this tape.
     *
     * @return The list of all archives on the tape.
     */
    const QList<Archive>& getChildren();

    /**
     * Set the name for the tape.
     *
     * @param name The new name for the tape.
     */
    void setName( const char* name );

    /**
     * Set the modification time for the tape to be the current time..
     */
    void setMTime( int mtime );

    /**
     * Set the total capacity of the tape.
     *
     * @param size The total size, in kilobytes, of the tape.
     */
    void setSize( int size );

    /**
     * Add an archive to the tape index.
     *
     * @param archive The archive to add.
     */
    void addChild( Archive* archive );

    /**
     * Remove an archive and all the archives that follow it from the index.
     *
     * @param archive The archive to remove.
     */
    void removeChild( Archive* archive );

    /**
     * Recursively destroy all children of the tape index.
     */
    void clear();
};

#endif
