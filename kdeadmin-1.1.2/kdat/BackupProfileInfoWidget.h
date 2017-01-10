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

#ifndef _BackupProfileInfoWidget_h_
#define _BackupProfileInfoWidget_h_

#include <qwidget.h>

class BackupProfileWidget;

/**
 * @short Display/edit information about a backup profile.
 */
class BackupProfileInfoWidget : public QWidget {
    Q_OBJECT
    BackupProfile*       _backupProfile;
    QLineEdit*           _name;
    BackupProfileWidget* _profile;
    QPushButton*         _apply;

    bool isModified();
private slots:
    void slotTextChanged( const char* text );
    void slotSomethingChanged();
    void slotApply();
    void slotSetSelection();
    void slotGetSelection();
public:
    /**
     * Create a new backup profile info widget.
     */
    BackupProfileInfoWidget( QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the backup profile widget.
     */
    ~BackupProfileInfoWidget();

    /**
     * Change the backup profile that the widget displays/edits.
     *
     * @param backupProfile The new backup profile to display/edit.
     */
    void setBackupProfile( BackupProfile* backupProfile );
};

#endif
