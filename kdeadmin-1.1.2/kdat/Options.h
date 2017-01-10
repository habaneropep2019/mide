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

#ifndef _Options_h_
#define _Options_h_

#include <qobject.h>

class KConfig;

/**
 * @short The central repository for user preferences.
 */
class Options : public QObject {
    Q_OBJECT
    KConfig* _config;
    int      _defaultTapeSize;
    int      _tapeBlockSize;
    QString  _tapeDevice;
    QString  _tarCommand;
    bool     _loadOnMount;
    bool     _lockOnMount;
    bool     _ejectOnUnmount;
    bool     _variableBlockSize;

    static Options* _instance;

    Options();
public:
    /**
     * Get a reference to the single instance of the Options object.
     *
     * @return A pointer to the options object.
     */
    static Options* instance();
    
    /**
     * Save the current user preference settings.
     */
    void sync();

    /**
     * Get the default size to use when formatting a tape.
     *
     * @return The default size in kilobytes of a tape.
     */
    int getDefaultTapeSize();

    /**
     * The size of a tape block can be different from the size of a tar block.
     * Tar blocks are always 512 bytes long.  Be careful!
     *
     * @return the size in bytes of a single tape block
     */
    int getTapeBlockSize();

    /**
     * Get the full path to the tape device.
     *
     * @return The tape device path.
     */
    const char* getTapeDevice();

    /**
     * Get the full path to the tar command.
     *
     * @return The path to the tar command.
     */
    const char* getTarCommand();

    /**
     * Get whether to load the tape before attempting to mount it.
     *
     * @return TRUE if the tape should be loaded.
     */
    bool getLoadOnMount();

    /**
     * Get whether to lock the tape drive when a tape is mounted.
     *
     * @return TRUE if the tape drive should be locked, otherwise FALSE.
     */
    bool getLockOnMount();

    /**
     * Get whether to automatically eject the tape when it is unmounted.
     *
     * @return TRUE if the tape should be ejected, otherwise FALSE.
     */
    bool getEjectOnUnmount();

    /**
     * Get whether the tape drive supports variable block sizes.
     *
     * @return TRUE if the tape drive can handle the MTSETBLK command.
     */
    bool getVariableBlockSize();

    /**
     * Set the default size for new tapes.
     *
     * @param kbytes The size in kilobytes.
     */
    void setDefaultTapeSize( int kbytes );

    /**
     * Set the size of tape block.
     *
     * @param bytes The size in bytes of one tape block.
     */
    void setTapeBlockSize( int bytes );

    /**
     * Set the path to the tape device.
     *
     * @param str The full path to the tape device.
     */
    void setTapeDevice( const char* str );

    /**
     * Set the path to the tar command.
     *
     * @param str The full path to the tar command.
     */
    void setTarCommand( const char* str );

    /**
     * Set whether to load the tape before attempting to mount it.
     *
     * @param b TRUE if the tape should be loaded.
     */
    void setLoadOnMount( bool b );

    /**
     * Set whether to lock the tape drive whenever a tape is mounted.
     *
     * @param b TRUE means lock the drive, FALSE means don't.
     */
    void setLockOnMount( bool b );

    /**
     * Set whether to eject the tape drive whenever a tape is unmounted.
     *
     * @param b TRUE means eject the tape, FALSE means don't.
     */
    void setEjectOnUnmount( bool b );

    /**
     * Set whether the tape drive can support variable block sizes.
     *
     * @param b TRUE means the tape drive understands MTSETBLK.
     */
    void setVariableBlockSize( bool b );
signals:
    /**
     * Emitted when the default tape size changes.
     */
    void sigDefaultTapeSize();

    /**
     * Emitted when the tape block size changes.
     */
    void sigTapeBlockSize();

    /**
     * Emitted when the path to the tape device changes.
     */
    void sigTapeDevice();

    /**
     * Emitted when the path to the tar command changes.
     */
    void sigTarCommand();

    /**
     * Emitted when the load on mount feature is enabled/disabled.
     */
    void sigLoadOnMount();

    /**
     * Emitted when the lock on mount feature is enabled/disabled.
     */
    void sigLockOnMount();

    /**
     * Emitted when the eject on umount feature is enabled/disabled.
     */
    void sigEjectOnUnmount();

    /**
     * Emitted when the variable block size feature is enabled/disabled.
     */
    void sigVariableBlockSize();
};

#endif
