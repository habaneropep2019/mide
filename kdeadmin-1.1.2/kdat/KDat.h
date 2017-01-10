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

#ifndef _KDat_h_
#define _KDat_h_

#include <ktopwidget.h>

class KMenuBar;
class KPanner;
class KStatusBar;
class KToolBar;
class KTreeView;
class KTreeViewItem;

class QPixmap;
class QPopupMenu;

class Archive;
class ArchiveInfoWidget;
class BackupProfile;
class BackupProfileInfoWidget;
class BackupProfileRootNode;
class File;
class FileInfoWidget;
class Node;
class RootNode;
class Tape;
class TapeDriveNode;
class TapeFileInfoWidget;
class TapeInfoWidget;

/**
 * @short The KDat main window.  Everything happens from here.
 */
class KDat : public KTopLevelWidget {
    Q_OBJECT
    bool                     _destroyed;
    KMenuBar*                _menu;
    QPopupMenu*              _fileMenu;
    QPopupMenu*              _editMenu;
    KToolBar*                _toolbar;
    KStatusBar*              _statusBar;
    KPanner*                 _panner;
    KTreeView*               _tree;
    QPopupMenu*              _tapeDriveMenu;
    QPopupMenu*              _archiveMenu;
    QPopupMenu*              _mountedArchiveMenu;
    QPopupMenu*              _mountedTapeFileMenu;
    QPopupMenu*              _localFileMenu;
    QPopupMenu*              _tapeMenu;
    QPopupMenu*              _backupProfileRootMenu;
    QPopupMenu*              _backupProfileMenu;
    ArchiveInfoWidget*       _archiveInfo;
    BackupProfileInfoWidget* _backupProfileInfo;
    TapeFileInfoWidget*      _tapeFileInfo;
    TapeInfoWidget*          _tapeInfo;
    FileInfoWidget*          _fileInfo;
    RootNode*                _rootNode;
    TapeDriveNode*           _tapeDriveNode;
    BackupProfileRootNode*   _backupProfileRootNode;

    static KDat* _instance;

    void doVerify( bool restore = FALSE );
    void setTapePresent( bool tapePresent, bool eject = TRUE );
    int calcBackupSize( const QString& workingDir, bool local, const QStrList& files,
                        bool incremental, const QString& snapshot, bool removeSnapshot );

    KDat();
private slots:
    void localExpanding( KTreeViewItem* item, bool& allow );
    void localExpanded( int index );
    void localCollapsed( int index );
    void localSelected( int index );
    void localHighlighted( int index );
    void localPopupMenu( int index, const QPoint& p );

    void fileBackup();
    void fileRestore();
    void fileVerify();
    void fileMountTape();
    void fileIndexTape();
    void fileDeleteArchive();
    void fileDeleteIndex();
    void fileFormatTape();
    void fileNewBackupProfile();
    void fileDeleteBackupProfile();
    void fileQuit();
    void editPreferences();
    void help();

    void slotTapeDevice();
    void slotTapeMounted();
    void slotTapeUnmounted();
protected:
    virtual void readProperties( KConfig* config );
    virtual void saveProperties( KConfig* config );
public:
    /**
     * Destroy the KDat main window.
     */
    ~KDat();

    /**
     * Get a reference to the single instance of the KDat main window.
     *
     * @return A pointer to the KDat main window.
     */
    static KDat* instance();

    /**
     * Display the tape drive node popup menu.
     * 
     * @param p The upper left corner of the menu.
     */
    void popupTapeDriveMenu( const QPoint& p );

    /**
     * Display the archive node popup menu.
     *
     * @param p The upper left corner of the menu.
     */
    void popupArchiveMenu( const QPoint& p );

    /**
     * Display the mounted archive node popup menu.
     *
     * @param p The upper left corner of the menu.
     */
    void popupMountedArchiveMenu( const QPoint& p );

    /**
     * Display the mounted tape file node popup menu.
     *
     * @param p The upper left corner of the menu.
     */
    void popupMountedTapeFileMenu( const QPoint& p );

    /**
     * Display the local file node popup menu.
     *
     * @param p The upper left corner of the menu.
     */
    void popupLocalFileMenu( const QPoint& p );

    /**
     * Display the tape index node popup menu.
     *
     * @param p The upper left corner of the menu.
     */
    void popupTapeMenu( const QPoint& p );

    /**
     * Display the backup profile root node popup menu.
     *
     * @param p The upper left corner of the menu.
     */
    void popupBackupProfileRootMenu( const QPoint& p );

    /**
     * Display the backup profile node popup menu.
     *
     * @param p The upper left corner of the menu.
     */
    void popupBackupProfileMenu( const QPoint& p );

    /**
     * Hide all of the info viewers.
     */
    void hideInfo();

    /**
     * Display the tape info widget for the given tape.
     *
     * @param tape The tape index to display.
     */
    void showTapeInfo( Tape* tape );

    /**
     * Display the archive info widget for the given archive.
     *
     * @param archive The archive to display.
     */
    void showArchiveInfo( Archive* archive );

    /**
     * Display the tape file info widget for the given file.
     *
     * @param file The file to display.
     */
    void showTapeFileInfo( File* file );

    /**
     * Display the backup profile info widget for the given backup profile.
     *
     * @param backupProfile The backup profile to display.
     */
    void showBackupProfileInfo( BackupProfile* backupProfile );

    /**
     * Display the local file info widget for the given file.
     *
     * @param file The full path name of the file to display.
     */
    void showFileInfo( const char* name );

    /**
     * Make sure that the user interface is consistent for the given mounted
     * tape.  This method mostly just enables or disables GUI objects based on
     * the currently mounted tape, and selections.
     *
     * @param tape The currently mounted tape. May be NULL.
     */
    void configureUI( Tape* tape );

    /**
     * Get a list of all the files selected for backup.
     *
     * @param files This list will be filled with the selected files.
     */
    void getBackupFiles( QStrList& files );

    /**
     * Set the list of all the files selected for backup.
     *
     * @param files This list will become the selection.
     */
    void setBackupFiles( const QStrList& files );
public slots:
    /**
     * Display a text message in the status bar.
     *
     * @param msg The message to display.
     */
    void status( const char* msg );
    
    /**
     * Initialize the KDat main window before displaying.
     */
    virtual void show();
};

#endif
