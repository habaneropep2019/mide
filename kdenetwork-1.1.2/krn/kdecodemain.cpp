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
// This file is copyright 1997 by                                           //
// Roberto Alsina <ralsina@unl.edu.ar>                                      //
// Magnus Reftel  <d96reftl@dtek.chalmers.se>                               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include <qapp.h>

#include <kapp.h>
#include <kconfig.h>

#include "kdecode.h"

KDecode *decoder;

QString pixpath;

KConfig *conf;


int main( int argc, char **argv )
{

    KApplication a( argc, argv, "kdecode" );

    conf=a.getConfig();
    pixpath = a.kde_icondir(); 
    
    decoder=new KDecode;

    for (int i=1;i<argc;i++)
        decoder->load(argv[i]);

    decoder->showWindow();
}