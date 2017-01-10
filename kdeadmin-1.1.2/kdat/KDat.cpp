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

#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

#include <qdatetm.h>
#include <qdir.h>
#include <qfileinf.h>
#include <qkeycode.h>
#include <qlayout.h>
#include <qmsgbox.h>

#include <kapp.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kmsgbox.h>
#include <kpanner.h>
#include <ktoolbar.h>

#include "ArchiveInfoWidget.h"
#include "BackupDlg.h"
#include "BackupOptDlg.h"
#include "BackupProfile.h"
#include "BackupProfileInfoWidget.h"
#include "BackupProfileManager.h"
#include "FileInfoWidget.h"
#include "FormatOptDlg.h"
#include "ImageCache.h"
#include "IndexDlg.h"
#include "InfoShellWidget.h"
#include "KDat.h"
#include "Node.h"
#include "Options.h"
#include "OptionsDlg.h"
#include "Range.h"
#include "Tape.h"
#include "TapeDrive.h"
#include "TapeFileInfoWidget.h"
#include "TapeInfoWidget.h"
#include "TapeManager.h"
#include "Util.h"
#include "VerifyDlg.h"
#include "VerifyOptDlg.h"
#include "kdat.h"
#include "ktreeview.h"

#include "KDat.moc"

KDat* KDat::_instance = 0;

KDat* KDat::instance()
{
    if ( _instance == 0 ) {
        _instance = new KDat();
    }
    return _instance;
}

