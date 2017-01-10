/****************************************************************************
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <qfiledlg.h>
#include <qmsgbox.h>
#include <klocale.h>
#include <kapp.h>
#include <kurl.h>

#include "kghostview.h"

Display *kde_display;

void Syntax(char *call)
{
	QString s( i18n( "Usage: " ) );
	s.append( call );
	s.append( i18n( " [filename] [siwtches]\n\n" ) );
	s.append( i18n( "See KDE and Qt documentation for\n"\
					"command line options.\n" ) );
	QMessageBox::warning( 0, i18n( "Error on command line" ), s );
}

class MyApp : public KApplication
{
public:

  	MyApp( int &argc, char **argv, const QString &rAppName );
 	virtual bool x11EventFilter( XEvent * );
	~MyApp();
};

MyApp::MyApp( int &argc, char **argv, const QString &rAppName )
			: KApplication( argc, argv, rAppName )
{
	
}

MyApp::~MyApp()
{
	//printf("MyApp::~MyApp()\n");
}

bool MyApp::x11EventFilter( XEvent *ev ) {
	
	for ( KGhostview *kg = KGhostview::windowList.first(); kg!=0;
		kg=KGhostview::windowList.next() )
	{
	if(ev->xany.type == ClientMessage) {
		kg->page->mwin = ev->xclient.data.l[0];
		//fprintf(stderr, 
		//	"kghostview: Ghostscript communication window set as 0x%lx\n",
		//	kg->page->mwin);
		
		XClientMessageEvent *cme = ( XClientMessageEvent * ) ev;
		
		if(cme->message_type == kg->page->gs_page) {
			kg->page->busy=False;
			kg->page->fullView->setCursor( arrowCursor );
			return TRUE;
			
		} else if(cme->message_type == kg->page->done) {
			kg->page->disableInterpreter();
			return TRUE;
			
		}
	}
	}
	
	if ( KApplication::x11EventFilter( ev ) )
		return TRUE;
	else
		return FALSE;	
}

int main( int argc, char **argv )
{
	MyApp a( argc, argv, "kghostview" );
	
	signal( SIGCHLD, SIG_DFL );
	
	struct stat sbuf;
	
	KGhostview::windowList.setAutoDelete( FALSE );
	
	if (argc > 2) Syntax(argv[0]);
	
	if ( a.isRestored() ) {
		int n = 1;
		while (KTopLevelWidget::canBeRestored(n)) {
	    	KGhostview *kg = new KGhostview ();
		CHECK_PTR( kg );
		kg->restore(n);
		n++;
		}
    } else if (argc == 2) {
		KGhostview *kg = new KGhostview ();
		CHECK_PTR( kg );
		
		QString name( argv[1] );
		if ( strcmp( name, "-" ) ) {
			KURL u( name );
			if ( strcmp( u.protocol(), "file" ) != 0 ||
					u.hasSubProtocol() ) {
			    kg->openNetFile( name );
			} else {
				kg->openFile( u.path() );
				stat( kg->filename.data(), &sbuf );
			    kg->mtime = sbuf.st_mtime;
			}
		}
	} else {
		KGhostview *kg = new KGhostview ();
		CHECK_PTR( kg );
	}
	
	return a.exec();
}

