/**************************************************************************

    collectdlg.h  - The collection manager dialog
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
#include "player/songlist.h"
#include "player/slman.h"

class QLabel;
class QPushButton;
class QListBox;
class CollectionDialog : public QDialog
{
	Q_OBJECT
private:
	SLManager *slman;
	SongList *currentsl;

protected:

	virtual void resizeEvent(QResizeEvent *qre);
	void fillInSongList(void); //clear the songs list and insert in it
				// the songs in currentsl
public:

	CollectionDialog(SLManager *slm,int selc,QWidget *parent,const char *name);

public slots:
	void collectionselected(int idx);
	void songselected(int idx);
	void newCollection();
	void copyCollection();
	void deleteCollection();
	void changeCollectionName(int idx);
	void addSong();
	void removeSong();

private:
QLabel *label;
QLabel *label2;
QPushButton *ok;
QPushButton *cancel;
QListBox *collections;
QListBox *songs;
QPushButton *newC;
QPushButton *copyC;
QPushButton *deleteC;
QPushButton *addS;
QPushButton *delS;


public:
static int selectedC;
static int selectedS;

};
