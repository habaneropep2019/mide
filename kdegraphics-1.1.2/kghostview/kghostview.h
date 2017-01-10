#ifndef KGHOSTVIEW_H
#define KGHOSTVIEW_H


#include <stdlib.h>
#include <math.h>

#include <kaccel.h>
#include <ktopwidget.h>

#include "kpswidget.h"
#include "info.h"
#include "viewcontrol.h"
#include "goto.h"
#include "print.h"
#include "copyright.h"
#include "marklist.h"
#include "zoom.h"
#include <signal.h>
#ifdef SIGNALRETURNSINT
#define SIGVAL int
#else
#define SIGVAL void
#endif

#include <qwidget.h>
#include <qlabel.h>
#include <qstring.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qgrpbox.h>
#include <qaccel.h>

#include <kfm.h>
#include <kapp.h>
#include <kurl.h>

#define TOOLBAR_HEIGHT 26
#define TOOLBAR_X_OFFSET 10
#define TOOLBAR_Y_OFFSET 5
#define TOOLBAR_BUTTON_HEIGHT 28
#define TOOLBAR_BUTTON_WIDTH 28

#define BUTTON_HEIGHT		25
#define BUTTON_WIDTH		25
#define BUTTON_SEPARATION	6

#define MENUBAR_HEIGHT 28

#define STATUSBAR_HEIGHT 20
#define STATUSBAR_PAGE_WIDTH 140
#define STATUSBAR_ZOOM_WIDTH 45
#define STATUSBAR_ORIENT_WIDTH 90

#define PAGE_X_OFFSET 2

enum {OPEN, PRINT_WHOLE, PRINT_MARKED, SAVE};

class KGhostview : public KTopLevelWidget
{
    Q_OBJECT

public:
	KGhostview( QWidget *parent=0, char *name=0 );
	//~KGhostview();

	// Tells us what kind of job kghostview is waiting for.
    
	KAccel *keys;
	
	enum action { GET, PUT };
	void openNetFile( const char *_url );

	static QList <KGhostview> windowList;

	Bool setup();
	void show_page(int number);
	void build_pagemedia_menu();
	void set_magstep(int i);
	void magnify(float *dpi, int magstep);
	void new_file(int number);
	void openFile( QString name );
	Bool same_document_media();
	Bool set_new_orientation(int number);
	Bool set_new_pagemedia( int number );
	Bool set_new_magstep();

	void writeSettings();
	void writeFileSettings();
	void readSettings();
	
	void createToolbar();
	void createMenubar();
	void createStatusbar();
	
	void setName();
	
	
	QFrame *mainFrame;
	QFrame *divider;
	MarkList *marklist;
	KPSWidget *page;
	
	ViewControl *vc;
	Zoom *zoomdlg;
	PrintDialog *pd;
	InfoDialog *infoDialog;
	
	Bool force_pagemedia;
	Bool force_orientation;
	int orientation;
		
	void printStart( int mode, bool reverseOrder, bool toFile, 
					QString printerName, QString spoolerCommmand,
					QString printerVariable,
					int pgStart, int pgEnd );
				
	QString printToFile( bool allMode, QStrList *ml  );
				
	QString printToPrinter( QString printerName, 
				QString spoolerCommand, QString printerVariable,
				bool allMode, QStrList *ml  );
	
	void psCopyDoc( FILE *fp, QStrList *ml );
	
	void bindKeys();
	void changeMenuAccel( QPopupMenu *menu, int id, const char *functionName );
	void updateMenuAccel();
	void changeFileRecord();
	
	FILE		*psfile;
	QString 	filename;	
	QString		oldfilename;	
	int			current_page;
	int 		pages_in_part[10];
	int 		num_parts;
	time_t		mtime;	
	struct 		document *doc;	
	struct 		document *olddoc;
	
