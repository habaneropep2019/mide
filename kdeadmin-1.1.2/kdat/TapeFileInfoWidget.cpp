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

#include <time.h>

#include <qlabel.h>
#include <qlayout.h>

#include <kapp.h>

#include "File.h"
#include "TapeFileInfoWidget.h"
#include "Util.h"

#include "TapeFileInfoWidget.moc"

TapeFileInfoWidget::TapeFileInfoWidget( QWidget* parent, const char* name )
        : QWidget( parent, name )
{
    QLabel* lbl1 = new QLabel( i18n( "File name:" ), this );
    QLabel* lbl2 = new QLabel( i18n( "Last modified:" ), this );
    QLabel* lbl3 = new QLabel( i18n( "Size:" ), this );
    QLabel* lbl4 = new QLabel( i18n( "Start record:" ), this );
    QLabel* lbl5 = new QLabel( i18n( "End record:" ), this );

    int max = lbl1->sizeHint().width();
    if ( lbl2->sizeHint().width() > max ) max = lbl2->sizeHint().width();
    if ( lbl3->sizeHint().width() > max ) max = lbl3->sizeHint().width();
    if ( lbl4->sizeHint().width() > max ) max = lbl4->sizeHint().width();
    if ( lbl5->sizeHint().width() > max ) max = lbl5->sizeHint().width();

    lbl1->setFixedSize( max, lbl1->sizeHint().height() );
    lbl2->setFixedSize( max, lbl2->sizeHint().height() );
    lbl3->setFixedSize( max, lbl3->sizeHint().height() );
    lbl4->setFixedSize( max, lbl4->sizeHint().height() );
    lbl5->setFixedSize( max, lbl5->sizeHint().height() );

    _fileName = new QLabel( "???", this );
    _fileName->setFixedHeight( _fileName->sizeHint().height() );

    _mtime = new QLabel( "???", this );
    _mtime->setFixedHeight( _mtime->sizeHint().height() );

    _size = new QLabel( "???", this );
    _size->setFixedHeight( _size->sizeHint().height() );

    _startRecord = new QLabel( "???", this );
    _startRecord->setFixedHeight( _startRecord->sizeHint().height() );

    _endRecord = new QLabel( "???", this );
    _endRecord->setFixedHeight( _endRecord->sizeHint().height() );

    QVBoxLayout* l1 = new QVBoxLayout( this, 4, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _fileName, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addWidget( lbl2 );
    l1_2->addWidget( _mtime, 1 );

    QHBoxLayout* l1_3 = new QHBoxLayout();
    l1->addLayout( l1_3 );
    l1_3->addWidget( lbl3 );
    l1_3->addWidget( _size, 1 );

    QHBoxLayout* l1_4 = new QHBoxLayout();
    l1->addLayout( l1_4 );
    l1_4->addWidget( lbl4 );
    l1_4->addWidget( _startRecord, 1 );

    QHBoxLayout* l1_5 = new QHBoxLayout();
    l1->addLayout( l1_5 );
    l1_5->addWidget( lbl5 );
    l1_5->addWidget( _endRecord, 1 );

    l1->addStretch( 1 );
}

TapeFileInfoWidget::~TapeFileInfoWidget()
{
}

void TapeFileInfoWidget::setFile( File* file )
{
    _file = file;

    if ( !_file ) {
        return;
    }

    _fileName->setText( _file->getName() );

    QString tmp;
    time_t tm = _file->getMTime();
    tmp = ctime( &tm );
    tmp = tmp.stripWhiteSpace();
    _mtime->setText( tmp );

    _startRecord->setText( Util::bytesToString( _file->getStartRecord() ) );

    _endRecord->setText( Util::bytesToString( _file->getEndRecord() ) );

    if ( _file->isDirectory() ) {
        QListIterator<Range> it( _file->getRanges() );
        int records = 0;
        for ( ; it.current(); ++it ) {
            records += it.current()->getEnd() - it.current()->getStart();
        }
        _size->setText( Util::kbytesToString( records / 2 ) );
    } else {
        _size->setText( Util::bytesToString( _file->getSize() ) );
    }
}
