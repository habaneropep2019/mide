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

#include "debug.h"
#include <qlayout.h>
#include "kiconfiledlg.h"
#include "kiconfileview.h"
#include "kiconcombiview.h"
#include "kicondetaillist.h"

KIconFileDlg::KIconFileDlg(const char *dirName, const char *filter,
				 QWidget *parent, const char *name, 
				 bool modal, bool acceptURLs)
    : KFileBaseDialog(dirName, filter, parent, name, modal, acceptURLs)
{
    init();
}

QString KIconFileDlg::getOpenFileName(const char *dir, const char *filter,
				     QWidget *parent, const char *name)
{
    QString filename;
    KIconFileDlg *dlg= new KIconFileDlg(dir, filter, parent, name, true, false);
    
    dlg->setCaption(i18n("Open"));
    
    if (dlg->exec() == QDialog::Accepted)
	filename = dlg->selectedFile();
    
    delete dlg;
    
    return filename;
}
 
QString KIconFileDlg::getSaveFileName(const char *dir, const char *filter,
				     QWidget *parent, const char *name)
{
    KIconFileDlg *dlg= new KIconFileDlg(dir, filter, parent, name, true, false);
    
    dlg->setCaption(i18n("Save As"));
    
    QString filename;
    
    if (dlg->exec() == QDialog::Accepted)
	filename= dlg->selectedFile();
    
    delete dlg;
    
    return filename;
}

QString KIconFileDlg::getOpenFileURL(const char *url, const char *filter,
				    QWidget *parent, const char *name)
{
    QString retval;
    
    KIconFileDlg *dlg = new KIconFileDlg(url, filter, parent, name, true, true);
    
    dlg->setCaption(i18n("Open"));
    
    if (dlg->exec() == QDialog::Accepted)
	retval = dlg->selectedFileURL();
    else
	retval = 0;
    
    delete dlg;
    if (!retval.isNull())
	debug("getOpenFileURL: returning %s", retval.data());
    
    return retval;
}

QString KIconFileDlg::getSaveFileURL(const char *url, const char *filter,
				    QWidget *parent, const char *name)
{
    QString retval;
    
    KIconFileDlg *dlg= new KIconFileDlg(url, filter, parent, name, true, true);
    
    dlg->setCaption(i18n("Save"));
    
    if (dlg->exec() == QDialog::Accepted)
	retval= dlg->selectedFileURL();
    
    delete dlg;
    
    return retval;
}

bool KIconFileDlg::getShowFilter() 
{
    return (kapp->getConfig()->readNumEntry("ShowFilter", 1) != 0);
}

KFileInfoContents *KIconFileDlg::initFileList( QWidget *parent )
{

    bool mixDirsAndFiles = 
	kapp->getConfig()->readBoolEntry("MixDirsAndFiles", false);
    
    bool showDetails = 
	(kapp->getConfig()->readEntry("ViewStyle", 
				      "SimpleView") == "DetailView");
    
    bool useSingleClick =
	kapp->getConfig()->readBoolEntry("SingleClick",true);
    
    QDir::SortSpec sort = static_cast<QDir::SortSpec>(dir->sorting() &
                                                      QDir::SortByMask);
                                                      
    if (kapp->getConfig()->readBoolEntry("KeepDirsFirst", true))
        sort = static_cast<QDir::SortSpec>(sort | QDir::DirsFirst);

    dir->setSorting(sort);    

    if (!mixDirsAndFiles)
	
        //return new KIconFileView(useSingleClick, dir->sorting(), parent, "_simple");

	return new KIconCombiView(KIconCombiView::DirList, 
				  showDetails ? KIconCombiView::DetailView 
				  : KIconCombiView::SimpleView,
				  useSingleClick, dir->sorting(),
				  parent, "_combi");
    else
	
	if (showDetails)
	    return new KIconDetailList(useSingleClick, dir->sorting(), parent, "_details");
	else
	    return new KIconFileView(useSingleClick, dir->sorting(), parent, "_simple");
    
}


