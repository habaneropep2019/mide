/**************************************************************************

    channelview.h - The ChannelView dialog
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

#ifndef CHANNELVIEW_H
#define CHANNELVIEW_H

#include <ktmainwindow.h>
#include <qscrbar.h>
#include "channel.h"


class ChannelView : public KTMainWindow
{
    Q_OBJECT
private:
    KMidChannel *Channel[16];
    int nvisiblechannels;

    virtual void resizeEvent(QResizeEvent *);
    virtual void closeEvent(QCloseEvent *e);
    
public:
	ChannelView(void);
	virtual ~ChannelView();

        void setScrollBarRange(void);

        void noteOn(int chn,int note);
        void noteOff(int chn,int note);
        void changeInstrument(int chn,int pgm);
        void changeForceState(int chn,bool i);

        void reset(int level=1);

        static int lookMode(void);

        void lookMode(int i);

public slots:
    void ScrollChn(int i);
    void slottokmidclient(int *data);

signals:
    void destroyMe();
    void signalToKMidClient(int *data);

private:
    QScrollBar *scrollbar;


    static int lookmode;


};
#endif
