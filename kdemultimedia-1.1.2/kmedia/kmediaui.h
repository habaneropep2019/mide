//-*-C++-*-
#ifndef KMEDIAUI_H
#define KMEDIAUI_H

/*
   Copyright (c) 1997-1998 Christian Esken (esken@kde.org)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


// $Id$
// KMediaUi header


#include <qtimer.h>
#include <qmsgbox.h>
#include <qslider.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include <signal.h>
#include <sys/utsname.h>
#include <unistd.h>

#include <kurl.h>
#include <kapp.h>
#include <kmsgbox.h>

extern "C" {
#include <mediatool.h>
}

//#include <kiconloader.h>
#include "kmediawin.h"



class KMediaUI : public QWidget { //: public KTopLevelWidget {
  Q_OBJECT

public:
  KMediaUI( QWidget *parent = 0, const char *name = 0 );
  KMediaWin	*kmw;


};

#endif /* KMEDIAUI_H */