KDat::KDat()
        : _destroyed( FALSE )
{
    resize( 600, 400 );
    
    setIconText( i18n( "KDat: <no tape>" ) );
    setCaption( i18n( "KDat: <no tape>" ) );

    // Create object popup menus.
    _tapeDriveMenu = new QPopupMenu();
    _tapeDriveMenu->insertItem( i18n( "Mount Tape" )         , this, SLOT( fileMountTape() ) );
    _tapeDriveMenu->insertItem( i18n( "Recreate Tape Index" ), this, SLOT( fileIndexTape() ) );
    _tapeDriveMenu->insertSeparator();
    _tapeDriveMenu->insertItem( i18n( "Format Tape..." ), this, SLOT( fileFormatTape() ) );

    _archiveMenu = new QPopupMenu();
    _archiveMenu->insertItem( i18n( "Delete Archive" ), this, SLOT( fileDeleteArchive() ) );

    _mountedArchiveMenu = new QPopupMenu();
    _mountedArchiveMenu->insertItem( i18n( "Verify..." )     , this, SLOT( fileVerify() ) );
    _mountedArchiveMenu->insertItem( i18n( "Restore..." )    , this, SLOT( fileRestore() ) );
    _mountedArchiveMenu->insertSeparator();
    _mountedArchiveMenu->insertItem( i18n( "Delete Archive" ), this, SLOT( fileDeleteArchive() ) );

    _mountedTapeFileMenu = new QPopupMenu();
    _mountedTapeFileMenu->insertItem( i18n( "Verify..." ) , this, SLOT( fileVerify() ) );
    _mountedTapeFileMenu->insertItem( i18n( "Restore..." ), this, SLOT( fileRestore() ) );

    _localFileMenu = new QPopupMenu();
    _localFileMenu->insertItem( i18n( "Backup..." ), this, SLOT( fileBackup() ) );

    _tapeMenu = new QPopupMenu();
    _tapeMenu->insertItem( i18n( "Delete Tape Index" ), this, SLOT( fileDeleteIndex() ) );
    
    _backupProfileRootMenu = new QPopupMenu();
    _backupProfileRootMenu->insertItem( i18n( "Create Backup Profile" ), this, SLOT( fileNewBackupProfile() ) );

    _backupProfileMenu = new QPopupMenu();
    _backupProfileMenu->insertItem( i18n( "Backup..." ), this, SLOT( fileBackup() ) );
    _backupProfileMenu->insertSeparator();
    _backupProfileMenu->insertItem( i18n( "Delete Backup Profile" ), this, SLOT( fileDeleteBackupProfile() ) );
    
    _fileMenu = new QPopupMenu;
    _fileMenu->insertItem( i18n( "Backup..." )            , this, SLOT( fileBackup() ) );
    _fileMenu->insertItem( i18n( "Restore..." )           , this, SLOT( fileRestore() ) );
    _fileMenu->insertItem( i18n( "Verify..." )            , this, SLOT( fileVerify() ) );
    _fileMenu->insertItem( i18n( "Mount Tape" )           , this, SLOT( fileMountTape() ) );
    _fileMenu->insertItem( i18n( "Recreate Tape Index" )  , this, SLOT( fileIndexTape() ) );
    _fileMenu->insertItem( i18n( "Create Backup Profile" ), this, SLOT( fileNewBackupProfile() ) );
    _fileMenu->insertSeparator();
    _fileMenu->insertItem( i18n( "Delete Archive" )       , this, SLOT( fileDeleteArchive() ) );
    _fileMenu->insertItem( i18n( "Delete Index" )         , this, SLOT( fileDeleteIndex() ) );
    _fileMenu->insertItem( i18n( "Delete Backup Profile" ), this, SLOT( fileDeleteBackupProfile() ) );
    _fileMenu->insertItem( i18n( "Format Tape..." )       , this, SLOT( fileFormatTape() ) );
    _fileMenu->insertSeparator();
    _fileMenu->insertItem( i18n( "&Quit" )                , this, SLOT( fileQuit() ), CTRL + Key_Q );

    _editMenu = new QPopupMenu;
    _editMenu->insertItem( i18n( "Preferences..." )       , this, SLOT( editPreferences() ) );

    _menu = new KMenuBar( this );
    _menu->insertItem( i18n( "&File" ), _fileMenu );
    _menu->insertItem( i18n( "&Edit" ), _editMenu );
    _menu->insertSeparator();
    QString about;
    about.sprintf( i18n( "KDat Version %s\n\nKDat is a tar-based tape archiver.\n\nCopyright (c) 1998 Sean Vyain\nsvyain@mail.tds.net" ), KDAT_VERSION );
    _menu->insertItem( i18n( "&Help" ), KApplication::getKApplication()->getHelpMenu( TRUE, about.data() ) );

    setMenu( _menu );
    
    _toolbar = new KToolBar( this );

    _toolbar->insertButton( *ImageCache::instance()->getTapeUnmounted(), 0, SIGNAL( clicked( int ) ), this, SLOT( fileMountTape() ), TRUE, i18n( "Mount/unmount tape" ) );

    _toolbar->insertSeparator();

    _toolbar->insertButton( *ImageCache::instance()->getBackup() , 1, SIGNAL( clicked( int ) ), this, SLOT( fileBackup() ) , TRUE, i18n( "Backup" ) );
    _toolbar->insertButton( *ImageCache::instance()->getRestore(), 2, SIGNAL( clicked( int ) ), this, SLOT( fileRestore() ), TRUE, i18n( "Restore" ) );
    _toolbar->insertButton( *ImageCache::instance()->getVerify() , 3, SIGNAL( clicked( int ) ), this, SLOT( fileVerify() ) , TRUE, i18n( "Verify" ) );

    _toolbar->insertSeparator();

    KIconLoader* iconLoader = KApplication::getKApplication()->getIconLoader();
    _toolbar->insertButton( iconLoader->loadIcon( "help.xpm" ), 4, SIGNAL( clicked( int ) ), this, SLOT( help() )    , TRUE, i18n( "Help" ) );

    _toolbar->insertSeparator();

    _toolbar->insertButton( iconLoader->loadIcon( "exit.xpm" ), 5, SIGNAL( clicked( int ) ), this, SLOT( fileQuit() ), TRUE, i18n( "Quit" ) );

    addToolBar( _toolbar );

    _statusBar = new KStatusBar( this );
    _statusBar->insertItem( i18n( "Ready." ), 0 );
    setStatusBar( _statusBar );

    _panner = new KPanner( this, "panner", KPanner::O_VERTICAL | KPanner::U_PERCENT, 50 );

    // Create info viewers.
    InfoShellWidget* infoShell = new InfoShellWidget( _panner->child1() );
    QVBoxLayout* infoLayout = new QVBoxLayout( _panner->child1(), 4, 4 );
    infoLayout->addWidget( infoShell );
    _tapeInfo = new TapeInfoWidget( infoShell );
    _archiveInfo = new ArchiveInfoWidget( infoShell );
    _backupProfileInfo = new BackupProfileInfoWidget( infoShell );
    _tapeFileInfo = new TapeFileInfoWidget( infoShell );
    _fileInfo = new FileInfoWidget( infoShell );

    _tree = new KTreeView( _panner->child0() );

    _tree->setExpandButtonDrawing( TRUE );

    QVBoxLayout* localLayout = new QVBoxLayout( _panner->child0(), 4, 4 );

    localLayout->addWidget( _tree, 1 );

    connect( _tree, SIGNAL( expanding( KTreeViewItem*, bool& ) ), this, SLOT( localExpanding( KTreeViewItem*, bool& ) ) );
    connect( _tree, SIGNAL( expanded( int ) ), this, SLOT( localExpanded( int ) ) );
    connect( _tree, SIGNAL( collapsed( int ) ), this, SLOT( localCollapsed( int ) ) );
    connect( _tree, SIGNAL( selected( int ) ), this, SLOT( localSelected( int ) ) );
    connect( _tree, SIGNAL( highlighted( int ) ), this, SLOT( localHighlighted( int ) ) );
    connect( _tree, SIGNAL( popupMenu( int, const QPoint& ) ), this, SLOT( localPopupMenu( int, const QPoint& ) ) );

    setView( _panner );

    _tree->insertItem( _tapeDriveNode = new TapeDriveNode() );
    _tree->insertItem( _rootNode = new RootNode() );
    _tree->insertItem( _backupProfileRootNode = new BackupProfileRootNode() );
    _tree->insertItem( new TapeIndexRootNode() );

    connect( TapeDrive::instance(), SIGNAL( sigStatus( const char* ) ), this, SLOT( status( const char* ) ) );

    setTapePresent( FALSE );

    connect( Options::instance(), SIGNAL( sigTapeDevice() ), this, SLOT( slotTapeDevice() ) );

    connect( TapeManager::instance(), SIGNAL( sigTapeMounted() )  , this, SLOT( slotTapeMounted() ) );
    connect( TapeManager::instance(), SIGNAL( sigTapeUnmounted() ), this, SLOT( slotTapeUnmounted() ) );

    configureUI( 0 );
}

KDat::~KDat()
{
    _destroyed = TRUE;

    if ( Options::instance()->getLockOnMount() ) {
        TapeDrive::instance()->unlock();
    }
}

void KDat::popupTapeDriveMenu( const QPoint& p )
{
    // Configure menu before popping up.
    if ( TapeManager::instance()->getMountedTape() ) {
        _tapeDriveMenu->changeItem( i18n( "Unmount Tape" ), _tapeDriveMenu->idAt( 0 ) );
        _tapeDriveMenu->setItemEnabled( _tapeDriveMenu->idAt( 1 ), TRUE );
    } else {
        _tapeDriveMenu->changeItem( i18n( "Mount Tape" ), _tapeDriveMenu->idAt( 0 ) );
        _tapeDriveMenu->setItemEnabled( _tapeDriveMenu->idAt( 1 ), FALSE );
    }

    _tapeDriveMenu->popup( p );
}

