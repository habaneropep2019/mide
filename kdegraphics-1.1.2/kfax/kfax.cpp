   /*

    $Id: kfax.cpp,v 1.17.2.1 1999/02/26 14:17:17 dfaure Exp $

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
 
#include <qfile.h>
#include <qstrlist.h> 

#include <stdlib.h>
#include <signal.h>
#include <time.h>

#include "kfax.h"
#include "kfax.moc"
#include "version.h"
#include "urldlg.h"

#include <kfiledialog.h>
#include <kstdaccel.h>

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp/"
#endif

TopLevel *toplevel;
QFrame *faxqtwin;


extern  int GetImage(struct pagenode *pn);      

void 	TurnFollowing(int How, struct pagenode *pn);
extern  int  viewfaxmain(int argc, char **argv);
void 	handle_X_event(XEvent event);
int 	notetiff(char *name);
void 	ShowLoop(void);
void 	SetupDisplay();
void    catchSignals();
void 	mysighandler(int sig);
void    setFaxDefaults();

void 	parse(char* buf, char** args);
void 	setfaxtitle(char* name);
int     copyfile(const char* to,char* from);

extern  "C"{
int    fax2psmain(char* faxtiff_file,FILE* psoutput, float width,float height,int scale);
int    fax2tiffmain(char* inputfile, char* outputfile,int bitorder,int stretchit,int type);
}

extern void g31expand(struct pagenode *pn, drawfunc df);
extern void g32expand(struct pagenode *pn, drawfunc df);
extern void g4expand(struct pagenode *pn, drawfunc df);

#define PATIENCE 100000


#undef 	min
#undef 	max
#define min(a,b)	((a)<(b)?(a):(b))
#define max(a,b)	((a)>(b)?(a):(b))

int 	ExpectConfNotify = 1;

GC 	PaintGC;
Cursor 	WorkCursor;
Cursor 	ReadyCursor;
Cursor 	MoveCursor;
Cursor 	LRCursor;
Cursor 	UDCursor;

extern  int abell;			/* audio bell */
extern 	int vbell;			/* visual bell */

extern Time Lasttime;
extern bool have_cmd_opt;
extern  struct pagenode *viewpage;

extern 	XImage *FlipImage(XImage *xi);
extern 	XImage *MirrorImage(XImage *xi);
extern 	XImage *RotImage(XImage *Image);
extern 	XImage *ZoomImage(XImage *Big);
extern 	void FreeImage(XImage *Image);

#define MAXZOOM	10

extern 	XImage *Image, *Images[MAXZOOM];

int 	xpos= 0, ox= 0;	/* x, old x, offset x, new xpos*/
int 	ypos= 0, oy = 0;	/* y, old y, offset y, new y */
int 	offx = 0;
int 	offy = 0;
int 	nx = 0;
int 	ny = 0;

int oz, Resize = 0, Refresh = 0;	/* old zoom, window size changed,
					   needs updating */
int PaneWidth, PaneHeight;		/* current size of our window */
int AbsX, AbsY;				/* absolute position of centre of window */

int Reparented = 0;

Display* qtdisplay;

int 	startingup;
Window  qtwin;    // the qt window
Window 	Win;
int 	counter = 0;
int 	qwindow_height;
int 	qwindow_width;
bool 	have_no_fax = TRUE;
QString fname;
bool	display_is_setup = FALSE;
struct 	optionsinfo fop;   // contains the fax options

extern 	struct pagenode *firstpage, *lastpage, *thispage, *helppage;
extern  struct pagenode* auxpage;

extern 	struct pagenode defaultpage;
extern  size_t Memused;

extern int zfactor;
int faxpagecounter = 0;
bool buttondown;



MyApp::MyApp(int &argc, char **argv , const QString& rAppName):
  KApplication(argc, argv, rAppName){
}

MyApp* mykapp = NULL;


bool MyApp::x11EventFilter( XEvent * ev){

  /*  printf("Event  %d for window %d\n", ev->type, ev->xany.window);*/

  if (KApplication::x11EventFilter(ev))
    return TRUE;

  if (ev->type  ==  ButtonRelease){
    /* this is so that the cursor goes back to normal on leaving the fax window
       and that the fax won't be moved when I reenter after I release the mouse*/

    if (buttondown == true){
      buttondown = false;
      XDefineCursor(qtdisplay, Win, ReadyCursor);
      XFlush(qtdisplay);
    }
  }
  if ( ev->xany.window == qtwin ||
       ev->xany.window == Win){

    if(startingup || have_no_fax)
      return FALSE;
    
    toplevel->handle_X_event(*ev);
    ev->xany.window = qtwin;

  }

  return FALSE;

}


TopLevel::TopLevel (QWidget *, const char *name)
    : KTopLevelWidget ( name)
{

  setMinimumSize (100, 100);

  kfm = 0L;
  file_dialog = NULL;
  printdialog = NULL;
  buttondown = false;
  statusbar_timer = new QTimer(this);
  connect(statusbar_timer, SIGNAL(timeout()),this,SLOT(timer_slot()));

  connect(mykapp,SIGNAL(kdisplayPaletteChanged()),this,SLOT(set_colors()));
  

  setupMenuBar();
  setupToolBar();
  setupStatusBar();

  readSettings();

  faxqtwin = new QFrame(this);

  qtwin = faxqtwin->winId();
  faxqtwin->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  // Create a Vertical scroll bar

  vsb = new QScrollBar( QScrollBar::Vertical,faxqtwin,"scrollBar" );
  vsb->hide();
  connect( vsb, SIGNAL(valueChanged(int)), SLOT(scrollVert(int)) );

  // Create a Horizontal scroll bar

  hsb = new QScrollBar( QScrollBar::Horizontal,faxqtwin,"scrollBar" );
  connect( hsb, SIGNAL(valueChanged(int)), SLOT(scrollHorz(int)) );
  hsb->hide();


  setView(faxqtwin,TRUE);
  set_colors();

  if ( hide_toolbar )	
    options->changeItem( i18n("Show &Tool Bar"), toolID );
  else
    options->changeItem(i18n( "Hide &Tool Bar"), toolID );

  if ( hide_statusbar )
    options->changeItem(i18n( "Show &Status Bar"), statusID );
  else
    options->changeItem( i18n("Hide &Status Bar"), statusID );

  setCaption("KFax "KFAXVERSION);

  KDNDDropZone * dropZone = new KDNDDropZone( this , DndURL);
  connect( dropZone, SIGNAL( dropAction( KDNDDropZone *) ), 
	   this, SLOT( slotDropEvent( KDNDDropZone *) ) );


  // Calling resize as is done here will reduce flicker considerably
  // ktoplevelwidget updateRect will be called only twice this way instead of 4 times.
  // If I leave the first resize out the toolbar will not be displayed and 
  // ktoplevelwidget updateRect will be called four times. All this needs to 
  // be looked into. ktoolbar <-> ktoplevelwidget

  resize(editor_width,editor_height);
  show();
  resize(editor_width,editor_height);
}


TopLevel::~TopLevel (){

  delete file;
  delete help;
  delete options;
}

void TopLevel::pageActivated(const char*){


}

void TopLevel::setFaxTitle(char* name){

  QString str("KFax: ");
  str.append(name);
  setCaption(str);
}

void TopLevel::setupEditWidget(){


}


void TopLevel::resizeEvent(QResizeEvent *e){

  KTopLevelWidget::resizeEvent(e);

  if(!faxqtwin || !display_is_setup)
    return;

  resizeView();
}

