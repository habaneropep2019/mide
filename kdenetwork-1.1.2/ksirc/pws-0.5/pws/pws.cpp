#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <qdir.h>

#include <pws.h>
#include <pws.moc>

void testDir( const char *_name );

void testDir( const char *_name )
{
    DIR *dp;
    QString c = KApplication::localkdedir();
    c += _name;
    dp = opendir( c.data() );
    if ( dp == NULL )
        ::mkdir( c.data(), S_IRWXU );
    else
        closedir( dp );
} 


PWS::PWS(QWidget *parent, const char *name)
	: KTopLevelWidget(name)
{
    createdUI = FALSE;
    menuBar = 0x0;
    statusBar = 0x0;
    toolBar = 0x0;
    view = 0x0;

    testDir ("/share");
    testDir ("/share/config");
    testDir ("/share/apps");
    testDir ("/share/apps/pws");
}

PWS::~PWS()
{
    if(view != 0)
        delete view;
}

void PWS::invokeHelp()
{
	kapp->invokeHTMLHelp("pws/pws.html", "");
}

void PWS::show(){
    if(createdUI == FALSE){
        QPopupMenu *file_menu = new("QPopupMenu") QPopupMenu();
        file_menu->insertItem(i18n("E&xit"), kapp, SLOT(quit()));

        QPopupMenu *help_menu = new("QPopupMenu") QPopupMenu();
        help_menu->insertItem(i18n("&Help"), this, SLOT(invokeHelp()));

        //	menuBar = new("KMenuBar") KMenuBar(this, "menubar");
        //	menuBar->insertItem(i18n("&File"), file_menu);
        //	menuBar->insertItem(i18n("&Help"), help_menu);

        //	setMenu(menuBar);

        //	toolBar = new("KToolBar") KToolBar(this);
        //	addToolBar(toolBar);

        //	statusBar = new("KStatusBar") KStatusBar(this);
        //	setStatusBar(statusBar);

        //        resize(600,440);
        view = new("PWSWidget") PWSWidget(this);
        setView(view);

        connect(view, SIGNAL(quitPressed(QObject *)),
                this, SLOT(closeView(QObject *)));

        createdUI = TRUE;
    }
    KTopLevelWidget::show();
}

void PWS::closeView(QObject *obj){
  emit quitPressed(obj);
  /*
   obj = 0x0;
   delete view;
   view = 0x0;
   createdUI = FALSE;
   */
}