void KDat::popupArchiveMenu( const QPoint& p )
{
    _archiveMenu->popup( p );
}

void KDat::popupMountedArchiveMenu( const QPoint& p )
{
    _mountedArchiveMenu->popup( p );
}

void KDat::popupMountedTapeFileMenu( const QPoint& p )
{
    _mountedTapeFileMenu->popup( p );
}

void KDat::popupLocalFileMenu( const QPoint& p )
{
    // Configure menu before popping up.
    _localFileMenu->setItemEnabled( _localFileMenu->idAt( 0 ), ( TapeManager::instance()->getMountedTape() != 0 ) && ( !TapeDrive::instance()->isReadOnly() ) );

    _localFileMenu->popup( p );
}

void KDat::popupTapeMenu( const QPoint& p )
{
    _tapeMenu->popup( p );
}

void KDat::popupBackupProfileRootMenu( const QPoint& p )
{
    _backupProfileRootMenu->popup( p );
}

void KDat::popupBackupProfileMenu( const QPoint& p )
{
    // Configure menu before popping up.
    _backupProfileMenu->setItemEnabled( _backupProfileMenu->idAt( 0 ), ( TapeManager::instance()->getMountedTape() != 0 ) && ( !TapeDrive::instance()->isReadOnly() ) );

    _backupProfileMenu->popup( p );
}

void KDat::hideInfo()
{
    _archiveInfo->hide();
    _backupProfileInfo->hide();
    _tapeInfo->hide();
    _tapeFileInfo->hide();
    _fileInfo->hide();
}

void KDat::showTapeInfo( Tape* tape )
{
    assert( tape );

    hideInfo();

    _tapeInfo->setTape( tape );
    _tapeInfo->show();
}

void KDat::showArchiveInfo( Archive* archive )
{
    assert( archive );

    hideInfo();

    _archiveInfo->setArchive( archive );
    _archiveInfo->show();
}

void KDat::showBackupProfileInfo( BackupProfile* backupProfile )
{
    assert( backupProfile );

    hideInfo();

    _backupProfileInfo->setBackupProfile( backupProfile );
    _backupProfileInfo->show();
}

void KDat::showTapeFileInfo( File* file )
{
    assert( file );

    hideInfo();

    _tapeFileInfo->setFile( file );
    _tapeFileInfo->show();
}

void KDat::showFileInfo( const char* name )
{
    assert( name );

    hideInfo();

    _fileInfo->setFile( name );
    _fileInfo->show();
}

void KDat::localExpanding( KTreeViewItem* item, bool& allow )
{
    ((Node*)item)->expanding( allow );
}

void KDat::localExpanded( int index )
{
    ((Node*)_tree->itemAt( index ))->expanded();
}

void KDat::localCollapsed( int index )
{
    ((Node*)_tree->itemAt( index ))->collapsed();
}

void KDat::localSelected( int index )
{
    Node* item = (Node*)_tree->itemAt( index );
    if ( item->isExpanded() ) {
        _tree->collapseItem( index );
    } else {
        _tree->expandItem( index );
    }
}

void KDat::localHighlighted( int index )
{
    Node* item = (Node*)_tree->itemAt( index );

    if ( item ) {
        item->selected();
    }

    configureUI( TapeManager::instance()->getMountedTape() );
}

void KDat::localPopupMenu( int index, const QPoint& p )
{
    Node* item = (Node*)_tree->itemAt( index );
    item->popupMenu( p );
}

