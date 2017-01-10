// -*- c++ -*-
/*
    kiconedit - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#ifndef KICONDETAILLIST_H
#define KICONDETAILLIST_H

#include <qlistview.h>
#include <kfileinfocontents.h>
#include <kfileinfo.h>
#include <kdir.h>
#include <qdict.h>
#include "kiconfilebase.h"

class KListViewItem : public QListViewItem
{
public:
    KListViewItem( QListView * parent,
		   const char *,     const char * = 0,
		   const char * = 0, const char * = 0,
		   const char * = 0, const char * = 0,
		   const char * = 0, const char * = 0 );

    void setItemHeight(int h) { setHeight(h);}
};


class KIconDetailList : protected QListView, public KFileInfoContents, public KIconFileBase {
    Q_OBJECT
    
public:
    /**
     * Create a file info list widget.
     */
    KIconDetailList(bool s, QDir::SortSpec sorting, QWidget *parent= 0, const char *name= 0);
    virtual ~KIconDetailList();
    
    virtual QWidget *widget() { return this; }
    virtual void setAutoUpdate(bool);
    virtual void clearView();
    //virtual void repaint(bool f = true);
    
    virtual bool acceptsFiles() { return true; }
    virtual bool acceptsDirs() { return true; }

protected slots:
    void reorderFiles(int inColumn);
    void selected(QListViewItem*);
    void openItem(QListViewItem*);
    void highlighted(QListViewItem*);

protected:
    virtual void highlightItem(unsigned int item);
    virtual bool insertItem(const KFileInfo *i, int index);
    virtual void keyPressEvent( QKeyEvent *e);
    virtual void focusInEvent ( QFocusEvent *e );

    int pixwidth;

    QDict<KFileInfo> fileinfos;
};

#endif // KICONDETAILLIST_H
