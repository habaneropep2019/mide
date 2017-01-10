// -*- c++ -*-
/* This file is part of the KDE libraries
    Copyright (C) 1997 Stephan Kulow <coolo@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef KICONFILEVIEW_H
#define KICONFILEVIEW_H

class KFileInfo;
class QWidget;

#include <qtableview.h>
#include <qpixmap.h>
#include <kfileinfocontents.h>
#include "kiconfilebase.h"

class KIconFileView : public QTableView, public KFileInfoContents, public KIconFileBase {
    Q_OBJECT

public:
    KIconFileView(bool s, QDir::SortSpec sorting, QWidget *parent, const char *name);
    virtual ~KIconFileView();
    
    virtual QWidget *widget() { return this; }
    virtual void setAutoUpdate(bool);
    virtual void clearView();
    
    virtual bool acceptsFiles() { return true; }
    virtual bool acceptsDirs() { return true; }

protected:
    virtual void highlightItem(unsigned int item);
    virtual bool insertItem(const KFileInfo *i, int index);
    virtual void paintCell( QPainter *, int, int);
    virtual void resizeEvent ( QResizeEvent *e );
    virtual void keyPressEvent( QKeyEvent* e );
    virtual void mousePressEvent( QMouseEvent* e );
    virtual void mouseDoubleClickEvent ( QMouseEvent *e );
    virtual void focusInEvent ( QFocusEvent * );
    virtual void focusOutEvent ( QFocusEvent * );
    virtual int  cellWidth ( int col );

    void highlightItem(int row, int col);

    int rowsVisible;
    int curCol, curRow;
    QList<QPixmap> pixmaps;
    // this flag is used for speed up, when autoUpdate is false
    bool touched;
    uint *width_array;
    uint width_length, height_length;
    uint width_max;
    void insertArray(uint item, uint pos);
};

#endif // KFILESIMPLEVIEW_H
