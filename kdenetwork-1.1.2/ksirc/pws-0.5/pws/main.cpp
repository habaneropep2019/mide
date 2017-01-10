#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <qdir.h>

#include"pws.h"

#include<kapp.h>
#include<kconfig.h>

KConfig *conf;

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

int main(int argc, char *argv[])
{
	KApplication app(argc, argv, "pws");
        conf=app.getConfig();

        testDir ("/share");
        testDir ("/share/config");
        testDir ("/share/apps");
        testDir ("/share/apps/pws");
        
        PWS widget;

	app.setMainWidget(&widget);
	app.setTopWidget(&widget);

	widget.show();

	return app.exec();
}