void TopLevel::resizeView(){


  if(!faxqtwin || !display_is_setup)
    return;

//printf("In resizeView() entered\n");

  qwindow_width = faxqtwin->width();
  qwindow_height = faxqtwin->height();

  if( hsb->isVisible())
    qwindow_height -= 16;

  if( vsb->isVisible())
    qwindow_width  -= 16;
    
  if(Image){
    PaneWidth = Image->width;
    PaneHeight = Image->height;
  }

  //  printf("faxw %d qtw %d\n", PaneWidth , faxqtwin->width());

  if( (PaneHeight  > qwindow_height ) &&
      (PaneWidth  > qwindow_width)){

    vsb->setGeometry(faxqtwin->width() - 16,0,16,faxqtwin->height()-16);
    hsb->setGeometry(0,faxqtwin->height() - 16 ,faxqtwin->width()-16,16);

    qwindow_width = faxqtwin->width() -16;
    qwindow_height = faxqtwin->height()-16;


    vsb->raise();
    vsb->show();
    hsb->show();
  }
  else{
    

    if( PaneHeight  > qwindow_height){
      vsb->setGeometry(faxqtwin->width() - 16,0,16,faxqtwin->height());


      qwindow_width = faxqtwin->width() -16 ;
      qwindow_height = faxqtwin->height();


      vsb->show();
      hsb->hide();
      hsb->raise();
    }
     else
       vsb->hide();

    if( PaneWidth  > qwindow_width ){
      hsb->setGeometry(0,faxqtwin->height() - 16 ,faxqtwin->width(),16);
      hsb->show();
      hsb->raise();
      vsb->hide();
      qwindow_width = faxqtwin->width() ;
      qwindow_height = faxqtwin->height() -16;

    }
    else
      hsb->hide();

  }

  if(Image){
    hsb->setRange(0,max(0,Image->width - qwindow_width));
    hsb->setSteps(5,qwindow_width/2);
    //    printf("hsb range: %d\n",max(0,Image->width - qwindow_width));
    vsb->setRange(0,max(0,Image->height - qwindow_height));
    vsb->setSteps(5,qwindow_height/2);
    //    printf("vsb range: %d\n",max(0,Image->height - qwindow_height));
    //    printf("vsb min %d vdb max %d\n",vsb->minValue(),vsb->maxValue());
  }


  Resize = 1;
  uiUpdate();

}



void TopLevel::setupMenuBar(){

  KStdAccel keys(mykapp->getConfig());

  file = 	new QPopupMenu ();
  help = 	new QPopupMenu ();
  view =        new QPopupMenu ();
  options = 	new QPopupMenu ();


  QString about;
  about.sprintf(i18n("\nKFax %s\n\n"
		     "Copyright 1997-98\n"
                     "Bernd Johannes Wuebben\n"
                     "wuebben@kde.org"), KFAXVERSION);

  help = mykapp->getHelpMenu(TRUE, about);

  help->insertItem (i18n("Help page"), 		this, 	SLOT(helpshort()));
  
  file->insertItem (i18n("&Open Fax..."),		this, 	SLOT(file_open()),keys.open());
  file->insertItem (i18n("&Add Fax..."),		this, 	SLOT(addFax()));
  //  file->insertItem ("&Save As...",		this, 	SLOT(dummy()));
  file->insertSeparator (-1);
  file->insertItem (i18n("Open &URL..."),	this,	SLOT(file_open_url()));
  file->insertItem (i18n("Save to U&RL..."),	this,	SLOT(file_save_url()));
  file->insertItem (i18n("&Close Fax"),		this,	SLOT(closeFax()),keys.close());
  file->insertSeparator (-1);
  file->insertItem (i18n("&Print..."),	this,	SLOT(print()),keys.print() );
  
  file->insertSeparator (-1);
  file->insertItem (i18n("E&xit"), 		this,	SLOT(quiteditor()),keys.quit());

  view->insertItem(i18n("&Next Page"),this,SLOT(nextPage()));
  view->insertItem(i18n("&Prev Page"),this,SLOT(prevPage()));
  view->insertItem(i18n("&First Page"),this,SLOT(goToStart()));
  view->insertItem(i18n("&Last Page"),this,SLOT(goToEnd()));
  view->insertSeparator(-1);
  view->insertItem(i18n("&Rotate Page"),this,SLOT(rotatePage()));
  view->insertItem(i18n("&Mirror Page"),this,SLOT(mirrorPage()));
  view->insertItem(i18n("&Flip Page"),this,SLOT(flipPage()));
  view->insertSeparator(-1);
  view->insertItem(i18n("Zoom &in"),this,SLOT(zoomin()));
  view->insertItem(i18n("Zoom &out"),this,SLOT(zoomout()));



  options->setCheckable(TRUE);
  options->insertItem(i18n("&Fax Options..."),this,SLOT(faxoptions()));
  options->insertSeparator(-1);
  toolID   = options->insertItem(i18n("&Tool Bar"),this,SLOT(toggleToolBar()));
  statusID = options->insertItem(i18n("&Status Bar"),this,SLOT(toggleStatusBar()));	


  menubar = new KMenuBar (this, "menubar");
  menubar->insertItem (i18n("&File"), file);
  menubar->insertItem (i18n("&View"), view);
  menubar->insertItem (i18n("&Options"), options);
  menubar->insertSeparator(-1);
  menubar->insertItem (i18n("&Help"), help);

  setMenu(menubar);
  

}


void TopLevel::setupToolBar(){


  QString PIXDIR = mykapp->kde_toolbardir().copy();

  PIXDIR += "/";

  QPixmap pixmap;
  toolbar = new KToolBar( this );

  pixmap.load(PIXDIR + "back.xpm");
  toolbar->insertButton(pixmap, ID_PREV, TRUE, i18n("Go back"));
  
  pixmap.load(PIXDIR + "forward.xpm");
  toolbar->insertButton(pixmap, ID_NEXT, TRUE, i18n("Go forward"));
  
  toolbar->insertSeparator();
  
  pixmap.load(PIXDIR + "start.xpm");
  toolbar->insertButton(pixmap, ID_START, TRUE, i18n("Go to start"));
  
  pixmap.load(PIXDIR + "finish.xpm");
  toolbar->insertButton(pixmap, ID_END, TRUE, i18n("Go to end"));


  toolbar->insertSeparator();
  
  pixmap.load(PIXDIR + "viewmag+.xpm");
  toolbar->insertButton(pixmap, ID_ZOOM_IN, TRUE,i18n( "Zoom in"));

  pixmap.load(PIXDIR + "viewmag-.xpm");
  toolbar->insertButton(pixmap, ID_ZOOM_OUT, TRUE, i18n("Zoom out"));
  
  toolbar->insertSeparator();
  
  pixmap.load(PIXDIR + "fileprint.xpm");
  toolbar->insertButton(pixmap, ID_PRINT, TRUE, i18n("Print document"));

  /*  
  pixmap.load(PIXDIR + "tick.xpm");
  toolbar->insertButton(pixmap, ID_MARK, TRUE, "Mark this page");
  */
    toolbar->insertSeparator();

  pixmap.load(PIXDIR + "page.xpm");
  toolbar->insertButton(pixmap, ID_PAGE, TRUE, i18n("Go to page ..."));
  

  toolbar1 = addToolBar( toolbar );
	
  toolbar->setBarPos( KToolBar::Top );
	
  connect(toolbar, SIGNAL( clicked( int ) ), this,
						SLOT( toolbarClicked( int ) ) );

  connect(toolbar, SIGNAL( moved( BarPosition ) ), this,
					SLOT( toolbarMoved(BarPosition ) ) );
											

}

void TopLevel::toolbarMoved(BarPosition  ){

  resizeView();

}

void TopLevel::toolbarClicked( int item ){

	switch ( item ) {
  		case ID_PREV:
		        prevPage();
  			break;
  		case ID_NEXT:
		        nextPage();
  			break;
  		case ID_PAGE:
		        helpselected();
  			break;
  		case ID_ZOOM_IN:
		        zoomin();
  			break;
  		case ID_ZOOM_OUT:
		        zoomout();
  			break;
  		case ID_PRINT:
  			print();
  			break;
  		case ID_START:
		        goToStart();
  			break;
  		case ID_END:
		        goToEnd();
  			break;
  		case ID_READ:
  			break;
  	}
}