void KDat::fileBackup()
{
    BackupProfile backupProfile;

    if ( ( _tree->getCurrentItem() ) && ( ((Node*)_tree->getCurrentItem())->isType( Node::BackupProfileNodeType ) ) ) {
        BackupProfile* bp = ((BackupProfileNode*)_tree->getCurrentItem())->getBackupProfile();
        backupProfile.setArchiveName( bp->getArchiveName() );
        backupProfile.setWorkingDirectory( bp->getWorkingDirectory() );
        backupProfile.setAbsoluteFiles( bp->getAbsoluteFiles() );
        backupProfile.setOneFilesystem( bp->isOneFilesystem() );
        backupProfile.setIncremental( bp->isIncremental() );
        backupProfile.setSnapshotFile( bp->getSnapshotFile() );
        backupProfile.setRemoveSnapshot( bp->getRemoveSnapshot() );
    } else {
        QString name;
        time_t tm = time( NULL );
        char date[80];
        strftime( date, 80, i18n( "%m/%d/%y" ), localtime( &tm ) );
        name.sprintf( i18n( "Archive created on %s" ), date );
        name = name.stripWhiteSpace();
        
        QStrList files;
        getBackupFiles( files );

        backupProfile.setArchiveName( name );
        backupProfile.setAbsoluteFiles( files );
        backupProfile.setOneFilesystem( TRUE );
        backupProfile.setIncremental( FALSE );
        backupProfile.setSnapshotFile( "snapshot" );
        backupProfile.setRemoveSnapshot( FALSE );
    }

    int ret = 0;
    BackupOptDlg dlg( &backupProfile, this );
    if ( dlg.exec() == QDialog::Accepted ) {
        // Begin backup.
        status( i18n( "Performing backup..." ) );
        int size = calcBackupSize( dlg.getWorkingDirectory(), dlg.isOneFilesystem(), dlg.getRelativeFiles(), dlg.isIncremental(), dlg.getSnapshotFile(), dlg.getRemoveSnapshot() );

        // Make sure the archive will fit on the tape.
        int tapeSize = 1;
        QListIterator<Archive> i( TapeManager::instance()->getMountedTape()->getChildren() );
        if ( i.toLast() != NULL ) {
            tapeSize = i.current()->getEndBlock();
            tapeSize = (int)((float)tapeSize / 1024.0 * (float)Options::instance()->getTapeBlockSize()) + 1;
        }
        if ( tapeSize + size >= TapeManager::instance()->getMountedTape()->getSize() ) {
            // Warn user that tape is probably too short.
            QString msg;
            msg.sprintf( i18n( "WARNING: The estimated archive size is %dKB.\nThe tape has only %dKB of space!\n\nBackup anyway?" ), size, TapeManager::instance()->getMountedTape()->getSize() - tapeSize );
            if ( KMsgBox::yesNo( this, i18n( "KDat: Backup" ), msg, KMsgBox::EXCLAMATION ) != 1 ) {
                status( i18n( "Backup canceled." ) );
                return;
            }
        }
        
        if ( TapeDrive::instance()->getFile() < 0 ) {
            // Rewind tape.
            status( i18n( "Rewinding tape..." ) );
            if ( !TapeDrive::instance()->rewind() ) {
                KMsgBox::message( this, i18n( "KDat: Cannot Rewind Tape" ), i18n( "Cannot rewind tape.  Backup aborted." ), KMsgBox::EXCLAMATION );
                status( i18n( "Backup aborted." ) );
                return;
            }
        }
        
        // Go to end of tape.
        status( i18n( "Skipping to end of tape..." ) );
        if ( !TapeDrive::instance()->nextFile( TapeManager::instance()->getMountedTape()->getChildren().count() + 1 - TapeDrive::instance()->getFile() ) ) {
            KMsgBox::message( this, i18n( "KDat: Error" ), i18n( "Cannot get to end of tape." ), KMsgBox::EXCLAMATION );
            status( i18n( "Backup aborted." ) );
            return;
        }
        
        status( i18n( "Backup in progress..." ) );
        BackupDlg backupDlg( dlg.getArchiveName(), dlg.getWorkingDirectory(), dlg.getRelativeFiles(), dlg.isOneFilesystem(), dlg.isIncremental(), dlg.getSnapshotFile(), dlg.getRemoveSnapshot(), size, TapeManager::instance()->getMountedTape(), this );
        ret = backupDlg.exec();
        
//        status( i18n( "Rewinding tape..." ) );
//        TapeDrive::instance()->rewind();
//        if ( !TapeDrive::instance()->prevFile( 1 ) ) {
//            status( i18n( "Rewinding tape..." ) );
//            TapeDrive::instance()->rewind();
//        }
    }

    // All done.
    if ( ret == QDialog::Accepted ) {
        status( i18n( "Backup complete." ) );
    } else {
        status( i18n( "Backup aborted." ) );
    }
}

void KDat::fileRestore()
{
    doVerify( TRUE );
}

void KDat::fileVerify()
{
    doVerify( FALSE );
}

void KDat::doVerify( bool restore )
{
    RangeableNode*      rangeableNode = 0;
    MountedArchiveNode* archiveNode = 0;
    QStrList files;
    Archive* archive = 0;
    // Check for marked files first.
    for ( int i = _tapeDriveNode->childCount() - 1; i >= 0; i-- ) {
        if ( ( ((SelectableNode*)_tapeDriveNode->childAt( i ))->isSelected() ) ||
             ( ((SelectableNode*)_tapeDriveNode->childAt( i ))->hasSelectedChildren() )) {
            archiveNode = (MountedArchiveNode*)_tapeDriveNode->childAt( i );
            archive = archiveNode->getArchive();
            break;
        }
    }

    if ( !archiveNode ) {
        if ( ((Node*)_tree->getCurrentItem())->isType( Node::RangeableNodeType ) ) {
            rangeableNode = (RangeableNode*)_tree->getCurrentItem();
            Node* parent = rangeableNode;
            for ( ; !parent->isType( Node::MountedArchiveNodeType ); parent = (Node*)parent->getParent() );
            assert( parent );
            archive = ((MountedArchiveNode*)parent)->getArchive();
        }
    }

    assert( archive );
    QListIterator<Archive> it( archive->getTape()->getChildren() );
    int fileno;
    for ( fileno = 1; ( it.current() ) && ( it.current() != archive ); ++it, fileno++ );

    assert( archiveNode || rangeableNode );
    assert( fileno > -1 );

    RangeList ranges;
    if ( rangeableNode ) {
        QListIterator<Range> it( rangeableNode->getRanges() );
        for ( ; it.current(); ++it ) {
            ranges.addRange( it.current()->getStart(), it.current()->getEnd() );
        }
        if ( rangeableNode->isType( Node::MountedArchiveNodeType ) ) {
            // Make sure the mounted archive node has populated its children.
            archiveNode = (MountedArchiveNode*)rangeableNode;
            if ( archiveNode->childCount() == 0 ) {
                bool dummy = TRUE;
                archiveNode->expanding( dummy );
            }

            for ( int i = rangeableNode->childCount() - 1; i >= 0; i-- ) {
                if ( ((Node*)rangeableNode->childAt( i ))->isType( Node::MountedTapeDirectoryNodeType ) ) {
                    files.append( ((MountedTapeDirectoryNode*)rangeableNode->childAt( i ))->getFullPath() );
                } else if ( ((Node*)rangeableNode->childAt( i ))->isType( Node::MountedTapeFileNodeType ) ) {
                    files.append( ((MountedTapeFileNode*)rangeableNode->childAt( i ))->getFullPath() );
                } else {
                    assert( FALSE );
                }
            }
        } else if ( rangeableNode->isType( Node::MountedTapeDirectoryNodeType ) ) {
            files.append( ((MountedTapeDirectoryNode*)rangeableNode)->getFullPath() );
        } else if ( rangeableNode->isType( Node::MountedTapeFileNodeType ) ) {
            files.append( ((MountedTapeFileNode*)rangeableNode)->getFullPath() );
        } else {
            assert( FALSE );
        }
    } else {
        // Compile a list of files to verify/restore.
        QStack<RangeableNode> stack;

        // Make sure the mounted archive node has populated its children.
        if ( archiveNode->childCount() == 0 ) {
            bool dummy = TRUE;
            archiveNode->expanding( dummy );
        }

        for ( int i = archiveNode->childCount() - 1; i >= 0; i-- ) {
            stack.push( (RangeableNode*)archiveNode->childAt( i ) );
        }
        RangeableNode* sel = 0;
        while ( stack.count() > 0 ) {
            sel = stack.pop();
            if ( sel->isSelected() ) {
                QListIterator<Range> it( sel->getRanges() );
                for ( ; it.current(); ++it ) {
                    ranges.addRange( it.current()->getStart(), it.current()->getEnd() );
                }

                if ( sel->isType( Node::MountedTapeDirectoryNodeType ) ) {
                    files.append( ((MountedTapeDirectoryNode*)sel)->getFullPath() );
                } else if ( sel->isType( Node::MountedTapeFileNodeType ) ) {
                    files.append( ((MountedTapeFileNode*)sel)->getFullPath() );
                } else {
                    assert( FALSE );
                }
            } else if ( sel->hasSelectedChildren() ) {
                for ( int i = sel->childCount() - 1; i >= 0; i-- ) {
                    stack.push( (RangeableNode*)sel->childAt( i ) );
                }
            }
        }
    }

    char buf[1024];
    VerifyOptDlg dlg( getcwd( buf, 1024 ), files, restore, this );
    if ( dlg.exec() == QDialog::Accepted ) {
        if ( restore ) {
            status( i18n( "Restore in progress..." ) );
        } else {
            status( i18n( "Verify in progress..." ) );
        }
        VerifyDlg verifyDlg( dlg.getWorkingDirectory(), fileno, ranges, restore, this );
        int ret = verifyDlg.exec();
        
        if ( ret == QDialog::Accepted ) {
            if ( restore ) {
                status( i18n( "Restore complete." ) );
            } else {
                status( i18n( "Verify complete." ) );
            }
        } else {
            if ( restore ) {
                status( i18n( "Restore aborted." ) );
            } else {
                status( i18n( "Verify aborted." ) );
            }
        }
    }
}

