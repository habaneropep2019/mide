/*
    KDE Draw - a small graphics drawing program for the KDE.
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

#ifndef __KICONTOOLS_H__
#define __KICONTOOLS_H__

#include <qdir.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qtablevw.h>
#include <qpopmenu.h>
#include <kapp.h>
#include "kiconcolors.h"

class Preview : public QTableView
{
  Q_OBJECT
public:
  Preview(QWidget *parent);
  ~Preview();

public slots:
  void setPixmap(const QPixmap & pixmap);

protected:
  virtual void paintCell( QPainter*, int, int );

private:
  QPixmap *ppixmap;
  int pw, ph;
};


#endif //__KICONVIEW_H__