void TopLevel::setupStatusBar(){

    statusbar = new KStatusBar( this );
    //    statusbar->insertItem("Mem: 000000000", ID_LINE_COLUMN);
    statusbar->insertItem(i18n("w: 00000 h: 00000"), ID_INS_OVR);
    statusbar->insertItem(i18n("Res: XXXXX"), ID_GENERAL);
    statusbar->insertItem(i18n("Type: XXXXXXX"), ID_TYPE);
    statusbar->insertItem(i18n("P:XXXofXXX"), ID_PAGE_NO);
    statusbar->insertItem("",ID_FNAME);

    //    statusbar->changeItem("", ID_LINE_COLUMN);
    statusbar->changeItem("", ID_INS_OVR);
    statusbar->changeItem("", ID_GENERAL);
    statusbar->changeItem("", ID_TYPE);
    statusbar->changeItem("", ID_PAGE_NO);
    statusbar->changeItem("",ID_FNAME);

    statusbar->setInsertOrder(KStatusBar::RightToLeft);

    //    statusbar->setAlignment(ID_INS_OVR,AlignCenter);

    //    statusbar->setInsertOrder(KStatusBar::LeftToRight);
    //    statusbar->setBorderWidth(1);

    setStatusBar( statusbar );

}



void TopLevel::file_open_url(){

  UrlDlg l( this,i18n("Open Location:"), "ftp://localhost/welcome");

  if ( l.exec() )
    {
      QString n = l.getText();
      if ( n.left(5) != "file:" && n.left(4) == "ftp:" )
	openNetFile( l.getText());
      else
	openNetFile( l.getText());
    }

}

void TopLevel::file_save_url(){

  if(!thispage || !thispage->pathname){
    QMessageBox::information(this,"KFax",i18n("You need to open a Fax first"));
    return;
  }

  if(thispage && thispage->pathname){

    UrlDlg l( this, i18n("Save to Location:"), thispage->pathname);

    if ( l.exec() )
      {
	QString n = l.getText();
	if ( n.left(5) != "file:" && n.left(4) == "ftp:" )
	  saveNetFile( l.getText() );	
	else		
	  saveNetFile( l.getText() );
      }
  }
}



void TopLevel::quiteditor(){
  
  writeSettings();
  mykapp->quit();
  
}




void TopLevel::file_close(){


}

void TopLevel::file_save(){


}


void TopLevel::file_open(){

  QString newfile;

  /*
  QString d;
  if ( !filename.isEmpty() )
    d.sprintf( QFileInfo( filename ).dirPath() );
  else
    d.sprintf( QDir::currentDirPath() );
    */


   newfile = KFileDialog::getOpenFileName(current_directory.data(),"*");
   newfile.detach();
    
    if (newfile.isEmpty()) {
      return ;
    }

    
  current_directory =  QFileInfo( newfile ).dirPath() ;
  current_directory.detach();

  FreeFax();

  fname = newfile.copy();
  
  (void) notetiff(fname.data());	
  
  thispage = auxpage;

  have_no_fax = false;
  newPage();
  resizeView();
  putImage();

}


void TopLevel::addFax(){

  QString newfile;

  /*
  QString d;
  if ( !filename.isEmpty() )
    d.sprintf( QFileInfo( filename ).dirPath() );
  else
    d.sprintf( QDir::currentDirPath() );
    */


   newfile = KFileDialog::getOpenFileName(current_directory.data(),"*");
   newfile.detach();
    
    if (newfile.isEmpty()) {
      return ;
    }

  current_directory =  QFileInfo( newfile ).dirPath() ;
  current_directory.detach();    

  fname = newfile.copy();

  openadd(fname);
}


void TopLevel::openadd(QString filename){

  bool this_is_the_first_fax;

  if(firstpage == lastpage)
    this_is_the_first_fax = TRUE;
  else
    this_is_the_first_fax = FALSE;

    
  auxpage = lastpage;

  (void) notetiff(filename.data());	
  
  if(!this_is_the_first_fax){
    if(auxpage->next)
      auxpage = auxpage->next;
  }

  // auxpage should now point to the first pagenode which was created for
  // the newly added fax file.


  have_no_fax = false;
  thispage = auxpage;
  newPage();
  resizeView();
  putImage();

}

/*
QFileDialog* TopLevel::getFileDialog(const char* captiontext){

  if(!file_dialog){

    file_dialog = new QFileDialog(current_directory.data(),"*",this,"file_dialog",TRUE);
  }

  file_dialog->setCaption(captiontext);
  file_dialog->rereadDir();

  return file_dialog;
}
*/


void TopLevel::setGeneralStatusField(QString text){

    statusbar_timer->stop();
    statusbar->changeItem(text.data(),ID_FNAME);
    statusbar_timer->start(10000,TRUE); // single shot
    
}


void TopLevel::file_save_as(){


}




void TopLevel::helpselected(){
  
  mykapp->invokeHTMLHelp( "" , "" );


}

void TopLevel::helpshort(){

  helptiff();

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
  resizeView();

}


void TopLevel::dummy(){


  QMessageBox::information(this,"KFax",i18n("Not yet implemented"));
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
  
  resizeView();
}	


void TopLevel::closeEvent( QCloseEvent *e ){

  (void) e;

  file_close();
  writeSettings();
  mykapp->quit();
};


void TopLevel::setStatusBarMemField(int mem){


    QString memstr;

    memstr.sprintf("Mem: %d",mem);
    //    statusbar->changeItem(memstr.data(), ID_LINE_COLUMN);

}


void TopLevel::print(){



  if(!thispage){
    QMessageBox::information(this,"KFax",i18n("     There is no document active      "));
    return;
  }

  if(!printdialog){
    printdialog = new PrintDialog(NULL,"print",true);
    connect(printdialog,SIGNAL(print()),this,SLOT(printIt()));

  }

  struct printinfo newpi;

  newpi.file = pi.file;
  newpi.file.detach();
  newpi.cmd = pi.cmd;
  newpi.cmd.detach();
  newpi.lpr = pi.lpr;
  newpi.scale = pi.scale;
  newpi.pagesize = pi.pagesize;
  newpi.pagesize.detach();
  newpi.margins = pi.margins;
  newpi.xmargin = pi.xmargin;
  newpi.ymargin = pi.ymargin;
  
  printdialog->setWidgets(&newpi);

  QPoint point = this->mapToGlobal (QPoint (0,0));

  QRect pos = this->geometry();
  printdialog->setGeometry(point.x() + pos.width()/2  - printdialog->width()/2,
			   point.y() + pos.height()/2 - printdialog->height()/2, 
			   printdialog->width(),
			   printdialog->height()
			   );

  printdialog->show();

}

