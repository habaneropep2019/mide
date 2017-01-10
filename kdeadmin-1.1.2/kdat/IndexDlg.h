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

#ifndef _IndexDlg_h_
#define _IndexDlg_h_

#include <qdialog.h>
#include <qlist.h>

#include "Range.h"

class QLabel;
class QPushButton;

class Archive;
class File;
class LoggerWidget;
class Tape;
class TarParser;

/**
 * @short Status dialog for recreating a tape index.
 */
class IndexDlg : public QDialog {
    Q_OBJECT
    TarParser*          _tarParser;
    Tape*               _tape;
    Archive*            _archive;
    QString             _leftover;
    QLabel*             _elapsedTime;
    QLabel*             _kbytesRead;
    QLabel*             _transferRate;
    QLabel*             _archives;
    QLabel*             _files;
    QLabel*             _totalFiles;
    LoggerWidget*       _log;
    QPushButton*        _ok;
    QPushButton*        _save;
    QPushButton*        _abort;
    int                 _startTime;
    float               _totalKBytes;
    int                 _archiveCount;
    int                 _fileCount;
    int                 _totalFileCount;
    bool                _aborted;
    int                 _numFiles;

    int     _fileSize;
    int     _fileMTime;
    int     _fileStartRecord;
    QString _fileName;

    void updateStats();
private slots:
    void slotOK();
    void slotAbort();
    void slotEntry( const char* name, int size, int mtime, int record );
protected:
    void show();
    void timerEvent( QTimerEvent* e );
public:
    /**
     * Create a new tape index dialog.
     *
     * @param tape   A pointer to the empty tape index to fill in.
     * @param parent The parent widget for this dialog.
     * @param name   The name of this dialog.
     */
    IndexDlg( Tape* tape, QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the tape index dialog.
     */
    ~IndexDlg();
};

#endif
