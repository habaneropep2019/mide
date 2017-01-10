// -*- C++ -*-

//
//  kjots
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de or chris@kde.org
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef KJotsMain_included
#define KJotsMain_included

#include <qwidget.h>
#include <qlayout.h>
#include <qdatetm.h>
#include <qstring.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qdialog.h>
#include <qlined.h>
#include <qbttngrp.h>
#include <qmlined.h>
#include <qpushbt.h>

#include <ktopwidget.h>

class KJotsMain;
class KToolBar;
class KAccel;
class SubjList;
class CPopupMenu;
class ConfDialog;
class QTextStream;

class TextEntry
{
  friend KJotsMain;
  friend SubjList;
public:
  TextEntry() {}
  virtual ~TextEntry() {}

protected:
  QString   text;
  QString   subject;
};

class MyMultiEdit : public QMultiLineEdit
{
  Q_OBJECT  
public:
  MyMultiEdit (QWidget* parent=0, const char* name=0);
  ~MyMultiEdit () {}

protected slots:
  void openUrl();

protected:
  virtual void keyPressEvent (QKeyEvent* e);
  virtual void mousePressEvent (QMouseEvent *e);

  CPopupMenu *web_menu;
};

class AskFileName : public QDialog
{
  Q_OBJECT
public:
  AskFileName(QWidget* parent=0, const char* name=0);
  virtual ~AskFileName() {}

  QString getName() { return i_name->text(); }

protected:
  QLineEdit *i_name;
};

class MyButtonGroup : public QButtonGroup
{
  Q_OBJECT  
public:
  MyButtonGroup (QWidget* parent=0, const char* name=0);
  ~MyButtonGroup () {}
  
  void forceResize() { resizeEvent(0); }                    // Sorry last minute hack

protected:
  virtual void resizeEvent ( QResizeEvent *e );
};

class KJotsMain : public KTopLevelWidget
{
  Q_OBJECT
public:
  KJotsMain( const char* name = NULL );
  virtual ~KJotsMain();

signals:
  void entryMoved(int);
  void entryChanged(int);
  void folderChanged(QList<TextEntry> *);

public slots:
  void saveFolder();

protected slots:
  void openFolder(int id);
  void createFolder();
  void deleteFolder();
  void nextEntry();
  void prevEntry();
  void newEntry();
  void deleteEntry();
  void barMoved(int);
  void addToHotlist();
  void removeFromHotlist();
  void toggleSubjList();
  void configure();
  void configureHide();
  void updateConfiguration();
  void writeBook();
  void writePage();
  void moveFocus();
  void configureKeys();
  void copySelection();
  
protected:
  virtual void resizeEvent ( QResizeEvent *e );
  int readFile(QString name);
  int writeFile( QString name);
  void writeEntry( QTextStream &st, TextEntry *entry );
  void saveProperties(KConfig*);

  QGridLayout   *top2bottom;
  QBoxLayout    *bg_bot_layout;
  QGridLayout   *bg_top_layout;
  QGridLayout   *labels_layout;
  QGridLayout   *f_text_layout;
  KAccel        *keys;

  QList<TextEntry> entrylist;
  int              current;
  bool             folderOpen;
  QString          current_folder_name;
  QStrList         folder_list;
  QStrList         hotlist;
  QList<QPushButton> button_list;
  QPopupMenu      *folders;
  QFont            button_font;
  int              unique_id;
  SubjList        *subj_list;
  ConfDialog      *confdiag;

  KToolBar        *toolbar;
  QFrame          *f_text;
  QFrame          *f_labels;
  QFrame          *f_main;
  MyButtonGroup   *bg_top;
  KMenuBar        *menubar;
  QScrollBar      *s_bar;
  QMultiLineEdit  *me_text;
  QLabel          *l_folder;
  QLineEdit       *le_subject;
};
#endif // KJotsMain_included



