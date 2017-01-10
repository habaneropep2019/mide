/*
    KDE Icon Editor - a small icon drawing program for the KDE.
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

#ifndef __KICONCONFIG_H__
#define __KICONCONFIG_H__

#include <qdir.h>
#include <qobject.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qfileinf.h>
#include <kapp.h>
#include <kaccel.h>
#include <kkeydialog.h>
#include <kmsgbox.h>
#include <knotebook.h>
#include <kbuttonbox.h>
#include <kcolorbtn.h>
#include "knew.h"
#include "utils.h"
#include "properties.h"

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

class KTemplateEditDlg : public QDialog
{
  Q_OBJECT
public:
  KTemplateEditDlg(QWidget *);
  ~KTemplateEditDlg() {};

  const char *getName();
  const char *getPath();
  void setName(QString);
  void setPath(QString);
  
public slots:
  void slotTextChanged(const char*);
  void slotBrowse();

protected:
  QPushButton *ok, *cancel, *browse;
  QLineEdit *ln_name, *ln_path;
  QLabel *lb_name, *lb_path;
};

class KTemplateConfig : public QWidget
{
  Q_OBJECT
public:
  KTemplateConfig(QWidget*);
  ~KTemplateConfig() {};

  void saveSettings();

protected slots:
  void add();
  void edit();
  void remove();
  void checkSelection(int);

protected:
  KIconListBox *templates;
  QBoxLayout *ml, *l;
  QGroupBox *grp;
  QList<KIconTemplate> *tlist;
  QPushButton *btadd, *btedit, *btremove;
};

class KBackgroundConfig : public QWidget
{
  Q_OBJECT
public:
  KBackgroundConfig(QWidget *parent);
  ~KBackgroundConfig();

public slots:
  void saveSettings();
  void selectColor();
  void selectPixmap();

signals:

protected slots:
  void slotBackgroundMode(int);

protected:
  Properties *pprops;
  QPushButton *btpix, *btcolor;
  QLabel *lb_ex;
  QButtonGroup *btngrp;
  QPixmap pix;
  QString pixpath;
  QColor color;
  QWidget::BackgroundMode bgmode;
};

class KToolBarConfig : public QWidget
{
  Q_OBJECT
public:
  KToolBarConfig(QWidget *parent);
  ~KToolBarConfig();

public slots:
  void drawToolBar(int);
  void mainToolBar(int);
  void saveSettings();

signals:

protected slots:

protected:
  Properties *pprops;
  QButtonGroup *btngrptool, *btngrpdraw;
  QRadioButton *rb;
  int dstat, mstat;
};

class KMiscConfig : public QWidget
{
  Q_OBJECT
public:
  KMiscConfig(QWidget *parent);
  ~KMiscConfig();

public slots:
  void saveSettings();
  void pasteMode(bool);
  void showRulers(bool);

protected slots:

signals:

protected:
  bool pastemode, showrulers;
  Properties *pprops;
  QButtonGroup *btngrp;
  QRadioButton *rbp;
};

class KIconConfig : public KNoteBook
{
  Q_OBJECT
public:
  KIconConfig(QWidget *parent);
  ~KIconConfig();

public slots:

signals:

protected slots:
  void saveSettings();

protected:
  Properties *pprops;
  KAccel *keys;
  KKeyChooser *keychooser;
  KTemplateConfig *temps;
  KBackgroundConfig *backs;
  KToolBarConfig *toolbars;
  KMiscConfig *misc;
};

#endif //__KICONCONFIG_H__
