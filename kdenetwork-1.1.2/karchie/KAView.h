/* -*- c++ -*- */
/***************************************************************************
 *                                 KAView.h                                *
 *                            -------------------                          *
 *                         Header file for KArchie                         *
 *                  -A programm to display archie queries                  *
 *                                                                         *
 *                KArchie is written for the KDE-Project                   *
 *                         http://www.kde.org                              *
 *                                                                         *
 *   Copyright (C) Oct 1997 Jörg Habenicht                                 *
 *                  E-Mail: j.habenicht@europemail.com                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          * 
 *                                                                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
 *                                                                         *
 ***************************************************************************/

#ifndef KAVIEW_H
#define KAVIEW_H

#include <qdialog.h>
//#include <qlistbox.h> 
#include <ktablistbox.h>
//#include <qframe.h> 
#include <qgrpbox.h> 

#include "KAQuery.h"

class QBoxLayout;
class QLabel;
//class QGroupBox;
class QFrame;
class KAViewLEdit;
class KAViewList;
class KLined;
class QPixmap;
class QPopupMenu;

class KAViewSearchterm: public QGroupBox
{
  // class with one Lineeditor to get the searchterm.
  // is not vertical sizeable, is horizontal sizeable
Q_OBJECT
public:
KAViewSearchterm( QWidget *parent=0, const char *name=0 );
~KAViewSearchterm();

  const char *getText() const;

signals:
  void sigTextSelected();
  
protected:
  void drawContents( QPainter * );

private:

  KLined *searchterm;
  QLabel *searchlabel;
  //  QGroupBox *searchbox;
  //  QPixmap *searchFilePix, *searchPathPix, *searchHostPix;

  static const int offsetwidth;
  static const char searchFilePixName[];
  static const char searchPathPixName[];
  static const char searchHostPixName[];

private slots:
  void slotReturnPressed() { /*debug("KAViewSearchterm::slotReturnPressed");*/ emit sigTextSelected(); }

};

class KAViewList: public KTabListBox
{
Q_OBJECT
public:
KAViewList( QWidget *parent=0, const char *name=0, WFlags f=0 );
~KAViewList();

  /* lists the files from the query result */
  void displayFileList( KAQueryList &fileList );

signals:
  void sigOpenFileSelected();
  void sigOpenDirSelected();
  void sigGetFileSelected();

protected:
  //  virtual void mousePressEvent(QMouseEvent *);

private:
  QPopupMenu *filemenu;

private slots:
  void slotPopupMenu(int, int);
  void slotFileOpen();
  void slotFileOpenDir();
  void slotFileGet();

};

class KAViewFiletype: public QFrame
{
  // class with host, path and filename, filesize, filemode, filename.
  // is not vertical sizeable, is horizontal sizeable
Q_OBJECT
public:
KAViewFiletype( QWidget *parent=0, const char *name=0, WFlags f=0, bool allowLines=TRUE );
~KAViewFiletype();

  void displayQueryFile( const KAQueryFile &rFile );
  void setFileDateWidth( int width )
  { filedatewidth = width; }
  
protected:
  void drawContents( QPainter * );

private:

  QLabel *filename,
    *filesize,
    *filemode,
    *filedate,
    *path,
    *host;
  QLabel *filenamelabel,
    *filesizelabel,
    *filemodelabel,
    *filedatelabel,
    *pathlabel,
    *hostlabel;
  QFrame *filenamebox,
    *filesizebox,
    *filemodebox,
    *filedatebox,
    *pathbox,
    *hostbox;

  int filedatewidth;

  static const int offsetwidth = 5;
  static const int boxoffsetwidth = 2;
};


class KAView : public QDialog // Fenster zum rummalen
{
Q_OBJECT
public:
KAView( QWidget *parent=0, const char *name=0, bool modal=FALSE, WFlags f=0 );
~KAView();

  KAViewSearchterm &getSearchterm() { return *searchterm; }
  KAViewList &getList() { return *list; }
  KAViewFiletype &getFiletype() { return *filetype; }

  /* calls KAViewList::displayFileList(...) */
  void newFileList( KAQueryList &fileList );

public slots:
  void slotShowFileDiscriptor( bool );
  void slotListFileSelected( int index );
  inline void slotListFileSelected( int index, int /*column*/ );

private:

  QBoxLayout  *box;
  KAViewSearchterm *searchterm;
  KAViewList  *list;
  KAViewFiletype *filetype;

  KAQueryList *fList;
};

void 
KAView::slotListFileSelected( int index, int /*column*/ )
{ slotListFileSelected(index); }

#endif
