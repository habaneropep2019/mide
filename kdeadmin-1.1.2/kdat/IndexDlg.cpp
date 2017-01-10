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
#include <kmsgbox.h>

#include "LoggerWidget.h"
#include "Options.h"
#include "IndexDlg.h"
#include "Tape.h"
#include "TapeDrive.h"
#include "TapeManager.h"
#include "TarParser.h"
#include "Util.h"

#include "IndexDlg.moc"

IndexDlg::IndexDlg( Tape* tape, QWidget* parent, const char* name )
        : QDialog( parent, name, TRUE ),
          _tarParser( NULL ),
          _tape( tape ),
          _archive( NULL ),
          _totalKBytes( 0.0 ),
          _archiveCount( 0 ),
          _fileCount( 0 ),
          _totalFileCount( 0 ),
          _aborted( FALSE ),
          _numFiles( 0 ),
          _fileSize( -1 ),
          _fileMTime( -1 ),
          _fileStartRecord( -1 )
{
    setCaption( i18n( "KDat: Index" ) );
    setIconText( i18n( "KDat: Index" ) );

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

    QLabel* lbl2 = new QLabel( i18n( "Archives:" ), f2 );
    lbl2->setFixedSize( labelWidth, lbl2->sizeHint().height() );

    _archives = new QLabel( i18n( "0" ), f2 );
    _archives->setFixedHeight( _archives->sizeHint().height() );

    QLabel* lbl3 = new QLabel( i18n( "Kbytes read:" ), f1 );
    lbl3->setFixedSize( labelWidth, lbl3->sizeHint().height() );

    _kbytesRead = new QLabel( i18n( "0k" ), f1 );
    _kbytesRead->setFixedHeight( _kbytesRead->sizeHint().height() );

    QLabel* lbl4 = new QLabel( i18n( "Files:" ), f2 );
    lbl4->setFixedSize( labelWidth, lbl4->sizeHint().height() );

    _files = new QLabel( i18n( "0" ), f2 );
    _files->setFixedHeight( _kbytesRead->sizeHint().height() );

    QLabel* lbl5 = new QLabel( i18n( "Transfer rate:" ), f1 );
    lbl5->setFixedSize( labelWidth, lbl5->sizeHint().height() );

    _transferRate = new QLabel( i18n( "0k/min" ), f1 );
    _transferRate->setFixedHeight( _transferRate->sizeHint().height() );

    QLabel* lbl6 = new QLabel( i18n( "Total files:" ), f2 );
    lbl6->setFixedSize( labelWidth, lbl6->sizeHint().height() );

    _totalFiles = new QLabel( i18n( "0" ), f2 );
    _totalFiles->setFixedHeight( _totalFiles->sizeHint().height() );

    _log = new LoggerWidget( i18n( "Index log:" ), this );

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
    l1_1_1_2->addWidget( _kbytesRead, 1 );

    QHBoxLayout* l1_1_1_3 = new QHBoxLayout();
    l1_1_1->addLayout( l1_1_1_3 );
    l1_1_1_3->addWidget( lbl5 );
    l1_1_1_3->addWidget( _transferRate, 1 );

    QVBoxLayout* l1_1_2 = new QVBoxLayout( f2, 4, 4 );

    QHBoxLayout* l1_1_2_1 = new QHBoxLayout();
    l1_1_2->addLayout( l1_1_2_1 );
    l1_1_2_1->addWidget( lbl2 );
    l1_1_2_1->addWidget( _archives, 1 );

    QHBoxLayout* l1_1_2_2 = new QHBoxLayout();
    l1_1_2->addLayout( l1_1_2_2 );
    l1_1_2_2->addWidget( lbl4 );
    l1_1_2_2->addWidget( _files, 1 );

    QHBoxLayout* l1_1_2_3 = new QHBoxLayout();
    l1_1_2->addLayout( l1_1_2_3 );
    l1_1_2_3->addWidget( lbl6 );
    l1_1_2_3->addWidget( _totalFiles, 1 );

    l1->addWidget( _log, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addStretch( 1 );
    l1_2->addWidget( _ok );
    l1_2->addWidget( _save );
    l1_2->addWidget( _abort );
}

IndexDlg::~IndexDlg()
{
    delete _tarParser;
}

void IndexDlg::show()
{
    startTimer( 100 );
    
    QDialog::show();
}

void IndexDlg::slotEntry( const char* name, int size, int mtime, int record )
{
    if ( _fileName.length() > 0 ) {
        _archive->addFile( _fileSize, _fileMTime, _fileStartRecord, record, _fileName );
    }

    _fileName        = name;
    _fileSize        = size;
    _fileMTime       = mtime;
    _fileStartRecord = record;

    QString tmp;
    tmp.setNum( ++_numFiles );
    _files->setText( tmp );
    _totalFileCount++;
    tmp.setNum( _totalFileCount );
    _totalFiles->setText( tmp );
    
    _log->append( name );
}

void IndexDlg::slotOK()
{
    if ( _aborted ) {
        reject();
    } else {
        accept();
    }
}

void IndexDlg::slotAbort()
{
    killTimers();
    _aborted = TRUE;
}

void IndexDlg::timerEvent( QTimerEvent* )
{
    killTimers();

    // Rewind tape.
    _log->append( i18n( "Rewinding tape." ) );
    if ( !TapeDrive::instance()->rewind() ) {
        KMsgBox::message( this, i18n( "KDat: Cannot Rewind Tape" ), i18n( "Cannot rewind tape.  Indexing aborted." ), KMsgBox::EXCLAMATION );
        _ok->setEnabled( TRUE );
        _save->setEnabled( TRUE );
        _abort->setEnabled( FALSE );
        _log->append( i18n( "Cannot rewind tape." ) );
        return;
    }

    // Skip tape ID.
    if ( !TapeDrive::instance()->nextFile( 1 ) ) {
        KMsgBox::message( this, i18n( "KDat: Error" ), i18n( "Failed to skip tape ID." ), KMsgBox::EXCLAMATION );
        _ok->setEnabled( TRUE );
        _save->setEnabled( TRUE );
        _abort->setEnabled( FALSE );
        _log->append( i18n( "Failed to skip tape ID." ) );
        return;
    }

    _startTime = time( NULL );

    int oldElapsed = 0;
    QString msg;
    int count;
    char buf[Options::instance()->getTapeBlockSize()];
    bool atLeastOneGoodRead = TRUE;
    while ( ( !_aborted ) && ( atLeastOneGoodRead ) ) {
        atLeastOneGoodRead = FALSE;
        _archive = NULL;
        
        delete _tarParser;
        _tarParser = new TarParser();
        connect( _tarParser, SIGNAL( sigEntry( const char*, int, int, int ) ), this, SLOT( slotEntry( const char*, int, int, int ) ) );

        int endBlock = 0;
        int leftover = 0;

        while ( ( count = TapeDrive::instance()->read( buf, Options::instance()->getTapeBlockSize() ) ) > 0 ) {
            // Is this the first block of the archive?
            if ( !atLeastOneGoodRead ) {
                atLeastOneGoodRead = TRUE;
                _archiveCount++;
                msg.setNum( _archiveCount );
                _archives->setText( msg );
                msg.sprintf( i18n( "Indexing archive %d." ), _archiveCount );
                _log->append( msg );

                msg.sprintf( i18n( "Archive %d" ), _archiveCount );
                _archive = new Archive( _tape, 0, msg );
                _fileName = 0;
                _fileSize = -1;
                _fileMTime = -1;
                _fileStartRecord = -1;
                _numFiles = 0;
            }

            leftover += count;
            endBlock += leftover / Options::instance()->getTapeBlockSize();
            leftover %= Options::instance()->getTapeBlockSize();

            _tarParser->slotData( buf, count );
            _totalKBytes += (float)count / 1024.0;

            // Update stats.
            int elapsed = time( NULL ) - _startTime;
            if ( elapsed > oldElapsed )
            {
                updateStats();
                KApplication::getKApplication()->processEvents();
                if ( _aborted ) {
                    break;
                }
                oldElapsed = elapsed;
            }
        }
        if ( _aborted ) {
            break;
        }
        if ( _archive ) {
            _archive->setEndBlock( endBlock );

            if ( _fileName.length() > 0 ) {
                _archive->addFile( _fileSize, _fileMTime, _fileStartRecord, _archive->getEndBlock() * ( Options::instance()->getTapeBlockSize() / 512 ), _fileName );
            }

            _tape->addChild( _archive );
        }
    }

    updateStats();

    TapeDrive::instance()->rewind();

    if ( !_aborted ) {
        _tape->setName( i18n( "Reindexed Tape" ) );
        _tape->setSize( Options::instance()->getDefaultTapeSize() );
    }

    _ok->setEnabled( TRUE );
    _save->setEnabled( TRUE );
    _abort->setEnabled( FALSE );
}

void IndexDlg::updateStats()
{
    QString str;
    int elapsed = time( NULL ) - _startTime;
                
    str.sprintf( i18n( "%02d:%02d:%02d" ), elapsed / 3600, elapsed / 60 % 60, elapsed % 60 );
    _elapsedTime->setText( str );
    
    str.sprintf( i18n( "%s" ), Util::kbytesToString( (int)_totalKBytes ).data() );
    _kbytesRead->setText( str );
    
    if ( elapsed > 0 ) {
        str.sprintf( i18n( "%s/min" ), Util::kbytesToString( (int)_totalKBytes * 60 / elapsed ).data() );
        _transferRate->setText( str );
    }
}
