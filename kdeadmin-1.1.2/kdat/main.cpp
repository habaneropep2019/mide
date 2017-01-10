// KDat - a tar-based DAT archiver
// Copyright (C) 1998  Sean Vyain, svyain@mail.tds.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <kapp.h>

#include "KDat.h"

// No prototype for mkdir?!?
extern "C" int mkdir( const char* path, int mode );

int main( int argc, char** argv )
{
    KApplication app( argc, argv, "kdat" );

    // Test for the existance of $HOME/.kdat
    QString fn = getenv( "HOME" );
    fn += "/.kdat";
    QFile file( fn );
    if ( !file.exists() ) {
        mkdir( fn.data(), 0700 );
    }

    app.setMainWidget( KDat::instance() );

    if ( app.isRestored() && KTopLevelWidget::canBeRestored( 1 ) ) {
        KDat::instance()->restore( 1 );
    } else {
        KDat::instance()->show();
    }

    return app.exec();
}
