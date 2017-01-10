/**************************************************************************

    kaskdlg.h  - A dialog that ask the user something definable
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

class QLabel;
class QPushButton;
class KLined;
class KAskDialog : public QDialog
{
	Q_OBJECT
private:

protected:

	virtual void resizeEvent(QResizeEvent *qre);
public:

	KAskDialog(const char *labeltext, const char *title,QWidget *parent,const char *name);

public slots:
	void OK_pressed();

private:
QLabel *label;
QPushButton *ok;
QPushButton *cancel;
KLined *kline;

static char *textresult;

public:
static	char *getResult(void);
};
