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
#include <qlined.h>
#include <qpushbt.h>

#include <kapp.h>

#include "Archive.h"
#include "ArchiveInfoWidget.h"
#include "Options.h"
#include "Tape.h"
#include "Util.h"

#include "ArchiveInfoWidget.moc"

ArchiveInfoWidget::ArchiveInfoWidget( QWidget* parent, const char* name )
        : QWidget( parent, name ),
          _archive( 0 )
{
    QLabel* lbl1 = new QLabel( i18n( "Archive name:" ), this );
    QLabel* lbl2 = new QLabel( i18n( "Created on:" ), this );
    QLabel* lbl3 = new QLabel( i18n( "Size:" ), this );

    int max = lbl1->sizeHint().width();
    if ( lbl2->sizeHint().width() > max ) max = lbl2->sizeHint().width();
    if ( lbl3->sizeHint().width() > max ) max = lbl3->sizeHint().width();

    lbl1->setFixedSize( max, lbl1->sizeHint().height() );
    lbl2->setFixedSize( max, lbl2->sizeHint().height() );
    lbl3->setFixedSize( max, lbl3->sizeHint().height() );

    _archiveName = new QLineEdit( this );
    _archiveName->setFixedHeight( _archiveName->sizeHint().height() );

    _ctime = new QLabel( "???", this );
    _ctime->setFixedHeight( _ctime->sizeHint().height() );

    _size = new QLabel( "???", this );
    _size->setFixedHeight( _size->sizeHint().height() );

    _apply = new QPushButton( i18n( "Apply" ), this );
    _apply->setFixedSize( 80, _apply->sizeHint().height() );
    _apply->setEnabled( FALSE );

    QVBoxLayout* l1 = new QVBoxLayout( this, 4, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _archiveName, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addWidget( lbl2 );
    l1_2->addWidget( _ctime );

    QHBoxLayout* l1_3 = new QHBoxLayout();
    l1->addLayout( l1_3 );
    l1_3->addWidget( lbl3 );
    l1_3->addWidget( _size );

    l1->addStretch( 1 );

    QHBoxLayout* l1_4 = new QHBoxLayout();
    l1->addLayout( l1_4 );
    l1_4->addStretch( 1 );
    l1_4->addWidget( _apply );

    connect( _archiveName, SIGNAL( textChanged( const char* ) ), this, SLOT( slotTextChanged( const char* ) ) );
    connect( _apply      , SIGNAL( clicked() )                 , this, SLOT( slotApply() ) );
}

ArchiveInfoWidget::~ArchiveInfoWidget()
{
}

void ArchiveInfoWidget::setArchive( Archive* archive )
{
    _archive = archive;

    if ( !_archive ) {
        return;
    }

    _archiveName->setText( _archive->getName() );

    QString tmp;
    time_t tm = _archive->getCTime();
    tmp = ctime( &tm );
    tmp = tmp.stripWhiteSpace();
    _ctime->setText( tmp );

    int used = _archive->getEndBlock();
    int blockSize = Options::instance()->getTapeBlockSize();
    if ( blockSize < 1024 ) {
        used /= 1024 / blockSize;
    } else if ( blockSize > 1024 ) {
        used *= blockSize / 1024;
    }
    _size->setText( Util::kbytesToString( used ) );
}

void ArchiveInfoWidget::slotTextChanged( const char* text )
{
    if ( !_archive ) {
        return;
    }

    _apply->setEnabled( _archive->getName() != text );
}

void ArchiveInfoWidget::slotApply()
{
    if ( !_archive ) {
        return;
    }

    if ( _archive->getName() != _archiveName->text() ) {
        _archive->setName( _archiveName->text() );
    }

    _apply->setEnabled( FALSE );
}