void KDat::fileMountTape()
{
    if ( !TapeManager::instance()->getMountedTape() ) {
        if ( Options::instance()->getLoadOnMount() ) {
            if ( !TapeDrive::instance()->load() ) {
                KMsgBox::message( this, i18n( "KDat: No Tape" ), i18n( "There is no tape in the drive." ), KMsgBox::EXCLAMATION );
                return;
            }
        }
        
        if ( TapeDrive::instance()->isTapePresent() ) {
            setTapePresent( TRUE );
        } else {
            KMsgBox::message( this, i18n( "KDat: No Tape" ), i18n( "There is no tape in the drive." ), KMsgBox::EXCLAMATION );
        }
    } else {
        setTapePresent( FALSE );
    }
}

void KDat::fileIndexTape()
{
    if ( KMsgBox::yesNo( this, i18n( "KDat: Index Tape" ), i18n( "The current tape index will be overwritten, continue?" ), KMsgBox::EXCLAMATION ) == 1 ) {
        TapeManager::instance()->getMountedTape()->clear();
        IndexDlg dlg( TapeManager::instance()->getMountedTape(), this );
        if ( dlg.exec() == QDialog::Accepted ) {
            QString title;
            title.sprintf( i18n( "KDat: %s" ), TapeManager::instance()->getMountedTape()->getName().data() );
            setCaption( title );
            setIconText( title );

            status( i18n( "Index complete." ) );
        } else {
            status( i18n( "Index aborted." ) );
        }
    }
}

void KDat::fileDeleteArchive()
{
    Node* sel = (Node*)_tree->getCurrentItem();
    if ( ( !sel ) || ( !sel->isType( Node::ArchiveNodeType ) && !sel->isType( Node::MountedArchiveNodeType ) ) ) {
        KMsgBox::message( this, i18n( "KDat: No Archive Selected" ), i18n( "An archive must be selected in the tree." ), KMsgBox::EXCLAMATION );
        return;
    }

    // Find the selected archive and see if it has any archives after it.
    Archive* archive = 0;
    if ( sel->isType( Node::ArchiveNodeType ) ) {
        archive = ((ArchiveNode*)sel)->getArchive();
    } else if ( sel->isType( Node::MountedArchiveNodeType ) ) {
        archive = ((MountedArchiveNode*)sel)->getArchive();
    }
    assert( archive );

    Tape*    tape    = archive->getTape();
    QListIterator<Archive> i( tape->getChildren() );
    for ( ; i.current(); ++i ) {
        if ( i.current() == archive ) {
            break;
        }
    }
    assert( i.current() );

    ++i;
    if ( i.current() ) {
        // There are other archives after this one on the tape.
        QString list;
        for ( ; i.current(); ++i ) {
            list.append( "\n        " );
            list.append( i.current()->getName() );
        }

        QString msg;
        msg.resize( 32768 );
        msg.sprintf( i18n( "An archive cannot be removed from the middle of the tape.  If\nthe archive '%s' is deleted then\nthe following archives will also be deleted:\n%s\n\n Delete all listed archives?" ), archive->getName().data(), list.data() );
        if ( KMsgBox::yesNo( this, i18n( "KDat: Delete Archives?" ), msg, KMsgBox::EXCLAMATION ) == 1 ) {
            tape->removeChild( archive );
            emit status( i18n( "Archives deleted." ) );
            if ( _tree->getCurrentItem() ) {
                ((Node*)_tree->getCurrentItem())->selected();
            }
            configureUI( TapeManager::instance()->getMountedTape() );
        }
    } else {
        // This is the last (known) archive on the tape.
        QString msg;
        msg.sprintf( i18n( "Really delete the archive '%s'?" ), archive->getName().data() );
        if ( KMsgBox::yesNo( this, i18n( "KDat: Delete Archive?" ), msg, KMsgBox::EXCLAMATION ) == 1 ) {
            tape->removeChild( archive );
            emit status( i18n( "Archive deleted." ) );
            if ( _tree->getCurrentItem() ) {
                ((Node*)_tree->getCurrentItem())->selected();
            }
            configureUI( TapeManager::instance()->getMountedTape() );
        }
    }
}

