/**************************************************************************

    main.cpp  - The main function for KMid
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

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
#include "kmidframe.h"
#include <stdlib.h>
#include <string.h>
#include <kapp.h>
#include <qwidget.h>
#include <signal.h>
#include "version.h"
//#include <locale.h>
#include <klocale.h>
#include <kmsgbox.h>

int main(int argc, char **argv)
{
    printf("%s Copyright (C) 1997,98 Antonio Larrosa Jimenez. Malaga (Spain)\n",VERSION_TXT);
    printf("KMid comes with ABSOLUTELY NO WARRANTY; for details view file COPYING\n");
    printf("This is free software, and you are welcome to redistribute it\n");
    printf("under certain conditions\n");

/*
    struct sigaction act;
    act.sa_handler = SIG_DFL;
    sigemptyset(&(act.sa_mask));
    act.sa_flags=0;

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
*/

    KApplication *app=new KApplication(argc,argv,"kmid");

    kmidFrame *kmidframe=new kmidFrame("KMid");

    app->setMainWidget ( kmidframe );

    QObject::connect(app,SIGNAL(shutDown()),kmidframe,SLOT(shuttingDown()));

    if (app->isRestored())
	{
	if (kmidframe->canBeRestored(1)) kmidframe->restore(1);
        }

    kmidframe->show();

/*
    if (app->isRestored())
	{
	RESTORE(kmidFrame);
        }
       else
        {
        kmidFrame *kmidframe=new kmidFrame("KMid");
	kmidframe->show();
        };
*/
    int ret= app->exec();

//    delete kmidframe;
    delete app;
    return ret;
};