void TopLevel::printIt(){

  if(!firstpage)
    return;

  if(!printdialog)
    return;

  mykapp->processEvents();

  struct printinfo *newpi;

  newpi = printdialog->getInfo();

  pi.file = newpi->file;
  pi.file.detach();
  pi.cmd = newpi->cmd;
  pi.cmd.detach();
  pi.lpr = newpi->lpr;
  pi.scale = newpi->scale;
  pi.pagesize = newpi->pagesize;
  pi.pagesize.detach();
  pi.margins = newpi->margins;
  pi.xmargin = newpi->xmargin;
  pi.ymargin = newpi->ymargin;


  float width = 8.5;
  float height = 11;

  if(strcmp(pi.pagesize.data(), US_LETTER) == 0){
    width = 8.5;
    height = 11;
  }

  if(strcmp(pi.pagesize.data(), US_LEGAL) == 0){
    width = 8.5;
    height = 14;
  }

  if(strcmp(pi.pagesize.data(), US_LEDGER) == 0){
    width = 11;
    height = 17;
  }

  if(strcmp(pi.pagesize.data(), US_EXECUTIVE) == 0){
    width = 7.25;
    height = 10.5;
  }

  if(strcmp(pi.pagesize.data(), DIN_A3) == 0){
    width = 12.69;
    height = 16.53;
  }

  if(strcmp(pi.pagesize.data(), DIN_A4) == 0){
    width = 8.267;
    height = 11.692;
  }

  if(strcmp(pi.pagesize.data(), DIN_A5) == 0){
    width = 5.944;
    height = 8.2675;
  }

  if(strcmp(pi.pagesize.data(), DIN_A6) == 0){
    width = 4.2125;
    height = 5.8258;
  }

  if(strcmp(pi.pagesize.data(), DIN_B4) == 0){
    width = 10.04;
    height = 14.33;
  }

  if(strcmp(pi.pagesize.data(), JAP_LETTER) == 0){
    width = 7.165;
    height = 10.1175;
  }

  if(strcmp(pi.pagesize.data(), JAP_LEGAL) == 0){
    width = 10.1175;
    height = 14.33;
  }


  if(pi.margins == 1){
    width  = width - pi.xmargin/2.54*2.0; /* 2.0 since we want this margin both at the
					     top as well as on the bottom */
    height = height - pi.ymargin/2.54*2.0;
  }

  if(width <= 0.0 || height <=0.0 ){

    QString str;
    str.sprintf(i18n("Invalid page dimensions:\nWidth %.2f Height %.2f\n"),
		width,height);
    QMessageBox::warning(this, "KFax",str.data(), "OK",0);

    return;
  }

  FILE* psfile;

  if(pi.lpr){

    psfile = NULL;
    psfile = popen(pi.cmd.data(),"w");

    if(psfile == NULL){
      QString str;
      str.sprintf(i18n("Can't print to \"%s\"\n"),pi.cmd.data());
      QMessageBox::warning(this, "KFax",str.data(), i18n("OK"),0);
      return;
    }

  }
  else{

    psfile = fopen(pi.file.data(),"w");
    if(psfile == NULL){
      QString str;
      str.sprintf(i18n("Couldn't create %s\n"),pi.file.data());
      QMessageBox::warning(this,"KFax",str.data(), i18n("OK"),0);
      return;
    }
  }

  QApplication::setOverrideCursor( waitCursor );
  XDefineCursor(qtdisplay, Win, WorkCursor);
  mykapp->processEvents();

  struct pagenode *pn;
  QStrList filelist;

  for(pn = firstpage; pn; pn = pn->next){

    if(filelist.find(pn->pathname) != -1) // we have printed this fax file already
      continue; 

    filelist.append(pn->pathname);

    if(pn->type == FAX_TIFF){

      fax2psmain(pn->pathname,psfile,width,height,pi.scale);

    }

    if(pn->type == FAX_RAW){

      int faxtype = 31;

      if(defaultpage.expander ==  g32expand)
	faxtype = 32;

      if(defaultpage.expander ==  g4expand)
	faxtype = 4;

      if(defaultpage.expander ==  g31expand)
	faxtype = 31;


      QString tempfile;
      tempfile = tmpnam(NULL);

      fax2tiffmain(pn->pathname,tempfile.data(),pn->lsbfirst,pn->vres?0:1,faxtype);
      fax2psmain(tempfile.data(),psfile,width,height,pi.scale);
      
      remove(tempfile.data());

    }

  }

  if(pi.lpr)
    pclose(psfile);
  else
    fclose(psfile);

  QApplication::restoreOverrideCursor();
  XDefineCursor(qtdisplay, Win, ReadyCursor);

}


void TopLevel::setSensitivity (){

}


void TopLevel::saving_slot(){

  setGeneralStatusField(i18n("Saving ..."));

}


void TopLevel::loading_slot(){

  setGeneralStatusField(i18n("Loading ..."));

}


void TopLevel::saveNetFile( const char * _url)
{
    
    int res;
    netFile = _url;
    netFile.detach();
    KURL u( netFile.data() );
    if ( u.isMalformed() )
    {
	QMessageBox::warning(this,"KFax", i18n("Malformed URL"), i18n("OK"),0);
	return;
    }
    
    // Just a usual file ?
    if ( strcmp( u.protocol(), "file" ) == 0 )
      {

	QString string;

	setGeneralStatusField(string);
	res = copyfile(u.path(),thispage->pathname);
	if (res==0) {
	  QMessageBox::warning(this, "KFax", i18n("Failure in 'copy file()'\n"\
				"Couldn't save file!"),i18n( "OK"),0);
	  return;
	} 


	setGeneralStatusField(i18n("Saved"));
	return;
      }
    
    if ( kfm != 0L )
      {
	QMessageBox::warning(this,"KFax", 
			      i18n("KFax is already waiting\n"\
			      "for an internet job to finish\n"\
			      "Please wait until has finished\n"), 
			      i18n("OK"),0);
	return;
    }

    kfm = new KFM;
    if ( !kfm->isOK() )
    {
	QMessageBox::warning(this,"KFax", 
			     i18n("Could not communicate with KFM"),
			     i18n("OK"),0);
	delete kfm;
	kfm = 0L;
	return;
    }
    
    tmpFile.sprintf( _PATH_TMP"/kfax%i", time( 0L ) );

    res = copyfile(tmpFile.data(),thispage->pathname );
    if (res==0) {
      QMessageBox::warning(this,"KFax", 
			   i18n("Failure in 'copy file()'\n"\
			    "Couldn't save file!"), 
			   i18n("OK"),0);
      return;
    } 

    connect( kfm, SIGNAL( finished() ), this, SLOT( slotKFMFinished() ) );
    kfm->copy( tmpFile.data(), netFile.data() );
    kfmAction = TopLevel::PUT;

}

void TopLevel::openNetFile( const char * _url)
{
    
  QString string;
  netFile = _url;
  netFile.detach();
  KURL u( netFile.data() );
  if ( u.isMalformed() )
    {
	QMessageBox::warning(this, "KFax", i18n("Malformed URL"), i18n("OK"),0);
	return;
    }

    // Just a usual file ?
    if ( strcmp( u.protocol(), "file" ) == 0 )
    {
      QString string;
      string.sprintf(i18n("Loading '%s'"),u.path() );
      setGeneralStatusField(string);
      openadd( u.path());
      setGeneralStatusField(i18n("Done"));
      return;
    }
    
    if ( kfm != 0L )
    {
	QMessageBox::warning(this, "KFax", 
			      i18n("KFax is already waiting\n"\
			      "for an internet job to finish\n"\
			      "Please wait until has finished\n"), 
			      i18n("OK"),0);
	return;
    }

    setGeneralStatusField(i18n("Calling KFM"));
    
    kfm = new KFM;
    if ( !kfm->isOK() )
    {
	QMessageBox::warning(this, "KFax", 
			     i18n("Could not communicate with KFM"), 
			     i18n("OK"),0);
	delete kfm;
	kfm = 0L;
	return;
    }
    
    setGeneralStatusField(i18n("Starting Job"));
    tmpFile.sprintf( "file:/tmp/kfax%i", time( 0L ) );
    connect( kfm, SIGNAL( finished() ), this, SLOT( slotKFMFinished() ) );
    setGeneralStatusField(i18n("Connected"));
    kfm->copy( netFile.data(), tmpFile.data() );
    setGeneralStatusField(i18n("Waiting..."));
    kfmAction = TopLevel::GET;


}

void TopLevel::slotKFMFinished()
{

  QString string;
  string.sprintf("Finished '%s'",tmpFile.data());
  setGeneralStatusField(string);

    if ( kfmAction == TopLevel::GET )
    {
	KURL u( tmpFile.data() );
	openadd( u.path());
	setCaption( netFile.data() );
	
	// Clean up
	unlink( tmpFile.data() );
	delete kfm;
	kfm = 0L;
    }
    if ( kfmAction == TopLevel::PUT )
    {
	unlink( tmpFile.data() );
	delete kfm;
	kfm = 0L;
    }

}

void TopLevel::slotDropEvent( KDNDDropZone * _dropZone)
{

  QStrList & list = _dropZone->getURLList();
    
  char *s;

  s = list.first();

   if(s != 0L){

     // Load the first file in this window

     if ( s == list.getFirst() )
       {
	 QString n = s;
	 openNetFile( n.data());

       }
   }
   // lets ignore others for the moment.... ;-)
}

void TopLevel::timer_slot(){

  if(thispage){
    if(thispage->name)
      statusbar->changeItem(thispage->name,ID_FNAME);
  }
  else
      statusbar->changeItem("",ID_FNAME);

}


void TopLevel::set_colors(){
 
  /*
  QPalette mypalette = (eframe->palette()).copy();

  QColorGroup cgrp = mypalette.normal();
  QColorGroup ncgrp(forecolor,cgrp.background(),
		    cgrp.light(),cgrp.dark(),cgrp.mid(),forecolor,backcolor);

  mypalette.setNormal(ncgrp);
  mypalette.setDisabled(ncgrp);
  mypalette.setActive(ncgrp);

  eframe->setPalette(mypalette);
  eframe->setBackgroundColor(backcolor);
  */
}


