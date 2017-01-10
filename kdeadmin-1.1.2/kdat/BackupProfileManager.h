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

#ifndef _BackupProfileManager_h_
#define _BackupProfileManager_h_

#include <qdict.h>
#include <qobject.h>

#include "BackupProfile.h"

/**
 * @short Control access to the set of backup profiles.
 *
 * Each user has a set of backup profiles that are stored under
 * <TT>$HOME/.kdat/</TT>.  This class provides a single point of access for
 * reading and writing these backup profiles.
 *
 * Other objects can register to be notified when a backup profile is added or
 * removed, and when a backup profile is modified.
 *
 * The BackupProfileManager follows the Singleton pattern.
 */
class BackupProfileManager : public QObject {
    Q_OBJECT

    static BackupProfileManager* _instance;

    QDict<BackupProfile> _backupProfiles;
    QStrList             _backupProfileNames;
    
    BackupProfileManager();
public:
    ~BackupProfileManager();

    /**
     * All access to the BackupProfileManager goes through this method.
     *
     * @return a pointer to the single instance of the BackupProfileManager.
     */
    static BackupProfileManager* instance();

    /**
     * Get the list of all known backup profiles.
     *
     * @return a QStrList containing the backup profile names.
     */
    const QStrList& getBackupProfileNames();

    /**
     * Retrieve the named backup profile.
     *
     * @param name The name of the backup profile.
     * @return A pointer to the backup profile.
     */
    BackupProfile* findBackupProfile( const char* name );

    /**
     * Add a new backup profile.
     *
     * @param backupProfile A pointer to the new backup profile.
     */
    void addBackupProfile( BackupProfile* backupProfile );

    /**
     * Remove a backup profile.  The backup profile is removed from memory and
     * from disk.  A signal is emitted before the profile is actually removed.
     *
     * @param backupProfile A pointer to the backup profile to remove.
     */
    void removeBackupProfile( BackupProfile* backupProfile );

    /**
     * Notify anyone who cares that the backup profile has been modified.
     *
     * @param backupProfile A pointer to the backup profile that was modified.
     */
    void backupProfileModified( BackupProfile* backupProfile );
signals:
    /**
     * Emitted after a new backup profile is created.
     *
     * @param backupProfile A pointer to the new backup profile.
     */
    void sigBackupProfileAdded( BackupProfile* backupProfile );

    /**
     * Emitted before a backup profile is destroyed.  This signal is emitted
     * immediately before the backup profile is deleted.
     *
     * @param backupProfile A pointer to the backup profile that is about to
     *                      be destroyed.
     */
    void sigBackupProfileRemoved( BackupProfile* backupProfile );

    /**
     * Emitted after a backup profile has been changed in some way.
     *
     * @param backupProfile A pointer to the backup profile that has been modified.
     */
    void sigBackupProfileModified( BackupProfile* backupProfile );
};

#endif
