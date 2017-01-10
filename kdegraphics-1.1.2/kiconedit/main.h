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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <kapp.h>
#include <qlist.h>
#include <qwidget.h>
#include <qimage.h>


#define UNNAMED  i18n("UnNamed.xpm")

/** Global list of top level windows. */
typedef QList<QWidget> WindowList;
extern WindowList* windowList;

class KSWApplication : public KApplication
{
  Q_OBJECT
public:
  KSWApplication ( int& argc, char** argv, const QString& rAppName );

  virtual bool     x11EventFilter( XEvent * );

signals:
  void ksw_save();
  void ksw_exit();

protected:
  Atom _ksw_save;
  Atom _ksw_exit;
};


#endif //__MAIN_H__



