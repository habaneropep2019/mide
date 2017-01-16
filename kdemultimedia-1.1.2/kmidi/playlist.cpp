/*
   kmidi
   
   $Id$

   Copyright 1997 Bernd Johannes Wuebben math.cornell.edu

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

#include "playlist.h"
#include "playlist.moc"
#include <stdio.h>

#include "kmidi.h"


#define BORDER_WIDTH 2

int loc_max_namewidth=0;
int loc_max_monthwidth=0;

char fontrefstring1[]="012345678901234567890";
char fontrefstring2[]="01234567890";
char fontrefstring3[]="00  00:00";

extern KApplication *thisapp;
extern KMidi *kmidi;

PlaylistDialog::PlaylistDialog(QWidget *parent, const char *name,QStrList *playlist)
  : QDialog(parent, name,TRUE){

  setCaption(i18n("Compose Play List"));

  starting_up = true;
  QPopupMenu *file = new QPopupMenu;
  CHECK_PTR( file );

  file->insertItem( i18n("Quit"), this, SLOT(reject()));
  
  QPopupMenu *edit = new QPopupMenu;
  CHECK_PTR( edit );
  edit->insertItem( i18n("Add"), this, SLOT(addEntry()) );
  edit->insertItem( i18n("Remove"), this, SLOT(removeEntry()) );
  
  QPopupMenu *help = new QPopupMenu;
  CHECK_PTR( help );
  help->insertItem( i18n("Help"), this, SLOT(help()));

  menu = new QMenuBar( this );
  CHECK_PTR( menu );
  menu->insertItem( i18n("&File"), file );
  menu->insertItem( i18n("&Edit"), edit );
  menu->insertSeparator();
  menu->insertItem( i18n("&Help"), help );
  
  panner = new KPanner(this, "_panner", KPanner::O_VERTICAL, 30);

  connect(panner, SIGNAL(positionChanged()), this, SLOT(pannerHasChanged()));
  listbox = new QListBox(panner->child1(),"listbox",0); 

  connect(listbox, SIGNAL(selected(int)), this, 
	    SLOT(removeEntry()));

  addButton = new QPushButton(this,"addButton");
  addButton->setText(i18n("Add"));
  connect(addButton,SIGNAL(clicked()),this,SLOT(addEntry()));

  removeButton = new QPushButton(this,"removeButton");
  removeButton->setText(i18n("Remove"));
  connect(removeButton,SIGNAL(clicked()),this,SLOT(removeEntry()));

  cancelButton = new QPushButton(this,"cancelButton");
  cancelButton->setText(i18n(i18n("Cancel")));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(reject()));

  okButton = new QPushButton(this,"okButton");
  okButton->setText(i18n("OK"));
  connect(okButton,SIGNAL(clicked()),this,SLOT(checkList()));

  local_list = new QListBox(panner->child0(), "local_list",0);
  //local_list = new QListBox(this, "local_list",0);


  local_list->insertItem("Local Directory", -1);
  connect(local_list, SIGNAL(selected(int )), this, SLOT(local_file_selected(int )));
  
  set_local_dir(QString(""));    

  this->resize(PLAYLIST_WIDTH,PLAYLIST_HEIGHT);

  songlist = playlist;
  if (songlist)
    listbox->insertStrList(songlist,-1);

  if (listbox->count() == 0)
    loadPlaylist("default");
  
  current_playlist = "default";


};


PlaylistDialog::~PlaylistDialog(){
  

};



void MyListBoxItem::paint( QPainter *p ){
 
  p->drawPixmap( 3, 0, pm );
  QFontMetrics fm = p->fontMetrics();


  int yPos;                       // vertical text position

  if ( pm.height() < fm.height() )
    yPos = fm.ascent() + fm.leading()/2;
  else
    yPos = pm.height()/2 - fm.height()/2 + fm.ascent();

  p->drawText( pm.width() + 5, yPos, filename.data() );

}



int MyListBoxItem::height(const QListBox *lb ) const{

  return QMAX( pm.height(), lb->fontMetrics().lineSpacing() + 1 );
}



int MyListBoxItem::width(const QListBox *lb ) const{

  QFontMetrics fm = lb->fontMetrics();

  QRect ref = fm.boundingRect(filename.data());
  int ref1 = ref.width();
  return 6 + pm.width() + ref1;

}	  



void  PlaylistDialog::parse_fileinfo(QFileInfo* fi, MyListBoxItem* lbitem){
  
  lbitem->filename =fi->fileName();

  QString size;
  size.setNum(fi->size());
  lbitem->size = size;

  QDate date = fi->lastModified().date();

  QString monthname = date.monthName(date.month());
  lbitem->month = monthname;
   
  QTime time = fi->lastModified().time();
  
  lbitem->day_and_time.sprintf("%02d  %02d:%02d",date.day(),time.hour(),time.minute());

  QFontMetrics fm = local_list->fontMetrics();

  QRect ref1 = fm.boundingRect(fi->fileName());
  loc_max_namewidth  = QMAX(loc_max_namewidth,ref1.width());
  
  ref1 = fm.boundingRect(monthname);
  loc_max_monthwidth  = QMAX(loc_max_monthwidth,ref1.width());
}



void PlaylistDialog::local_file_selected(int index){

  QString dir = local_list->text((uint) index);

  //  printf("File:%s\n",cur_local_fileinfo.at((uint)index)->filePath());

  if( cur_local_fileinfo.at((uint)index)->isDir()){
    set_local_dir(dir);
    return;
  }

  listbox->insertItem(cur_local_fileinfo.at((uint)index)->filePath(),-1);
  
}



void PlaylistDialog::set_local_dir(QString dir){


  if (!dir.isEmpty()){
    if ( !cur_local_dir.setCurrent(dir.data())){
      QString str;
      str.sprintf(i18n("Can not enter directory: %s\n"),dir.data());
      QMessageBox::message(i18n("Sorry"),str.data(),i18n("OK"));
      return;
    }
  }

   cur_local_dir  = QDir::current();
   cur_local_dir.setSorting( cur_local_dir.sorting() | QDir::DirsFirst );  

  qApp ->setOverrideCursor( waitCursor );
  local_list ->setAutoUpdate( FALSE );
  local_list->clear();
  
  cur_local_fileinfo.clear();

  const QFileInfoList  *filist = cur_local_dir.entryInfoList();

  loc_max_namewidth = 0; // reset the fontmetric length of the longest filename
  loc_max_monthwidth = 0; // same for the month

  if ( filist ) {
    QFileInfoListIterator it( *filist );
    QFileInfo		 *fi = it.current();
    while ( fi && fi->isDir() ) {

      QString tmp = fi->fileName();

      if (strcmp(tmp.data(),"..") == 0 ){

	MyListBoxItem* mylistboxitem1 = 
	  new MyListBoxItem(fi->fileName().data(),kmidi->cdup_pixmap);

	parse_fileinfo(fi,mylistboxitem1);

	local_list->insertItem( mylistboxitem1 );

	cur_local_fileinfo.append(fi);

      }else{
	if (strcmp(tmp.data(),".") == 0){

	  // skip we don't want to see this.

	}else{
	  MyListBoxItem* mylistboxitem1 = new MyListBoxItem(fi->fileName().data()
							,kmidi->folder_pixmap);
	  parse_fileinfo(fi,mylistboxitem1);
	  local_list->insertItem( mylistboxitem1 );
	  cur_local_fileinfo.append(fi);
	}
      }

      // printf("directory:%s\n",fi->fileName().data());
      
      fi = ++it;
    }
    while ( fi ) {
      QString displayline;

      MyListBoxItem* mylistboxitem2 = new MyListBoxItem(fi->fileName().data()
							,kmidi->file_pixmap);
      parse_fileinfo(fi,mylistboxitem2);
      local_list->insertItem( mylistboxitem2 );
      cur_local_fileinfo.append(fi);
      //printf("file:%s\n",fi->fileName().data());
      fi = ++it;
    }
  } else {
    qApp->restoreOverrideCursor();
    QMessageBox::message( i18n("Sorry"), i18n("Cannot open or read directory.") );
    qApp ->setOverrideCursor( waitCursor );
  }
  local_list->setAutoUpdate( TRUE );
  local_list->repaint();
  
  //updatePathBox( d.path() );

  qApp->restoreOverrideCursor();
  
  //  printf("Current Dir Path: %s\n",QDir::currentDirPath().data());
}

void PlaylistDialog::pannerHasChanged(){

  //    resizeEvent(0);
  local_list->resize( panner->child0()->width(), panner->child0()->height());  
  listbox->resize( panner->child1()->width(), panner->child1()->height());

}


void PlaylistDialog::addEntry(){

  //  printf("File:%s\n",cur_local_fileinfo.at(local_list->currentItem())->filePath());

  if (local_list->currentItem() != -1)
    if(!cur_local_fileinfo.at(local_list->currentItem())->isDir()){
      listbox->insertItem(
	  cur_local_fileinfo.at(local_list->currentItem())->filePath(),-1);  
  }
}




void PlaylistDialog::removeEntry(){

  if (listbox->currentItem() != -1){
    //    printf("removing:%s:\n",listbox->text(listbox->currentItem()));
    listbox->removeItem(listbox->currentItem());

  }
}




void PlaylistDialog::checkList(){

  songlist->clear();
  if (listbox->count()){
    for(int index = 0; index < (int)listbox->count(); index ++){
      //      printf("Appending:%s:\n",listbox->text(index));
      songlist->append( listbox->text(index));
    }
  }

  savePlaylist();
  accept();
}



void PlaylistDialog::resizeEvent(QResizeEvent *e){

  int w = (e->size()).width() ;
  int h = (e->size()).height();
  
  panner->setGeometry( BORDER_WIDTH, menu->height() + BORDER_WIDTH ,
  	       w - 2*BORDER_WIDTH, h - 37 -  menu->height());

  panner->setSeparator(panner->getSeparator());

  local_list->resize( panner->child0()->width(), panner->child0()->height());  
  listbox->resize( panner->child1()->width(), panner->child1()->height());

  okButton->setGeometry(BORDER_WIDTH + 70 + 7 ,h - 28 ,70,25);  
  removeButton->setGeometry(w - 70 -70 - 10 - BORDER_WIDTH ,h - 28, 70,25);
  cancelButton->setGeometry(BORDER_WIDTH +2 ,h -28,70,25);  
  addButton->setGeometry(w - 73 - BORDER_WIDTH  ,h - 28,70,25);

} 


void PlaylistDialog::newPlaylist(){

}

void PlaylistDialog::openPlaylist(){

}

void PlaylistDialog::savePlaylist(){

  savePlaylistbyName(current_playlist);

}

void PlaylistDialog::savePlaylistbyName(QString name){


  if(name.isEmpty())
    return;


  QString home = QDir::homeDirPath();
  home = home + "/.kde/share/apps/kmidi";
  QDir savedir(home.data());

  if(!savedir.exists()){
    savedir.mkdir(home.data());
  }

  QString defaultlist;
  defaultlist = home + "/" + name;
  QFile f(defaultlist.data());

  f.open( IO_ReadWrite | IO_Translate|IO_Truncate);

  QString tempstring;

  for (int i = 0; i < (int)listbox->count(); i++){
    tempstring = listbox->text(i);
    tempstring += '\n';
    f.writeBlock(tempstring.data(),tempstring.length());
  }

  f.close();

}

void PlaylistDialog::saveasPlaylist(){



}

void PlaylistDialog::deletePlaylist(){

}

void PlaylistDialog::loadPlaylist(QString name){

     
  current_playlist = name;
  
  QString home = QDir::homeDirPath();
  home = home + "/.kde/share/apps/kmidi";
  
  QDir savedir(home.data());

  if(!savedir.exists()){
    savedir.mkdir(home.data());
  }

 QString defaultlist;
 defaultlist = home + "/" + current_playlist;
  
 QFile f(defaultlist.data());

 f.open( IO_ReadWrite | IO_Translate);

 char buffer[1024];
 QString tempstring;

 while(!f.atEnd()){
   buffer[0] = (char) 0;
   f.readLine(buffer,511);
   //printf("Found:%s\n",buffer);
   tempstring = buffer;
   tempstring = tempstring.stripWhiteSpace();
   if (!tempstring.isEmpty()){
     //    printf("Insertring:%s\n",tempstring.data());
     listbox->insertItem(tempstring,-1);
   }
 }

 f.close();
 
}

void PlaylistDialog::help(){

  if(!thisapp)
    return;

  thisapp->invokeHTMLHelp("kmidi/kmidi.html","");

}


