/*
 *   khexdit - a little hex editor
 *   Copyright (C) 1996,97,98  Stephan Kulow
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <time.h>
#include <qpopmenu.h>
#include <qkeycode.h>
#include <qaccel.h> 
#include <kfiledialog.h> 
#include <qdir.h>
#include <string.h>
#include <stdlib.h>
#include <qmsgbox.h> 
#include <ktopwidget.h>
#include <ktoolbar.h>
#include "hexfile.h"
#include "hexwidget.h"
#include "klocale.h"
#include <kiconloader.h>

#define KHEXDIT_VERSION "0.6"

QList<HexWidget> HexWidget::windowList;

enum { ID_FILE_OPEN = 1,
       ID_FILE_OPEN_URL,
       ID_FILE_SAVE,
       ID_FILE_SAVEAS,
       ID_FILE_SAVE_URL,
       ID_FILE_CLOSE,
       ID_FILE_NEWWIN,
       ID_FILE_QUIT,
       ID_FILE_PRINT,

       ID_EDIT_COPY = 50,
       ID_EDIT_PASTE,
       ID_EDIT_CUT,
       ID_EDIT_SEARCH,
       ID_EDIT_SEARCHAGAIN,

       ID_VIEW_TOOLBAR = 60,
       ID_VIEW_STATUSBAR
};


int HexWidget::initMenu() {

    windowList.append(this);

    CurrentFile=new HexFile(this);
    unsaved( false );
    connect(CurrentFile, SIGNAL( unsaved(bool) ), SLOT( unsaved(bool) ));

    QPopupMenu *file = new QPopupMenu;
    file->insertItem( i18n("&New Window..."), ID_FILE_NEWWIN);
    file->insertSeparator();
    file->insertItem(  i18n("&Open..."), ID_FILE_OPEN );
    file->insertItem(  i18n("&Save"), ID_FILE_SAVE);
    file->insertItem(  i18n("Save &As..."), ID_FILE_SAVEAS);
    file->insertItem(  i18n("&Close"), ID_FILE_CLOSE);
    file->insertSeparator();
    file->insertItem(  i18n("&Quit"), ID_FILE_QUIT);
    
    QPopupMenu *edit = new QPopupMenu;
    edit->insertItem( i18n("&Copy"), ID_EDIT_COPY);
    edit->insertItem( i18n("&Paste"), ID_EDIT_PASTE);
    edit->insertItem( i18n("C&ut"), ID_EDIT_CUT);

    QPopupMenu *view = new QPopupMenu;
    view->insertItem( i18n("&Toggle Toolbar"), ID_VIEW_TOOLBAR);
    //  view->insertItem( "Toggle Statusbar", ID_VIEW_STATUSBAR);
    
    QPopupMenu *help = kapp->getHelpMenu(true, QString(i18n("Hex Editor")) 
					 + " " + KHEXDIT_VERSION 
					 + i18n("\n\nby Stephan Kulow") 
					 + " (coolo@kde.org)");

    connect (file, SIGNAL (activated (int)), SLOT (menuCallback (int)));
    connect (edit, SIGNAL (activated (int)), SLOT (menuCallback (int)));
    connect (help, SIGNAL (activated (int)), SLOT (menuCallback (int)));
    connect (view, SIGNAL (activated (int)), SLOT (menuCallback (int)));
    
    menu = new KMenuBar( this );
    CHECK_PTR( menu );
    menu->insertItem( i18n("&File"), file );
    menu->insertItem( i18n("&Edit"), edit);
    menu->insertItem( i18n("&View"), view );
    menu->insertSeparator();
    menu->insertItem( i18n("&Help"), help );
    menu->show();
    setMenu(menu);
    
    KIconLoader *loader = kapp->getIconLoader();
    toolbar = new KToolBar(this);
    
    toolbar->insertButton(loader->loadIcon("filenew.xpm"),
			  ID_FILE_NEWWIN, true, 
			  i18n("New Window"));
    toolbar->insertButton(loader->loadIcon("fileopen.xpm"),ID_FILE_OPEN, true, 
			  i18n("Open a file"));
    toolbar->insertButton(loader->loadIcon("filefloppy.xpm"), ID_FILE_SAVE, 
			  true, i18n("Save the file"));
    toolbar->insertSeparator();
    toolbar->insertButton(loader->loadIcon("editcut.xpm"),ID_EDIT_CUT, false, 
			  i18n("Not implemented"));
    toolbar->insertButton(loader->loadIcon("editcopy.xpm"),ID_EDIT_COPY, false,
			  i18n("Not implemented"));
    toolbar->insertButton(loader->loadIcon("editpaste.xpm"),ID_EDIT_PASTE, 
			  false, i18n("Not implemented"));
    toolbar->insertSeparator();
    toolbar->insertButton(loader->loadIcon("fileprint.xpm"),ID_FILE_PRINT, 
			  false, i18n("Not implemented"));
    
    addToolBar(toolbar);
    toolbar->setBarPos(KToolBar::Top);
    toolbar->show();
    connect(toolbar, SIGNAL(clicked(int)), SLOT(menuCallback(int)));
    
    // KStatusBar *stat = new KStatusBar(this);
    // stat->show();
    // setStatusBar(stat);
    
    kfm = 0L;
    setView(CurrentFile);
    dropZone = new KDNDDropZone( this , DndURL);
    connect( dropZone, SIGNAL( dropAction( KDNDDropZone *) ), 
	     SLOT( slotDropEvent( KDNDDropZone *) ) );
    
    show();
    return 0;
}

void HexWidget::menuCallback(int item) {
    switch (item) {
	HexWidget *dummy;
    case ID_FILE_NEWWIN: {
	dummy = new HexWidget();
	break;
    }
    case ID_FILE_OPEN: {
	if (CurrentFile->isModified()) 
	    if (QMessageBox::warning(0, i18n("Warning"), 
				     i18n("The current file has been modified.\nDo you want to save it?"),
				     i18n("Yes"),
				     i18n("No")))
		CurrentFile->save();
	
	QString fileName = KFileDialog::getOpenFileName();
	open(fileName, READWRITE);
	break;
    }
    case ID_FILE_SAVE:
	if (CurrentFile->isModified())
	    CurrentFile->save();
	break;
    case ID_FILE_SAVEAS: {
	QString fileName = KFileDialog::getSaveFileName();
	
	CurrentFile->setFileName(fileName.data());
	CurrentFile->save();
	break;
    }
    case ID_FILE_CLOSE: {
	if (CurrentFile->isModified()) {
	    if (!QMessageBox::warning(0, i18n("Warning"),
				    	i18n("The current file has been modified.\nDiscard your changes?"),
				    	i18n("No"),
				    	i18n("Yes"))) 
		return;
	}
        close();
        break;
    }
    case ID_FILE_QUIT: {
        for (HexWidget *w = windowList.first(); w ; w = windowList.next())
           w->close();
        kapp->exit();
	break;
    }

    case ID_EDIT_COPY: {
	CurrentFile->copyClipBoard();
	break;
    }

    case ID_VIEW_TOOLBAR: 
	enableToolBar(KToolBar::Toggle);
	break;
	
    case ID_VIEW_STATUSBAR:
	enableStatusBar();
	break;
    }
}

void HexWidget::unsaved(bool flag)
{
    setUnsavedData(flag);
}

void HexWidget::open(const char *fileName, const char *url, KIND_OF_OPEN) 
{
    if (!fileName)
	return;

    if (!CurrentFile)
	CurrentFile = new HexFile(fileName,this);
    else
	CurrentFile->open(fileName); 

    CurrentFile->setFocus();
    char Caption[300];
    sprintf(Caption,"%s: %s",kapp->getCaption(), url);
    setCaption(Caption);
    update();
}

void HexWidget::open(const char *fileName, KIND_OF_OPEN kind) 
{
    open(fileName, fileName, kind);
}

void HexWidget::openURL(const char *_url, KIND_OF_OPEN _mode) 
{
    /* This code is from KEDIT (torben's I guess) */
    netFile = _url;
    netFile.detach();
    KURL u( netFile.data() );
    if ( u.isMalformed())   {
	QMessageBox::warning (0, 
			      i18n("Error"), 
			      i18n("Malformed URL"));
	return;
    }
  
    // Just a usual file ?
    if ( strcmp( u.protocol(), "file" ) == 0 && !u.hasSubProtocol() ) {
	QString decoded( u.path() );
	KURL::decodeURL( decoded );
	open( decoded, _mode );
	return;
    }
    
    
    if ( kfm != 0L ) {
	QMessageBox::warning (0, i18n("Error"), 
			      i18n("KHexdit is already waiting\nfor an internet job to finish\n\nWait until this one is finished\nor stop the running one."));
	return;
    }
    
    kfm = new KFM;
    
    if ( !kfm->isOK() ) {
	QMessageBox::warning (0, i18n("Error"), 
			      i18n("Could not start KFM"));
	delete kfm;
	kfm = 0L;
	return;
    }
    
    tmpFile.sprintf( "file:/tmp/khexdit%li", time( 0L ) );
    connect( kfm, SIGNAL( finished() ), SLOT( slotKFMFinished() ) );
  
    kfm->copy( netFile.data(), tmpFile.data() );
    
    kfmAction = HexWidget::GET;
    //openMode = _mode;
}

