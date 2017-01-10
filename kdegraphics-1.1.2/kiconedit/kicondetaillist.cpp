// -*- c++ -*-
/*
    kiconedit - a small graphics drawing program for the KDE
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

#include <kdir.h>
#include "kicondetaillist.h"
#include "kiconfileview.h"
#include <qkeycode.h>
#include <kapp.h>
//#include "xview.h"

KListViewItem::KListViewItem( QListView * parent,
		   const char *a, const char *b,
		   const char *c, const char *d,
		   const char *e, const char *f,
		   const char *g, const char *h )
              :QListViewItem( parent, a, b, c, d, e, f, g, h)
{
}


KIconDetailList::KIconDetailList(bool s, QDir::SortSpec sorting, 
				 QWidget *parent, const char *name)
    : QListView(parent, name), KFileInfoContents(s,sorting)
{
  pixwidth = 0;

  QWidget::setFocusPolicy(QWidget::StrongFocus);

  addColumn("Pix", pixwidth);
  setColumnWidthMode(0, Manual);
  QFontMetrics fm = fontMetrics();
  QString text = i18n("Name");
  addColumn(text, QMAX(fm.width(text + "_"), 150));
  text = i18n("Size");
  addColumn(text, QMAX(fm.width(text + "_"), 50));
  setColumnAlignment(2, AlignRight);
  text = i18n("Permissions");
  addColumn(text, QMAX(fm.width(text + "_"), 80));
  text = i18n("Date");
  int _width = QMAX(fm.width(text + "_"), 50);
  addColumn(text, QMAX(_width, fm.width(KFileInfo::dateTime(0) + "_")));
  text = i18n("Owner");
  addColumn(text, QMAX(fm.width(text + "_"), 70));
  text = i18n("Group");
  addColumn(text, QMAX(fm.width(text + "_"), 70));
    
  setAllColumnsShowFocus(true);

  //setSortMode(Switching);
  QListView::setSorting(1);
    
  connect(this,SIGNAL(currentChanged(QListViewItem*)), SLOT(highlighted(QListViewItem*)));
  connect(this, SIGNAL(selectionChanged(QListViewItem*)), SLOT(selected(QListViewItem*)));
  connect(this, SIGNAL(doubleClicked(QListViewItem*)), SLOT(openItem(QListViewItem*)));

  //connect(this, SIGNAL(headerClicked(int)), SLOT(reorderFiles(int)));
}
/*
void KIconDetailList::repaint(bool)
{
    KTabListBox::repaint();
}
*/
void KIconDetailList::keyPressEvent( QKeyEvent *e)
{
    QListViewItem* oldRow = currentItem(), *newRow = 0L;
    
    switch( e->key() ) {
    case Key_Up:
	if( oldRow != firstChild() ) {
            newRow = oldRow->itemAbove();
	    QListView::setSelected(newRow, true);
            debug("File: %s", newRow->text(1));
	    KFileInfoContents::setCurrentItem(newRow->text(1));
            ensureItemVisible(newRow);
	}
	break;
    case Key_Down:
        newRow = oldRow->itemBelow();
        if(newRow)
        {
          QListView::setSelected(newRow, true);
          KFileInfoContents::setCurrentItem(newRow->text(1));
          ensureItemVisible(newRow);
        }
	break;          
    case Key_Enter:
    case Key_Return:
	select( fileinfos.find(currentItem()->text(1)) );
	break;
/*
    case Key_PageUp:
	if ( oldRow > 0 ) {
          if ( oldRow >= numVisible ) {
              KTabListBox::setCurrentItem(oldRow - numVisible);
              edge = topCell() - numVisible;
              if ( currentItem() - edge >= numVisible )
                  edge++;
              if ( edge >= 0)
                  setTopItem(edge);
              else
                  setTopItem(0);
          } else {
              KTabListBox::setCurrentItem(0);
              setTopItem(0);
          }
        } 
	break;
    case Key_PageDown:
	if ( oldRow < numRows()-1 ) {
          if ( oldRow < numRows()-numVisible-1 ) {
              KTabListBox::setCurrentItem(oldRow + numVisible);
              edge = lastRowVisible();
              if ( currentItem()+numVisible-1 >=  edge )
                  setTopItem( topCell() + numVisible );
              else
                  setTopItem( numRows() - numVisible );
          } else {
              KTabListBox::setCurrentItem(numRows()-1);
              setTopItem( numRows() - numVisible );
          }

        } 
	break;
*/
    default:       
	e->ignore(); 
	return;     
    }    

}

