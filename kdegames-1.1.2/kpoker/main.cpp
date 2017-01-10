/*
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *
 * Comment:
 * This is my first "bigger" application I did with Qt and my very first KDE application.
 * So please apologize some strange coding styles --> if you can't stand some really
 * bad parts just send me a patch including you "elegant" code ;)
*/
    
#include <kapp.h>
#include "global.h"
#include "top.h"

KLocale *locale;
KConfig *conf;

int main( int argc, char *argv[] )
{
	KApplication *myApp = new KApplication( argc, argv );
	PokerWindow *MainScreen = new PokerWindow;

//	locale = kapp->getLocale();

	if (myApp->isRestored()) {
	  if (KTopLevelWidget::canBeRestored(1))
	    MainScreen->restore(1);
	}

	myApp->setMainWidget( MainScreen );
	MainScreen->show();
		  
	int ret = myApp->exec();
		  
	delete MainScreen;
	delete myApp;
	
	return ret;

}
