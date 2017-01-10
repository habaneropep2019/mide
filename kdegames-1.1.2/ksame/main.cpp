/*
 *   ksame 0.4 - simple Game
 *   Copyright (C) 1997,1998  Marcus Kreutzberger
 * 
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *    
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <kapp.h>
#include <qwidget.h>
#include <qtabbar.h>
#include <qtabdlg.h>
#include <qsize.h>
#include "KSameWidget.h"
#undef TrueColor


void myMessageOutput( QtMsgType type, const char *msg ) {
    switch ( type ) {
    case QtDebugMsg:
	fprintf( stderr, "Debug: %s\n", msg );
	break;
    case QtWarningMsg:
	fprintf( stderr, "Warning: %s\n", msg );
	break;
    case QtFatalMsg:
	fprintf( stderr, "Fatal: %s\n", msg );
	abort();                        // dump core on purpose
    }
}

int main( int argc, char **argv ) {
     qInstallMsgHandler( myMessageOutput );
     KApplication::setColorSpec(QApplication::TrueColor+QApplication::CustomColor);
     KApplication a( argc, argv , "ksame");
     
     
     KSameWidget w;
     if (kapp->isRestored()) {
	  if (KTopLevelWidget::canBeRestored(1))
	       w.restore(1);
     } else {
	  w.show();
     }
     a.setMainWidget(&w);
     return a.exec();
}









