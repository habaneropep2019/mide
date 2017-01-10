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

#ifndef _VerifyDlg_h_
#define _VerifyDlg_h_

#include <qdialog.h>
#include <qlist.h>

#include "Range.h"

class QLabel;
class QPushButton;

class KProcess;

class LoggerWidget;
class TapeDrive;

/**
 * @short Status dialog for verifying/restoring parts of an archive.
 */
class VerifyDlg : public QDialog {
    Q_OBJECT
    bool                _restore;
    KProcess*           _proc;
    QString             _workingDir;
    int                 _fileno;
    const RangeList&    _ranges;
    QString             _leftover;
    QLabel*             _elapsedTime;
    QLabel*             _timeRemaining;
    QLabel*             _kbytesRead;
    QLabel*             _transferRate;
    QLabel*             _files;
    LoggerWidget*       _log;
    QPushButton*        _ok;
    QPushButton*        _save;
    QPushButton*        _abort;
    int                 _startTime;
    float               _totalKBytes;
    int                 _fileCount;
    int                 _archiveSize;
    bool                _wroteStdin;
    bool                _aborted;
    bool                _done;
    QString             _lastFileName;

    void updateStats();
private slots:
    void slotProcessExited( KProcess* proc );
    void slotStdout( KProcess* proc, char* buf, int len );
    void slotWroteStdin( KProcess* proc );
    void slotOK();
    void slotAbort();
protected:
    void show();
    void timerEvent( QTimerEvent* e );
public:
    /**
     * Create a new verify/restore dialog.
     *
     * @param workingDir The directory to restore files to or verify files against.
     * @param fileno     The tape file number of the archive to verify/restore.
     * @param ranges     A list of tar block ranges to read.
     * @param restore    TRUE means restore, FALSE means verify.
     * @param parent     The parent widget for the dialog.
     * @param name       The name of this widget.
     */
    VerifyDlg( const char* workingDir, int fileno, const RangeList& ranges,
               bool restore = FALSE, QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the verify/restore dialog.
     */
    ~VerifyDlg();
};

#endif
