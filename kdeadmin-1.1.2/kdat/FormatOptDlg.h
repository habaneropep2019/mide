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

#ifndef _FormatOptDlg_h_
#define _FormatOptDlg_h_

#include <qdialog.h>
#include <qstring.h>

class QComboBox;
class QLineEdit;

/**
 * @short Display/edit options for formatting a tape.
 */
class FormatOptDlg : public QDialog {
    Q_OBJECT
    QString    _name;
    int        _size;
    QLineEdit* _entry;
    QLineEdit* _tapeSize;
    QComboBox* _tapeSizeUnits;
private slots:
    void okClicked();
public:
    /**
     * Create a new format options dialog.
     *
     * @param def    The default value for the tape name.
     * @param parent The parent widget for the dialog.
     * @param name   The name for the dialog.
     */
    FormatOptDlg( const char* def, QWidget* parent=0, const char* name=0 );

    /**
     * Destroy the format options dialog.
     */
    ~FormatOptDlg();

    /**
     * Get the name for the new tape.
     *
     * @return The name for the tape being formatted.
     */
    const char* getName();

    /**
     * Get the capacity of the new tape.
     *
     * @return The size of the tape.
     */
    int getSize();
};

#endif
