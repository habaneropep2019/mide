/**************************************************************************

    collectdlg.cpp  - The collections manager dialog  
    Copyright (C) 1998  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include "collectdlg.h"
#include <qpushbt.h>
#include <qlistbox.h>
#include <qlabel.h> 
#include <kfiledialog.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kurl.h>
#include "version.h"
#include "player/songlist.h"
#include "player/slman.h"
#include "kaskdlg.h"

CollectionDialog::CollectionDialog(SLManager *slm,int selC,QWidget *parent,const char *name) : QDialog(parent,name,TRUE)
{
setCaption(i18n("Collections Manager"));
ok=new QPushButton(i18n("OK"),this);
ok->setGeometry(140,200,100,30);
connect(ok,SIGNAL(clicked()),SLOT(accept()) );
cancel=new QPushButton(i18n("Cancel"),this);
cancel->setGeometry(250,200,100,30);
connect(cancel,SIGNAL(clicked()),SLOT(reject()) );

label=new QLabel(i18n("Available collections :"),this);
label->adjustSize();
label->move(10,10);
collections=new QListBox(this,"collectionlist");
collections->setGeometry(10,20+label->height(),340,90);
connect(collections,SIGNAL(highlighted(int)),SLOT(collectionselected(int)));
connect(collections,SIGNAL(selected(int)),SLOT(changeCollectionName(int)));
slman=slm;
for (int i=0;i<=slman->numberOfCollections();i++)
    {
    collections->insertItem(slman->getCollectionName(i),i);
#ifdef COLLECTDLGDEBUG
    printf("Name : %s\n",slman->getCollectionName(i));
#endif
    };
selectedC=selC;
#ifdef COLLECTDLGDEBUG
printf("selectedC : %d\n",selectedC);
#endif

label2=new QLabel(i18n("Songs in selected collection:"),this);
label2->adjustSize();
label2->move(10,collections->y()+collections->height()+10);

songs=new QListBox(this,"songlist");
songs->setGeometry(10,label2->y()+label2->height()+10,340,120);
connect(songs,SIGNAL(highlighted(int)),SLOT(songselected(int)));
currentsl=slman->getCollection(selectedC);
if (slman->numberOfCollections()>0) 
    {	
    collections->setCurrentItem(selectedC);
    collections->centerCurrentItem();
    };
//fillInSongList();
newC=new QPushButton(i18n("New"),this);
newC->adjustSize();
newC->move(360,collections->y()+5);
connect(newC,SIGNAL(clicked()),SLOT(newCollection()) );
copyC=new QPushButton(i18n("Copy"),this);
copyC->adjustSize();
copyC->move(360,newC->y()+newC->height()+5);
connect(copyC,SIGNAL(clicked()),SLOT(copyCollection()) );
deleteC=new QPushButton(i18n("Delete"),this);
deleteC->adjustSize();
deleteC->move(360,copyC->y()+copyC->height()+5);
connect(deleteC,SIGNAL(clicked()),SLOT(deleteCollection()) );

addS=new QPushButton(i18n("Add"),this);
addS->adjustSize();
addS->move(360,songs->y()+5);
connect(addS,SIGNAL(clicked()),SLOT(addSong()) );
delS=new QPushButton(i18n("Remove"),this);
delS->adjustSize();
delS->move(360,addS->y()+addS->height()+5);
connect(delS,SIGNAL(clicked()),SLOT(removeSong()) );

ok->move(ok->x(),songs->y()+songs->height()+10);
cancel->move(ok->x()+ok->width()+5,ok->y());

setMinimumSize(400,ok->y()+ok->height()+5);
//setMaximumSize(360,240);
};

void CollectionDialog::collectionselected(int idx)
{
selectedC=idx;
#ifdef COLLECTDLGDEBUG
printf("Selected collection : %d\n",selectedC);
#endif
currentsl=slman->getCollection(selectedC);
fillInSongList();
};

void CollectionDialog::fillInSongList(void)
{
QString qs;
songs->clear();
if (currentsl!=NULL)
   {
   currentsl->iteratorStart();
   int i=0;
   while (!currentsl->iteratorAtEnd())
       {
       qs=QString(currentsl->getIteratorName());
       KURL::decodeURL(qs);	
       songs->insertItem(qs,i);
       currentsl->iteratorNext();
       i++;    
       };
   songs->setCurrentItem(currentsl->getActiveSongID()-1);
   songs->centerCurrentItem();
   };
};

void CollectionDialog::songselected(int idx)
{
selectedS=idx;
currentsl->setActiveSong(idx+1);

#ifdef COLLECTDLGDEBUG
printf("Selected song : %d\n",selectedS);
#endif
};

void CollectionDialog::newCollection()
{
KAskDialog *dlg;
dlg=new KAskDialog(i18n("Enter the name of the new collection"),
			i18n("New Collection"),this,"kask");
if (dlg->exec()==QDialog::Accepted)
   {
   int i=slman->createCollection(KAskDialog::getResult());
   if (i==-1)
	{
	char s[500];
	sprintf(s,i18n("The name '%s' is already used"),
			KAskDialog::getResult());
	KMsgBox::message(this,i18n("Warning"),s);
	}
	else
	{
	collections->insertItem(KAskDialog::getResult(),i);
	collections->setCurrentItem(i);
        collections->centerCurrentItem();
	};
   };
};

void CollectionDialog::copyCollection()
{
SongList *src=currentsl;
KAskDialog *dlg;
int i;
dlg=new KAskDialog(i18n("Enter the name of the copy collection"),
			i18n("Copy Collection"),this,"kask");
if (dlg->exec()==QDialog::Accepted)
   {
   i=slman->createCollection(KAskDialog::getResult());
   if (i==-1)
	{
	char s[500];
	sprintf(s,i18n("The name '%s' is already used"),
			KAskDialog::getResult());
	KMsgBox::message(this,i18n("Warning"),s);
	}
	else
	{
	collections->insertItem(KAskDialog::getResult(),i);
	SongList *tgt=slman->getCollection(i);
	src->iteratorStart();
	while (!src->iteratorAtEnd())
	   {
	   tgt->AddSong(src->getIteratorName());
	   src->iteratorNext();
	   };
	collections->setCurrentItem(i);
        collections->centerCurrentItem();
	};
   };
};

void CollectionDialog::deleteCollection()
{
if (selectedC==0) return;
slman->deleteCollection(selectedC);
int i=selectedC;
collections->removeItem(selectedC);
collections->setCurrentItem(i);
collections->centerCurrentItem();
};

void CollectionDialog::changeCollectionName(int idx)
{
if (idx==0) return;
KAskDialog *dlg;
dlg=new KAskDialog(i18n("Enter the new name for the selected collection"),
			i18n("Change Collection Name"),this,"kask");
if (dlg->exec()==QDialog::Accepted)
   {
   if (slman->getCollection(KAskDialog::getResult())!=NULL)
	{
	char s[500];
	sprintf(s,i18n("The name '%s' is already used"),
			KAskDialog::getResult());
	KMsgBox::message(this,i18n("Warning"),s);
	}
	else
	{
	slman->changeCollectionName(idx,KAskDialog::getResult());
	collections->changeItem(KAskDialog::getResult(),idx);
	};
   };
};

void CollectionDialog::addSong()
{
if (currentsl==NULL) return;
char name[200];
name[0]=0;
QString filename;
filename=KFileDialog::getOpenFileName(0,"*.kar *.mid",this,name);
if (filename.isNull()) return;
int id=currentsl->AddSong(filename);
if (id==-1)
    {
    printf("Couldn't add song to collection\n");
    return;
    };
songs->insertItem(filename,id-1);
};

void CollectionDialog::removeSong()
{
if (currentsl==NULL) return;
currentsl->DelSong(selectedS+1);
int i=selectedS;
songs->removeItem(selectedS);
songs->setCurrentItem(i);
};

void CollectionDialog::resizeEvent(QResizeEvent *)
{
int maxw=newC->width();
if (copyC->width()>maxw) maxw=copyC->width();
if (deleteC->width()>maxw) maxw=deleteC->width();
if (addS->width()>maxw) maxw=addS->width();
if (delS->width()>maxw) maxw=delS->width();

newC->setGeometry(width()-maxw-5,newC->y(),maxw,newC->height());
copyC->setGeometry(width()-maxw-5,copyC->y(),maxw,copyC->height());
deleteC->setGeometry(width()-maxw-5,deleteC->y(),maxw,deleteC->height());

collections->resize(width()-maxw-20,(height()*35)/100);
label2->move(10,collections->y()+collections->height()+10);
songs->setGeometry(10,label2->y()+label2->height()+10,width()-maxw-20,height()-(label2->y()+label2->height()+10+ok->height()+10));
addS->setGeometry(width()-maxw-5,songs->y()+5,maxw,addS->height());
delS->setGeometry(width()-maxw-5,addS->y()+addS->height()+5,maxw,delS->height());

cancel->move(width()-cancel->width()-5,height()-cancel->height()-5);
ok->move(cancel->x()-5-ok->width(),height()-ok->height()-5);
};

int CollectionDialog::selectedC=0;
int CollectionDialog::selectedS=0;
