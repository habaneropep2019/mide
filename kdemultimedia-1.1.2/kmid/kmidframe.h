/**************************************************************************

    kmidframe.h  - The main widget of KMid
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
#ifndef _KMIDFRAME_H
#define _KMIDFRAME_H

//#include <kapp.h>
//#include <qwidget.h>
#include <ktmainwindow.h>
#include <kmenubar.h>
#include <kslider.h>
#include "player/midiout.h"
#include "player/player.h"
#include "player/track.h"
//#include <pthread/mit/pthread.h>
#include <qtimer.h>
//#include "kdisptext.h"

class KApplication;
class KConfig;
class kmidClient;
class KToolBar;
class KDNDDropZone;
class KAccel;

class kmidFrame : public KTMainWindow
{
    Q_OBJECT
private:
#ifdef KMidDEBUG
    long passcount;
#endif

    midiOut *Midi;
    player *Player;

    int playerProcessID;
    PlayerController *pctl;
    
    int donttoggle;

    QPopupMenu *m_file;
    QPopupMenu *m_song;
    QPopupMenu *m_collections;
    QPopupMenu *m_options;
    QPopupMenu *m_help;

    KAccel *kKeysAccel;
protected:
    int autoAddSongToCollection(char *filename=NULL,int setactive=1);


    virtual void saveProperties(KConfig *kcfg);
    virtual void readProperties(KConfig *kcfg);

//    virtual void closeEvent(QCloseEvent *e);
public:
    kmidFrame(const char *name=0);
    virtual ~kmidFrame();

//    void readConfig(KConfig *kconf);

public slots:

    void buttonClicked(int i);
    void buttonSClicked(int i);
    void file_Open();
    void file_SaveLyrics();
    void song_Pause();
    void song_stopPause(); // release the pause button and quit the pause mode
    void song_Loop();
    void collect_organize();
    void collect_inOrder();
    void collect_shuffle();
    void collect_autoadd();
    void options_GM();
    void options_MT32();
    void options_Text();
    void options_Lyrics();
    void options_AutomaticText();
    void options_ShowVolumeBar();
    void options_ShowChannelView();
    void options_ChannelViewOptions();
    void options_FontChange();
    void options_KeyConfig();
    void options_MidiSetup();
    void spacePressed();

    void slotDropEvent( KDNDDropZone * _dropZone );
 
    void rechooseTextEvent();

    void channelViewDestroyed();
    void shuttingDown();

private:
    KMenuBar *menu;
    kmidClient *kmidclient;
    KToolBar *toolbar;
    KToolBar *toolbarSC; // toolbar for Song Collections
    KToolBar *toolbarSong; // toolbar for Songs
};

#endif