void KDat::fileDeleteIndex()
{
    Node* sel = (Node*)_tree->getCurrentItem();
    if ( ( !sel ) || ( !sel->isType( Node::TapeNodeType ) ) ) {
        KMsgBox::message( this, i18n( "KDat: No Tape Index Selected" ), i18n( "A tape index must be selected in the tree." ), KMsgBox::EXCLAMATION );
        return;
    }

    Tape* tape = ((TapeNode*)sel)->getTape();
    assert( tape );
    if ( tape == TapeManager::instance()->getMountedTape() ) {
        KMsgBox::message( this, i18n( "KDat: Tape Is Mounted" ), i18n( "The index for a mounted tape cannot be deleted.\n\nUnmount the tape and try again." ), KMsgBox::EXCLAMATION );
        return;
    }

    QString msg;
    msg.sprintf( i18n( "Really delete the index for '%s'?" ), tape->getName().data() );
    if ( KMsgBox::yesNo( this, i18n( "KDat: Delete Tape Index?" ), msg, KMsgBox::EXCLAMATION ) == 1 ) {
        TapeManager::instance()->removeTape( tape );
        emit status( i18n( "Tape index deleted." ) );
        if ( _tree->getCurrentItem() ) {
            ((Node*)_tree->getCurrentItem())->selected();
        }
        configureUI( TapeManager::instance()->getMountedTape() );
    }
}

void KDat::fileFormatTape()
{
    if ( !TapeDrive::instance()->isTapePresent() ) {
        KMsgBox::message( this, i18n( "KDat: No Tape In Drive" ), i18n( "There is no tape in the tape drive.\nPlease insert a tape and try again." ), KMsgBox::EXCLAMATION );
        return;
    }

    if ( TapeDrive::instance()->isReadOnly() ) {
        KMsgBox::message( this, i18n( "KDat: Write Protected Tape" ), i18n( "The tape in the drive is write protected.\nPlease disable write protection and try again." ), KMsgBox::EXCLAMATION );
        return;
    }

    if ( KMsgBox::yesNo( this, i18n( "KDat: Format Tape" ), i18n( "All data will be lost, continue?" ), KMsgBox::EXCLAMATION ) == 1 ) {
        QString name;
        time_t tm = time( NULL );
        char date[80];
        strftime( date, 80, i18n( "%m/%d/%y" ), localtime( &tm ) );
        name.sprintf( i18n( "Tape created on %s" ), date );
        FormatOptDlg dlg( name.stripWhiteSpace(), this );
        if ( dlg.exec() != QDialog::Accepted ) {
            return;
        }

        // Delete old index file.
        if ( TapeManager::instance()->getMountedTape() ) {
            TapeManager::instance()->removeTape( TapeManager::instance()->getMountedTape() );
            TapeManager::instance()->unmountTape();
        }

        Tape* tape = new Tape();
        tape->setName( dlg.getName() );
        tape->setSize( dlg.getSize() );
        
        status( i18n( "Formatting tape..." ) );
        tape->format();
        TapeManager::instance()->addTape( tape );
        status( i18n( "Format complete." ) );

        setTapePresent( FALSE, FALSE );
        setTapePresent( TRUE, FALSE );
    }
}

void KDat::fileNewBackupProfile()
{
    BackupProfile* backupProfile = new BackupProfile();

    // Pick a unique name.
    QString name;
    for ( int i = 1; ; i++ ) {
        name.sprintf( i18n( "Backup Profile %d" ), i );
        QStrListIterator it( BackupProfileManager::instance()->getBackupProfileNames() );
        for ( ; it.current(); ++it ) {
            if ( name == it.current() ) {
                break;
            }
        }
        if ( !it.current() ) {
            // Name is unique.
            break;
        }
    }

    QStrList files;
    getBackupFiles( files );
    
    backupProfile->setName( name );
    backupProfile->setArchiveName( i18n( "Archive" ) );
    backupProfile->setWorkingDirectory( Util::longestCommonPath( files ) );
    if ( !backupProfile->getWorkingDirectory() ) {
        backupProfile->setWorkingDirectory( "/" );
    }
    backupProfile->setAbsoluteFiles( files );
    backupProfile->setOneFilesystem( TRUE );
    backupProfile->setIncremental( FALSE );
    backupProfile->setSnapshotFile( "snapshot" );
    backupProfile->setRemoveSnapshot( FALSE );
    backupProfile->save();

    BackupProfileManager::instance()->addBackupProfile( backupProfile );

    _backupProfileRootNode->setSelected( backupProfile );
}

void KDat::fileDeleteBackupProfile()
{
    Node* sel = (Node*)_tree->getCurrentItem();
    if ( ( !sel ) || ( !sel->isType( Node::BackupProfileNodeType ) ) ) {
        KMsgBox::message( this, i18n( "KDat: No Backup Profile Selected" ), i18n( "A backup profile must be selected in the tree." ), KMsgBox::EXCLAMATION );
        return;
    }

    BackupProfile* backupProfile = ((BackupProfileNode*)sel)->getBackupProfile();
    assert( backupProfile );

    QString msg;
    msg.sprintf( i18n( "Really delete backup profile '%s'?" ), backupProfile->getName() );
    if ( KMsgBox::yesNo( this, i18n( "KDat: Delete Backup Profile?" ), msg, KMsgBox::EXCLAMATION ) == 1 ) {
        BackupProfileManager::instance()->removeBackupProfile( backupProfile );
        emit status( i18n( "Backup profile deleted." ) );
        if ( _tree->getCurrentItem() ) {
            ((Node*)_tree->getCurrentItem())->selected();
        }
        configureUI( TapeManager::instance()->getMountedTape() );
    }
}

