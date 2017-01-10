//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This file os part of KRN, a newsreader for the KDE project.              //
// KRN is distributed under the GNU General Public License.                 //
// Read the acompanying file COPYING for more info.                         //
//                                                                          //
// KRN wouldn't be possible without these libraries, whose authors have     //
// made free to use on non-commercial software:                             //
//                                                                          //
// MIME++ by Doug Sauder                                                    //
// Qt     by Troll Tech                                                     //
//                                                                          //
// This file is copyright 1998 by                                           //
// Roberto Alsina <ralsina@unl.edu.ar>                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <qdir.h>

#include <kapp.h>
#include "new_globals.h"
#include "new_groupdlg.h"

KApplication *app;
Groupdlg  *main_widget;
KLocale *nls;

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

int main( int argc, char **argv )
{

    KApplication a( argc, argv, "krn" );

    app=&a;
    conf=a.getConfig();

    nls=a.getLocale();
    
    // Create our directory. If it exists, no problem
    // Should do some checking, though

    testDir( "/share" );
    testDir( "/share/config" );
    testDir( "/share/apps" );
    testDir( "/share/apps/krn" );
    testDir( "/share/apps/krn/folders" );
    testDir( "/share/apps/krn/cache" );
    testDir( "/share/apps/krn/groupinfo" );
    testDir( "/share/apps/krn/outgoing" );
    
    Groupdlg *k=new Groupdlg();
    main_widget = k;

    //insert this: (said Matthias)
    if (a.isRestored())
        k->restore(1);

    a.setMainWidget( k );
    
    k->setMinimumSize( 250, 250 );
    k->show();

    a.exec();

    k->close();
}