void HexWidget::slotKFMFinished()
{
    if ( kfmAction == HexWidget::GET ) {
	KURL u(tmpFile);
	QString decoded( u.path() );
	KURL::decodeURL( decoded );
	open( decoded, netFile, READWRITE );
		
	delete kfm;
	kfm = 0L;
    }
    if ( kfmAction == HexWidget::PUT )
    {
	unlink( tmpFile.data() );
	delete kfm;
	kfm = 0L;
    }
}

void HexWidget::saveProperties(KConfig *config )
{
    config->writeEntry("Name", CurrentFile->Title());
}


void HexWidget::readProperties(KConfig *config)
{
    QString entry = config->readEntry("Name"); // no default
    if (entry.isNull())
	return;
    open(entry, READWRITE );
}

HexWidget::HexWidget() {
    initMenu();
    setCaption(kapp->getCaption());
}

HexWidget::HexWidget(const char* file) {
    initMenu();
    openURL(file,READWRITE);
}

HexWidget::~HexWidget() {
    delete CurrentFile;
    delete kfm;
}

void HexWidget::closeEvent ( QCloseEvent *e) {
    windowList.remove(this);
    delete toolbar;
    toolbar = 0L;
    delete menu;
    menu = 0L;
    delete dropZone;
    dropZone = 0L;
    if (windowList.isEmpty())
	kapp->quit();
    e->accept();
}

