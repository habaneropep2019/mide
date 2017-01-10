/**************************************************************************

    rhythmview.cpp  - The RhythmView widget
    Copyright (C) 1998  Antonio Larrosa Jimenez

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
#include "rhythmview.h"

RhythmView::RhythmView (QWidget *parent, char *name) : QWidget (parent,name)
{
    lamps=NULL;
    setRhythm(4,4);
}

RhythmView::~RhythmView()
{
    if(lamps!=NULL)
    {
        for (int i=0;i<num;i++) delete lamps[i];
        delete lamps;
        lamps=NULL;
    }
}

void RhythmView::setRhythm(int numerator,int denominator)
{
    int i;
    if(lamps!=NULL)
    {
        for (i=0;i<num;i++) delete lamps[i];
        delete lamps;
        lamps=NULL;
    }
    num=numerator;
    den=denominator;
    
    lamps=new (KLedLamp *)[num];
    int w=width()/num;
    int x=0;

    for (i=0;i<num;i++)
    {
        lamps[i]=new KLedLamp(this);
        lamps[i]->setState(KLedLamp::Off);
        lamps[i]->setGeometry(x+2,0,w-4,height());
        lamps[i]->show();
        x+=w;
    }
//    lamps[0]->setState(KLedLamp::On);

}

void RhythmView::Beat(int j)
{
    if (j>num) setRhythm(j,4); // This is a preventive case
    for (int i=0;i<num;i++)
    {
        lamps[i]->setState(KLedLamp::Off);
    }
    lamps[j-1]->setState(KLedLamp::On);
}

void RhythmView::Beat(void)
{
    
}

void RhythmView::resizeEvent(QResizeEvent *)
{
    int w=width()/num;
    int x=0;

    for (int i=0;i<num;i++)
    {
        lamps[i]->setGeometry(x+2,0,w-4,height());
        x+=w;
    }
}
