/*  
    KDE Icon Editor - a small graphics drawing program for the KDE

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

#ifndef __KICONFILEDLG_H__
#define __KICONFILEDLG_H__


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <qdir.h>
#include <kfiledialog.h>
#include <kapp.h>
#include <kfileinfo.h>
#include <kdir.h>
#include <kcombiview.h>
#include <kfiledetaillist.h>
#include "kiconfileview.h"


class KIconFileDlg : public KFileBaseDialog
{
  Q_OBJECT
public:
  KIconFileDlg(const char *dirName, const char *filter= 0,
		QWidget *parent= 0, const char *name= 0, 
		bool modal = false, bool acceptURLs = true);

  static QString getOpenFileName(const char *dir= 0, const char *filter= 0,
				   QWidget *parent= 0, const char *name= 0);
  static QString getSaveFileName(const char *dir= 0, const char *filter= 0,
				   QWidget *parent= 0, const char *name= 0);
  static QString getOpenFileURL(const char *url= 0, const char *filter= 0,
				  QWidget *parent= 0, const char *name= 0);
  static QString getSaveFileURL(const char *url= 0, const char *filter= 0,
				  QWidget *parent= 0, const char *name= 0);

protected:
  virtual KFileInfoContents *initFileList( QWidget *parent);
  virtual bool getShowFilter();
};

#endif //__KICONFILEDLG_H__



