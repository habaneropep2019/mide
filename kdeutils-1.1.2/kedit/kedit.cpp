    /*

    $Id: kedit.cpp,v 1.44.2.3 1999/07/02 17:19:05 porten Exp $

    Copyright (C) 1997 Bernd Johannes Wuebben
                       wuebben@math.cornell.edu

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

    */

#include <time.h>

#include <qfile.h>
#include <qstrlist.h>
#include <qpainter.h>
#include <qdir.h>
#include <qtabdlg.h>

#include "kedit.h"
#include "urldlg.h"
#include "filldlg.h"
#include "kcolordlg.h"
#include "mail.h"
#include "keditcl.h"
#include "kwm.h"

#include "kedit.moc"

#include <klocale.h>
#include <kstdaccel.h>
#include <kiconloader.h>
#include "version.h"

QList<TopLevel> TopLevel::windowList;

KApplication *mykapp;

int default_open = KEdit::OPEN_READWRITE;

TopLevel::TopLevel (QWidget *, const char *name)
    : KTopLevelWidget ( name)
{

  windowList.setAutoDelete( FALSE );
  setMinimumSize (100, 100);

  kfm = 0L;
  recent_files.setAutoDelete(TRUE);

  statusbar_timer = new QTimer(this);
  connect(statusbar_timer, SIGNAL(timeout()),this,SLOT(timer_slot()));

  connect(mykapp,SIGNAL(kdisplayPaletteChanged()),this,SLOT(set_colors()));

  setupMenuBar();
  setupToolBar();
  setupStatusBar();

  readSettings();

  recentpopup->clear();

  for ( int i =0 ; i < (int)recent_files.count(); i++){
    recentpopup->insertItem(recent_files.at(i));

  }

  setupEditWidget();
  set_colors();

  if ( hide_toolbar )	
    options->changeItem( i18n("Show &Tool Bar"), toolID );
  else
    options->changeItem( i18n("Hide &Tool Bar"), toolID );

  if ( hide_statusbar )
    options->changeItem( i18n("Show &Status Bar"), statusID );
  else
    options->changeItem( i18n("Hide &Status Bar"), statusID );

  //  setCaption("KEdit "KEDITVERSION);

  KDNDDropZone * dropZone = new KDNDDropZone( this , DndURL);
  connect( dropZone, SIGNAL( dropAction( KDNDDropZone *) ),
	   this, SLOT( slotDropEvent( KDNDDropZone *) ) );


  resize(editor_width,editor_height);

}


TopLevel::~TopLevel (){

  delete file;
  delete edit;
  delete options;
  delete recentpopup;
  delete toolbar;
}



void TopLevel::setupEditWidget(){


  eframe = new KEdit (mykapp,this, "eframe", i18n("Untitled"));

  connect(eframe, SIGNAL(CursorPositionChanged()),this,SLOT(statusbar_slot()));
  connect(eframe, SIGNAL(toggle_overwrite_signal()),this,SLOT(toggle_overwrite()));
  connect(eframe, SIGNAL(loading()),this,SLOT(loading_slot()));
  connect(eframe, SIGNAL(saving()),this,SLOT(saving_slot()));

  setView(eframe,FALSE);

  eframe->setFillColumnMode(fill_column_value,fill_column_is_set);
  eframe->setWordWrap(word_wrap_is_set);
  eframe->saveBackupCopy(backup_copies_is_set);
  eframe->setFont(generalFont);

  connect(eframe, SIGNAL(fileChanged()), SLOT(setFileCaption()));
  setSensitivity ();

  eframe->setFocus ();

  right_mouse_button = new QPopupMenu;

  right_mouse_button->insertItem (i18n("Open..."),
				  this, 	SLOT(file_open()));
  right_mouse_button->insertItem (i18n("Save"),
				  this, 	SLOT(file_save()));
  right_mouse_button->insertItem (i18n("Save as..."),
				  this, SLOT(file_save_as()));
  right_mouse_button->insertSeparator(-1);
  right_mouse_button->insertItem(i18n("Copy"),
				 this, SLOT(copy()));
  right_mouse_button->insertItem(i18n("Paste"),
				 this, SLOT(paste()));
  right_mouse_button->insertItem(i18n("Cut"),
				 this, SLOT(cut()));
  right_mouse_button->insertItem(i18n("Select All"),
				 this, SLOT(select_all()));
  right_mouse_button->insertSeparator(-1);
  right_mouse_button->insertItem(i18n("Font..."),
				 this,	SLOT(font()));

  eframe->installRBPopup(right_mouse_button);


}


