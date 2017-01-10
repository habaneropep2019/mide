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

#include <qchkbox.h>
#include <qcombo.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>

#include <kapp.h>

#include "BackupProfile.h"
#include "BackupProfileWidget.h"
#include "Util.h"

#include "BackupProfileWidget.moc"

BackupProfileWidget::BackupProfileWidget( QWidget* parent, const char* name )
        : KTabCtl( parent, name )
{
    QWidget* one = new QWidget( this );
    addTab( one, i18n( "Backup" ) );

    QLabel* lbl1 = new QLabel( i18n( "Archive name:" ), one );
    lbl1->setFixedSize( lbl1->sizeHint() );

    _archiveName = new QLineEdit( one );
    _archiveName->setFixedHeight( _archiveName->sizeHint().height() );

    QLabel* lbl2 = new QLabel( i18n( "Working directory:" ), one );
    lbl2->setFixedSize( lbl2->sizeHint() );

    _workingDir = new QComboBox( FALSE, one );
    _workingDir->setFixedHeight( _workingDir->sizeHint().height() );

    QLabel* lbl3 = new QLabel( i18n( "Backup files:" ), one );
    lbl3->setFixedHeight( lbl3->sizeHint().height() );

    _files = new QListBox( one );

    QWidget* two = new QWidget( this );
    addTab( two, i18n( "Tar Options" ) );

    _oneFilesystem = new QCheckBox( i18n( "Stay on one filesystem" ), two );
    _oneFilesystem->setFixedHeight( _oneFilesystem->sizeHint().height() );

    _incremental = new QCheckBox( i18n( "GNU listed incremental" ), two );
    _incremental->setFixedHeight( _incremental->sizeHint().height() );
    connect( _incremental, SIGNAL( toggled( bool ) ), this, SLOT( slotIncrementalToggled( bool ) ) );

    _snapshotLabel = new QLabel( i18n( "Snapshot file:" ), two );
    _snapshotLabel->setFixedSize( _snapshotLabel->sizeHint() );

    _snapshotFile = new QLineEdit( two );
    _snapshotFile->setFixedHeight( _snapshotFile->sizeHint().height() );

    _removeSnapshot = new QCheckBox( i18n( "Remove snapshot file before backup" ), two );
    _removeSnapshot->setFixedHeight( _removeSnapshot->sizeHint().height() );

    slotIncrementalToggled( FALSE );

    QVBoxLayout* l1 = new QVBoxLayout( one, 8, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _archiveName, 1 );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addWidget( lbl2 );
    l1_2->addWidget( _workingDir, 1 );

    l1->addWidget( lbl3 );
    l1->addWidget( _files, 1 );

    QVBoxLayout* l2 = new QVBoxLayout( two, 8, 4 );
    l2->addWidget( _oneFilesystem );
    l2->addWidget( _incremental );

    QHBoxLayout* l2_1 = new QHBoxLayout();
    l2->addLayout( l2_1 );
    l2_1->addSpacing( 20 );
    l2_1->addWidget( _snapshotLabel );
    l2_1->addWidget( _snapshotFile, 1 );

    QHBoxLayout* l2_2 = new QHBoxLayout();
    l2->addLayout( l2_2 );
    l2_2->addSpacing( 20 );
    l2_2->addWidget( _removeSnapshot );

    l2->addStretch( 1 );

    connect( _archiveName   , SIGNAL( textChanged( const char* ) ), this, SLOT( slotTextChanged( const char* ) ) );
    connect( _workingDir    , SIGNAL( activated( const char* ) )  , this, SLOT( slotWorkingDirActivated( const char* ) ) );
    connect( _oneFilesystem , SIGNAL( toggled( bool ) )           , this, SLOT( slotToggled( bool ) ) );
    connect( _incremental   , SIGNAL( toggled( bool ) )           , this, SLOT( slotIncrementalToggled( bool ) ) );
    connect( _snapshotFile  , SIGNAL( textChanged( const char* ) ), this, SLOT( slotTextChanged( const char* ) ) );
    connect( _removeSnapshot, SIGNAL( toggled( bool ) )           , this, SLOT( slotToggled( bool ) ) );
}