	QStrList lastOpened;
	
public slots:
	//void pageActivated( const char * text); // Dutta 16/3/98
	void pageActivated( int pg );	// Dutta 16/3/98
	void applyViewChanges();
	void applyZoomDlgChange();
	void scrollDown();
	void scrollUp();
	void scrollLeft();
	void scrollRight();
	//	void about();
	//	void help();
	void dummy();
	void viewControl();			
	void nextPage();
	void prevPage();
	void goToPage();
	void goToStart();
	void goToEnd();
	void readDown();
	void print();
	void openNewFile();
	void zoomIn();
	void zoomOut();
	void zoom();
	void optionsMenuActivated( int item );
	void toolbarClicked( int item );
	void markPage();
	void configureKeybindings();
	void shrinkWrap();
	void redisplay();
	void newWindow();
	void closeWindow();
	void info();
	//	void copyright();
	void configureGhostscript();
	void openRecent( int id );
	
	void slotDropEvent( KDNDDropZone * _dropZone );
	void slotKFMFinished();
     
protected:
	void paletteChange( const QPalette & );
	void closeEvent( QCloseEvent * );
	
protected slots:
	void updateRects();
   
	
private:

	KMenuBar *menubar;
	QPopupMenu *m_file0, *m_file;
	//	QPopupMenu *m_recent;
	QPopupMenu *m_view;
	QPopupMenu *m_go;
	QPopupMenu *m_pagemarks;
	QPopupMenu *m_options;
	QPopupMenu *m_help;
	
	KToolBar *toolbar;
	KStatusBar *statusbar;
	
	int topOffset, bottomOffset;
	
	int viewControlID;
	int pgsetupID;
	int printID;
	int openID;
	int newID;
	int closeID;
	int quitID;
	int zoomInID;
	int zoomOutID;
	int nextID;
	int prevID;
	int goToPageID;
	int goToStartID;
	int goToEndID;
	int readDownID;
	int messagesID;
	int helpID;
	int shrinkWrapID;
	int redisplayID;
	int infoID;
	int markCurrentID;
	int markAllID;
	int markEvenID;
	int markOddID;
	int toggleMarksID;
	int removeMarksID;

	QString printerName;
	int printSelection;
	bool isNetFile;
	Bool hide_toolbar;
	Bool hide_statusbar;
	Bool hide_pagelist;
	Bool fitWindow;
	int options_width;
	int options_height;
	int shrink_magsteps, expand_magsteps, magstep, current_magstep;
	Bool	force_document_media;	
	int	base_papersize;
	Bool	toc_text;
	int	toc_length;
	int	toc_entry_length;
	char page_total_label[20];
	char part_total_label[20];
	QString part_string;
	QString page_string;
	QString page_label_text;
	QString part_label_text;
	QString position_label;
	float default_xdpi, default_ydpi;
	int current_llx;
	int current_lly;
	int current_urx;
	int current_ury;
	int current_pagemedia, default_pagemedia, document_media;
	int current_orientation;
	Bool changed;

	int *fileidlist, nrecent, sepindex;
	
	 /// KFM client
    /**
      Only one KFM connection should be opened at once. Otherwise kgv could get
      confused. If this is 0L, you may create a new connection to kfm.
      */
    KFM * kfm;
    
    /// Temporary file for internet purposes
    /**
      If kgv is waiting for some internet task to finish, this is the
      file that is involved. Mention that it is a complete URL like
      "file:/tmp/mist.txt".
      */
    QString tmpFile;

    /// If we load a file from the net this is the corresponding URL
    QString netFile;
    
    /// Tells us what kfm is right now doing for us
    /**
      If this is for example GET, then KFM loads a file from the net
      to the local file system.
      */
    action kfmAction;
    
    /// The open mode for a net file
    /**
      If kgv is waiting for an internet file, this is the mode in which
      it should open the file.
      */
    int openMode;


    void showSBMagstep (void);
};

#endif // KGHOSTVIEW_H