void TopLevel::setupMenuBar(){

  KStdAccel keys(mykapp->getConfig());

  file = 	new QPopupMenu ();
  edit = 	new QPopupMenu ();
  help = 	new QPopupMenu ();
  options = 	new QPopupMenu ();
  colors =  	new QPopupMenu ();
  recentpopup = new QPopupMenu ();

  colors->insertItem(i18n("&Foreground Color"),
		     this, SLOT(set_foreground_color()));
  colors->insertItem(i18n("&Background Color"),
		     this, SLOT(set_background_color()));



	/*
  help->insertItem (i18n("&Help"),
		    this, 	SLOT(helpselected()));
  help->insertSeparator();
  help->insertItem (i18n("&About..."),
		    this, 	SLOT(about()));
		    */

   // DO NOT use keys.openNew() here until we have actual key
   // management in KDE. keys.openNew() currently evalutates to
   // CTRL N and that conflict with the emacs key bindings of
   // the text widget that we all rely on so much.
   // Bernd

   //  file->insertItem (i18n("Ne&w..."),
   //		    this, 	SLOT(file_new()), keys.openNew());

     file->insertItem (i18n("Ne&w..."),
		    this, 	SLOT(file_new()));


  file->insertItem (i18n("&Open..."),
		    this, 	SLOT(file_open()), keys.open());

  file->insertItem (i18n("Open Recen&t..."), recentpopup);
  connect( recentpopup, SIGNAL(activated(int)), SLOT(openRecent(int)) );

  file->insertSeparator (-1);

  file->insertItem (i18n("&Save"),
		    this, 	SLOT(file_save()), keys.save());
  file->insertItem (i18n("S&ave as..."),
		    this, 	SLOT(file_save_as()));
  file->insertItem (i18n("&Close"),
		    this,	SLOT(file_close()), keys.close());
  file->insertSeparator (-1);
  file->insertItem (i18n("Open &URL..."),
		    this,	SLOT(file_open_url()));
  file->insertItem (i18n("Save to U&RL..."),	
		    this,	SLOT(file_save_url()));
  file->insertSeparator (-1);

   // DO NOT use keys.print() here until we have actual key
   // management in KDE. keys.print() currently evalutates to
   // CTRL P and that conflict with the emacs key bindings of
   // the text widget that we all rely on so much.
   // Bernd
   //  file->insertItem (i18n("&Print..."),
   //		    this,	SLOT(print()), keys.print());

  file->insertItem (i18n("&Print..."),
		    this,	SLOT(print()));

  file->insertSeparator (-1);
  file->insertItem (i18n("&Mail..."),
		    this,	SLOT(mail()) );
  //  file->insertItem ("&Fancy Print...",this,SLOT(fancyprint()) );
  file->insertSeparator (-1);
  file->insertItem (i18n("New &Window"),
		    this,	SLOT(newTopLevel()) );
  file->insertSeparator (-1);
  file->insertItem (i18n("E&xit"),
		    this,	SLOT(quiteditor()), keys.quit());

  edit->insertItem(i18n("&Copy"),
		   this, 	SLOT(copy()));
  edit->insertItem(i18n("&Paste"),
		   this, 	SLOT(paste()));
  edit->insertItem(i18n("C&ut"),
		   this, 	SLOT(cut()));
  edit->insertItem(i18n("&Select All"),
		   this, 	SLOT(select_all()));
  edit->insertSeparator(-1);
  edit->insertItem(i18n("&Insert File"),
		   this, 	SLOT(insertFile()));
  edit->insertItem(i18n("Insert &Date"),
		   this, 	SLOT(insertDate()));
  edit->insertSeparator(-1);
  edit->insertItem(i18n("&Find..."),
		   this, 	SLOT(search()), keys.find());
  edit->insertItem(i18n("Find &Again"),
		   this, 	SLOT(search_again()));
  edit->insertItem(i18n("&Replace"),
		   this, 	SLOT(replace()), keys.replace());
  edit->insertItem(i18n("Spellcheck..."),
		   this, SLOT(spellcheck()));


  edit->insertSeparator(-1);
  edit->insertItem(i18n("&Goto Line..."),
		   this, SLOT(gotoLine()));

  options->setCheckable(TRUE);
  options->insertItem(i18n("&Font..."),
		      this,	SLOT(font()));
  options->insertItem(i18n("&Colors"), colors);
  options->insertSeparator(-1);
  options->insertItem("&Spellchecker...",
		      this,  SLOT (spell_configure()));

  options->insertItem(i18n("KEdit &Options..."),
		      this, 	SLOT(fill_column_slot()));
  indentID = options->insertItem(i18n("Auto &Indent"),
				 this, SLOT(toggle_indent_mode()));
  options->insertSeparator(-1);
  toolID   = options->insertItem(i18n("&Tool Bar"),
				 this,SLOT(toggleToolBar()));
  statusID = options->insertItem(i18n("&Status Bar"),
				 this,SLOT(toggleStatusBar()));	
  options->insertSeparator(-1);
  options->insertItem(i18n("Save Options"),
		      this, 	SLOT(save_options()));

  menubar = new KMenuBar (this, "menubar");
  menubar->insertItem (i18n("&File"), file);
  menubar->insertItem (i18n("&Edit"), edit);
  menubar->insertItem (i18n("&Options"), options);
  menubar->insertSeparator(-1);

  QString about;
  about.sprintf(i18n("KEdit %s\n\n"
                     "Copyright 1997-98\n"
                     "Bernd Johannes Wuebben\n"
                     "wuebben@kde.org"),
		KEDITVERSION);
  help = mykapp->getHelpMenu(TRUE, about);


  menubar->insertItem(i18n("&Help"), help);

  setMenu(menubar);


}


