/***************************************************************************
 *                                KAView.cpp                               *
 *                            -------------------                          *
 *                         Source file for KArchie                         *
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

#include "KAView.moc"
#include "KAQueryFile.h"

#include <kapp.h>      // needed for i18n()
#include <stdio.h>
#include <unistd.h>
#include <qlayout.h> 
#include <qdialog.h> 
#include <qlabel.h> 
#include <qgrpbox.h> 
#include <klined.h> 
#include <qtooltip.h>
#include <qpopmenu.h>
//#include <qpixmap.h>

#define COL_NUM_IN_VIEW 3
#define LIST_SEPARATOR '|'

/* Static members */
const int KAViewSearchterm::offsetwidth = 5;
const char KAViewSearchterm::searchFilePixName[] = "/usr/local/kde/share/icons/mini/document.xpm";
const char KAViewSearchterm::searchPathPixName[] = "/usr/local/kde/share/icons/mini/folder.xpm";
const char KAViewSearchterm::searchHostPixName[] = "/usr/local/kde/lib/pics/toolbar/home.xpm";



//KAViewSearchterm::KAViewSearchterm( QWidget *parent=0, const char *name=0, WFlags f=0, bool allowLines=TRUE )
KAViewSearchterm::KAViewSearchterm(QWidget *parent, const char *name)
  //  :QGroupBox( parent, name, f, allowLines )
  :QGroupBox( parent, name )
{
  setFrameStyle( Box|Raised );

  // setup the searchterm line editor
  searchterm = new KLined( this, "searchtermled" );
  searchterm->adjustSize();
  QToolTip::add( searchterm, i18n("Searchstring, press <ENTER> to start search") );

  searchlabel = new QLabel( searchterm, i18n("Search &Term"), this, "searchlabel" );
  searchlabel->adjustSize();
  searchlabel->move( offsetwidth, searchterm->height()/2 - searchlabel->height()/2 + offsetwidth );

  searchterm->move( offsetwidth*2 + searchlabel->width(), offsetwidth );

  // fix the size of this() to the lineeditor
  setFixedHeight( offsetwidth*2 + searchterm->height() );

  // fix the length of the lineeditor to the widget
  //  searchterm->resize( contentsRect().width() - offsetwidth*2 - searchlabel->width(), searchterm->height() );
  //  searchterm->resize( frameRect().width(), searchterm->height() );

  // load the pixmaps
  /*
  searchFilePix = new QPixmap(searchFilePixName);
  searchPathPix = new QPixmap(searchPathPixName);
  searchHostPix = new QPixmap(searchHostPixName);
  */

  connect(searchterm, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
}

KAViewSearchterm::~KAViewSearchterm()
{
  delete searchterm;
  delete searchlabel;
  /*
  delete searchFilePix;
  delete searchPathPix;
  delete searchHostPix;
  */
}

const char *
KAViewSearchterm::getText() const
{
  return searchterm->text();
}

void 
KAViewSearchterm::drawContents( QPainter * )
{
  // fix the length of the lineeditor to the widget
  searchterm->resize( contentsRect().width() - offsetwidth*2 - searchlabel->width(), searchterm->height() );

  /*
  searchterm->resize( contentsRect().width() - offsetwidth*3 - searchlabel->width() - searchFilePix->width() - searchPathPix->width() - searchHostPix->width(), searchterm->height() );

  int pixheight = offsetwidth;
  int ntmp = searchlabel->width() + offsetwidth + searchterm->width();
  searchFilePix->move( ntmp, pixheight );
  ntmp += searchFilePix->width() + offsetwidth;
  searchPathPix->move( ntmp, pixheight );
  ntmp += searchPathPix->width() + offsetwidth;
  searchHostPix->move( ntmp, pixheight );
  */  
}


KAViewList::KAViewList(QWidget *parent, const char *name, WFlags f)
  :KTabListBox( parent, name, COL_NUM_IN_VIEW, f )
{
  // initiale breite der Tab-Felder herausfinden
  //  QFontMetrics *fm = &fontMetrics();

  const char *tablabel1 = i18n("Host");
  const char *tablabel2 = i18n("Path");
  const char *tablabel3 = i18n("File");

  //    width = fm->boundingRect(fileList.at(i)->getDateTime().toString()).width();
  //  fm->boundingRect(tablabel1).width()

  // a simple width() doesn't work,
  // so get the initial size from kappmainwidget
  int tmp = kapp->topWidget()->width()/5;
  setColumn(0, tablabel1, tmp );
  setColumn(1, tablabel2, tmp*2 );
  setColumn(2, tablabel3, tmp*2 );
  //  readConfig();

  setSeparator(LIST_SEPARATOR);

  filemenu = new QPopupMenu();
  CHECK_PTR( filemenu );
  //  filemenu->insertItem( i18n("O&pen in editor"), SLOT(slotFileOpen()) );
  filemenu->insertItem( i18n("&Open Dir"), SLOT(slotFileOpenDir()) );
  filemenu->insertItem( i18n("&Download..."), SLOT(slotFileGet()) );

  connect ( this, SIGNAL(popupMenu(int,int)), SLOT(slotPopupMenu(int, int)));
  //  connect ( this, SIGNAL(selected(int,int)), SLOT(slotPopupMenu(int, int)));

}

KAViewList::~KAViewList()
{
  delete filemenu;
}

void 
KAViewList::displayFileList( KAQueryList &fileList )
{
  clear();
  setAutoUpdate ( FALSE );

  KAQueryFile *file;
  QString tmp;
  //  tmp.setNum( fileList.count() );
  //  tmp += " insert file in viewlist";
  //  debug( tmp );
  int i, count = fileList.count();
  for ( i=0; i<count; i++ ) {
    //    fprintf(stderr,"[%i] ", fileList.at());
    file = fileList.at(i);
    tmp = "";
    tmp += file->getHost();
    tmp += LIST_SEPARATOR;
    tmp += file->getPath();
    tmp += LIST_SEPARATOR;
    tmp += file->getFile();
    insertItem( tmp );
  }

  setAutoUpdate( TRUE );
  setCurrentItem( 0 );
  repaint(/*FALSE*/);
}

/*
void 
KAViewList::mousePressEvent(QMouseEvent *e)
{
*/
  /* Though I can't use the signal popupmenu(int,int)
   * properly, I have to catch the mouse signal on my own.
   * 
   * If the right mouse button has been pressed, process it,
   * else give to parent class.
   */
/*
debug("Mouse event occured");
  if (RightButton == e->button()) {
    QPoint p (e->pos());
    // get the row under the mousepointer and highlight it
    setCurrentItem (p.y());
    // show the menu, where the mouse has been before
    filemenu->popup(p);
  }
  else
    KTabListBox::mousePressEvent(e);
}
*/

void
KAViewList::slotPopupMenu(int row, int col)
{
  /* show popup menu like file->menu
   */
  //  debug("popup listmenu");
  //highlight the current row
  setCurrentItem (row);
  // get the position of the row
  int x, y; // widget coordinates
  if (colXPos(col , &x) && rowYPos(row, &y)) {
    QPoint p(x,y);
  //  filemenu->exec();
    filemenu->popup(mapToGlobal(p));
  }
}

void 
KAViewList::slotFileOpen()
{
  //  debug ("fileopen sel");
  emit sigOpenFileSelected();
}

void 
KAViewList::slotFileOpenDir()
{
  //  debug("fileopendir sel");
  emit sigOpenDirSelected();
}

void 
KAViewList::slotFileGet()
{
  //  debug("fileget sel");
  emit sigGetFileSelected();
}

KAViewFiletype::KAViewFiletype(QWidget *parent, const char *name, WFlags f, bool allowLines)
  :QFrame( parent, name, f, allowLines ),
   filedatewidth(160)
{
  int height; // used for computing the position of the labels and heigth over all

  setFrameStyle( Box|Raised );
  
  // setup the labels
  hostlabel = new QLabel( i18n("Host:"), this, "hostlabel" );
  hostlabel->adjustSize();
  pathlabel = new QLabel( i18n("Path:"), this, "pathlabel" );
  pathlabel->adjustSize();
  filenamelabel = new QLabel( i18n("File:"), this, "filenamelabel" );
  filenamelabel->adjustSize();
  filesizelabel = new QLabel( i18n("Size:"), this, "filesizelabel" );
  filesizelabel->adjustSize();
  filemodelabel = new QLabel( i18n("Mode:"), this, "filemodelabel" );
  filemodelabel->adjustSize();
  filedatelabel = new QLabel( i18n("Date:"), this, "filedatelabel" );
  filedatelabel->adjustSize();

  // get the height of a label for sizing the boxes below
  height = filedatelabel->height();

  // setup the boxes
  // size the frame boxes to the inside labels
  hostbox = new QFrame( this, "hostbox" );
  hostbox->setFrameStyle( Panel | Sunken );
  hostbox->setFixedHeight( height + 2*boxoffsetwidth );
  pathbox = new QFrame( this, "pathbox" );
  pathbox->setFixedHeight( height + 2*boxoffsetwidth );
  pathbox->setFrameStyle( Panel | Sunken );
  filenamebox = new QFrame( this, "filenamebox" );
  filenamebox->setFrameStyle( Panel | Sunken );
  filenamebox->setFixedHeight( height + 2*boxoffsetwidth );
  filesizebox = new QFrame( this, "filesizebox" );
  filesizebox->setFrameStyle( Panel | Sunken );
  filesizebox->setFixedHeight( height + 2*boxoffsetwidth );
  filemodebox = new QFrame( this, "filemodebox" );
  filemodebox->setFrameStyle( Panel | Sunken );
  filemodebox->setFixedHeight( height + 2*boxoffsetwidth );
  filedatebox = new QFrame( this, "filedatebox" );
  filedatebox->setFrameStyle( Panel | Sunken );
  filedatebox->setFixedHeight( height + 2*boxoffsetwidth );

  // setup the fileinformation labels
  // autoresize the labels: contents change often, and 
  // move 'em a bit into the framebox
  // maybe just size then to the length of the boxes -> less computing ??
  host = new QLabel( "", hostbox, "host" );
  host->setAutoResize( TRUE );
  host->move( boxoffsetwidth, boxoffsetwidth );
  path = new QLabel( "", pathbox, "path" );
  path->setAutoResize( TRUE );
  path->move( boxoffsetwidth, boxoffsetwidth );
  filename = new QLabel( "", filenamebox, "filename" );
  filename->setAutoResize( TRUE );
  filename->move( boxoffsetwidth, boxoffsetwidth );
  filesize = new QLabel( "", filesizebox, "filesize" );
  filesize->setAutoResize( TRUE );
  filesize->move( boxoffsetwidth, boxoffsetwidth );
  filemode = new QLabel( "----------", filemodebox, "filemode" );
  filemode->setAutoResize( TRUE );
  filemode->move( boxoffsetwidth, boxoffsetwidth );
  filedate = new QLabel( "", filedatebox, "filedate" );
  filedate->setAutoResize( TRUE );
  filedate->move( boxoffsetwidth, boxoffsetwidth );

  // move the boxes and the outside labels on the widget
  height = offsetwidth;
  hostlabel->move( offsetwidth, height + boxoffsetwidth );
  hostbox->move( hostlabel->width() + 2*offsetwidth, height );
  height += hostbox->height() + offsetwidth;
  pathlabel->move( offsetwidth, height + boxoffsetwidth );
  pathbox->move( pathlabel->width() + 2*offsetwidth, height );
  height += pathbox->height() + offsetwidth;
  filenamelabel->move( offsetwidth, height + boxoffsetwidth );
  filenamebox->move( filenamelabel->width() + 2*offsetwidth, height );
  height += filenamebox->height() + offsetwidth;
  filesizelabel->move( offsetwidth, height + boxoffsetwidth );
  filesizebox->move( filesizelabel->width() + 2*offsetwidth, height );
  height += filesizebox->height() + offsetwidth;

  filemodebox->setFixedWidth( filemode->width() + offsetwidth );
  // moving of filedatebox & filemodebox and
  // resizing of the boxes are done by drawContents

  // fix the height of the widget including all the boxes and labels
  setFixedHeight( height );

}

KAViewFiletype::~KAViewFiletype()
{
  delete filenamelabel;
  delete filesizelabel;
  delete filemodelabel;
  delete filedatelabel;
  delete pathlabel;
  delete hostlabel;

  delete filename;
  delete filesize;
  delete filemode;
  delete filedate;
  delete path;
  delete host;
 
  delete filenamebox;
  delete filesizebox;
  delete filemodebox;
  delete filedatebox;
  delete pathbox;
  delete hostbox;

}

void 
KAViewFiletype::displayQueryFile( const KAQueryFile &rFile )
{
  //  debug(rFile.getHost());
  host->setText( rFile.getHost() );
  //  debug(rFile.getPath());
  path->setText( rFile.getPath() );
  //  debug(rFile.getFile());
  filename->setText( rFile.getFile() );
  filesize->setNum( rFile.getSize() );
  filemode->setText( rFile.getMode() );
  filedate->setText( rFile.getDateTime().toString() );

  filenamebox->update();
  filesizebox->update();
  filemodebox->update();
  filedatebox->update();
  pathbox->update();
  hostbox->update();
}

void 
KAViewFiletype::drawContents( QPainter * )
{
  const int contwidth = contentsRect().width();
  const int labely = filesizelabel->y();
  const int boxy = filesizebox->y();
  const int boxheight = filesizebox->height();
  const int modewidth = filemodelabel->width() + offsetwidth + filemodebox->width();

  /* mode gets a fixed width
   * each label of size, date gets a half
   * (minus mode width) of the actual width
   * date gets a fixed width if the width goes below a size,
   * filesizebox gets the remaining space */
  int remsize = (contwidth - modewidth - filesizelabel->width() - filedatelabel->width() - 5*offsetwidth)/2;
  if ( remsize<filedatewidth )
    remsize = filedatewidth;
  filedatebox->resize( remsize, boxheight );
  //  fprintf(stderr,"filedatebox->width %i\n",filedatebox->width());
  const int datewidth = filedatelabel->width() + offsetwidth + filedatebox->width();
  filesizebox->resize( contwidth - modewidth - datewidth - filesizelabel->width() - 4*offsetwidth, boxheight );
  filemodelabel->move( filesizebox->x() + filesizebox->width() + offsetwidth,
		       labely );
  filemodebox->move( filemodelabel->x() + filemodelabel->width() + offsetwidth,
		     boxy );
  filedatelabel->move(filemodelabel->x() + modewidth + offsetwidth,
		       labely );
  filedatebox->move( filedatelabel->x() + filedatelabel->width() + offsetwidth,
		     boxy );

  // resize the boxes to the widget length
  hostbox->resize( contwidth - hostlabel->width() - 2*offsetwidth,
		   boxheight );
  pathbox->resize( contwidth - pathlabel->width() - 2*offsetwidth,
		   boxheight );
  filenamebox->resize( contwidth - filenamelabel->width() - 2*offsetwidth,
		       boxheight );
  /*  int modex = filemodelabel->x();
   filesizebox->resize( modex - filesizelabel->x() - filesizelabel->width() - 2*offsetwidth,
		       boxheight );*/
  /*  int datex = filedatelabel->x();
      filemodebox->resize( *//*datex - modex - filemodelabel->width() - 2*offsetwidth*/ /* 0,
		       boxheight );
  filedatebox->resize( contwidth - datex - filedatelabel->width() - 2*offsetwidth,
		       boxheight );
		       */
}



KAView::KAView(QWidget *parent, const char *name, bool modal, WFlags f)
  : QDialog( parent, name, modal, f )
{

  // setup searchterm line editor
  searchterm = new KAViewSearchterm( this, "viewsearchterm" );
  // lineeditor shouldn't grow vertical, only horiontal
  //  searchterm->setFixedHeight( 120 );

  // setup list
  list = new KAViewList( this, "viewlist" );

  // setup fileview
  //  config = KApplication::getKApplication()->getConfig();  
  slotShowFileDiscriptor(KApplication::getKApplication()
			 ->getConfig()
			 ->readBoolEntry("FAttr", true));
  //  filetype = new KAViewFiletype( this, "viewfiletype" );

  //  box = new QBoxLayout( this, QBoxLayout::Down );
  //  box->addWidget( searchterm );
  //  box->addWidget( list );
  //  box->addWidget( filetype );

  connect( list, SIGNAL(highlighted(int,int)), this, SLOT(slotListFileSelected(int,int)));
}

KAView::~KAView()
{
  delete box;
  delete searchterm;
  delete list;
  if ( filetype != 0 )
    delete filetype;
}

void 
KAView::newFileList( KAQueryList &fileList )
{
  fList = &fileList;
  // max breite des filedate Feldes herausfinden
  //QFontMetrics *fm = &fontMetrics();
  int width, maxwidth = 0, i, count = fileList.count();
  if ( count > 150 ) // don't count too much lines
    count = 150;
  for ( i=0; i<count; i++ ) {
    width = fontMetrics().boundingRect(fileList.at(i)->getDateTime().toString()).width();
    if ( width > maxwidth )
      maxwidth = width;
  }
  //debug("filetype is %ld", (long)filetype);
  if (filetype)
    filetype->setFileDateWidth( maxwidth );
  list->displayFileList( fileList );
}

void 
KAView::slotShowFileDiscriptor( bool showit )
{
  // if show true, show it to the world
  if ( showit ){
    // erase the boxlayout
    if (box)
      delete box;
    // and do a new with the filetype
    box = new QBoxLayout( this, QBoxLayout::Down );
    box->addWidget( searchterm );
    box->addWidget( list );
    // construct a new filetypeview if nessesary
    if ( filetype == 0 )
        // setup fileview
      filetype = new KAViewFiletype( this, "viewfiletype" );
    box->addWidget( filetype );
    int currentitem = list->currentItem();
    if (-1 != currentitem)
      filetype->displayQueryFile( *(fList->at(currentitem)) );
    filetype->show();
  }
  else{
    // erase filetypeview
    if ( filetype != 0 ){
      delete filetype;
      filetype = 0;
    }
    // erase the boxlayout
    if (box)
      delete box;
    // and do a new without the filetype
    box = new QBoxLayout( this, QBoxLayout::Down );
    box->addWidget( searchterm );
    box->addWidget( list );
  }
  box->activate();
  searchterm->show();
  list->show();
  show();
}

void 
KAView::slotListFileSelected( int index )
{
  if (filetype)
    filetype->displayQueryFile( *(fList->at( index )));
}

