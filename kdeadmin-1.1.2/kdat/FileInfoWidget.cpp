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

#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <time.h>

#include <qlabel.h>
#include <qlayout.h>

#include <kapp.h>

#include "FileInfoWidget.h"
#include "Util.h"

#include "FileInfoWidget.moc"

FileInfoWidget::FileInfoWidget( QWidget* parent, const char* name )
        : QWidget( parent, name )
{
    QLabel* lbl1 = new QLabel( i18n( "File name:" ), this );
    QLabel* lbl2 = new QLabel( i18n( "Created on:" ), this );
    QLabel* lbl3 = new QLabel( i18n( "Last modified:" ), this );
    QLabel* lbl4 = new QLabel( i18n( "Last accessed:" ), this );
    QLabel* lbl5 = new QLabel( i18n( "Size:" ), this );
    QLabel* lbl6 = new QLabel( i18n( "Owner:" ), this );
    QLabel* lbl7 = new QLabel( i18n( "Group:" ), this );

    int max = lbl1->sizeHint().width();
    if ( lbl2->sizeHint().width() > max ) max = lbl2->sizeHint().width();
    if ( lbl3->sizeHint().width() > max ) max = lbl3->sizeHint().width();
    if ( lbl4->sizeHint().width() > max ) max = lbl4->sizeHint().width();
    if ( lbl5->sizeHint().width() > max ) max = lbl5->sizeHint().width();
    if ( lbl6->sizeHint().width() > max ) max = lbl6->sizeHint().width();
    if ( lbl7->sizeHint().width() > max ) max = lbl7->sizeHint().width();

    lbl1->setFixedSize( max, lbl1->sizeHint().height() );
    lbl2->setFixedSize( max, lbl2->sizeHint().height() );
    lbl3->setFixedSize( max, lbl3->sizeHint().height() );
    lbl4->setFixedSize( max, lbl4->sizeHint().height() );
    lbl5->setFixedSize( max, lbl5->sizeHint().height() );
    lbl6->setFixedSize( max, lbl6->sizeHint().height() );
    lbl7->setFixedSize( max, lbl7->sizeHint().height() );

    _fileName = new QLabel( "???", this );
    _fileName->setFixedHeight( _fileName->sizeHint().height() );
    
    _ctime = new QLabel( "???", this );
    _ctime->setFixedHeight( _ctime->sizeHint().height() );

    _mtime = new QLabel( "???", this );
    _mtime->setFixedHeight( _mtime->sizeHint().height() );
    
    _atime = new QLabel( "???", this );
    _atime->setFixedHeight( _atime->sizeHint().height() );

    _size = new QLabel( "???", this );
    _size->setFixedHeight( _size->sizeHint().height() );

    _owner = new QLabel( "???", this );
    _owner->setFixedHeight( _owner->sizeHint().height() );

    _group = new QLabel( "???", this );
    _group->setFixedHeight( _group->sizeHint().height() );

    QVBoxLayout* l1 = new QVBoxLayout( this, 4, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _fileName, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addWidget( lbl2 );
    l1_2->addWidget( _ctime, 1 );

    QHBoxLayout* l1_3 = new QHBoxLayout();
    l1->addLayout( l1_3 );
    l1_3->addWidget( lbl3 );
    l1_3->addWidget( _mtime, 1 );

    QHBoxLayout* l1_4 = new QHBoxLayout();
    l1->addLayout( l1_4 );
    l1_4->addWidget( lbl4 );
    l1_4->addWidget( _atime, 1 );

    QHBoxLayout* l1_5 = new QHBoxLayout();
    l1->addLayout( l1_5 );
    l1_5->addWidget( lbl5 );
    l1_5->addWidget( _size, 1 );

    QHBoxLayout* l1_6 = new QHBoxLayout();
    l1->addLayout( l1_6 );
    l1_6->addWidget( lbl6 );
    l1_6->addWidget( _owner, 1 );

    QHBoxLayout* l1_7 = new QHBoxLayout();
    l1->addLayout( l1_7 );
    l1_7->addWidget( lbl7 );
    l1_7->addWidget( _group, 1 );

    l1->addStretch( 1 );
}

FileInfoWidget::~FileInfoWidget()
{
}

void FileInfoWidget::setFile( const char* name )
{
    if ( !name ) {
        return;
    }

    _fileName->setText( name );

    struct stat info;
    lstat( name, &info );

    QString tmp;
    tmp = ctime( &info.st_ctime );
    tmp = tmp.stripWhiteSpace();
    _ctime->setText( tmp );

    tmp = ctime( &info.st_mtime );
    tmp = tmp.stripWhiteSpace();
    _mtime->setText( tmp );

    tmp = ctime( &info.st_atime );
    tmp = tmp.stripWhiteSpace();
    _atime->setText( tmp );
    
    _size->setText( Util::bytesToString( info.st_size ) );

    struct passwd* p;
    p = getpwuid( info.st_uid );
    if ( p ) {
        tmp = p->pw_name;
    } else {
        tmp.setNum( info.st_uid );
    }
    _owner->setText( tmp );

    struct group* g;
    g = getgrgid( info.st_gid );
    if ( g ) {
        tmp = g->gr_name;
    } else {
        tmp.setNum( info.st_gid );
    }
    _group->setText( tmp );
}
