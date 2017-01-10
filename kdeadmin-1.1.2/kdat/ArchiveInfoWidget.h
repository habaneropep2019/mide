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

#ifndef _ArchiveInfoWidget_h_
#define _ArchiveInfoWidget_h_

#include <qwidget.h>

class QLabel;
class QLineEdit;
class QPushButton;

class Archive;

/**
 * @short Display/edit information about an archive index.
 */
class ArchiveInfoWidget : public QWidget {
    Q_OBJECT
    Archive*     _archive;
    QLineEdit*   _archiveName;
    QLabel*      _ctime;
    QLabel*      _size;
    QPushButton* _apply;
private slots:
    void slotTextChanged( const char* text );
    void slotApply();
public:
    /**
     * Create a new archive info widget.
     */
    ArchiveInfoWidget( QWidget* parent = 0, const char* name = 0 );

    /**
     * Destroy the archive info widget.
     */
    ~ArchiveInfoWidget();

    /**
     * Change the archive index that the widget displays/edits.
     *
     * @param archive The new archive index to display/edit.
     */
    void setArchive( Archive* archive );
};

#endif
