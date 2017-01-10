//
//  kjots
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de chris@kde.org
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

#include <kapp.h>

#include "SubjList.h"
#include "KJotsMain.h"

SubjList::SubjList( QWidget*, const char* )
{
  initMetaObject();

  lb_subj = new QListBox(this);
  lb_subj->setFocus();
  connect( lb_subj, SIGNAL(highlighted(int)), this, SLOT(highlighted(int)) );
  b_ok = new QPushButton(klocale->translate("Close"), this);
  b_ok->setFixedSize(54, 24);
  top2bottom = new QGridLayout( this, 2, 1, 4 );
  top2bottom->addWidget(lb_subj, 0, 0, AlignCenter);
  top2bottom->setRowStretch(0,1);
  top2bottom->addWidget(b_ok, 1, 0, AlignCenter);
  top2bottom->activate();

  connect( b_ok, SIGNAL(clicked()), this, SLOT(hideMyself()) );
  connect( lb_subj, SIGNAL(selected(int)), this, SLOT(hideMyself()) );
}

void SubjList::rebuildList( QList<TextEntry> * new_list )
{
  lb_subj->clear();
  lb_subj->setUpdatesEnabled(FALSE);
  entrylist = new_list;
  QListIterator<TextEntry> it(*entrylist);
  it.toFirst();
  TextEntry *item;
  for ( ; (item = it.current()); ++it ) 
    {
      lb_subj->insertItem(item->subject);
    }
  lb_subj->setUpdatesEnabled(TRUE);
  lb_subj->repaint();
}

void SubjList::select( int index )
{
  current = index;
  lb_subj->setCurrentItem(index);
}

void SubjList::entryChanged( const char * new_subj)
{
  QString temp = lb_subj->text(current);
  if( temp == (QString) new_subj )
    return;
  lb_subj->setAutoUpdate(FALSE);
  /*
  lb_subj->removeItem(current);            // this looks ridiculous but it's necessary
  lb_subj->insertItem(new_subj, current ); // changeItem does not work properly
  */
  lb_subj->changeItem(new_subj, current );
  lb_subj->setCurrentItem(current);
  lb_subj->setAutoUpdate(TRUE);
  lb_subj->repaint();
}
