/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */
#include <config.h>

#include <kapp.h>
#include <klocale.h>
#include <time.h>
#include "toplevel.h"

#include <X11/Xlib.h>

int main( int argc, char *argv[] )
{
    KApplication app( argc, argv, "kasteroids" );

    srandom( time(0) );

    KAstTopLevel mainWidget;
    mainWidget.show();
    app.setMainWidget( &mainWidget );

    app.exec();

    XAutoRepeatOn( qt_xdisplay() );

    return 0;
}

