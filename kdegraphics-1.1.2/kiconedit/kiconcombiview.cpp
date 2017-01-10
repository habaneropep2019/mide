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

// $Id: kiconcombiview.cpp,v 1.1 1998/10/16 15:26:36 tanghus Exp $

#include "kiconcombiview.h"
#include <kdirlistbox.h>
#include "kicondetaillist.h"
#include <kfileinfo.h>
#include "kiconfileview.h"
#include "debug.h"

#include <qpainter.h>
#include <qlistbox.h>

#include <qdir.h>

#include <kapp.h>

#ifdef Unsorted // the "I hate X.h" modus
#undef Unsorted
#endif

KIconCombiView::KIconCombiView(  FileView dirs,  FileView files, 
                         bool s, QDir::SortSpec sorting,
			 QWidget * parent, const char * name ) 
    : KNewPanner(parent, name, KNewPanner::Vertical) , 
      KFileInfoContents(s,sorting)
{
    setSorting( QDir::Unsorted ); // we sort in the part views

    switch (dirs) {
	
    case DirList:
	dirList = new KDirListBox(s, sorting, this, "_dirs");
	break;
    case Custom:
	dirList = getDirList();
	break;
    default:
	fatal("this view for dirs not support");
    }

    switch (files) {
	
    case DetailView:
	fileList = new KIconDetailList(false, sorting, this, "_detail");
	break;
    case SimpleView:
	fileList = new KIconFileView(false, sorting, this, "_simple");
	break;
    case DirList: // who ever wants this
	fileList = new KDirListBox(false, sorting, this, "_dirs");
	break;
    case Custom:
	fileList = getFileList();
    }

    bool showListLabels =  
	(kapp->getConfig()->readNumEntry("ShowListLabels", 1) != 0);
    
    if (showListLabels) {
	setLabels(i18n("Folders:"),
		  i18n("Contents:"));
	showLabels(showListLabels);
    }
    
    bool dirsLeft = kapp->getConfig()->readNumEntry("ShowDirsLeft", 1);

    if (dirsLeft)
	activate(dirList->widget(), fileList->widget());
    else
	activate(fileList->widget(), dirList->widget());

    int pan = kapp->getConfig()->readNumEntry("PannerPosition", 30);
    setSeparatorPos(pan);

    dirList->connectDirSelected(this, SLOT(dirActivated(KFileInfo*)));
    fileList->connectFileSelected(this, SLOT(fileActivated(KFileInfo*)));
    fileList->connectFileHighlighted(this, SLOT(fileHighlighted(KFileInfo*)));
}

KIconCombiView::~KIconCombiView() 
{
    KConfig *c = kapp->getConfig();
    QString oldgroup = c->group();
    c->writeEntry("PannerPosition", separatorPos(), true, true);
    c->setGroup(oldgroup);
}

void KIconCombiView::setAutoUpdate(bool f)
{
    fileList->setAutoUpdate(f);
    dirList->setAutoUpdate(f);
}

bool KIconCombiView::insertItem(const KFileInfo *i, int) 
{
    if (i->isDir()) // for dirs the index is correct
	dirList->addItem(i);
    else
	fileList->addItem(i);

    return true;
}

void KIconCombiView::clearView()
{
    dirList->clear();
    fileList->clear();
}

void KIconCombiView::highlightItem(unsigned int)
{
    warning("KIconCombiView::highlightItem: does nothing");
}

void KIconCombiView::setCurrentItem(const char *item, const KFileInfo *i)
{
    if (item != 0) {
	i = 0;
	debug("setCurrentItem %s",item);
	for (uint j = 0; j < count(); j++)
	    if (at(j)->fileName() == item)
		i = at(j);
    }
    
    if (!i) {
	warning("setCurrentItem: no match found.");
	return; 
    }

    if (i->isDir())
	dirList->setCurrentItem(0, i);
    else
	fileList->setCurrentItem(0, i);
    
}

void KIconCombiView::repaint(bool f)
{
    dirList->repaint(f);
    fileList->repaint(f);
}

QString KIconCombiView::findCompletion( const char *base, bool )
{
    // first try the files.
    QString found = fileList->findCompletion(base);

    // if no file completion possible, try the dirs
    if (!found) {
	found = dirList->findCompletion(base, true);
    } else {
        // complete in dirList so possible directories are highlighted
        QString foundDir = dirList->findCompletion(base, false);
        // if we find anything -> highlight it and least common denominator
        // is the last result
	if ( foundDir ) {
	    unsigned int i;
            for ( i=1; (i<=found.length()) && (i<=foundDir.length()) ; i++) {
                if ( strncmp( found, foundDir, i) != 0 )
                    break;
            }
            if ( i==1 ) i++;
	    found.truncate(i-1);
        }
    }
    return found;
}

void KIconCombiView::dirActivated(KFileInfo *i) 
{
    select(i);
}

void KIconCombiView::fileActivated(KFileInfo *i)
{
    select(i);
}

void KIconCombiView::fileHighlighted(KFileInfo *i)
{
    highlight(i);
}


