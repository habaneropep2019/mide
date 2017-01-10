
/*  This file is part of the KDE libraries
    Copyright (C) 1997 Richard Moore (moorer@cs.man.ac.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/


#include <stdio.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qpen.h>
#include <qcolor.h>
#include "ledlamp.h"
#include "ledlamp.moc"

LedLamp::LedLamp(QWidget *parent) : QFrame(parent),
  width( 10 ), height( 7 ), dx( 4 )
{
  // Make sure we're in a sane state
  s= Off;

  // Set the frame style
  //  setFrameStyle(Sunken | Box);
  setGeometry(0,0,height,width);
}

void LedLamp::drawContents(QPainter *painter)
{

  QBrush lightBrush(this->foregroundColor());
  QBrush darkBrush(this->backgroundColor());
  QPen pen(this->backgroundColor()

);

  switch(s) {

  case On:
    painter->setBrush(lightBrush);
    painter->drawRect(1,1,height-2, width-2);
    break;

  case Off:
    painter->setBrush(darkBrush);
    painter->drawRect(1,1,height-2, width-2);
    painter->setPen(pen);
    //    painter->drawLine(2,2,width-2, 2);
    //painter->drawLine(2,height-2,width-2,height-2);
    // Draw verticals
    //int i;
    //for (i= 2; i < width-1; i+= dx)
    //painter->drawLine(i,2,i,height-2);
    break;

  default:
    fprintf(stderr, "LedLamp: INVALID State (%d)\n", s);

  }
}