void TopLevel::readSettings(){

	// let's set the defaults 

	editor_width = 550;
	editor_height = 400;
	hide_statusbar = FALSE;
	hide_toolbar = FALSE;
	
	// fas display options

	fop.width = 1728;
	fop.height = 2339;
	fop.resauto = 1;
	fop.geomauto = 1;
	fop.fine = 1;
	fop.landscape = 0;
	fop.flip = 0;
	fop.invert =0;
	fop.lsbfirst = 0;
	fop.raw = 3;  // raw faxes are g3 by default -- 2 stands for g32d and 4 for g4
	
	current_directory = QDir::currentDirPath();


	QString str;
	
	config = mykapp->getConfig();

	///////////////////////////////////////////////////
	config->setGroup("General Options");


	str = config->readEntry("DefaultPath");
	if ( !str.isEmpty() ){
	  current_directory = str;
	  current_directory.detach();
	}

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
	config->setGroup("Fax Options");

	str = config->readEntry("width");
		if ( !str.isNull() )
		  fop.width = atoi(str.data());

	str = config->readEntry("resauto");
		if ( !str.isNull() )
		  fop.resauto = atoi(str.data());

	str = config->readEntry("geomauto");
		if ( !str.isNull() )
		  fop.geomauto = atoi(str.data());

	str = config->readEntry("height");
		if ( !str.isNull() )
		  fop.height = atoi(str.data());

	str = config->readEntry("resolution");
		if ( !str.isNull() )
		  fop.fine = atoi(str.data());

	str = config->readEntry("flip");
		if ( !str.isNull() )
		  fop.flip = atoi(str.data());

	str = config->readEntry("invert");
		if ( !str.isNull() )
		  fop.invert = atoi(str.data());

	str = config->readEntry("lsb");
		if ( !str.isNull() )
		  fop.lsbfirst = atoi(str.data());

	str = config->readEntry("raw");
		if ( !str.isNull() )
		  fop.raw = atoi(str.data());

		
	 setFaxDefaults();
	///////////////////////////////////////////////////

	config->setGroup("Printing");

	pi.cmd = config->readEntry("PrintCommand","lpr");
	pi.file = config->readEntry("PrintFile","");
	pi.lpr = config->readNumEntry("print",1);
	pi.scale = config->readNumEntry("scale",0);
	pi.margins = config->readNumEntry("margins",0);
	str = config->readEntry("xmargin","0.0");
	pi.xmargin = atof(str.data());
	str = config->readEntry("ymargin","0.0");
	pi.ymargin = atof(str.data());
	pi.pagesize = config->readEntry("PageSize",US_LETTER);
}

void TopLevel::writeSettings(){
		

  	config = mykapp->getConfig();
	
	///////////////////////////////////////////////////

	
	config->setGroup("General Options");

        config->writeEntry("DefaultPath",current_directory);

	QString widthstring;
	widthstring.sprintf("%d", this->width() );
	config->writeEntry("Width",  widthstring);

	QString heightstring;
	heightstring.sprintf("%d", this->height() );

	config->writeEntry("Height",  heightstring);

	config->writeEntry( "StatusBar", hide_statusbar ? "off" : "on" );
	config->writeEntry( "ToolBar", hide_toolbar ? "off" : "on" );

	QString string;
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

	config->setGroup("Fax Options");

	config->writeEntry("resauto",fop.resauto);
	config->writeEntry("geomauto",fop.geomauto);
	config->writeEntry("width",fop.width);
	config->writeEntry("height",fop.height);
	config->writeEntry("resolution",fop.fine);
	config->writeEntry("flip",fop.flip);
	config->writeEntry("invert",fop.invert);
	config->writeEntry("lsb",fop.lsbfirst);
	config->writeEntry("raw",fop.raw);

       ////////////////////////////////////////////

	config->setGroup("Printing");

	config->writeEntry("PrintCommand",pi.cmd);
	config->writeEntry("PrintFile",pi.file);
	config->writeEntry("print",pi.lpr);
	config->writeEntry("scale",pi.scale);
	config->writeEntry("PageSize",pi.pagesize);
	config->writeEntry("margins",pi.margins);
	string.sprintf("%.2f",pi.xmargin);
	config->writeEntry("xmargin",string);
	string.sprintf("%.2f",pi.ymargin);
	config->writeEntry("ymargin",string);
	config->sync();	
}


void SetupDisplay(){

  if(display_is_setup){
    return;
  }

  display_is_setup = TRUE;

  xpos = ypos = ox = oy = 0;
  ExpectConfNotify = 1;

  /*  XSizeHints size_hints;*/

  zfactor = 4; // a power of two 
  // the original image size is zfactor 0 the next small size (half as large)
  // is zfactor 1 etc. 

  /*
  int faxh = Pimage(thispage)->height;
  int faxw = Pimage(thispage)->width;

  // TODO Let the user choose this in a settings dialog

  
  int i;
  for (size_hints.width = faxw, i = 1; i < zfactor; i *= 2)
    size_hints.width = (size_hints.width + 1) /2;

  for (size_hints.height = faxh, i = 1; i < zfactor; i *= 2)
    size_hints.height = (size_hints.height + 1) /2;

  Win = XCreateSimpleWindow(qtdisplay,qtwin,0,0,
			    size_hints.width,size_hints.height,
			    0,
			    BlackPixel(qtdisplay,XDefaultScreen(qtdisplay)),
			    WhitePixel(qtdisplay,XDefaultScreen(qtdisplay)));

			    */

  Win = XCreateSimpleWindow(qtdisplay,qtwin,1,1,
			    1,1,
			    0,
			    BlackPixel(qtdisplay,XDefaultScreen(qtdisplay)),
			    WhitePixel(qtdisplay,XDefaultScreen(qtdisplay)));

  PaintGC = XCreateGC(qtdisplay, Win, 0L, (XGCValues *) NULL);
  XSetForeground(qtdisplay, PaintGC, BlackPixel(qtdisplay, XDefaultScreen(qtdisplay) ));
  XSetBackground(qtdisplay, PaintGC, WhitePixel(qtdisplay, XDefaultScreen(qtdisplay) ));
  XSetFunction(qtdisplay, PaintGC, GXcopy);
  WorkCursor = XCreateFontCursor(qtdisplay, XC_watch);
  //ReadyCursor = XCreateFontCursor(qtdisplay, XC_plus);
  ReadyCursor = XCreateFontCursor(qtdisplay, XC_hand2);
  MoveCursor = XCreateFontCursor(qtdisplay, XC_fleur);
  LRCursor = XCreateFontCursor(qtdisplay, XC_sb_h_double_arrow);
  UDCursor = XCreateFontCursor(qtdisplay, XC_sb_v_double_arrow);

  XSelectInput(qtdisplay, Win, Button2MotionMask | ButtonPressMask |
	       ButtonReleaseMask | ExposureMask | KeyPressMask |
	       SubstructureNotifyMask | LeaveWindowMask | OwnerGrabButtonMask |
	       StructureNotifyMask);

  XMapRaised(qtdisplay, Win);

  XDefineCursor(qtdisplay, Win, ReadyCursor);
  XFlush(qtdisplay);
  
  for (oz = 0; oz < MAXZOOM; oz++)
    Images[oz] = NULL;


  // setup oz the default zoom factor   
  for (oz = 0; oz < MAXZOOM && zfactor > (1 << oz); oz++){
  }


}




void TopLevel::zoomin(){

  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;
  
  if (oz > 0) {
    Image = Images[--oz];
    zfactor >>= 1;
    
    Resize = Refresh = 1;
  }

  int i;

  if (Image == NULL) {

    for (i = oz; i && (Images[i] == NULL); i--)
      ;
    for (; i != oz; i++){
      Images[i+1] = ZoomImage(Images[i]);
      Image = Images[i+1];

      if(Images[i+1] == NULL){ // out of memory	
	Image = Images[i]; 
	break;
      }
    }

    
  }

  PaneWidth = Image->width;
  PaneHeight = Image->height;

  resizeView();
  putImage();
  
  uiUpdate();

}


