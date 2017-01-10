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

#ifndef _TapeManager_h_
#define _TapeManager_h_

#include <qdict.h>
#include <qobject.h>
#include <qstrlist.h>

#include "Tape.h"

/**
 * @short Control access to the set of tape indexes.
 *
 * Each user has a set of tape indexes that are stored under
 * <TT>$HOME/.kdat/</TT>.  This class provides a single point of access for
 * reading and writing these index files.
 *
 * Other objects can register to be notified when a tape index is added or
 * removed, and when a tape index is modified.
 *
 * A reference to the index of the currently mounted tape is maintained, and
 * other objects can register to be notified when a tape is mounted and
 * unmounted.
 *
 * The TapeManager follows the Singleton pattern.
 */
class TapeManager : public QObject {
    Q_OBJECT

    static TapeManager* _instance;

    QDict<Tape> _tapes;
    QStrList    _tapeIDs;
    Tape*       _mountedTape;
    
    TapeManager();
public:
    ~TapeManager();

    /**
     * All access to the TapeManager goes through this method.
     *
     * @return a pointer to the single instance of the TapeManager.
     */
    static TapeManager* instance();

    /**
     * Get the list of all known tape IDs.
     *
     * @return a QStrList containing the tape IDs.
     */
    const QStrList& getTapeIDs();

    /**
     * Retrieve the index for a tape.
     *
     * @param id the ID of the tape.
     * @return the tape's index.
     */
    Tape* findTape( const char* id );

    /**
     * Add a new tape index.
     *
     * @param tape a pointer to the new tape index.
     */
    void addTape( Tape* tape );

    /**
     * Remove a tape index.  The tape index is removed from memory and from
     * disk.  A signal is emitted before the tape is actually removed.
     *
     * @param tape a pointer to the tape index to remove.
     */
    void removeTape( Tape* tape );

    /**
     * Notify anyone who cares that the tape index has been modified.
     *
     * @param tape a pointer to the tape index that was modified.
     */
    void tapeModified( Tape* tape );

    /**
     * Call this method whenever a tape is first mounted.
     *
     * @param tape a pointer to the newly mounted tape's index.
     */
    void mountTape( Tape* tape );

    /**
     * Call this method whenever the current tape is about to be unmounted.
     */
    void unmountTape();

    /**
     * Get a handle on the currently mounted tape's index.
     *
     * @return a pointer to the mounted tape's index, or NULL if no tape is
     * mounted.
     */
    Tape* getMountedTape();
signals:
    /**
     * Emitted after a new tape index is created.
     *
     * @param tape a pointer to the new tape index.
     */
    void sigTapeAdded( Tape* tape );

    /**
     * Emitted before a tape index is destroyed.  This signal is emitted
     * immediately before the tape index is deleted.
     *
     * @param tape a pointer to the tape index that is about to be destroyed.
     */
    void sigTapeRemoved( Tape* tape );

    /**
     * Emitted after a tape index has been changed in some way.
     *
     * @param tape a pointer to the tape index that has been modified.
     */
    void sigTapeModified( Tape* tape );

    /**
     * Emitted after a tape has been mounted.
     */
    void sigTapeMounted();

    /**
     * Emitted just before the current tape is unmounted.
     */
    void sigTapeUnmounted();
};

#endif
