/*  
    kiconedit - a small graphics drawing program the KDE.

    Copyright (C) 1998 Thomas Tanghus (tanghus@kde.org)

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

#ifndef __PROPS_H__
#define __PROPS_H__

#include <qlist.h>
#include <qwidget.h>
#include <qimage.h>
#include <kaccel.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include "knew.h"

class KIconEditProperties;

#define props(x) KIconEditProperties::getProperties(x)


struct Properties
{
  KAccel *keys;
  QDict<KKeyEntry> keydict;
  QList<KIconTemplate> *tlist;
  QStrList *recentlist;
  QString backgroundpixmap;
  QColor backgroundcolor;
  QWidget::BackgroundMode backgroundmode;
  bool showgrid;
  bool pastetransparent;
  bool showrulers;

  // initial settings from config
  KToolBar::BarPosition maintoolbarpos;
  KToolBar::BarPosition drawtoolbarpos;
  KMenuBar::menuPosition menubarpos;
  int maintoolbartext;
  int drawtoolbartext;
  int maintoolbarstat;
  int drawtoolbarstat;
  int statusbarstat;

  // save pointers for easy access
  KToolBar *maintoolbar;
  KToolBar *drawtoolbar;
  KMenuBar *menubar;
  KStatusBar *statusbar;

  int winwidth;
  int winheight;
  int gridscaling;
};

class KIconEditProperties : public QObject
{
  Q_OBJECT
public:
  KIconEditProperties(QWidget *);
  ~KIconEditProperties();

  /**
  * The QList must not be deleted! Delete the object instead as it automatically
  * deletes the list when there are no more instances.
  */
  static struct Properties *getProperties(QWidget*);
  static void saveProperties(QWidget*);

protected:
  static struct Properties *pprops;
  static int instances;
};


#endif //__PROPS_H__



