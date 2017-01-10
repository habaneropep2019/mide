/**************************************************************************

    channelview.cpp  - The ChannelView dialog
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

#include "channelview.h"
#include "channel3d.h"
#include "channel4d.h"
#include <kapp.h>


ChannelView::ChannelView(void) : KTMainWindow("ChannelView")
{
    setCaption(i18n("ChannelView"));
    for (int i=0;i<16;i++)
    {
        if (lookMode()==0)
            Channel[i]=new KMidChannel3D(i+1,this);
        else
            Channel[i]=new KMidChannel4D(i+1,this);
        connect(Channel[i],SIGNAL(signalToKMidClient(int *)),this,SLOT(slottokmidclient(int *)));
        Channel[i]->setGeometry(5,5+i*CHANNELHEIGHT,width()-20,CHANNELHEIGHT);
        Channel[i]->show();
    }
    scrollbar=new QScrollBar(1,16,1,1,1,QScrollBar::Vertical,this,"Channelscrollbar");
    connect(scrollbar,SIGNAL(valueChanged(int)),this,SLOT(ScrollChn(int)));
    setScrollBarRange();
    
}

ChannelView::~ChannelView()
{

}

void ChannelView::closeEvent(QCloseEvent *e)
{
    emit destroyMe();
    e->accept();
}

void ChannelView::resizeEvent(QResizeEvent *)
{
    scrollbar->setGeometry(width()-16,0,16,height());
    for (int i=0;i<16;i++)
    {
        Channel[i]->setGeometry(5,5+(i-(scrollbar->value()-1))*CHANNELHEIGHT,width()-20,CHANNELHEIGHT);
    }
    setScrollBarRange();

}

void ChannelView::setScrollBarRange(void)
{
    nvisiblechannels=height()/CHANNELHEIGHT;
    if (nvisiblechannels<16)
    scrollbar->setRange(1,16-nvisiblechannels+1);
   else
    scrollbar->setRange(1,1);
}

void ChannelView::ScrollChn(int v)
{
    for (int i=0;i<16;i++)
    {
        Channel[i]->move(5,5+(i-(v-1))*CHANNELHEIGHT);
    }
}

void ChannelView::noteOn(int chn,int note)
{
    Channel[chn]->noteOn(note);
}

void ChannelView::noteOff(int chn,int note)
{
    Channel[chn]->noteOff(note);
}

void ChannelView::changeInstrument(int chn,int pgm)
{
    Channel[chn]->changeInstrument(pgm);
}

void ChannelView::changeForceState(int chn,bool i)
{
    Channel[chn]->changeForceState(i);
}


void ChannelView::reset(int level)
{
    for (int i=0;i<16;i++)
    {
        Channel[i]->reset(level);
    }
}

int ChannelView::lookmode=0;

int ChannelView::lookMode(void)
{
    KConfig *kcfg=(KApplication::getKApplication())->getConfig();

    kcfg->setGroup("KMid");
    lookmode=kcfg->readNumEntry("ChannelViewLookMode",0);

    return lookmode;
}

void ChannelView::lookMode(int i)
{
    KConfig *kcfg=(KApplication::getKApplication())->getConfig();

    lookmode=i;

    kcfg->setGroup("KMid");
    kcfg->writeEntry("ChannelViewLookMode",lookmode);

    bool tmp[128];
    int pgm;
    for (int i=0;i<16;i++)
    {
        Channel[i]->saveState(tmp,&pgm);
        delete Channel[i];
        
        if (lookmode==0)
            Channel[i]=new KMidChannel3D(i+1,this);
        else
            Channel[i]=new KMidChannel4D(i+1,this);

        connect(Channel[i],SIGNAL(signalToKMidClient(int *)),this,SLOT(slottokmidclient(int *)));
        Channel[i]->setGeometry(5,5+(i-(scrollbar->value()-1))*CHANNELHEIGHT,width()-20,CHANNELHEIGHT);
        Channel[i]->loadState(tmp,&pgm);
        Channel[i]->show();
    }

}

void ChannelView::slottokmidclient(int *data)
{
    emit signalToKMidClient(data);
}
