/**************************************************************************

    channelcfgdlg.h  - The channel view config dialog
    Copyright (C) 1998  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QRadioButton;
class ChannelViewConfigDialog : public QDialog
{
    Q_OBJECT

public:
    
    ChannelViewConfigDialog(QWidget *parent,const char *name);
    
private:
    QButtonGroup *qbg;
    QRadioButton *rb0;
    QRadioButton *rb1;
    
    QPushButton *ok;
    QPushButton *cancel;
    
public slots:
    void modeselected(int idx);

public:
    static int selectedmode;
};
