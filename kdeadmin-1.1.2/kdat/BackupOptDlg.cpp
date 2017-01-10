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

#include <qlayout.h>
#include <qpushbt.h>

#include <kapp.h>

#include "BackupOptDlg.h"
#include "BackupProfileWidget.h"

#include "BackupOptDlg.moc"

BackupOptDlg::BackupOptDlg( BackupProfile* backupProfile, QWidget* parent, const char* name )
        : QDialog( parent, name, TRUE )
{
    setIconText( i18n( "KDat: Backup Options" ) );
    setCaption( i18n( "KDat: Backup Options" ) );

    resize( 400, 300 );

    _profile = new BackupProfileWidget( this );
    _profile->setBackupProfile( backupProfile );

    QPushButton* ok = new QPushButton( i18n( "OK" ), this );
    ok->setFixedSize( 80, ok->sizeHint().height() );
    connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    QPushButton* cancel = new QPushButton( i18n( "Cancel" ), this );
    cancel->setFixedSize( 80, ok->sizeHint().height() );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    QVBoxLayout* l1 = new QVBoxLayout( this, 8, 4 );
    l1->addWidget( _profile, 1 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addStretch( 1 );
    l1_1->addWidget( ok );
    l1_1->addWidget( cancel );
}

BackupOptDlg::~BackupOptDlg()
{
}

const char* BackupOptDlg::getArchiveName()
{
    return _profile->getArchiveName();
}

const char* BackupOptDlg::getWorkingDirectory()
{
    return _profile->getWorkingDirectory();
}

const QStrList& BackupOptDlg::getRelativeFiles()
{
    return _profile->getRelativeFiles();
}

bool BackupOptDlg::isOneFilesystem()
{
    return _profile->isOneFilesystem();
}

bool BackupOptDlg::isIncremental()
{
    return _profile->isIncremental();
}

const char* BackupOptDlg::getSnapshotFile()
{
    return _profile->getSnapshotFile();
}

bool BackupOptDlg::getRemoveSnapshot()
{
    return _profile->getRemoveSnapshot();
}
