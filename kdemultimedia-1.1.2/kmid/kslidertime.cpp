/**************************************************************************

    kslidertime.cpp  - A widget that displays time tags under a KSlider 
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
#include "kslidertime.h"
#include <qwidget.h>
#include <qpainter.h>
#include <stdio.h>

#define ARROW_LENGTH    13 

KSliderTime::KSliderTime( KSlider *ksl, QWidget *parent, const char *name)
					: QWidget (parent,name)
{
kslider=ksl;

QPainter painter;
QFontMetrics qfmt(painter.font());
fontheight=qfmt.height();

};

char *KSliderTime::formatMillisecs(int ms,char *tmp)
{
  if (ms<60000)
      {
      sprintf(tmp,"0:%02d",ms/1000);
      }
  else
      sprintf(tmp,"%d:%02d",ms/60000,(ms%60000)/1000);
return tmp;
};

void KSliderTime::paintEvent( QPaintEvent * )
{
  QPainter painter;

  painter.begin( this );
  erase();
  drawTimeMarks(&painter);
  painter.end();
}

int quantizeTimeStep(int t)
{
if (t<=2000) t=2000;
  else if (t<=5000) t=5000;
  else if (t<=10000) t=10000;
  else if (t<=15000) t=15000;
  else if (t<=30000) t=30000;
  else if (t<=60000) t=60000;
  else if (t<=120000) t=120000;
return t;
};

void KSliderTime::drawTimeMarks(QPainter *painter)
{
if (kslider==NULL) return;
  int i;
  int maxV = kslider->maxValue();
  QFontMetrics qfmt(painter->font());
  fontheight=qfmt.height();
  int ntimetags = width()/(qfmt.width("-88:88-"));
  int timestep;
  if (ntimetags>1) timestep = maxV/(ntimetags);
    else timestep=maxV;
  timestep = quantizeTimeStep(timestep);
  ntimetags = maxV/timestep;
 
 // draw time tags (only in horizontal !!)
  int posy=qfmt.height();
  char *tmp=new char[100];    
  int pos=0;
  int deltapos=0;
  formatMillisecs(0,tmp);
  painter->drawText( 0, posy,tmp);
  for ( i = timestep; i <= maxV - timestep; i += timestep )
     {
     pos = (width()-10) * i / maxV + 5;
     formatMillisecs(i,tmp);
     deltapos=qfmt.width(tmp)/2;
     painter->drawText( pos-deltapos, posy,tmp);
     }

  pos = width()- 5;
  formatMillisecs(maxV,tmp);
  deltapos=qfmt.width(tmp);
    
  painter->drawText( pos-deltapos, posy,tmp);
    
}  
