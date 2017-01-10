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

#include <unistd.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>

#include <kapp.h>
#include <kfiledialog.h>

#include "VerifyOptDlg.h"

#include "VerifyOptDlg.moc"

VerifyOptDlg::VerifyOptDlg( const char* def, const QStrList& files, bool restore, QWidget* parent, const char* name )
        : QDialog( parent, name, TRUE ),
          _restore( restore )
{
    if ( _restore ) {
        setIconText( i18n( "KDat: Restore Options" ) );
        setCaption( i18n( "KDat: Restore Options" ) );
    } else {
        setIconText( i18n( "KDat: Verify Options" ) );
        setCaption( i18n( "KDat: Verify Options" ) );
    }

    QLabel* lbl1;
    if ( _restore ) {
        lbl1 = new QLabel( i18n( "Restore to directory:" ), this );
    } else {
        lbl1 = new QLabel( i18n( "Verify in directory:" ), this );
    }
    lbl1->setFixedSize( lbl1->sizeHint() );

    _entry = new QLineEdit( this );
    _entry->setText( def );
    _entry->setFixedHeight( _entry->sizeHint().height() );

    QPushButton* browse = new QPushButton( i18n( "..." ), this );
    browse->setFixedSize( browse->sizeHint() );

    QLabel* lbl2;
    if ( _restore ) {
        lbl2 = new QLabel( i18n( "Restore files:" ), this );
    } else {
        lbl2 = new QLabel( i18n( "Verify files:" ), this );
    }
    lbl2->setFixedHeight( lbl2->sizeHint().height() );

    QListBox* fileList = new QListBox( this );
    QStrListIterator it( files );
    for ( ; it.current(); ++it ) {
        fileList->insertItem( it.current() );
    }

    QPushButton* ok     = new QPushButton( i18n( "OK" ), this );
    ok->setFixedSize( 80, ok->sizeHint().height() );
    QPushButton* cancel = new QPushButton( i18n( "Cancel" ), this );
    cancel->setFixedSize( 80, cancel->sizeHint().height() );

    QVBoxLayout* l1 = new QVBoxLayout( this, 8, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _entry, 1 );
    l1_1->addWidget( browse );
    
    l1->addWidget( lbl2 );
    l1->addWidget( fileList, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2, 0 );
    l1_2->addStretch( 1 );
    l1_2->addWidget( ok, 0 );
    l1_2->addWidget( cancel, 0 );

    resize( 400, 300 );

    _entry->setFocus();
    _entry->selectAll();

    connect( _entry, SIGNAL( returnPressed() ), this, SLOT( okClicked() ) );
    connect( browse, SIGNAL( clicked() )      , this, SLOT( slotBrowse() ) );
    connect( ok    , SIGNAL( clicked() )      , this, SLOT( okClicked() ) );
    connect( cancel, SIGNAL( clicked() )      , this, SLOT( reject() ) );
}

VerifyOptDlg::~VerifyOptDlg()
{
}

void VerifyOptDlg::okClicked()
{
    _workingDir = _entry->text();
    accept();
}

const char* VerifyOptDlg::getWorkingDirectory()
{
    return _workingDir;
}

void VerifyOptDlg::slotBrowse()
{
    QString tmp;
    tmp = KFileBaseDialog::getDirectory( _entry->text() );
    if ( tmp.length() ) {
        _entry->setText( tmp );
    }
}