void TopLevel::zoomout(){

  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;
 
  if (oz < MAXZOOM && Image->width >= 64 && zfactor < 32) {
    Image = Images[++oz];
    zfactor <<= 1;

    Resize = Refresh = 1;
    
  }

  int i;

  if (Image == NULL) {
    for ( i = oz; i && (Images[i] == NULL); i--)
      ;
    for (; i != oz; i++){
      Images[i+1] = ZoomImage(Images[i]);
      Image = Images[i+1];

      if(Images[i+1] == NULL){ // out of memory	
	Image = Images[i]; 
	break;
      }
    }
  }

  PaneWidth = Image->width;
  PaneHeight = Image->height;
  
  resizeView();
  putImage();

  uiUpdate();

}

void TopLevel::handle_X_event(XEvent Event){


  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  if(!Image)
    return;

  if(!faxqtwin || !display_is_setup)
    return;


  bool putimage = false; // Do we actually have to write the image to the scree?



  do {
    switch(Event.type) {
    case MappingNotify:
      XRefreshKeyboardMapping((XMappingEvent *)(&Event));
		break;

    case LeaveNotify:
      /*      buttondown = false;
	XDefineCursor(qtdisplay, Win, ReadyCursor);
	XFlush(qtdisplay);*/
      break;
    case Expose:
      {

	if(Event.xexpose.count != 0)
	  break;
	
	if(!Image)
	  break;

	putimage = TRUE;
      }	
    break;

    case KeyPress:
      if (ExpectConfNotify &&
	  (Event.xkey.time < (Lasttime + PATIENCE)))
	break;
      Lasttime = Event.xkey.time;
      ExpectConfNotify = 0;
      switch(XKeycodeToKeysym(qtdisplay, Event.xkey.keycode, 0)) {
      case XK_Help:
      case XK_h:
	helptiff();
	break;
      case XK_m:
	mirrorPage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_M, thispage->next);
  	break;
      case XK_o:
	zoomout();
	break;

      case XK_i:
	zoomin();
	break;

      case XK_Up:
	ypos-= qwindow_height / 3;
	putimage = TRUE;
	break;
      case XK_Down:
	ypos+= qwindow_height / 3;
	putimage = TRUE;
	break;
      case XK_Left:
	xpos-= qwindow_width / 4;
	putimage = TRUE;
	break;
      case XK_Right:
	xpos+= qwindow_width / 4;
	putimage = TRUE;
	break;
      case XK_Home:
      case XK_R7:	       
	if (Event.xkey.state & ShiftMask) {
	  thispage = firstpage;
	  newPage();
	  resizeView();
	  putImage();
	  break;
	}
	xpos= 0;
	ypos= 0;
	putImage();
	break;
      case XK_End:
      case XK_R13:
	if (Event.xkey.state & ShiftMask) {
	  thispage = lastpage;
	  newPage();
	  resizeView();
	  putImage();
	  break;
	}
	xpos= Image->width;
	ypos= Image->height;
	putImage();
	break;
      case XK_l:
	rotatePage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_L, thispage->next);
	break;
      case XK_p:
      case XK_minus:
      case XK_Prior:
      case XK_R9:
      case XK_BackSpace:
	prevPage();
	break;
      case XK_n:
      case XK_plus:
      case XK_space:
      case XK_Next:
      case XK_R15:
	nextPage();
	break;
      case XK_u:
	flipPage();
	if (Event.xkey.state & ShiftMask)
	  TurnFollowing(TURN_U, thispage->next);
	break;

      case XK_q:
	if (viewpage) {
	  thispage = viewpage;
	  viewpage = NULL;
	  newPage();
	  resizeView();
	  putImage();
	}

      }
    
      break;

    case ButtonPress:

      if (ExpectConfNotify && (Event.xbutton.time < (Lasttime + PATIENCE)))
	break;

      Lasttime = Event.xbutton.time;
      ExpectConfNotify = 0;


      switch (Event.xbutton.button) {

      case Button1: 
	buttondown = true;

	switch (((Image->width > qwindow_width)<<1) |
		(Image->height > qwindow_height)) {
	case 0:
	  break;
	case 1:
	  XDefineCursor(qtdisplay, Win, UDCursor);
	  break;
	case 2:
	  XDefineCursor(qtdisplay, Win, LRCursor);
	  break;
	case 3:
	  XDefineCursor(qtdisplay, Win, MoveCursor);
	}

	XFlush(qtdisplay);
		offx = Event.xbutton.x;
		offy = Event.xbutton.y;
	break;

      }

      break;

    case MotionNotify:
      if(!buttondown)
	break;
      do { 

	nx = Event.xmotion.x;
	ny = Event.xmotion.y;


      } while (XCheckTypedEvent(qtdisplay, MotionNotify, &Event));


      xpos+= offx - nx;
      ypos+= offy - ny;

      offx = nx;
      offy = ny;

      putimage = TRUE;

      break;

    case ButtonRelease:
    
      if (Event.xbutton.button == Button1) {

	buttondown = false;
	XDefineCursor(qtdisplay, Win, ReadyCursor);
	XFlush(qtdisplay);
      }

    }

  } while (XCheckWindowEvent(qtdisplay, Win, KeyPressMask|ButtonPressMask, &Event));

  if(putimage == TRUE){
    Refresh = Resize = 1;
    putImage();
  }

}

void TopLevel::rotatePage(){

  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;

  XImage *newrotimage = NULL;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);
  
  newrotimage = RotImage(Images[0]);
  
  if(newrotimage == NULL){ // out of memory
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }
  
  thispage->extra = Image = newrotimage;
  thispage->orient ^= TURN_L;
  
  int i;
  for (i = 1; Images[i]; i++) {
    FreeImage(Images[i]);
    Images[i] = NULL;
  }
  
  Images[0] = Image;
  
  for (i = 1; i <= oz; i++){
    
    Images[i] = ZoomImage(Images[i-1]);
    Image = Images[i];
    if(Images[i] == NULL){// out of memory
      Image = Images[i -1];
      break;
    }
  }
  
  
  { int t = xpos ; xpos= ypos; ypos= t; }

  Refresh = Resize = 1;

  //	XDefineCursor(qtdisplay, Win, ReadyCursor);

  putImage();

}

void TopLevel::flipPage(){

  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;


  XImage *newflipimage = NULL;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);
  
  newflipimage = FlipImage(Images[0]);
  
  if(newflipimage == NULL){ // out of memory
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }
  
  thispage->extra = Images[0] = newflipimage;
  thispage->orient ^= TURN_U;
  
  int i;
  for (i = 1; Images[i]; i++) {
    FreeImage(Images[i]);
    Images[i] = ZoomImage(Images[i-1]);
    if(Images[i]== NULL){// out of memory
      break;
    }
  }
  
  Image = Images[min(i-1,oz)];
  

  
/*
  XPutImage(qtdisplay, Win, PaintGC, Image,
  xpos -PaneWidth/2, ypos -PaneHeight/2,
  0, 0, PaneWidth, PaneHeight);
  
  XDefineCursor(qtdisplay, Win, ReadyCursor);
  */

  Refresh = Resize = 1;
  putImage();

}




void TopLevel::mirrorPage(){

  if(!thispage || !Image || !faxqtwin || !display_is_setup)
    return;


  XImage *newmirror = NULL;

  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);
  
  newmirror = MirrorImage(Images[0]);
  
  if(newmirror == NULL){ // out of memory
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }
  thispage->extra = Images[0] = newmirror;
  
  thispage->orient ^= TURN_M;

  int i;
  for (i = 1; Images[i]; i++) {
    FreeImage(Images[i]);
    Images[i] = ZoomImage(Images[i-1]);
    if (Images[i] == NULL) // out of memory
      break;
  }
  Image = Images[min(oz,i-1)];

  /*	XPutImage(qtdisplay, Win, PaintGC, Image,
	xpos - PaneWidth/2, ypos - PaneHeight/2,
	0, 0, PaneWidth, PaneHeight);
	XDefineCursor(qtdisplay, Win, ReadyCursor);*/

    Refresh = Resize = 1;
    putImage();


}