void KDat::fileQuit()
{
    KApplication::getKApplication()->quit();
}

void KDat::editPreferences()
{
    OptionsDlg dlg( this );
    dlg.exec();
}

void KDat::help()
{
    KApplication::getKApplication()->invokeHTMLHelp( "", "" );
}

void KDat::setTapePresent( bool tapePresent, bool eject )
{
    if ( TapeManager::instance()->getMountedTape() ) {
        if ( !tapePresent ) {
            TapeManager::instance()->unmountTape();

            if ( ( eject ) && ( Options::instance()->getLockOnMount() ) ) {
                TapeDrive::instance()->unlock();
            }

            if ( ( eject ) && ( Options::instance()->getEjectOnUnmount() ) ) {
                TapeDrive::instance()->eject();
            }

            status( i18n( "Tape unmounted." ) );
        }
    } else {
        if ( tapePresent ) {
            status( i18n( "Reading tape header..." ) );
            Tape* tape = TapeDrive::instance()->readHeader();

            if ( tape ) {
                TapeManager::instance()->mountTape( tape );
            } else {
                if ( TapeDrive::instance()->isReadOnly() ) {
                    KMsgBox::message( this, i18n( "KDat: Unformatted Tape" ), i18n( "This tape has not been formatted by KDat." ), KMsgBox::EXCLAMATION );
                } else {
                    if ( KMsgBox::yesNo( this, i18n( "KDat: Unformatted Tape" ), i18n( "This tape has not been formatted by KDat.\n\nWould you like to format it now?" ), KMsgBox::EXCLAMATION ) == 1 ) {
                        fileFormatTape();
                        return;
                    }
                }
            }

            if ( Options::instance()->getLockOnMount() ) {
                TapeDrive::instance()->lock();
            }

            status( i18n( "Tape mounted." ) );
        }
    }
}

void KDat::status( const char* msg )
{
    _statusBar->changeItem( msg, 0 );
    KApplication::getKApplication()->processEvents();
}

void KDat::show()
{
    KTopLevelWidget::show();

    hideInfo();
}

int KDat::calcBackupSize( const QString& workingDir, bool local, const QStrList& files,
                          bool incremental, const QString& snapshot, bool removeSnapshot )
{
    chdir( workingDir );

    bool useSnapshot = ( snapshot.length() > 0 ) && !removeSnapshot;

    int tmp = 0;
    if ( incremental && !removeSnapshot ) {
        QFile snap( snapshot );
        if ( snap.exists() && snap.open( IO_ReadOnly ) ) {
            QTextStream t( &snap );
            t >> tmp;
        } else {
            useSnapshot = FALSE;
        }
    }
    QDateTime mtime;
    mtime.setTime_t( tmp );

    int size = 0;

    QStrListIterator ii( files );
    for ( ; ii.current(); ++ii ) {

        // Is this a normal file, or a directory?
        QFileInfo finfo( ii.current() );
        if ( !finfo.isDir() ) {
            if ( ( !useSnapshot ) || ( finfo.lastModified() > mtime ) ) {
                size += finfo.size() / 512;
                if ( finfo.size() % 512 ) {
                    size++;
                }
            }
            continue;
        }

        QStack<QString> dirStack;
        dirStack.push( new QString( ii.current() ) );

        // If stay on one file system, get device of starting directory.
        dev_t device = 0;
        struct stat info;
        if ( local ) {
            if ( lstat( ii.current(), &info ) != 0 ) {
                device = 0;
            } else {
                device = info.st_dev;
            }
        }
        
        QString msg;
        msg.resize( 4096 );
        QDir dir;
        const QFileInfoList* infoList;
        while ( !dirStack.isEmpty() ) {
            QString* path = dirStack.pop();
            msg.sprintf( i18n( "Estimating backup size: %s, %s" ), Util::kbytesToString( size / 2 ).data(), path->data() );
            status( msg.data() );
            KApplication::getKApplication()->processEvents();
            dir.setPath( *path );
            infoList = dir.entryInfoList( QDir::Hidden | QDir::Files | QDir::Dirs, 0 );
            if ( infoList ) {
                QFileInfoListIterator i( *infoList );
                for ( ; i.current(); ++i ) {
                    if ( ( i.current()->fileName() != "." ) && ( i.current()->fileName() != ".." ) ) {
                        size++;
                        if ( i.current()->isSymLink() ) {
                        } else if ( i.current()->isDir() ) {
                            if ( local ) {
                                if ( lstat( i.current()->absFilePath().data(), &info ) == 0 ) {
                                    if ( device == info.st_dev ) {
                                        dirStack.push( new QString( i.current()->absFilePath() ) );
                                    }
                                }
                            } else {
                                dirStack.push( new QString( i.current()->absFilePath() ) );
                            }
                        } else if ( ( !useSnapshot ) || ( i.current()->lastModified() > mtime ) ) {
                            size += i.current()->size() / 512;
                            if ( i.current()->size() % 512 ) {
                                size++;
                            }
                        }
                    }
                }
            }
        }
    }

    // Convert size in tar blocks to size in kbytes.
    return size / 2;
}

