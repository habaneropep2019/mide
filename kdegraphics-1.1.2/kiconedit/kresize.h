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

#ifndef __KRESIZE_H__
#define __KRESIZE_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qfiledlg.h> 
#include <qbttngrp.h>
#if QT_VERSION >= 140
#include <qpushbutton.h>
#else
#include <qpushbt.h>
#endif
#include <qradiobt.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <kiconloader.h>
#include <kapp.h>
#include <kintegerline.h>

class KResize : public QDialog
{
  Q_OBJECT
public:
  KResize(QWidget *parent, const char *name, bool m, const QSize s);
  ~KResize();

  const QSize getSize();

signals:
  void validsize(bool);

public slots:
  void checkValue(int);
  void slotReturnPressed();

protected:
  bool eventFilter(QObject*, QEvent*);

  KIntegerLine *x_line, *y_line;
  QBoxLayout *ml, *l0, *l1;
  QGroupBox *grp;
  QPushButton *ok, *cancel;
};


#endif //__KRESIZE_H__