void TopLevel::scrollHorz(int){
  

  if(!Image)
    return;

  //  printf("hsb value: %d\n",hsb->value());
  xpos=  hsb->value() + qwindow_width/2;

  Refresh = 1;
  putImage();

}

void TopLevel::scrollVert(int ){

  if(!Image)
    return;

  //  printf("vsb value: %d\n",vsb->value());
  ypos=  vsb->value() + qwindow_height/2;


  Refresh = 1;
  putImage();
}

void TopLevel::helptiff(){

  if(!thispage){
    
    QMessageBox::warning(this, "KFax",i18n("You need to open a fax page first."),
			 i18n("OK"),0);
    return;
  }


  if( viewpage ) // we are already displaying the help page
    return;

  
  if (helppage == NULL) {
    QString file;
    file = mykapp->kde_datadir().copy();
    file += "/kfax/pics/kfax.tif";
    if (!notetiff(file.data())){
      return;
    }
    else {

      helppage = lastpage;
      lastpage = helppage->prev;
      lastpage->next = helppage->prev = NULL;

    }
  }
  viewpage = thispage;
  thispage = helppage;

  newPage();
  resizeView();
  putImage();

}
void TopLevel::goToEnd(){

  bool have_new_page = FALSE;

  if(!thispage)
    return;

  while(thispage->next != NULL) {
    
    have_new_page = TRUE;
    thispage = thispage->next;

  }

  if(have_new_page == TRUE){
    newPage();
    resizeView();
    putImage();
  }
  else
    error();

}
void TopLevel::goToStart(){

  bool have_new_page = FALSE;

  if(!thispage)
    return;


  while(thispage->prev != NULL) {

    have_new_page = TRUE;
    thispage = thispage->prev;

  }

  if(have_new_page == TRUE){
    newPage();
    resizeView();
    putImage();
  }
  else
    error();

}
void TopLevel::nextPage(){

  if(!thispage)
    return;

  if (thispage->next == NULL) {

    error();
    return;
  }

  thispage = thispage->next;

  newPage();
  resizeView();
  putImage();
}

void TopLevel::prevPage(){

  if(!thispage)
    return;

  if (thispage->prev == NULL){

    error();
    return;

  }

  thispage = thispage->prev;
  
  newPage();
  resizeView();
  putImage();

}

void TopLevel::newPage(){


  if(!display_is_setup)
    SetupDisplay();


  XDefineCursor(qtdisplay, Win, WorkCursor);
  XFlush(qtdisplay);


  int i;

  for (i = 1; Images[i]; i++) {
    FreeImage(Images[i]);
    Images[i] = NULL;
  }

  int k = -1;


  if(!thispage) {
    XDefineCursor(qtdisplay, Win, ReadyCursor);
    return;
  }

  if (Pimage(thispage) == NULL){

    while((k != 0) && (k != 3) && (k !=1))
      k = GetImage(thispage);

  }
  
  if (k == 3 ){

    XDefineCursor(qtdisplay, Win, ReadyCursor);
    FreeFax();
    /*    QMessageBox::message("Sorry","Bad Fax File k=3");*/
    return;
  }

  if (k == 0 ){

    XDefineCursor(qtdisplay, Win, ReadyCursor);
    FreeFax();
    /*    QMessageBox::message("Sorry","Bad Fax File k=0");*/
    return;
 } 

  Image =  Images[0] = Pimage(thispage);

  setfaxtitle(thispage->name);

  for (i = 1; i <= oz; i++){
    Images[i] = ZoomImage(Images[i-1]);
    Image = Images[i];
    if (Images[i] == NULL){ // out of memory
      Image = Images[i-1];
      break;
    }
  }

  PaneWidth = Image->width;
  PaneHeight = Image->height;
  Refresh = 1;

  XDefineCursor(qtdisplay, Win, ReadyCursor);
  uiUpdate();

}


void TopLevel::closeFax(){

  FreeFax();

  setCaption("KFax");
  // TODO replace this with unmapping the window.
  if(display_is_setup)
    XResizeWindow(qtdisplay,Win,1,1); // we want a clear gray background.

  resizeView();
  vsb->hide();
  hsb->hide();
}



void TopLevel::FreeFax(){
                    

  if(display_is_setup)
    XClearWindow(qtdisplay, Win);

  for (int i = 1; Images[i]; i++) {
	  FreeImage(Images[i]);
	  Images[i] = NULL;
  }

  pagenode *pn;
  
  for (pn = firstpage; pn; pn = pn->next){
    if(Pimage(pn)){
       FreeImage(Pimage(pn));
    }
  }

  Image = NULL;

  for (pn = firstpage; pn; pn = pn->next){
    if(pn->pathname){
       free(pn->pathname);
    }
  }
  

  if(firstpage){
    for(pn = firstpage->next; pn; pn = pn->next){
      if(pn->prev){
	free(pn->prev);
      }
    }	
  }

  if(lastpage)
    free(lastpage);

  // TODO what about the help page? Well it'll be freed when kfax exits
  firstpage = lastpage = viewpage = helppage = thispage = auxpage = NULL;

  uiUpdate();

}


void TopLevel::error(){

  QApplication::beep();

}

void TopLevel::uiUpdate(){


  if(!thispage){

    toolbar->setItemEnabled( ID_ZOOM_IN, FALSE );
    toolbar->setItemEnabled( ID_ZOOM_OUT, FALSE );
    toolbar->setItemEnabled( ID_PREV, FALSE );
    toolbar->setItemEnabled( ID_START, FALSE );
    toolbar->setItemEnabled( ID_NEXT, FALSE );
    toolbar->setItemEnabled( ID_END, FALSE );
    toolbar->setItemEnabled( ID_PRINT,FALSE);

    statusbar->changeItem("", ID_INS_OVR);
    statusbar->changeItem("", ID_GENERAL);
    statusbar->changeItem("", ID_PAGE_NO);
    statusbar->changeItem("",ID_FNAME);
    statusbar->changeItem("",ID_TYPE);

    return;
  }

  if(  thispage->next != NULL){

    toolbar->setItemEnabled( ID_NEXT, TRUE );
    toolbar->setItemEnabled( ID_END, TRUE );

  }
  else{

    toolbar->setItemEnabled( ID_NEXT, FALSE );
    toolbar->setItemEnabled( ID_END, FALSE );

  }


  if(  thispage->prev != NULL){
   
    toolbar->setItemEnabled( ID_PREV, TRUE );
    toolbar->setItemEnabled( ID_START, TRUE );

  }
  else{

    toolbar->setItemEnabled( ID_PREV, FALSE );
    toolbar->setItemEnabled( ID_START, FALSE );

  }



  if(thispage){

    struct pagenode *pn ;
    int pages = 0;
    int currentpage = 0;

    for(pn = firstpage; pn ; pn = pn->next){
      pages ++;
      if (thispage == pn)
	currentpage = pages;
    }
    
    QString pagestr;
    pagestr.sprintf(i18n("P.%d of %d"),currentpage,pages);

    statusbar->changeItem(pagestr.data(), ID_PAGE_NO);

    toolbar->setItemEnabled( ID_ZOOM_IN, TRUE );
    toolbar->setItemEnabled( ID_ZOOM_OUT, TRUE );
    toolbar->setItemEnabled( ID_PRINT,TRUE);
    if(Image){

    QString wh;

    wh.sprintf(i18n("W: %d H: %d"),Image->width,Image->height);
    statusbar->changeItem(wh.data(), ID_INS_OVR);
    }

    QString resolution;
    resolution.sprintf(i18n("Res: %s"),thispage->vres ? i18n("Fine") : i18n("Normal"));
    statusbar->changeItem(resolution.data(), ID_GENERAL);

    statusbar->changeItem(thispage->name,ID_FNAME);

    QString typestring;

    if(thispage->type == FAX_TIFF){
      typestring = i18n("Type: Tiff ");
    }	
    else if ( thispage->type == FAX_RAW){
      typestring = i18n("Type: Raw ");
    }

    if ( thispage->expander == g31expand )
      typestring += "G3";

    if ( thispage->expander == g32expand )
      typestring += "G3 2D";

    if ( thispage->expander == g4expand )
      typestring += "G4";

    statusbar->changeItem(typestring.data(),ID_TYPE);
    QString memstr;

    memstr.sprintf("Mem: %d",Memused);
    //    statusbar->changeItem(memstr.data(), ID_LINE_COLUMN);

  }
  
}