BackupProfileWidget::~BackupProfileWidget()
{
}

void BackupProfileWidget::slotTextChanged( const char* )
{
    emit sigSomethingChanged();
}

void BackupProfileWidget::slotToggled( bool )
{
    emit sigSomethingChanged();
}

void BackupProfileWidget::slotIncrementalToggled( bool set )
{
    _snapshotLabel->setEnabled( set );
    _snapshotFile->setEnabled( set );
    _removeSnapshot->setEnabled( set );

    emit sigSomethingChanged();
}

void BackupProfileWidget::slotWorkingDirActivated( const char* text )
{
    while ( _relativeFiles.first() ) {
        _relativeFiles.removeFirst();
    }
    _files->clear();

    QStrListIterator i( _absoluteFiles );
    int remove = strlen( text );
    if ( remove > 1 ) {
        remove++;
    }
    for ( ; i.current(); ++i ) {
        QString fn = i.current();
        fn.remove( 0, remove );
        if ( fn.length() == 0 ) {
            fn = ".";
        }
        _files->insertItem( fn );
        _relativeFiles.append( fn );
    }

    emit sigSomethingChanged();
}

void BackupProfileWidget::setBackupProfile( BackupProfile* backupProfile )
{
    // Set the archive name.
    _archiveName->setText( backupProfile->getArchiveName() );

    setAbsoluteFiles( backupProfile->getAbsoluteFiles() );

    if ( backupProfile->getWorkingDirectory() ) {
        for ( int ii = 0; ii < _workingDir->count(); ii++ ) {
            if ( !strcmp( _workingDir->text( ii ), backupProfile->getWorkingDirectory() ) ) {
                _workingDir->setCurrentItem( ii );
                break;
            }
        }
    }
    slotWorkingDirActivated( _workingDir->currentText() );

    _oneFilesystem->setChecked( backupProfile->isOneFilesystem() );
    _incremental->setChecked( backupProfile->isIncremental() );
    _snapshotFile->setText( backupProfile->getSnapshotFile() );
    _removeSnapshot->setChecked( backupProfile->getRemoveSnapshot() );

    slotIncrementalToggled( backupProfile->isIncremental() );
}

void BackupProfileWidget::setAbsoluteFiles( const QStrList& files )
{
    // Copy the string list.
    _absoluteFiles.clear();
    QStrListIterator i( files );
    for ( ; i.current(); ++i ) {
        _absoluteFiles.append( i.current() );
    }

    QString prefix = Util::longestCommonPath( files );

    _workingDir->clear();
    for ( int pos = prefix.length(); pos > 0; pos = prefix.findRev( '/', pos - 1 ) ) {
        _workingDir->insertItem( prefix.left( pos ) );
    }
    _workingDir->insertItem( "/" );
    _workingDir->setCurrentItem( 0 );

    slotWorkingDirActivated( _workingDir->currentText() );
}

const char* BackupProfileWidget::getArchiveName()
{
    return _archiveName->text();
}

const char* BackupProfileWidget::getWorkingDirectory()
{
    if ( _workingDir->count() > 0 ) {
        return _workingDir->currentText();
    } else {
        return 0;
    }
}

const QStrList& BackupProfileWidget::getRelativeFiles()
{
    return _relativeFiles;
}

const QStrList& BackupProfileWidget::getAbsoluteFiles()
{
    return _absoluteFiles;
}

bool BackupProfileWidget::isOneFilesystem()
{
    return _oneFilesystem->isChecked();
}

bool BackupProfileWidget::isIncremental()
{
    return _incremental->isChecked();
}

const char* BackupProfileWidget::getSnapshotFile()
{
    return _snapshotFile->text();
}

bool BackupProfileWidget::getRemoveSnapshot()
{
    return _removeSnapshot->isChecked();
}
