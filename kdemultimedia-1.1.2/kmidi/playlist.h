/*
   kModPlayer - Qt-Based Mod Player
   
   $Id$

   Copyright 1996 Bernd Johannes Wuebben math.cornell.edu

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef PLAY_LIST_H
#define PLAY_LIST_H

#include <qdatetm.h> 
#include <qlined.h>
#include <qdialog.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qstrlist.h> 
#include <qlistbox.h> 
#include <qpushbt.h>
#include <qfiledlg.h>
#include <qfile.h>
#include <qtstream.h> 
#include <qdir.h>
#include <qstrlist.h>
#include <qpainter.h>
#include <qapp.h>    
#include <qmsgbox.h>
#include <qpopmenu.h>
#include <qkeycode.h>
#include <qmenubar.h> 

#include <kpanner.h>

#include "kmidi.h"



class MyListBoxItem : public QListBoxItem{

public:

  MyListBoxItem( const char *s, const QPixmap p )
    : QListBoxItem(), pm(p)
    { setText( s ); }
  
protected:
  virtual void paint( QPainter * );
  virtual int height( const QListBox * ) const;
  virtual int width( const QListBox * ) const;
  virtual const QPixmap *pixmap() { return &pm; }
  
public:

  QString filename;
  QString size;
  QString month;
  QString day_and_time;
  QPixmap pm;
};


class PlaylistDialog : public QDialog {

Q_OBJECT

public:
       PlaylistDialog(QWidget *parent=0, const char *name = 0,QStrList *playlist = 0);
       ~PlaylistDialog();

private:
      void resizeEvent(QResizeEvent *e);

private slots:

  void openPlaylist();
  void newPlaylist();
  void saveasPlaylist();
  void help();
  void savePlaylist();
  void savePlaylistbyName(QString name);
  void deletePlaylist();
  void removeEntry();
  void addEntry();
  void checkList();
  void local_file_selected(int index);
  void pannerHasChanged();
  void loadPlaylist(QString name);


protected:
    void parse_fileinfo(QFileInfo*, MyListBoxItem*);
    void set_local_dir(QString dir);

public:
    QString current_playlist;

private:

    bool starting_up;  
    QDir cur_local_dir;
    QList<QFileInfo> cur_local_fileinfo;
    QListBox *local_list;
    KPanner *panner;
    QLabel *statusbar;

    QMenuBar *menu;    
    QListBox* listbox;
    QPushButton* addButton;
    QPushButton* okButton;
    QPushButton* removeButton;
    QPushButton* cancelButton;
    QStrList*  songlist;


};




#endif /* PLAY_LIST_H */