void kfaxerror(char* title, char* error){
  QMessageBox::warning(0,title,error,"OK",0);
}

void TopLevel::putImage(){

  // TODO do I really need to set Refresh or Resize to 1 , is there 
  // really still someonce calling this with out haveing set Refresh or Resize to 1?

  if(!Image)
    return;

  if(!display_is_setup)
    return;
 
  if(!thispage)
    return;


  if ( qwindow_width > Image->width){
	  xpos= Image->width/2;
  }
  else{
    if(xpos< qwindow_width/2){
      xpos = qwindow_width/2;
    }
    else{
      if(xpos> Image->width - qwindow_width/2){
	xpos= Image->width - qwindow_width/2;
      }
      
    }
  }
  
  if ( qwindow_height > Image->height){
    ypos= Image->height/2;
  }
  else{
    if(ypos< qwindow_height/2){
      ypos = qwindow_height/2;
    }
    else{
      if(ypos> Image->height - qwindow_height/2){
	ypos= Image->height - qwindow_height/2;
      }
      
    }
  }

  if (xpos!= ox || ypos!= oy || Refresh || Resize){
    
    /* In the following we use qwindow_height -1 etc since the main view
       has a sunken frame and I need to offset by 1 pixel to the right and
       one pixel down so that I don't paint my fax into the border of the frame*/

    XResizeWindow(qtdisplay,Win,min(qwindow_width -1,Image->width ),
		  min(qwindow_height -1,Image->height ));
    
    XPutImage(qtdisplay, Win, PaintGC, Image,
	      max(xpos - qwindow_width/2,0), max(ypos - qwindow_height/2,0),
	      0, 0, min(qwindow_width -1,Image->width)  , 
	      min(qwindow_height -1,Image->height) );

    vsb->setValue(max(ypos - qwindow_height/2,0));
    hsb->setValue(max(xpos - qwindow_width/2,0));
    
    XFlush(qtdisplay);
  }


  ox = xpos;
  oy = ypos;

  Resize = Refresh = 0;
  
}


void TopLevel::faxoptions(){

 OptionsDialog * opd = new OptionsDialog(NULL,"options");
 opd->setWidgets(&fop);

 if(opd->exec()){

   struct optionsinfo *newops;
   newops = opd->getInfo();

   fop.resauto 	=  newops->resauto;
   fop.geomauto	=  newops->geomauto;
   fop.width 	=  newops->width;
   fop.height 	=  newops->height;
   fop.fine 	=  newops->fine;
   fop.landscape=  newops->landscape;
   fop.flip 	=  newops->flip;
   fop.invert 	=  newops->invert;
   fop.lsbfirst =  newops->lsbfirst;
   fop.raw 	=  newops->raw;


   setFaxDefaults();

 }
 
 delete opd;

}

void setFaxDefaults(){

  // fop is called in readSettings, so this can't be 
  // called after a TopLevel::readSettings()

  if(have_cmd_opt ) // we have commad line options all kfaxrc defaults are 
    return;         // overridden

  if(fop.resauto == 1){
    defaultpage.vres = -1;
  }
  else{
    defaultpage.vres = fop.fine;
  }

  if(fop.geomauto == 1){
    defaultpage.width = 0;
    defaultpage.height = 0;
  }
  else{
    defaultpage.width = fop.width;
    defaultpage.height = fop.height;
  }

  if( fop.landscape)
    defaultpage.orient |= TURN_L;

  if(fop.flip)
     defaultpage.orient |= TURN_U;

  defaultpage.inverse =    fop.invert;
  defaultpage.lsbfirst = fop.lsbfirst;

  if(fop.raw == 2)
    defaultpage.expander = g32expand;
  if(fop.raw == 4)
    defaultpage.expander = g4expand;
  if((fop.raw != 4) && (fop.raw != 2) )
    defaultpage.expander = g31expand;

}

int main (int argc, char **argv)
{

  toplevel = NULL;

  catchSignals();
  mykapp = new MyApp (argc, argv, "kfax");
  
  qtdisplay = qt_xdisplay();

  viewfaxmain(argc,argv);    

  toplevel = new TopLevel();
  toplevel->show ();

  startingup = 1;
  mykapp->processEvents();
  mykapp->flushX();
  
  startingup = 0;

  faxinit();    

  if(!have_no_fax){

    thispage = firstpage;

    toplevel->newPage();
    toplevel->resizeView();
    //TODO : I don't think I need this putImage();
    toplevel->putImage();
  }

  toplevel->uiUpdate();

  return mykapp->exec ();
}

void setfaxtitle(char* name){

  toplevel->setFaxTitle(name);
  
}
 
void setstatusbarmem(int mem){

  if(toplevel)
    toplevel->setStatusBarMemField(mem);
  
}


void mysighandler(int ){

  //  printf("signal received %d\n",sig);
  catchSignals(); // reinstall signal handler

}


void catchSignals()
{
  /*
	signal(SIGHUP, mysighandler);	
	signal(SIGINT, mysighandler);	
	signal(SIGTERM, mysighandler);	
	signal(SIGCHLD, mysighandler);
  	signal(SIGABRT, mysighandler);
	signal(SIGUSR1, mysighandler);
	signal(SIGALRM, mysighandler);
	signal(SIGFPE, mysighandler);
	signal(SIGILL, mysighandler);*/

	signal(SIGPIPE, mysighandler);

/*	signal(SIGQUIT, mysighandler);
	signal(SIGSEGV, mysighandler);

#ifdef SIGBUS
	signal(SIGBUS, mysighandler);
#endif
#ifdef SIGPOLL
	signal(SIGPOLL, mysighandler);
#endif
#ifdef SIGSYS
	signal(SIGSYS, mysighandler);
#endif
#ifdef SIGTRAP
	signal(SIGTRAP, mysighandler);
#endif
#ifdef SIGVTALRM
	signal(SIGVTALRM, mysighandler);
#endif
#ifdef SIGXCPU
	signal(SIGXCPU, mysighandler);
#endif
#ifdef SIGXFSZ
	signal(SIGXFSZ, mysighandler);
#endif
*/

}



void parse(char* buf, char** args){

  while(*buf != '\0'){
    
    // Strip whitespace. Use nulls, so that the previous argument is terminated 
    // automatically.
     
    while ((*buf == ' ' ) || (*buf == '\t' ) || (*buf == '\n' ) )
      *buf++ ='\0';
    
    // save the argument
    if(*buf != '\0')
    *args++ = buf;
    
    while ((*buf != '\0') && (*buf != '\n') && (*buf != '\t') && (*buf != ' '))
      buf++;
    
  }
 
  *args ='\0';;

}

int copyfile(const char* toname,char* fromname){

  char buffer[4*1028];
  int count = 0;
  int count2;

  FILE*  fromfile;
  FILE*  tofile;

  if (QFile::exists(toname)) {
    if(QMessageBox::warning( 0, "KFax:",
			      i18n("A file with this name already exists\n"
			      "Do you want to overwrite it?\n\n"),
			      i18n("Yes"), i18n("No"),
			      0, 1 ))
      return 1;                    
  }

  if((fromfile = fopen(fromname,"r")) == NULL)
    return 0;

  if((tofile =  fopen(toname,"w")) == NULL){
    fclose(fromfile);
    return 0;
  }


  while((count = fread(buffer,sizeof(char),4*1028,fromfile))){
    count2 = fwrite(buffer,sizeof(char),count,tofile);
    if (count2 != count){
      fclose(fromfile);
      fclose(tofile);
      return 0;
    }

  }
  fclose(fromfile);
  fclose(tofile);
  return 1;

}