void KDat::getBackupFiles( QStrList& files )
{
    if ( !_rootNode->isSelected() && !_rootNode->hasSelectedChildren() ) {
        // Backup the entire subtree.
        if ( ( _tree->getCurrentItem() ) && ( ((Node*)_tree->getCurrentItem())->isType( Node::ArchiveableNodeType ) ) ) {
            files.append( ((ArchiveableNode*)_tree->getCurrentItem())->getFullPath() );
        }
    } else {
        // Compile a list of files to backup.
        QStack<ArchiveableNode> stack;
        stack.push( _rootNode );
        ArchiveableNode* sel = 0;
        while ( stack.count() > 0 ) {
            sel = stack.pop();
            if ( sel->isSelected() ) {
                files.append( sel->getFullPath() );
            } else if ( sel->hasSelectedChildren() ) {
                for ( int i = sel->childCount() - 1; i >= 0; i-- ) {
                    stack.push( (ArchiveableNode*)sel->childAt( i ) );
                }
            }
        }
    }
}

void KDat::setBackupFiles( const QStrList& files )
{
    _rootNode->setSelected( FALSE );

    QString tmp;
    QStrListIterator it( files );
    for ( ; it.current(); ++it ) {
        ArchiveableNode* n = _rootNode;
        while ( n ) {
            if ( !strcmp( n->getFullPath(), it.current() ) ) {
                n->setSelected( TRUE );
                n = 0;
            } else {
                if ( n->childCount() == 0 ) {
                    bool dummy = TRUE;
                    n->expanding( dummy );
                }
                int i;
                for ( i = n->childCount() - 1; i >=0; i-- ) {
                    tmp = ((ArchiveableNode*)n->childAt( i ))->getFullPath();
                    if ( !strncmp( tmp.data(), it.current(), tmp.length() ) ) {
                        n = (ArchiveableNode*)n->childAt( i );
                        break;
                    }
                }
                if ( i < 0 ) {
                    n = 0;
                }
            }
        }
    }
}

void KDat::slotTapeDevice()
{
    setTapePresent( FALSE );
}

void KDat::slotTapeMounted()
{
    configureUI( TapeManager::instance()->getMountedTape() );
}

void KDat::slotTapeUnmounted()
{
    configureUI( 0 );
}

void KDat::configureUI( Tape* tape )
{
    if ( _destroyed ) {
        return;
    }

    Node* sel  = (Node*)_tree->getCurrentItem();

    // Title bar
    if ( tape ) {
        QString title;
        title.sprintf( i18n( "KDat: %s" ), tape->getName().data() );
        setCaption( title );
        setIconText( title );
    } else {
        setCaption( i18n( "KDat: <no tape>" ) );
        setIconText( i18n( "KDat: <no tape >" ) );
    }

    // Backup
    bool canBackup = ( tape ) && ( !TapeDrive::instance()->isReadOnly() ) &&
        ( ( ( sel ) && ( sel->isType( Node::ArchiveableNodeType ) || sel->isType( Node::BackupProfileNodeType ) ) ) ||
          ( _rootNode->isSelected() || _rootNode->hasSelectedChildren() ) );
    _toolbar->setItemEnabled( 1, canBackup );
    _fileMenu->setItemEnabled( _fileMenu->idAt( 0 ), canBackup );

    // Restore/verify
    bool canRestore = ( tape ) && ( sel ) && sel->isType( Node::RangeableNodeType );
    if ( !canRestore ) {
        for ( int i = _tapeDriveNode->childCount() - 1; i >= 0; i-- ) {
            if ( ( ((SelectableNode*)_tapeDriveNode->childAt( i ))->isSelected() ) ||
                 ( ((SelectableNode*)_tapeDriveNode->childAt( i ))->hasSelectedChildren() )) {
                canRestore = TRUE;
                break;
            }
        }
    }
    if ( canRestore ) {
        for ( Node* parent = (Node*)sel->getParent(); ( parent ) && ( parent->getParent() ); parent = (Node*)parent->getParent() ) {
            if ( parent->isType( Node::TapeNodeType ) ) {
                canRestore = FALSE;
            }
        }
    }
    _toolbar->setItemEnabled( 2, canRestore );
    _toolbar->setItemEnabled( 3, canRestore );
    _fileMenu->setItemEnabled( _fileMenu->idAt( 1 ), canRestore );
    _fileMenu->setItemEnabled( _fileMenu->idAt( 2 ), canRestore );

    // Mount/unmount tape
    if ( tape ) {
        _toolbar->setButtonPixmap( 0, *ImageCache::instance()->getTapeMounted() );
        _fileMenu->changeItem( i18n( "Unmount Tape" ), _fileMenu->idAt( 3 ) );
    } else {
        _toolbar->setButtonPixmap( 0, *ImageCache::instance()->getTapeUnmounted() );
        _fileMenu->changeItem( i18n( "Mount Tape" ), _fileMenu->idAt( 3 ) );
    }

    // Index tape
    _fileMenu->setItemEnabled( _fileMenu->idAt( 4 ), tape != NULL );

    // Delete archive
    _fileMenu->setItemEnabled( _fileMenu->idAt( 7 ), ( sel ) && sel->isType( Node::ArchiveNodeType ) );

    // Delete index
    _fileMenu->setItemEnabled( _fileMenu->idAt( 8 ), ( sel ) && sel->isType( Node::TapeNodeType ) );

    // Delete backup profile
    _fileMenu->setItemEnabled( _fileMenu->idAt( 9 ), ( sel ) && sel->isType( Node::BackupProfileNodeType ) );

    // Format tape
    _fileMenu->setItemEnabled( _fileMenu->idAt( 10 ), TapeManager::instance()->getMountedTape() && !TapeDrive::instance()->isReadOnly() );
}

void KDat::readProperties( KConfig* config )
{
    _panner->setSeparator( config->readNumEntry( "panner", 50 ) );
}

void KDat::saveProperties( KConfig* config )
{
    config->writeEntry( "panner", _panner->getSeparator() );
}