void TopLevel::setupToolBar(){


  toolbar = new KToolBar( this );

  KIconLoader *loader = kapp->getIconLoader();

  QPixmap pixmap;

  pixmap = loader->loadIcon("filenew2.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(file_new()), TRUE, i18n("New Document"));


  pixmap = loader->loadIcon("fileopen.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(file_open()), TRUE, i18n("Open Document"));

  pixmap = loader->loadIcon("filefloppy.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(file_save()), TRUE, i18n("Save Document"));

  toolbar->insertSeparator();

  pixmap = loader->loadIcon("editcopy.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(copy()), TRUE, i18n("Copy"));

  pixmap = loader->loadIcon("editpaste.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(paste()), TRUE, i18n("Paste"));

  pixmap = loader->loadIcon("editcut.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(cut()), TRUE, i18n("Cut"));

  toolbar->insertSeparator();


  pixmap = loader->loadIcon("fileprint.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(print()), TRUE, i18n("Print Document"));

  pixmap = loader->loadIcon("send.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(mail()), TRUE, i18n("Mail Document"));


  toolbar->insertSeparator();
  pixmap = loader->loadIcon("help.xpm");
  toolbar->insertButton(pixmap, 0,
		      SIGNAL(clicked()), this,
		      SLOT(helpselected()), TRUE, i18n("Help"));

  toolbar->setBarPos( KToolBar::Top );

  toolbar1 = addToolBar(toolbar);

  //  toolbar->show();

}

void TopLevel::setupStatusBar(){

    statusbar = new KStatusBar( this );
    statusbar->insertItem("Line:000000 Col: 000", ID_LINE_COLUMN);
    statusbar->insertItem("XXX", ID_INS_OVR);
    statusbar->insertItem("", ID_GENERAL);

    statusbar->changeItem("Line: 1 Col: 1", ID_LINE_COLUMN);
    statusbar->changeItem("INS", ID_INS_OVR);
    statusbar->changeItem("", ID_GENERAL);

    statusbar->setInsertOrder(KStatusBar::RightToLeft);
    statusbar->setAlignment(ID_INS_OVR,AlignCenter);

    //    statusbar->setInsertOrder(KStatusBar::LeftToRight);
    //    statusbar->setBorderWidth(1);

    setStatusBar( statusbar );

}




void TopLevel::saveProperties(KConfig* config){

    if(strcmp(eframe->getName(), i18n("Untitled"))== 0 && !eframe->isModified())
	return;
    
    config->writeEntry("filename",eframe->getName().data());
    config->writeEntry("modified",eframe->isModified());

    if(eframe->isModified()){

      QString fullname;
      //make sure eframe->getName() is a full pathname !!!

      if (eframe->getName().data()[0] != '/'){

	char* s = new char[1024];
	fullname =(getcwd(s, 1024));
	fullname +="/";
	delete [] s;

      }

      fullname += eframe->getName().data();
      QString string = mykapp->tempSaveName(fullname);
      eframe->saveasfile(string.data());

    }
}


void TopLevel::readProperties(KConfig* config){

    QString filename = config->readEntry("filename","");
    int modified = config->readNumEntry("modified",0);

    if(!filename.isEmpty() && modified){
        bool ok;

        const char* fn = mykapp->checkRecoverFile(filename.data(),ok);

	if(ok){
	  QString file = fn;
	  file.detach();
	  eframe->loadFile(file,KEdit::OPEN_READWRITE);
	  eframe->setModified();
	  eframe->setFileName(filename.data());
	  setFileCaption();

	}
    }
    else{

      if(!filename.isEmpty()){
	eframe->loadFile(filename,KEdit::OPEN_READWRITE);
      }
    }
}


void TopLevel::copy(){

  eframe->copyText();

}

void TopLevel::select_all(){

  eframe->selectAll();

}

void TopLevel::insertFile(){

  int returncode;

retry_insertFile:

  returncode = eframe->insertFile();
  if(returncode == KEdit::KEDIT_OK)
    setGeneralStatusField(i18n("Done"));
  if(returncode == KEdit::KEDIT_RETRY)
    goto retry_insertFile;

  statusbar_slot();
}




void TopLevel::insertDate(){

  int line, column;

  QString string;
  QDateTime dt = QDateTime::currentDateTime();
  string = dt.toString();

  eframe->getCursorPosition(&line,&column);
  eframe->insertAt(string,line,column);
  eframe->toggleModified(TRUE);

  statusbar_slot();
}

void TopLevel::paste(){

 eframe->paste();
 eframe->toggleModified(TRUE);

 statusbar_slot();
}

void TopLevel::cut(){

 eframe->cut();
 eframe->toggleModified(TRUE);
 statusbar_slot();

}

void TopLevel::file_new(){

  eframe->newFile();
  statusbar_slot();

}

void TopLevel::spellcheck()
{

  statusbar->changeItem("Spellcheck:  Started.", ID_GENERAL);

  connect (eframe, SIGNAL (spellcheck_progress (unsigned int)),
          this, SLOT (spell_progress (unsigned int)));
  connect (eframe, SIGNAL (spellcheck_done()),
          this, SLOT (spell_done ()));

  if(eframe){
    eframe->spellcheck();
  }
}

void TopLevel::spell_progress (unsigned int percent)
{
  char s[100];
  sprintf (s,"Spellcheck:  %d%% complete",percent);

  statusbar->changeItem (s, ID_GENERAL);
}

void TopLevel::spell_done()
{
  statusbar->changeItem ("Spellcheck:  Complete", ID_GENERAL);
}

void TopLevel::file_open(){

  int result;

tryagain_fileopen:

  result = eframe->openFile(KEdit::OPEN_READWRITE);
  switch (result){

  case KEdit::KEDIT_OK :	
    {
      QString string;
      setGeneralStatusField(i18n("Done"));
      add_recent_file(eframe->getName());
      break;
    }

  case KEdit::KEDIT_RETRY :
    {
      goto tryagain_fileopen;
      break;
    }
  case KEdit::KEDIT_OS_ERROR :
    {
      break;
    }
  default:
    break;
  }

  statusbar_slot();

}


void TopLevel::file_open_url(){

  UrlDlg l(this, i18n("Open Location:"), url_location.data());
  if ( l.exec() )
    {
      QString n = l.getText();
      url_location = n;
      /*      if ( n.left(5) != "file:" && n.left(4) == "ftp:" )*/
      openNetFile( l.getText(), KEdit::OPEN_READWRITE );
    }

  statusbar_slot();
}

void TopLevel::file_save_url(){

  UrlDlg l( this, i18n("Save to Location:"), eframe->getName());
  if ( l.exec() )
    {
      QString n = l.getText();
      url_location = n;
      /*if ( n.left(5) != "file:" && n.left(4) == "ftp:" )*/
      saveNetFile( l.getText() );
    }

  statusbar_slot();
}

void TopLevel::quiteditor(){

  bool frame_modified = false;
  int result;

  // check the normal case of only 1 editor window
  if(windowList.count() == 1){
    if(!eframe->isModified()){
      writeSettings();
      mykapp->quit();
      return;
    }
    else{ /// modified

      switch( QMessageBox::information( this, i18n("Message"),
			     i18n("This Document has been modified.\n"\
						"Would you like to save it?"),
					i18n("Yes"),
					i18n("No"),
					i18n("Cancel"),
					0,      // Enter == button 0
					2 ) ){

      case 0: // Save clicked, Enter pressed.
	result = eframe->doSave();
	
	if( result == KEdit::KEDIT_USER_CANCEL)
	  return;
	if(result != KEdit::KEDIT_OK){


	  switch( QMessageBox::warning( this, i18n("Sorry"),
			  i18n("Could not save the file.\n"\
					     "Exit anyways?"),
					     i18n("Yes"),
					     i18n("No"),
					0,      // Enter == button 0
					1 ) ){
	  case 0: // yes exit
	    break;
	  case 1: // no don't exit
	    return;
	    break;
	  }
	}
	
	break;
    case 1: // Don't Save clicked
        // don't save but exit
        break;
    case 2: // Cancel clicked, Alt-C or Escape pressed
        return;
        break;
	}


    }
      writeSettings();
      mykapp->quit();
      return;
	
  }


  TopLevel *ptoplevel;
  for ( ptoplevel=windowList.first(); ptoplevel!= 0; ptoplevel=windowList.next() ){
    if (ptoplevel->eframe->isModified())
      frame_modified = true;
  }

  if(!frame_modified){ // no frame was modified
    writeSettings();
    mykapp->quit();
  }

  switch( QMessageBox::warning(
		    this,
		    i18n("Message"),
		    i18n("There are windows with modified content open.\n"\
		    "If you exit now, you will loose those changes\n"\
		    "Quit anyways?"),
		     i18n("Yes"),
		     i18n("No"),
		    "",
		     1,      // Enter == button 0
		     1 ) ){
  case 0: // yes exit
    writeSettings();
    mykapp->quit();
    break;
  case 1: // no don't exit
    return;
    break;
  }

  return;

}

void TopLevel::openRecent(int i){

  if (eframe->isModified ()) {


    switch( QMessageBox::information(
			    this,
			    i18n("Message"),
			    i18n("The current document has been modified.\n"\
					       "Continue anyways ?"),
			    i18n("Yes"),
			    i18n("No"),
			    i18n("Cancel"),
			    1,      // Enter == button 0
			    2 ) ){

      case 0: // Save clicked, Enter pressed.
	break;

      case 1:
	return;
        break;
    case 2:
        return;
        break;
	}

  }

  openNetFile( recent_files.at(i), KEdit::OPEN_READWRITE );	

	
}

void TopLevel::add_recent_file(const char* newfile){

  if(recent_files.find(newfile) != -1)
    return; // it's already there

  if( recent_files.count() < 5)
    recent_files.insert(0,newfile);
  else{
    recent_files.remove(4);
    recent_files.insert(0,newfile);
  }

  recentpopup->clear();

  for ( int i =0 ; i < (int)recent_files.count(); i++){
    recentpopup->insertItem(recent_files.at(i));

  }

}

void TopLevel::newTopLevel(){

  TopLevel *t = new TopLevel ();
  t->show ();
  windowList.append( t );
}


void TopLevel::file_close(){

  int result;

  if (eframe->isModified ()) {


      switch( QMessageBox::information( this,i18n("Message"),
			     i18n("This Document has been modified.\n"\
						"Would you like to save it?"),
					i18n("Yes"),
					i18n("No"),
					i18n("Cancel"),
					0,      // Enter == button 0
					2 ) ){

      case 0: // Save clicked, Enter pressed.

	result = eframe->doSave();
	if( result == KEdit::KEDIT_USER_CANCEL)
	  return;
	if(result != KEdit::KEDIT_OK){


	  switch( QMessageBox::warning( this,i18n("Sorry"),
			  i18n("Could not save the file.\n"\
					     "Exit anyways?"),
					     i18n("Yes"),
					     i18n("No"),
					0,      // Enter == button 0
					1 ) ){
	  case 0: // yes exit
	    break;
	  case 1: // no don't exit
	    return;
	    break;
	  }
	}
	
	break;

    case 1: // Don't Save clicked
        // don't save but exit
        break;
    case 2: // Cancel clicked, Alt-C or Escape pressed
        return;
        break;
	}



  }

  if ( windowList.count() > 1 ){  /* more than one window is open */

    windowList.remove( this );
    delete this;
  }	
  else{
    writeSettings();
    mykapp->quit();
  }

}

void TopLevel::file_save(){

  if ( eframe->isModified() )
  {
      KURL u( eframe->getName() );
      if ( !u.isMalformed() && strcmp( u.protocol(), "file" ) != 0L )
      {
	
	  url_location = eframe->getName();
	  saveNetFile( url_location );
	  statusbar_slot();
	  return;
      }

      int result = KEdit::KEDIT_OK;

      result =  eframe->doSave(); // error messages are handled by ::KEdit

      if ( result == KEdit::KEDIT_OK ){
	  setFileCaption();
	  QString string;
	  string.sprintf(i18n("Wrote: %s"),eframe->getName().data());
	  setGeneralStatusField(string);
      }
  }
  else
  {
      setGeneralStatusField(i18n("No changes need to be saved"));
  }
}

void TopLevel::setGeneralStatusField(QString text){

    statusbar_timer->stop();

    statusbar->changeItem(text.data(),ID_GENERAL);
    statusbar_timer->start(10000,TRUE); // single shot

}


void TopLevel::file_save_as(){

  if (eframe->saveAs()== KEdit::KEDIT_OK){
    setFileCaption();
    QString string;
    string.sprintf(i18n("Saved as: %s"),eframe->getName().data());
    setGeneralStatusField(string);
  }
}

void TopLevel::mail(){

  Mail* maildlg ;

  maildlg = new Mail(this,"maildialog");

  if(!maildlg->exec()){
    return;
  }

  mykapp->processEvents();
  mykapp->flushX();

  FILE* mailpipe;

  QString cmd;
  /*  cmd = mailcmd.copy();*/
  cmd = cmd.sprintf(mailcmd.data(),
		    maildlg->getSubject().data(),maildlg->getRecipient().data());

  /*  printf("%s\n",cmd.data());*/

  delete maildlg;

  mailpipe = popen(cmd.data(),"w");

  if(mailpipe == NULL){
    QString str;
    str.sprintf(i18n("Could not pipe the contents"\
				 "of this Document into:\n %s"),cmd.data());

    QMessageBox::warning(
			 this,
			 i18n("Sorry"),
			 str.data(),
			 i18n("OK"),
			 "",
			 "",
			 0,0);
    return;
  }

  QTextStream t(mailpipe,IO_WriteOnly );

  int line_count = eframe->numLines();

  for(int i = 0 ; i < line_count ; i++){
    t << eframe->textLine(i) << '\n';
  }
  pclose(mailpipe);

}


/*
void TopLevel::fancyprint(){

  QPrinter prt;
  char buf[200];
  if ( prt.setup(0) ) {

    int y =10;
    QPainter p;
    p.begin( &prt );
    p.setFont( eframe->font() );
    QFontMetrics fm = p.fontMetrics();

    int numlines = eframe->numLines();
    for(int i = 0; i< numlines; i++){
      y += fm.ascent();
      QString line;
      line = eframe->textLine(i);
      line.replace( QRegExp("\t"), "        " );
      strncpy(buf,line.data(),160);
      for (int j = 0 ; j <150; j++){
	if (!isprint(buf[j]))
	    buf[j] = ' ';
      }
      buf[line.length()] = '\0';
      p.drawText( 10, y, buf );
      y += fm.descent();
    }

    p.end();
  }
  return ;
}
*/
/*
void TopLevel::about(){

  QDialog *dlg = new About(0);

  QPoint point = this->mapToGlobal (QPoint (0,0));

  QRect pos = this->geometry();
  dlg->setGeometry(point.x() + pos.width()/2  - dlg->width()/2,
		   point.y() + pos.height()/2 - dlg->height()/2,
		   dlg->width(),dlg->height());

  dlg->exec();
  delete dlg;
}
*/
void TopLevel::helpselected(){

  mykapp->invokeHTMLHelp( "" , "" );

}

void TopLevel::toggle_indent_mode(){

  if(!eframe)
    return;

  bool mode = eframe->AutoIndentMode();
  mode = !mode;

  eframe->setAutoIndentMode(mode);
  options->setItemChecked( indentID, mode);

  if(mode)
    setGeneralStatusField(i18n("Auto Indent Mode: On"));
  else
    setGeneralStatusField(i18n("Auto Indent Mode: Off"));

}

void TopLevel::search(){

      eframe->Search();
      statusbar_slot();
}

void TopLevel::replace(){

      eframe->Replace();
      statusbar_slot();
}

void TopLevel::font(){

  eframe->selectFont();
  generalFont = eframe->font();

}


void TopLevel::toggleStatusBar(){

  if(hide_statusbar) {

    hide_statusbar=FALSE;
    enableStatusBar( KStatusBar::Show );
    options->changeItem(i18n("Hide &Status Bar"), statusID);

  }
  else {

    hide_statusbar=TRUE;
    enableStatusBar( KStatusBar::Hide );
    options->changeItem(i18n("Show &Status Bar"), statusID);

  }

}


void TopLevel::toggleToolBar(){

  if(hide_toolbar) {

    hide_toolbar=FALSE;
    enableToolBar( KToolBar::Show, toolbar1 );
    options->changeItem(i18n("Hide &Tool Bar"), toolID);

  }
  else {

    hide_toolbar=TRUE;
    enableToolBar( KToolBar::Hide, toolbar1 );
    options->changeItem(i18n("Show &Tool Bar"), toolID);

  }

}	


void TopLevel::closeEvent( QCloseEvent *e ){

  (void) e;

  file_close();

};

void TopLevel::search_again(){

      eframe->repeatSearch();
      statusbar_slot();
}


void TopLevel::setFileCaption(){

  QString string;
  /*  string = mykapp->getCaption() + " ";*/
  string += eframe->getName();
  setCaption(string);
}


void TopLevel::gotoLine() {
	eframe->doGotoLine();
}

void TopLevel::statusbar_slot(){

  QString linenumber;

  linenumber.sprintf(i18n("Line: %d Col: %d"),
		     eframe->currentLine() + 1,
		     eframe->currentColumn() +1
		     );

  statusbar->changeItem(linenumber.data(),ID_LINE_COLUMN);

}




void TopLevel::fill_column_slot(){

  FillDlg *dlg;

  dlg = new FillDlg(this, "optionsdialog");
  dlg->setCaption(i18n("KEdit Options"));

  struct fill_struct fillstr;

  fillstr.fill_column_is_set  = fill_column_is_set;
  fillstr.word_wrap_is_set    = word_wrap_is_set;
  fillstr.backup_copies_is_set= backup_copies_is_set;
  fillstr.fill_column_value   = fill_column_value;
  fillstr.mailcmd 	      = mailcmd.copy();

  dlg->setWidgets(fillstr);


  if(dlg->exec() == QDialog::Accepted){

    struct fill_struct fillstr = dlg->getFillCol();

    word_wrap_is_set = fillstr.word_wrap_is_set;;
    eframe->setWordWrap(word_wrap_is_set);

    backup_copies_is_set = fillstr.backup_copies_is_set;
    eframe->saveBackupCopy(backup_copies_is_set);

    fill_column_is_set = fillstr.fill_column_is_set;
    fill_column_value = fillstr.fill_column_value;;

    //    if(fill_column_is_set)
      eframe->setFillColumnMode(fill_column_value,fill_column_is_set);
    /*    else	
      eframe->setFillColumnMode(0);*/

  }

  delete dlg;

}


void TopLevel::print(){


  QString command;
  QString com;
  QString file;

  int result;
  /*  if(strcmp(eframe->getName(), i18n("Untitled"))!= 0){*/


  if (eframe->isModified ()) {


      switch( QMessageBox::information( this,i18n("Message"),
			    i18n("The current document has been modified.\n"\
					       "Would you like to save the changes before \n"\
					       "printing this Document?"),
			    i18n("Yes"),
			    i18n("No"),
			    i18n("Cancel"),
			    0,      // Enter == button 0
			    2 ) ){

      case 0: // Save clicked, Enter pressed.

	result = eframe->doSave();
	if( result == KEdit::KEDIT_USER_CANCEL)
	  return;
	if(result != KEdit::KEDIT_OK){


	  switch( QMessageBox::warning( this,i18n("Sorry"),
			  i18n("Could not save the file.\n"\
					     "Print anyways?"),
					     i18n("Yes"),
					     i18n("No"),
					0,      // Enter == button 0
					1 ) ){
	  case 0: // yes exit
	    break;
	  case 1: // no don't exit
	    return;
	    break;
	  }
	}
	
	break;

    case 1: // Don't Save clicked
        // don't save but exit
        break;
    case 2: // Cancel clicked, Alt-C or Escape pressed
        return;
        break;
	}

  }

  /*  }*/

  PrintDialog *printing;
  printing = new PrintDialog(this,"print",true);
  printing->setWidgets(pi);


  if(printing->exec() == QDialog::Accepted){

    int result = 0;

    pi = printing->getCommand();

    pi.command.detach();
  if(strcmp(eframe->getName(), i18n("Untitled"))== 0){

      // we go through all of this so that we can print an "Untitled" document
      // quickly without going through the hassle of saving it. This will
      // however result in a temporary filename and your printout will
      // usually show that temp name, such as /tmp/00432aaa
      // for a non "untitled" document we don't want that to happen so
      // we asked the user to save before we print the document.

      // TODO find a smarter solution for the above!

      QString tmpname = tmpnam(NULL);

      QFile file(tmpname);
      file.open(IO_WriteOnly);


      if(pi.selection){
	if(file.writeBlock(eframe->markedText().data(),
			 eframe->markedText().length()) == -1) {
	  result = KEdit::KEDIT_OS_ERROR;
	}
	else {
	  result = KEdit::KEDIT_OK;
	}
      }
      else{
	if(file.writeBlock(eframe->text().data(),
			   eframe->text().length()) == -1) {
	  result = KEdit::KEDIT_OS_ERROR;
	}
	else {
	  result = KEdit::KEDIT_OK;
	}	
      }

      file.close();
      // TODO error handling

      if (pi.raw){
	command = "lpr";
      }	
      else{
	command = pi.command;
	command.detach();
      }

      com.sprintf("%s %s ; rm %s &",command.data(),
		  tmpname.data(),tmpname.data());

      system(com.data());
      QString string;
      if(pi.selection)
	string.sprintf(i18n("Printing: %s Untitled (Selection)")
		       , command.data());
      else
	string.sprintf(i18n("Printing: %s Untitled")
		       , command.data());
      setGeneralStatusField(string);

    }
    else{ // document name != Untiteled


      QString tmpname = tmpnam(NULL);
      if (pi.selection){ // print only the selection


	QFile file(tmpname);
	file.open(IO_WriteOnly);


	if(file.writeBlock(eframe->markedText().data(),
			 eframe->markedText().length()) == -1) {
	  result = KEdit::KEDIT_OS_ERROR;
	}
	file.close();

	// TODO error handling
      }

      if (pi.raw){
	command = "lpr";
      }	
      else{
	command = pi.command;
	command.detach();
      }

      if(!pi.selection){ // print the whole file
	
	com.sprintf("%s '%s' &",command.data(), eframe->getName().data());
	system(com.data());
	QString string;	
	string.sprintf(i18n("Printing: %s"),com.data());
	setGeneralStatusField(string);

      }
      else{ // print only the selection
	
	com.sprintf("%s %s ; rm %s &",command.data(),
		  tmpname.data(),tmpname.data());
	system(com.data());
	QString string;	
	string.sprintf(i18n("Printing: %s %s (Selection)"),
		       command.data(), eframe->getName().data());
	setGeneralStatusField(string);

      }
      printf("%s\n",com.data());
    }

  }
}


void TopLevel::setSensitivity (){

}

void TopLevel::save_options(){

  writeSettings();

}

void TopLevel::saving_slot(){

  setGeneralStatusField(i18n("Saving ..."));

}


void TopLevel::loading_slot(){

  setGeneralStatusField(i18n("Loading ..."));

}


void TopLevel::saveNetFile( const char *_url )
{

    netFile = _url;
    netFile.detach();
    KURL u( netFile.data() );
    if ( u.isMalformed() )
    {
	QMessageBox::message (i18n("Sorry"),
        i18n("Malformed URL"),
	i18n("OK"));
	return;
    }


    // Just a usual file ?
    if ( strcmp( u.protocol(), "file" ) == 0 )
   {
      QString string;
      setGeneralStatusField(string);
      eframe->doSave( u.path() );
      return;
    }

    if ( kfm != 0L )
    {
	QMessageBox::information(
			      this,
			      i18n("Sorry"),
			      i18n("KEdit is already waiting\n"\
						 "for an internet job to finish\n"\
						 "Please wait until has finished\n"\
						 "Alternatively stop the running one."),
			      i18n("OK"),
			      "",
			      "",
			      0,0);
	return;
    }

    kfm = new KFM;
    if ( !kfm->isOK() )
    {
	QMessageBox::warning(
			     this,
			     i18n("Sorry"),
			     i18n("Could not start or find KFM"),
			     i18n("OK"),
			     "",
			     "",
			     0,0);
	delete kfm;
	kfm = 0L;
	return;
    }

    tmpFile.sprintf( "file:/tmp/kedit%li", time( 0L ) );

    eframe->toggleModified( TRUE );
    eframe->doSave( tmpFile.data() + 5 );
    eframe->toggleModified( TRUE );

    connect( kfm, SIGNAL( finished() ), this, SLOT( slotKFMFinished() ) );
    kfm->copy( tmpFile.data(), netFile.data() );
    kfmAction = TopLevel::PUT;
}

void TopLevel::openNetFile( const char *_url, int _mode )
{

  QString string;
  netFile = _url;
  netFile.detach();
  KURL *u = new KURL( netFile.data() );
  if ( u->isMalformed() )
    {
        delete u;

        if (netFile.data()[0] == '/'){
	  // absolute path
          u=new KURL( QString(QString("file:")+QString(netFile.data())) );
	}
        else{
          u=new KURL( QString(QString("file:")+
			      QDir::currentDirPath()+QString(netFile.data())) );
	}
        if (u->isMalformed()){
	  QString string;
	  string.sprintf(i18n( "Malformed URL\n%s"),netFile.data());

  	  QMessageBox::warning(this,
			       i18n("Sorry"),
			       string.data(),
			       i18n("OK"),
			       "",
			       "",
			       0,0);
	  delete u;
          return;
	}
    }

    // Just a usual file ?
    if ( strcmp( u->protocol(), "file" ) == 0 && !u->hasSubProtocol() )
    {
	QString decoded( u->path() );
	KURL::decodeURL( decoded );
	QString string;
	string.sprintf(i18n("Loading '%s'"),decoded.data() );
	setGeneralStatusField(string);
	eframe->loadFile( decoded, _mode );
	add_recent_file( decoded );
	setGeneralStatusField("Done");
	delete u;
	return;
    }

    if ( kfm != 0L )
    {
	QMessageBox::information(
				 this,
				 i18n("Sorry"),
				 i18n("KEdit is already waiting\n"\
						    "for an internet job to finish\n"\
						    "Please wait until has finished\n"\
						    "Alternatively stop the running one."),
				 i18n("OK"),
				 "",
				 "",
				 0,0);
	return;
    }
    setGeneralStatusField(i18n("Calling KFM"));

    kfm = new KFM;
    setGeneralStatusField(i18n("Done"));
    if ( !kfm->isOK() )
    {
	QMessageBox::warning(
			     this,
			     i18n("Sorry"),
			     i18n("Could not start or find KFM"),
			     i18n("OK"),
			     "",
			     "",
			     0,0);
	delete kfm;
	kfm = 0L;
	return;
    }

    setGeneralStatusField(i18n("Starting Job"));
    tmpFile.sprintf( "file:/tmp/kedit%li", time( 0L ) );
    connect( kfm, SIGNAL( finished() ), this, SLOT( slotKFMFinished() ) );
    setGeneralStatusField(i18n("Connected"));
    kfm->copy( netFile.data(), tmpFile.data() );
    setGeneralStatusField(i18n("Waiting..."));
    kfmAction = TopLevel::GET;
    openMode = _mode;
}

void TopLevel::slotKFMFinished()
{
  QString string;
  string.sprintf(i18n("Finished '%s'"),tmpFile.data());
  setGeneralStatusField(string);

    if ( kfmAction == TopLevel::GET )
    {
	KURL u( tmpFile.data() );
	eframe->loadFile( u.path(), openMode );
	eframe->setName( netFile.data() );
	setCaption( netFile.data() );
	add_recent_file(netFile.data());
	// Clean up
	unlink( tmpFile.data() );
	delete kfm;
	kfm = 0L;
    }
    if ( kfmAction == TopLevel::PUT )
    {
	eframe->toggleModified( FALSE );
	unlink( tmpFile.data() );
	delete kfm;
	kfm = 0L;
    }
}

void TopLevel::slotDropEvent( KDNDDropZone * _dropZone )
{
    QStrList & list = _dropZone->getURLList();

    char *s;

    for ( s = list.first(); s != 0L; s = list.next() )
    {
	// Load the first file in this window
	if ( s == list.getFirst() && !eframe->isModified() )
	{
	    QString n = s;
	    /*	    if ( n.left(5) != "file:" && n.left(4) == "ftp:" )*/
		openNetFile( n.data(), KEdit::OPEN_READWRITE );
	}
	else
	{
	    setGeneralStatusField(i18n("New Window"));
	    TopLevel *t = new TopLevel ();
	    t->show ();
	    windowList.append( t );
	    setGeneralStatusField(i18n("New Window Created"));
	    QString n = s;
	    /*	    if ( n.left(5) != "file:" && n.left(4) == "ftp:" )*/
		t->openNetFile( n.data(), KEdit::OPEN_READWRITE );
		/*	    else
		t->openNetFile( n.data(), KEdit::OPEN_READWRITE );*/
	    setGeneralStatusField(i18n("Load Command Done"));
	}
    }
}

void TopLevel::timer_slot(){

  statusbar->changeItem("",ID_GENERAL);

}


void TopLevel::set_foreground_color(){

  QColor color;

  color = forecolor;

  if(KColorDialog::getColor(color) != QDialog::Accepted)
    return;

  forecolor = color;
  set_colors();



}

void TopLevel::set_background_color(){

  QColor color;

  color = backcolor;

  if( KColorDialog::getColor(color) != QDialog::Accepted)
    return;

  backcolor = color;
  set_colors();

}


void TopLevel::set_colors(){


  QPalette mypalette = (eframe->palette()).copy();

  QColorGroup cgrp = mypalette.normal();
  QColorGroup ncgrp(forecolor,cgrp.background(),
		    cgrp.light(),cgrp.dark(),cgrp.mid(),forecolor,backcolor);

  mypalette.setNormal(ncgrp);
  mypalette.setDisabled(ncgrp);
  mypalette.setActive(ncgrp);

  eframe->setPalette(mypalette);
  eframe->setBackgroundColor(backcolor);

}


void TopLevel::readSettings(){

	// let's set the defaults

	generalFont = QFont ("courier", 12, QFont::Normal);
	editor_width = 550;
	editor_height = 400;
	hide_statusbar = FALSE;
	hide_toolbar = FALSE;
	forecolor = QColor(black);
	backcolor = QColor(white);
	fill_column_is_set = true;
	word_wrap_is_set = true;
	backup_copies_is_set = true;
	fill_column_value = 79;
	mailcmd = "mail -s \"%s\" \"%s\"";

	pi.command = "enscript -2rG";
	pi.raw = 1;
	pi.selection = 0;

	//////////////////////////////////////////////////

	QString str;
	
	config = mykapp->getConfig();

	///////////////////////////////////////////////////

       	config->setGroup( "Text Font" );

	generalFont = config->readFontEntry("KEditFont", &generalFont);

	/*	str = config->readEntry( "Family" );
	if ( !str.isNull() )
		generalFont.setFamily(str.data());
		
		
	str = config->readEntry( "Point Size" );
		if ( !str.isNull() )
		generalFont.setPointSize(atoi(str.data()));
		
	
	str = config->readEntry( "Weight" );
		if ( !str.isNull() )
	generalFont.setWeight(atoi(str.data()));
		
	
	str = config->readEntry( "Italic" );
		if ( !str.isNull() )
			if ( atoi(str.data()) != 0 )
				generalFont.setItalic(TRUE);
				*/
	///////////////////////////////////////////////////

	config->setGroup("Recently_Opened_Files");

	str = config->readEntry("1","");
	if (!str.isEmpty())
	  recent_files.append(str.data());

	str = config->readEntry("2","");
	if (!str.isEmpty())
	  recent_files.append(str.data());

	str = config->readEntry("3","");
	if (!str.isEmpty())
	  recent_files.append(str.data());

	str = config->readEntry("4","");
	if (!str.isEmpty())
	  recent_files.append(str.data());

	str = config->readEntry("5","");
	if (!str.isEmpty())
	  recent_files.append(str.data());

	///////////////////////////////////////////////////

	config->setGroup("General Options");


	url_location = config->readEntry("default_url","ftp://localhost/welcome.msg");

	str = config->readEntry("MailCmd");
		if ( !str.isNull() )
		  mailcmd = str.data();

	str = config->readEntry("Width");
		if ( !str.isNull() )
		  editor_width = atoi(str.data());

	str = config->readEntry("Height");
		if ( !str.isNull() )
		  editor_height = atoi(str.data());

	str = config->readEntry( "StatusBar" );
	if ( !str.isNull() && str.find( "off" ) == 0 ) {
		hide_statusbar = TRUE;
		enableStatusBar( KStatusBar::Hide );

	} else
	  {
		hide_statusbar = FALSE;
		enableStatusBar( KStatusBar::Show );
	  }

	str = config->readEntry( "ToolBar" );

	if ( !str.isNull() && str.find( "off" ) == 0 ) {
		hide_toolbar = TRUE;
		enableToolBar( KToolBar::Hide, toolbar1 );
	} else{
		hide_toolbar = FALSE;
		enableToolBar( KToolBar::Show, toolbar1 );

	}
	str = config->readEntry("UseFillColumn");
		if ( !str.isNull() )
		  fill_column_is_set = (bool) atoi(str.data());

	str = config->readEntry("WordWrap");
		if ( !str.isNull() )
		  word_wrap_is_set = (bool) atoi(str.data());

	str = config->readEntry("BackupCopies");
		if ( !str.isNull() )
		  backup_copies_is_set = (bool) atoi(str.data());

	str = config->readEntry("FillColumn");
		if ( !str.isNull() )
		  fill_column_value = atoi(str.data());


        str = config->readEntry( "ForeColor" );
		if ( !str.isNull() )
			forecolor.setNamedColor( str );

        str = config->readEntry( "BackColor" );
		if ( !str.isNull() )
			backcolor.setNamedColor( str );

	str = config->readEntry( "Toolbar position" );
	if ( !str.isNull() ) {
		if( str == "Left" ) {
			toolbar->setBarPos( KToolBar::Left );
		} else if( str == "Right" ) {
			toolbar->setBarPos( KToolBar::Right );
		} else if( str == "Bottom" ) {
			toolbar->setBarPos( KToolBar::Bottom );
		} else
			toolbar->setBarPos( KToolBar::Top );
	}			

	///////////////////////////////////////////////////

	config->setGroup("Printing");

	str = config->readEntry("PrntCmd1");
		if ( !str.isNull() )
		  pi.command = str;

	str = config->readEntry("PrintSelection");
		if ( !str.isNull() )
		  pi.selection = atoi(str.data());

	str = config->readEntry("PrintRaw");
		if ( !str.isNull() )
		  pi.raw = atoi(str.data());


}

void TopLevel::writeSettings(){
		
	config = mykapp->getConfig();
	
	///////////////////////////////////////////////////

	config->setGroup( "Text Font" );

	config->writeEntry("KEditFont",generalFont);

	/*
	config->writeEntry("Family", generalFont.family());
	
	QString pointSizeStr;
	pointSizeStr.sprintf("%d", generalFont.pointSize() );
	config->writeEntry("Point Size", pointSizeStr);
	
	QString weightStr;
	weightStr.sprintf("%d", generalFont.weight() );
	config->writeEntry("Weight", weightStr);
	
	QString italicStr;
	italicStr.sprintf("%d", (int)generalFont.italic() );
	config->writeEntry("Italic", italicStr);
	*/
	////////////////////////////////////////////////////

	config->setGroup("Recently_Opened_Files");

	QString recent_number;
	for(int i = 0; i <(int) recent_files.count();i++){
	  recent_number.setNum(i+1);
	  config->writeEntry(recent_number.data(),recent_files.at(i));
	}	

	////////////////////////////////////////////////////
	config->setGroup("General Options");

	config->writeEntry("default_url", url_location);

	QString widthstring;
	widthstring.sprintf("%d", this->width() );
	config->writeEntry("Width",  widthstring);

	QString heightstring;
	heightstring.sprintf("%d", this->height() );

	config->writeEntry("Height",  heightstring);

	config->writeEntry( "StatusBar", hide_statusbar ? "off" : "on" );
	config->writeEntry( "ToolBar", hide_toolbar ? "off" : "on" );
	config->writeEntry("MailCmd",mailcmd);

	QString string;
	string.sprintf("%d", fill_column_is_set );
	config->writeEntry("UseFillColumn", string);

	string="";
	string.sprintf("%d", word_wrap_is_set );
	config->writeEntry("WordWrap", string);

	string="";
	string.sprintf("%d", backup_copies_is_set);
	config->writeEntry("BackupCopies", string);

	string="";
	string.sprintf("%d", fill_column_value );
	config->writeEntry("FillColumn", string);

	string="";
	string.sprintf("#%02x%02x%02x", forecolor.red(),
		       forecolor.green(), forecolor.blue());
	config->writeEntry( "ForeColor", string );

	string="";
	string.sprintf("#%02x%02x%02x", backcolor.red(),
		       backcolor.green(), backcolor.blue());
	config->writeEntry( "BackColor", string );
	
	string="";
	if ( toolbar->barPos() == KToolBar::Left )
		string.sprintf("Left");
	else if ( toolbar->barPos() == KToolBar::Right )
		string.sprintf("Right");
	else if ( toolbar->barPos() == KToolBar::Bottom )
		string.sprintf("Bottom");
	else
		string.sprintf("Top");

	config->writeEntry( "Toolbar position", string );

	////////////////////////////////////////////////////

	config->setGroup("Printing");

	config->writeEntry("PrntCmd1", pi.command);

        string="";
	string.sprintf("%d", pi.selection );
	config->writeEntry("PrintSelection", string);

	string="";
	string.sprintf("%d", pi.raw );
	config->writeEntry("PrintRaw", string);


	config->sync();

	
}

void TopLevel::toggle_overwrite(){

  if(eframe->isOverwriteMode()){
    statusbar->changeItem("OVR",ID_INS_OVR);
  }
  else{
    statusbar->changeItem("INS",ID_INS_OVR);
  }

}

int main (int argc, char **argv)
{

  bool have_top_window = false;
  mykapp = new KApplication (argc, argv, "kedit");

  if ( mykapp->isRestored() ) {

      int n = 1;

      while (KTopLevelWidget::canBeRestored(n)) {
	  TopLevel *tl = new TopLevel();
	  tl->restore(n);
	  TopLevel::windowList.append( tl );
          n++;
	  have_top_window = true;
      }

  }
  else{

    have_top_window = false;
    if (argc > 1) {

    /*
     * check cmdline args
     * FIXME: not completely done yet
     */

        for (int i = 1; i < argc; i++) {

	  bool ok = true;
	
	  if (*argv[i] == '-'){
	    fprintf(stderr,"%s: %s -- no such option\n",argv[0],argv[i]);
	    continue;
	  }
	
	    TopLevel *t = new TopLevel ();
	    t->show ();
	    TopLevel::windowList.append( t );
	    have_top_window = true;

	    QString f = argv[i];
	
	    if ( f.find( ":/" ) == -1 && f.left(1) != "/" )
	    {
		char buffer[ 1024 ];
		getcwd( buffer, 1023 );
		f.sprintf( "%s/%s", buffer, argv[i] );
	    }
	    if( f.find(":/") == -1){
	      // a normal file, let's see whether it exists. If it doesn't
	      // exist try to creat  and open it

	      QFileInfo info(f.data());

	      if ( !info.exists()){

		QFile file(f.data());

		if(!file.open( IO_ReadWrite)){

		  QString string;
		  string.sprintf(i18n("Can not create:\n%s"),f.data());
		  QMessageBox::warning(0,
			     i18n("Sorry"),
			     string.data(),
			     i18n("OK"),
			     0,0);
		  ok = false;

		}
		file.close();
	      }

	    }
		
	    if( ok){
	      t->openNetFile( f.data(), default_open );
	    }
        }
    }

    if(!have_top_window){
      TopLevel *t = new TopLevel ();
      t->show ();
      TopLevel::windowList.append( t );
    }
  }

  return mykapp->exec ();
}


///////////
// Spellchecking

void TopLevel::spell_configure ()
{
  KWM kwm;
  QTabDialog qtd (this, "tabdialog", true);

  qtd.setCaption(i18n("Spellchecker Options"));

  KSpellConfig ksc (&qtd, 0, eframe->ksConfig());
  qtd.addTab (&ksc, "Spellchecker");
  qtd.setCancelButton ();

  //  qtd.resize (ksc.sizeHint().width(), ksc.sizeHint().height());
  kwm.setMiniIcon (qtd.winId(), kapp->getMiniIcon());

  if (qtd.exec())
    {
      ksc.writeGlobalSettings();
      eframe->setKSConfig (ksc);
    }

}
