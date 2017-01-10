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

#ifndef _BackupOptDlg_h_
#define _BackupOptDlg_h_

#include <qdialog.h>

class BackupProfile;
class BackupProfileWidget;

/**
 * @short Display/edit the parameters for a backup operation.
 */
class BackupOptDlg : public QDialog {
    Q_OBJECT
    BackupProfileWidget* _profile;
public:
    /**
     * Create a backup options dialog.
     *
     * @param backupProfile The backup profile.
     */
    BackupOptDlg( BackupProfile* backupProfile, QWidget* parent=0, const char* name=0 );

    /**
     * Destroy the backup options dialog.
     */
    ~BackupOptDlg();

    /**
     * Query the name of the archive.
     *
     * @return The name of the new archive.
     */
    const char* getArchiveName();

    /**
     * Query the working directory for the tar command.
     *
     * @return The working directory.
     */
    const char* getWorkingDirectory();

    /**
     * Query the list of files to backup, relative to the working directory.
     *
     * @return The file list.
     */
    const QStrList& getRelativeFiles();

    /**
     * Query whether or not to cross filesystem boundaries when performing the
     * backup.
     *
     * @return TRUE if the backup is restricted to a single filesystem, FALSE
     *         if the backup can cross filesystem boundaries.
     */
    bool isOneFilesystem();

    /**
     * Query whether this is to be a GNU incremental backup.
     *
     * @return TRUE if incremental, otherwise FALSE.
     */
    bool isIncremental();

    /**
     * Query the name of the snapshot file to use for an incremental backup.
     *
     * @return The name of the snapshot file.
     */
    const char* getSnapshotFile();

    /**
     * Query whether to remove the snapshot file before beginning an
     * incremental backup.  This has the effect of performing a full backup.
     *
     * @return TRUE if the snapshot file should be removed, otherwise FALSE.
     */
    bool getRemoveSnapshot();
};

#endif