void KIconDetailList::focusInEvent ( QFocusEvent *)
{
  if (!currentItem())
   QListView::setCurrentItem(firstChild());
  ensureItemVisible(currentItem());
}

KIconDetailList::~KIconDetailList()
{
debug("KIconDetailList - destructor");
}

void KIconDetailList::setAutoUpdate(bool f)
{
    //KTabListBox::setAutoUpdate(f);
}

void KIconDetailList::highlightItem(unsigned int i)
{
/*
    KTabListBox::setCurrentItem(i);
    unsigned j = (KTabListBox::lastRowVisible()-KTabListBox::topItem())/2;
    j = (j>i) ? 0 : (i-j);
    KTabListBox::setTopItem( j );
*/
}

void KIconDetailList::clearView()
{
  QListView::clear();
  fileinfos.clear();
}


void KIconDetailList::reorderFiles(int inColumn)
{
/*
    QDir::SortSpec new_sort;
    
    // QDir::SortSpec oldFlags = sorting() & (~QDir::SortByMask);

    switch ( inColumn ) {
    case 1:
	new_sort = QDir::Name;
	break;
    case 2:  
	new_sort = QDir::Size;
	break;
    case 4:  
	new_sort = QDir::Time;
	break;
    default: 
	return;
    }
    
    setSorting(new_sort);
*/
}

bool KIconDetailList::insertItem(const KFileInfo *i, int index)
{
    fileinfos.insert(i->fileName(), i);

    const char *type;
    if (i->isDir())
	if (i->isReadable())
	    type = "folder";
	else
	    type = "l_folder";
    else
	if (i->isReadable())
	    type = "file";
	else
	    type = "l_file";
    
    QString s;
    s.setNum(i->size());

    KListViewItem *item = new KListViewItem(this,
                 "",
                 i->fileName(),
                 s.data(),
                 i->access(),
                 i->date(),
                 i->owner(),
                 i->group());

    QString path;
    if(type == "file")
    {
      item->setPixmap(0, *file_pixmap);
/*
      path = i->absURL();
      path += i->fileName();
      QPixmap pix;
      if(checkXVFile(i, path.data(), &pix))
        item->setPixmap(0, pix);
      else
        item->setPixmap(0, *pix_file);
*/
    }
    else if(type == "folder")
    {
        item->setPixmap(0, *folder_pixmap);
        //item->setPixmap(0, *pix_folder);
    }
    else if(type == "l_folder")
    {
        item->setPixmap(0, *locked_folder);
        //item->setPixmap(0, *pix_lfolder);
    }
    else if(type == "l_file")
    {
        item->setPixmap(0, *locked_file);
        //item->setPixmap(0, *pix_lfile);
    }

    if(pixwidth < item->pixmap(0)->width())
      pixwidth = item->pixmap(0)->width();

    setColumnWidth(0, pixwidth);

    item->invalidateHeight();
    item->setItemHeight(item->pixmap(0)->height()); // > fontMetrics().height() ? item->pixmap(0)->height() : fontMetrics().height());
    item->widthChanged(0);

    triggerUpdate();

    // TODO: find out, if a repaint is really necessary
    return true;
}

void KIconDetailList::selected(QListViewItem *item)
{
  setSelected(item, true);
  KFileInfoContents::setCurrentItem(item->text(1));
}

void KIconDetailList::highlighted(QListViewItem *item)
{
  QListView::setCurrentItem(item);
}

void KIconDetailList::openItem(QListViewItem *item)
{
  select( fileinfos.find(currentItem()->text(1)) );
}



