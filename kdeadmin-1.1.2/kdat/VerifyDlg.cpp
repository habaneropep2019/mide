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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbt.h>

#include <kapp.h>
#include <kprocess.h>

#include "LoggerWidget.h"
#include "Options.h"
#include "VerifyDlg.h"
#include "TapeDrive.h"
#include "Util.h"

#include "VerifyDlg.moc"

VerifyDlg::VerifyDlg( const char* workingDir, int fileno, const RangeList& ranges,
                      bool restore, QWidget* parent, const char* name )
        : QDialog( parent, name, TRUE ),
          _restore( restore ),
          _proc( NULL ),
          _workingDir( workingDir ),
          _fileno( fileno ),
          _ranges( ranges ),
          _totalKBytes( 0.0 ),
          _fileCount( 0 ),
          _wroteStdin( TRUE ),
          _aborted( FALSE ),
          _done( FALSE )
{
    // Calculate size of verify.
    QListIterator<Range> i( _ranges.getRanges() );
    _archiveSize = 0;
    for ( ; i.current(); ++i ) {
        _archiveSize += i.current()->getEnd() - i.current()->getStart();
    }
    _archiveSize = ( _archiveSize + 1 ) / 2;
    
    if ( _restore ) {
        setCaption( i18n( "KDat: Restore" ) );
        setIconText( i18n( "KDat: Restore" ) );
    } else {
        setCaption( i18n( "KDat: Verify" ) );
        setIconText( i18n( "KDat: Verify" ) );
    }

    resize( 500, 300 );

    const int labelWidth = 96;

    QFrame* f1 = new QFrame( this );
    f1->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    QFrame* f2 = new QFrame( this );
    f2->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    QLabel* lbl1 = new QLabel( i18n( "Elapsed time:" ), f1 );
    lbl1->setFixedSize( labelWidth, lbl1->sizeHint().height() );

    _elapsedTime = new QLabel( i18n( "00:00:00" ), f1 );
    _elapsedTime->setFixedHeight( _elapsedTime->sizeHint().height() );

    QLabel* lbl2 = new QLabel( i18n( "Time remaining:" ), f2 );
    lbl2->setFixedSize( labelWidth, lbl2->sizeHint().height() );

    _timeRemaining = new QLabel( i18n( "00:00:00" ), f2 );
    _timeRemaining->setFixedHeight( _timeRemaining->sizeHint().height() );

    QLabel* lbl3 = new QLabel( i18n( "Total kbytes:" ), f1 );
    lbl3->setFixedSize( labelWidth, lbl3->sizeHint().height() );

    QLabel* totalKbytes = new QLabel( Util::kbytesToString( _archiveSize ), f1 );
    totalKbytes->setFixedHeight( totalKbytes->sizeHint().height() );

    QLabel* lbl4 = new QLabel( i18n( "Kbytes read:" ), f2 );
    lbl4->setFixedSize( labelWidth, lbl4->sizeHint().height() );

    _kbytesRead = new QLabel( i18n( "0k" ), f2 );
    _kbytesRead->setFixedHeight( _kbytesRead->sizeHint().height() );

    QLabel* lbl5 = new QLabel( i18n( "Transfer rate:" ), f1 );
    lbl5->setFixedSize( labelWidth, lbl5->sizeHint().height() );

    _transferRate = new QLabel( i18n( "0k/min" ), f1 );
    _transferRate->setFixedHeight( _transferRate->sizeHint().height() );

    QLabel* lbl6;
    if ( _restore ) {
        lbl6 = new QLabel( i18n( "Files:" ), f2 );
        lbl6->setFixedSize( labelWidth, lbl6->sizeHint().height() );
    } else {
        lbl6 = new QLabel( i18n( "Differences:" ), f2 );
        lbl6->setFixedSize( labelWidth, lbl6->sizeHint().height() );
    }

    _files = new QLabel( "0", f2 );
    _files->setFixedHeight( _files->sizeHint().height() );

    if ( _restore ) {
        _log = new LoggerWidget( i18n( "Restore log:" ), this );
    } else {
        _log = new LoggerWidget( i18n( "Verify log:" ), this );
    }

    _ok = new QPushButton( i18n( "OK" ), this );
    _ok->setFixedSize( 80, _ok->sizeHint().height() );
    connect( _ok, SIGNAL( clicked() ), this, SLOT( slotOK() ) );
    _ok->setEnabled( FALSE );

    _save = new QPushButton( i18n( "Save Log..." ), this );
    _save->setFixedSize( 80, _save->sizeHint().height() );
    connect( _save, SIGNAL( clicked() ), _log, SLOT( save() ) );
    _save->setEnabled( FALSE );

    _abort = new QPushButton( i18n( "Abort" ), this );
    _abort->setFixedSize( 80, _abort->sizeHint().height() );
    connect( _abort, SIGNAL( clicked() ), this, SLOT( slotAbort() ) );

    QVBoxLayout* l1 = new QVBoxLayout( this, 8, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addStrut( 3 * lbl1->height() + 16 );
    l1_1->addWidget( f1 );
    l1_1->addWidget( f2 );
    
    QVBoxLayout* l1_1_1 = new QVBoxLayout( f1, 4, 4 );

    QHBoxLayout* l1_1_1_1 = new QHBoxLayout();
    l1_1_1->addLayout( l1_1_1_1 );
    l1_1_1_1->addWidget( lbl1 );
    l1_1_1_1->addWidget( _elapsedTime, 1 );

    QHBoxLayout* l1_1_1_2 = new QHBoxLayout();
    l1_1_1->addLayout( l1_1_1_2 );
    l1_1_1_2->addWidget( lbl3 );
    l1_1_1_2->addWidget( totalKbytes, 1 );

    QHBoxLayout* l1_1_1_3 = new QHBoxLayout();
    l1_1_1->addLayout( l1_1_1_3 );
    l1_1_1_3->addWidget( lbl5 );
    l1_1_1_3->addWidget( _transferRate, 1 );

    QVBoxLayout* l1_1_2 = new QVBoxLayout( f2, 4, 4 );

    QHBoxLayout* l1_1_2_1 = new QHBoxLayout();
    l1_1_2->addLayout( l1_1_2_1 );
    l1_1_2_1->addWidget( lbl2 );
    l1_1_2_1->addWidget( _timeRemaining, 1 );

    QHBoxLayout* l1_1_2_2 = new QHBoxLayout();
    l1_1_2->addLayout( l1_1_2_2 );
    l1_1_2_2->addWidget( lbl4 );
    l1_1_2_2->addWidget( _kbytesRead, 1 );

    QHBoxLayout* l1_1_2_3 = new QHBoxLayout();
    l1_1_2->addLayout( l1_1_2_3 );
    l1_1_2_3->addWidget( lbl6 );
    l1_1_2_3->addWidget( _files, 1 );

    l1->addWidget( _log, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addStretch( 1 );
    l1_2->addWidget( _ok );
    l1_2->addWidget( _save );
    l1_2->addWidget( _abort );
}

VerifyDlg::~VerifyDlg()
{
}

void VerifyDlg::show()
{
    chdir( _workingDir );

    _proc = new KProcess();
    _proc->setExecutable( Options::instance()->getTarCommand() );
    if ( _restore ) {
        *_proc << "-xvf" << "-";
    } else {
        *_proc << "-dvf" << "-";
    }

    connect( _proc, SIGNAL( processExited( KProcess* ) ), this, SLOT( slotProcessExited( KProcess* ) ) );
    connect( _proc, SIGNAL( receivedStdout( KProcess*, char*, int ) ), this, SLOT( slotStdout( KProcess*, char*, int ) ) );
    connect( _proc, SIGNAL( wroteStdin( KProcess* ) ), this, SLOT( slotWroteStdin( KProcess* ) ) );

    _startTime = time( NULL );
    startTimer( 100 );

    _proc->start( KProcess::NotifyOnExit, KProcess::All );
    
    QDialog::show();
}

void VerifyDlg::slotProcessExited( KProcess* )
{
    killTimers();
    delete _proc;

    // Set this, or we get caught in a loop.
    _done = TRUE;

    _ok->setEnabled( TRUE );
    _save->setEnabled( TRUE );
    _abort->setEnabled( FALSE );
}

void VerifyDlg::slotStdout( KProcess*, char* buf, int len )
{
    QString data( len + 1 );
    data.replace( 0, len, buf );
    data[len] = '\0';
    _leftover += data;

    int newlineIndex;
    while ( ( newlineIndex = _leftover.find( '\n' ) ) > -1 ) {
        _log->append( _leftover.left( newlineIndex ) );

        // Count differences.
        if ( _restore ) {
            _fileCount++;
        } else {
            int len = _lastFileName.length();
            if ( len > 0 ) {
                if ( _lastFileName == _leftover.left( len ) ) {
                    if ( ( _leftover[len] == ':' ) && ( _leftover[len+1] == ' ' ) ) {
                        _fileCount++;
                    } else {
                        _lastFileName = _leftover.left( newlineIndex );
                    }
                } else {
                    _lastFileName = _leftover.left( newlineIndex );
                }
            } else {
                _lastFileName = _leftover.left( newlineIndex );
            }
        }
        
        _leftover.remove( 0, newlineIndex + 1 );
        QString tmp;
        tmp.setNum( _fileCount );
        _files->setText( tmp );
    }
}

void VerifyDlg::slotWroteStdin( KProcess* )
{
    _wroteStdin = TRUE;
}

void VerifyDlg::slotOK()
{
    if ( _aborted ) {
        reject();
    } else {
        accept();
    }
}

void VerifyDlg::slotAbort()
{
    _aborted = TRUE;
}

void VerifyDlg::timerEvent( QTimerEvent* )
{
    killTimers();

    int oldElapsed = 0;

    int bytesToRead;
    int count;
    char buf[Options::instance()->getTapeBlockSize()];
    QListIterator<Range> i( _ranges.getRanges() );
    for ( ; ( !_done ) && ( !_aborted ) && ( i.current() ); ++i ) {
        // Move to the beginning of the next range.
        TapeDrive::instance()->seek( _fileno, i.current()->getStart() );

        // Read in the records and forward them to tar.
        bytesToRead = ( i.current()->getEnd() - i.current()->getStart() ) * 512;
        while ( bytesToRead > 0 ) {
            count = TapeDrive::instance()->read( buf, bytesToRead > Options::instance()->getTapeBlockSize() ? Options::instance()->getTapeBlockSize() : bytesToRead );

            if ( count == 0 ) {
                // I hope this means end-of-file.  Break out of the while loop, and process the next range.
                break;
            }

            if ( count < 0 ) {
                printf( i18n( "failed while reading tape data.\n" ) );
                _proc->closeStdin();
                return;
            }

            while ( ( !_done ) && ( !_aborted ) && ( !_wroteStdin ) ) KApplication::getKApplication()->processEvents();
            if ( _done || _aborted ) {
                break;
            }
            _wroteStdin = FALSE;
            _proc->writeStdin( buf, count );
            bytesToRead -= count;
            _totalKBytes += (float)count / 1024.0;

            // Update stats.
            int elapsed = time( NULL ) - _startTime;
            if ( elapsed > oldElapsed )
            {
                updateStats();
                KApplication::getKApplication()->processEvents();
                if ( _done || _aborted ) {
                    break;
                }
                oldElapsed = elapsed;
            }
        }
    }

    // Update stats.
    updateStats();

    // Send an EOF block to tar.
    bzero( buf, Options::instance()->getTapeBlockSize() );
    _proc->writeStdin( buf, Options::instance()->getTapeBlockSize() );
    
    _proc->closeStdin();
}

void VerifyDlg::updateStats()
{
    QString str;
    int elapsed = time( NULL ) - _startTime;
                
    str.sprintf( i18n( "%02d:%02d:%02d" ), elapsed / 3600, elapsed / 60 % 60, elapsed % 60 );
    _elapsedTime->setText( str );
    
    int remain = 0;
    if ( (int)_totalKBytes > 0 ) {
        remain = (int)(( (float)_archiveSize - _totalKBytes ) * (float)elapsed / _totalKBytes);
    }
    if ( remain < 0 ) {
        remain = 0;
    }
    str.sprintf( i18n( "%02d:%02d:%02d" ), remain / 3600, remain / 60 % 60, remain % 60 );
    _timeRemaining->setText( str );
    
    str.sprintf( i18n( "%s" ), Util::kbytesToString( (int)_totalKBytes ).data() );
    _kbytesRead->setText( str );
    
    if ( elapsed > 0 ) {
        str.sprintf( i18n( "%s/min" ), Util::kbytesToString( (int)_totalKBytes * 60 / elapsed ).data() );
        _transferRate->setText( str );
    }
}