void HexWidget::slotDropEvent( KDNDDropZone * _dropZone ) {
    QStrList & list = _dropZone->getURLList();
  
    char *s;
    
    for ( s = list.first(); s != 0L; s = list.next() )
	{
	    // Load the first file in this window
	    
	    if ( s == list.getFirst() && !CurrentFile->isModified() ) 
		{
		    QString n = s;
		    if ( n.left(5) != "file:" && n.left(4) == "ftp:" )
			openURL( n.data(), READWRITE );
		    else
			openURL( n.data(), READONLY );
		}
	    else 
		{
		    HexWidget *h = new HexWidget();
		    QString n = s;
		    if ( n.left(5) != "file:" && n.left(4) == "ftp:" )
			h->openURL( n.data(), READWRITE );
		    else
			h->openURL( n.data(), READONLY );
		}
	}    
}

int main(int argc, char **argv) {
    KApplication a(argc,argv,"khexdit");  
    HexWidget *dummy;
    
    if ( a.isRestored() ) {
	int n = 1;
	while (KTopLevelWidget::canBeRestored(n)) {
	    HexWidget *hw = new HexWidget();
	    hw->restore(n++);
	}
    } else
	if (argc>1) {
	    for (int i=1; i < argc; i++) {
		if (*argv[i] == '-')	/* ignore options */
		    continue;
		
		QString f = argv[i];
		
		if ( f.find( ":/" ) == -1 && f.left(1) != "/" )
		    {
			char buffer[ 1024 ];
			getcwd( buffer, 1023 );
			f.sprintf( "file:%s/%s", buffer, argv[i] );
		    }
		
		dummy = new HexWidget(f.data());
	    }
	} else 
	    dummy = new HexWidget();
    
    return a.exec();
}

#include "hexwidget.moc"



