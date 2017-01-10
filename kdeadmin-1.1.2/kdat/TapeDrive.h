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

#ifndef _TapeDrive_h_
#define _TapeDrive_h_

#include <qobject.h>

class Tape;

/**
 * @short An OO interface to the tape drive.
 */
class TapeDrive : public QObject {
    Q_OBJECT
    int   _fd;
    bool  _readOnly;
    Tape* _tape;
    char* _writeBuf;
    char* _readBuf;
    int   _writeBufIdx;
    int   _readBufIdx;

    static TapeDrive* _instance;

    TapeDrive();
public:
    /**
     * Destroy the tape drive (figuratively speaking, of course).
     */
    ~TapeDrive();

    /**
     * Get a reference to the single instance of the tape drive object.
     *
     * @return A pointer to the tape drive object.
     */
    static TapeDrive* instance();

    /**
     * Close the tape device.  If data was written to the tape drive, then this
     * will also cause a single end-of-file marker to be written out.  This is
     * the preferred way to write an end-of-file marker to the tape.
     */
    void close();

    /**
     * Physically eject the tape from the drive.  Some tape drives do not
     * support this operation well.
     */
    void eject();

    /**
     * If there is unwritten data in the buffer, it will be flushed out.  This
     * method is called when necessary by the other tape drive methods.
     */
    void flush();

    /**
     * Get the current tape file number.  The first tape file is number 0.
     *
     * @return >= 0 on success, -1 on failure.
     */
    int  getFile();

    /**
     * Get the current tape block number within the current file.  The first
     * tape block number within a file is 0.
     *
     * @return >= 0 on success, -1 on failure.
     */
    int  getBlock();

    /**
     * Determine whether the tape can be written to.
     *
     * @return TRUE if the tape cannot be written to, otherwise FALSE.
     */
    bool isReadOnly();

    /**
     * Determine whether there is a tape in the drive.
     *
     * @return TRUE if there is a tape, otherwise FALSE.
     */
    bool isTapePresent();

    /**
     * Load the tape into the drive.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool load();

    /**
     * Lock the tape in the drive, so that it cannot be ejected manually by the
     * user.  Not all tape drives support this operation.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool lock();

    /**
     * Skip over one or more end-of-file markers on the tape.  The tape is
     * positioned just after the last skipped end-of-file marker.
     *
     * @param count The number of end-of-file markers to skip over.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool nextFile( int count );

    /**
     * Skip over one or more tape blocks within the current tape file.
     *
     * @param count The number of tape blocks to skip over.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool nextRecord( int count );

    /**
     * Open the tape device for reading and writing.  If this fails, try
     * opening the tape device for reading only.  Check isReadOnly() to see
     * if the tape device was opened for reading and writing.
     *
     * @return TRUE if the tape device was opened for reading and/or writing,
     *         otherwise FALSE.
     */
    void open();

    /**
     * Determine wether the tape is positioned just after an end-of-file
     * marker.
     *
     * @return TRUE if the tape is positioned after an end-of-file marker,
     *         otherwise FALSE.
     */
    bool pastEOF();

    /**
     * Backspace over one or more end-of-file markers.  The tape is positioned
     * just before the last end-of-file marker.
     *
     * @param count The number of end-of-file markers to backspace over.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool prevFile( int count );

    /**
     * Backspace over one or more tape blocks.
     *
     * @param count The number of tape block to backspace over.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool prevRecord( int count );

    /**
     * Read data from the tape.
     *
     * @param buf The buffer to read into.
     * @param len The number of bytes to read.
     *
     * @return The actual number of bytes read, or -1 on failure.
     */
    int read( char* buf, int len );

    /**
     * Read the KDat tape header (if there is one), and get the index
     * associated with the tape.
     *
     * @return A pointer to the tape index, or NULL if the tape does not have
     *         a KDat header.
     */
    Tape* readHeader();

    /**
     * Rewind the tape.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool rewind();

    /**
     * Move the tape to the given tar block within the given tape file.  This
     * method will intelligently move the tape to the desired position.
     *
     * @param file     The tape file number.
     * @param tarBlock The desired tar block (NOT tape block).
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool seek( int file, int tarBlock );

    /**
     * Set the hardware tape block size.
     *
     * @param blockSize The new tape block size in bytes.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool setBlockSize( int blockSize );

    /**
     * Unlock the tape in the drive, so that it can be ejected manually by the
     * user.  Not all tape drives support this operation.
     *
     * @return TRUE on success, otherwise FALSE.
     */
    bool unlock();

    /**
     * Write data to the tape.
     *
     * @param buf The buffer to write from.
     * @param len The number of bytes to write.
     *
     * @return The actual number of bytes written, or -1 on failure.
     */
    int write( const char* buf, int len );
signals:
    /**
     * This signal is emitted for one-line, informational messages.
     *
     * @param msg The informational message.
     */
    void sigStatus( const char* msg );
};

#endif
