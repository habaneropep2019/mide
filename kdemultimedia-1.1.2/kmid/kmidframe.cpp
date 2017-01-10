/**************************************************************************

    kmidframe.cpp  - The main widget of KMid
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

#include <kmidframe.h>
#include <qkeycode.h>
#include <stdio.h>
#include <kfiledialog.h>
#include <kapp.h>
#include <qstring.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <kmsgbox.h>
#include <ktoolbar.h>
#include <kiconloader.h>
#include "kmidclient.h"
#include <kurl.h>
#include <drag.h>
#include <kfontdialog.h>
//#include <kkeyconf.h>
#include <kaccel.h>
#include <kkeydialog.h>
#include "midicfgdlg.h"
#include "collectdlg.h"
#include "channelcfgdlg.h"
#include "channelview.h"
#include "version.h"



kmidFrame::kmidFrame(const char *name)
    :KTMainWindow(name)
{
    kmidclient=new kmidClient(this,"KMidClient");
    kmidclient->songType(1);
    kmidclient->show();
    setView(kmidclient,FALSE);
    
    donttoggle=FALSE;
    
    kKeysAccel=new KAccel(this);
    kKeysAccel->insertItem(i18n("Play/Pause"),"Play/Pause", Key_Space);
    kKeysAccel->connectItem("Play/Pause", this, SLOT(spacePressed()));
    kKeysAccel->insertItem(i18n("Stop"),"Stop", Key_Backspace);
    kKeysAccel->connectItem("Stop",kmidclient,SLOT(song_Stop()));
    kKeysAccel->insertItem(i18n("Previous Song"),"Previous Song", Key_Left);
    kKeysAccel->connectItem("Previous Song",kmidclient,SLOT(song_PlayPrevSong()));
    kKeysAccel->insertItem(i18n("Next Song"),"Next Song", Key_Right);
    kKeysAccel->connectItem("Next Song",kmidclient,SLOT(song_PlayNextSong()));
    kKeysAccel->insertItem(i18n("See &Text events"),"See Text events",Key_1);
    kKeysAccel->connectItem("See Text events",this,SLOT(options_Text()));
    kKeysAccel->insertItem(i18n("See &Lyrics events"),"See Lyrics events",Key_2);
    kKeysAccel->connectItem("See Lyrics events",this,SLOT(options_Lyrics()));
    kKeysAccel->insertItem(i18n("Scroll down karaoke"),"Scroll down karaoke",Key_Down);
    kKeysAccel->connectItem("Scroll down karaoke",kmidclient->getKDisplayText(),SLOT(ScrollDown()));
    kKeysAccel->insertItem(i18n("Scroll up karaoke"),"Scroll up karaoke",Key_Up);
    kKeysAccel->connectItem("Scroll up karaoke",kmidclient->getKDisplayText(),SLOT(ScrollUp()));
    kKeysAccel->insertItem(i18n("Scroll page down karaoke"),"Scroll page down karaoke",Key_PageDown);
    kKeysAccel->connectItem("Scroll page down karaoke",kmidclient->getKDisplayText(),SLOT(ScrollPageDown()));
    kKeysAccel->insertItem(i18n("Scroll page up karaoke"),"Scroll page up karaoke",Key_PageUp);
    kKeysAccel->connectItem("Scroll page up karaoke",kmidclient->getKDisplayText(),SLOT(ScrollPageUp()));

    kKeysAccel->readSettings();

    m_file = new QPopupMenu;
    m_file->insertItem( i18n("&Open..."), this, 
                        SLOT(file_Open()), CTRL+Key_O );
    m_file->insertSeparator();
    m_file->insertItem( i18n("&Save Lyrics ..."), this, 
                        SLOT(file_SaveLyrics()) );
    m_file->insertSeparator();
    m_file->insertItem( i18n("&Quit"), qApp, SLOT(quit()), CTRL+Key_Q );
    m_song = new QPopupMenu;
    m_song->setCheckable(TRUE);
    m_song->insertItem( i18n("&Play"), kmidclient, SLOT(song_Play()) );
    kKeysAccel->changeMenuAccel(m_song,0,"Play/Pause");
    m_song->insertItem( i18n("P&ause"), this, SLOT(song_Pause()) );
    m_song->insertItem( i18n("&Stop"), kmidclient, SLOT(song_Stop()));
    m_song->setId(2,2);
    kKeysAccel->changeMenuAccel(m_song,2,"Stop");
    m_song->insertSeparator();
    m_song->insertItem( i18n("P&revious Song"), kmidclient, SLOT(song_PlayPrevSong()) );
    m_song->setId(4,4);
    kKeysAccel->changeMenuAccel(m_song,4,"Previous Song");
    m_song->insertItem( i18n("&Next Song"), kmidclient, SLOT(song_PlayNextSong()) );
    m_song->setId(5,5);
    kKeysAccel->changeMenuAccel(m_song,5,"Next Song");
    m_song->insertSeparator();
    m_song->insertItem( i18n("&Loop"), this, SLOT(song_Loop()) );
    m_song->setId(7,7);
    m_song->setItemChecked(7,FALSE);
    
    m_collections = new QPopupMenu;
    m_collections->setCheckable(TRUE);
    m_collections->insertItem( i18n("&Organize ..."), this, SLOT(collect_organize()));
    m_collections->setId(0,0);
    m_collections->insertSeparator();
    m_collections->insertItem( i18n("In order mode"), this, SLOT(collect_inOrder()));
    m_collections->setId(2,2);
    m_collections->setItemChecked(2,TRUE);
    m_collections->insertItem( i18n("Shuffle mode"), this, SLOT(collect_shuffle()));
    m_collections->setId(3,3);
    m_collections->setItemChecked(3,FALSE);
    m_collections->insertSeparator();
    m_collections->insertItem( i18n("AutoAdd to Collection"), this, SLOT(collect_autoadd()));
    m_collections->setId(5,5);
    m_collections->setItemChecked(5,FALSE);
    
    m_options = new QPopupMenu;
    m_options->setCheckable(TRUE);
    m_options->insertItem( i18n("&General Midi file"), this, 
                           SLOT(options_GM()) );
    m_options->setId(0,0);
    m_options->setItemChecked(0,TRUE);
    m_options->insertItem( i18n("&MT-32 file"), this, 
                           SLOT(options_MT32()) );
    m_options->setId(1,1);
    m_options->setItemChecked(1,FALSE);
    m_options->insertSeparator();
    m_options->insertItem( i18n("See &Text events"), this, 
                           SLOT(options_Text()) );
    m_options->setId(3,3);
    m_options->setItemChecked(3,TRUE);
    kKeysAccel->changeMenuAccel(m_options,3,"See Text events");

    m_options->insertItem( i18n("See &Lyrics events"), this, 
                           SLOT(options_Lyrics()) );
    m_options->setId(4,4);
    m_options->setItemChecked(4,FALSE);
    kKeysAccel->changeMenuAccel(m_options,4,"See Lyrics events");
    
    m_options->insertItem( i18n("&Automatic Text chooser"), this, 
                           SLOT(options_AutomaticText()) );
    m_options->setId(5,5);
    m_options->setItemChecked(5,TRUE);
    m_options->insertSeparator();
    
    m_options->insertItem( i18n("Show &Volume Bar"), this,
                           SLOT(options_ShowVolumeBar()) );
    m_options->setId(7,7);
    m_options->setItemChecked(7,FALSE);
    
    m_options->insertItem( i18n("Show &Channel View"), this,
                           SLOT(options_ShowChannelView()) );
    m_options->setId(8,8);
    m_options->setItemChecked(8,FALSE);
    
    m_options->insertItem( i18n("Channel View &Options"), this,
                           SLOT(options_ChannelViewOptions()) );
    
    m_options->insertSeparator();
    m_options->insertItem( i18n("&Font Change ...") , this, 
                           SLOT(options_FontChange()));
    m_options->insertItem( i18n("Configure &Keys ...") , this,
                           SLOT(options_KeyConfig()));
    m_options->insertSeparator();
    m_options->insertItem( i18n("Midi &Setup ...") , this,
                           SLOT(options_MidiSetup()));
    
    char aboutstring[500];
    sprintf(aboutstring,
            i18n("%s\n\n" \
                 "(C) 1997,98 Antonio Larrosa Jimenez\n" \
		 "larrosa@kde.org\t\tantlarr@arrakis.es\n" \
                 "Malaga (Spain)\n\n" \
                 "Midi/Karaoke file player\n" \
		 "KMid's homepage is at : http://www.arrakis.es/~rlarrosa/kmid.html\n\n" \
                 "KMid comes with ABSOLUTELY NO WARRANTY; for details view file COPYING\n" \
                 "This is free software, and you are welcome to redistribute it\n" \
                 "under certain conditions\n"), VERSION_TXT );
    m_help = (KApplication::getKApplication())->getHelpMenu(true, aboutstring);
    
    menu = new KMenuBar(this);
    menu->insertItem(i18n("&File"),m_file);
    menu->insertItem(i18n("&Song"),m_song);
    menu->insertItem(i18n("&Collections"),m_collections);
    menu->insertItem(i18n("&Options"),m_options);
    menu->insertSeparator();
    menu->insertItem(i18n("&Help"),m_help);
    menu->show();
    setMenu(menu);
    
    toolbar=new KToolBar(this);
    
    toolbar->insertButton(Icon("kmid_prev.xpm"),1,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Previous Song"));
    toolbar->insertButton(Icon("kmid_frewind.xpm"),2,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Rewind"));
    toolbar->insertButton(Icon("kmid_stop.xpm"),3,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Stop"));
    toolbar->insertButton(Icon("kmid_pause.xpm"),4,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Pause"));
    toolbar->setToggle(4,TRUE);
    toolbar->insertButton(Icon("kmid_play.xpm"),5,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Play"));
    toolbar->insertButton(Icon("kmid_fforward.xpm"),6,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Forward"));
    toolbar->insertButton(Icon("kmid_next.xpm"),7,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Next Song"));
    
    toolbar->insertSeparator();
    
    toolbar->insertButton(Icon("kmid_chn.xpm"),8,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Show Channel View"));
    toolbar->setToggle(8,TRUE);
    
    toolbar->insertButton(Icon("kmid_volume.xpm"),9,
                          SIGNAL(clicked(int)),this,SLOT(buttonClicked(int)),TRUE,
                          i18n("Show Volume Bar"));
    toolbar->setToggle(9,TRUE);
    
    addToolBar(toolbar);
    
    KConfig *kcfg=(KApplication::getKApplication())->getConfig();
    kcfg->setGroup("KMid");
    if (kcfg->readNumEntry("TypeOfTextEvents",5)==5)
        options_Lyrics();
    else
        options_Text();
    if (kcfg->readNumEntry("TypeOfMidiFile",0)==0)
        options_GM();
    else
        options_MT32();
    if ((kcfg->readNumEntry("Loop",0))==0)
    {
        m_song->setItemChecked(7,FALSE);
        kmidclient->setSongLoop(0);
    }
    else
    {
        m_song->setItemChecked(7,TRUE);
        kmidclient->setSongLoop(1);
    };
    if (kcfg->readNumEntry("ShowVolumeBar",0)==1)
    {
        m_options->setItemChecked(7,TRUE);
        kmidclient->visibleVolumeBar(1);
        toolbar->toggleButton(9);
    };
    
    if (kcfg->readNumEntry("CollectionPlayMode",0)==0)
        collect_inOrder();
    else
        collect_shuffle();
    
    m_collections->setItemChecked(5,
                                  (kcfg->readNumEntry("AutoAddToCollection",0)==1)? TRUE :FALSE);
    m_options->setItemChecked(5,
                              (kcfg->readNumEntry("AutomaticTextEventChooser",1)==1)?TRUE:FALSE);
    
    KDNDDropZone * dropZone = new KDNDDropZone( this , DndURL);
    connect( dropZone, SIGNAL( dropAction( KDNDDropZone *) ),
             this, SLOT( slotDropEvent( KDNDDropZone *) ) );
    
    connect( kmidclient, SIGNAL( mustRechooseTextEvent() ),
             this, SLOT( rechooseTextEvent() ) );
    
    connect( kmidclient, SIGNAL( song_stopPause() ),
             this, SLOT( song_stopPause() ) );
    
    connect( kmidclient, SIGNAL( channelView_Destroyed() ),
             this, SLOT( channelViewDestroyed() ) );
    
    if ((kapp->argc()>1)&&(kapp->argv()[1][0]!='-'))
    {
        printf("Opening command line file...\n");
        int backautoadd=kcfg->readNumEntry("AutoAddToCollection",0);
        kcfg->writeEntry("AutoAddToCollection",0);
        
        char ttt[40];
        sprintf(ttt,"%d",kapp->argc());
        int i=1;
        int c=autoAddSongToCollection((kapp->argv())[i],1);
        i++;
        while (i<kapp->argc()) 
        {
            autoAddSongToCollection((kapp->argv())[i],0);
            i++;
        };
        
        kmidclient->setActiveCollection(c);
        
        ///	    kmidclient->openURL((kapp->argv())[1]);
        /*	    if ((kcfg->readNumEntry("AutomaticTextEventChooser",1))==1)
         {
         if (kmidclient->ChooseTypeOfTextEvents()==1)
         options_Text();
         else
         options_Lyrics();
         }*/
        if (kmidclient->midiFileName()!=NULL) kmidclient->song_Play();
        kcfg->writeEntry("AutoAddToCollection",backautoadd);
    };
        /*
         kKeys->addKey("Play/Pause",Key_Space);
         kKeys->registerWidget("KMidFrame",this);
         kKeys->connectFunction("KMidFrame","Play/Pause",this,SLOT(spacePressed()));
         */
    kKeysAccel->writeSettings(kcfg);
};

kmidFrame::~kmidFrame()
{
    delete kmidclient;
    
    delete m_file;
    delete m_song;
    delete m_options;
    delete m_help;
    delete menu;
    delete toolbar;
    
    delete kKeysAccel;
};

void kmidFrame::file_Open()
{
char name[200];
name[0]=0;
QString filename;
filename=KFileDialog::getOpenFileName(0,"*.kar *.mid",this,name);
if (!filename.isNull())
        {
	char *s=new char[filename.length()+10];
	sprintf(s,"file:%s",(const char *)filename);
	int c=autoAddSongToCollection(s,1);
	delete s;
	kmidclient->setActiveCollection(c);
	};
};

void kmidFrame::song_Pause()
{
toolbar->toggleButton(4);
kmidclient->song_Pause();
};

void kmidFrame::song_stopPause()
{
if (kmidclient->isPaused()) 
   {
   song_Pause(); 
   };
};

void kmidFrame::buttonSClicked(int i)
{
switch (i)
    {
    case (1) : file_Open();break;
    };
};

void kmidFrame::buttonClicked(int i)
{
switch (i)
    {
    case (1) : kmidclient->song_PlayPrevSong();break;
    case (2) : kmidclient->song_Rewind();break;
    case (3) : kmidclient->song_Stop();break;
    case (4) : kmidclient->song_Pause();break;
    case (5) : kmidclient->song_Play();break;
    case (6) : kmidclient->song_Forward();break;
    case (7) : kmidclient->song_PlayNextSong();break; 
    case (8) : donttoggle=TRUE;options_ShowChannelView();donttoggle=FALSE;break;
    case (9) : donttoggle=TRUE;options_ShowVolumeBar();donttoggle=FALSE;break;
    };
};

void kmidFrame::options_GM()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
kcfg->writeEntry("TypeOfMidiFile",0);
kmidclient->songType(1);
m_options->setItemChecked(0,TRUE);
m_options->setItemChecked(1,FALSE);
};
void kmidFrame::options_MT32()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
kcfg->writeEntry("TypeOfMidiFile",1);
kmidclient->songType(0);
m_options->setItemChecked(0,FALSE);
m_options->setItemChecked(1,TRUE);
};
void kmidFrame::options_Text()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
kcfg->writeEntry("TypeOfTextEvents",1);
kcfg->sync();
m_options->setItemChecked(3,TRUE);
m_options->setItemChecked(4,FALSE);
kmidclient->repaintText(1);
};
void kmidFrame::options_Lyrics()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
kcfg->writeEntry("TypeOfTextEvents",5);
kcfg->sync();
m_options->setItemChecked(3,FALSE);
m_options->setItemChecked(4,TRUE);
kmidclient->repaintText(5);
};

void kmidFrame::options_AutomaticText()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
int automatic=kcfg->readNumEntry("AutomaticTextEventChooser",1);
automatic=(automatic==1) ? 0 : 1; 
kcfg->writeEntry("AutomaticTextEventChooser",automatic);
m_options->setItemChecked(5,(automatic==1) ? TRUE : FALSE);
};

void kmidFrame::options_FontChange()
{
KFontDialog *kfd=new KFontDialog(this);
QFont font;
font=*kmidclient->getFont();
kfd->getFont(font);
delete kfd;
KConfig *config=(KApplication::getKApplication())->getConfig();
config->setGroup("KMid");
config->writeEntry("KaraokeFont",font);
config->sync();
kmidclient->fontChanged();
};

int kmidFrame::autoAddSongToCollection(char *filename,int setactive)
{
  int r;
  SLManager *slman;
  SongList *sl;
  KConfig *kcfg=KApplication::getKApplication()->getConfig();
  kcfg->setGroup("KMid");
  if (kcfg->readNumEntry("AutoAddToCollection",0)==0)
        {
	r=0;
	slman=kmidclient->getSLManager();
	if (setactive) slman->createTemporaryCollection();
        sl=slman->getCollection(0);
	if (filename==NULL) sl->AddSong(kmidclient->midiFileName());
		else sl->AddSong(filename);
        }
       else
        {
        slman=kmidclient->getSLManager();
        sl=slman->getCollection(kmidclient->getActiveCollection());
	r=kmidclient->getActiveCollection();
	if (sl==NULL) return 0;
        int id;
	if (filename==NULL) id=sl->AddSong(kmidclient->midiFileName());
                else id=sl->AddSong(filename);
        if (setactive)
	   { 
           sl->setActiveSong(id);
	   };
        };
return r;
};


void kmidFrame::slotDropEvent( KDNDDropZone * _dropZone )
{
  QStrList & list = _dropZone->getURLList();

  char *s=list.first();

  int c=autoAddSongToCollection(s,1);
  while ((s=list.next())!=NULL)
	autoAddSongToCollection(s,0);

  kmidclient->setActiveCollection(c);

  if ((!kmidclient->isPlaying())&&(kmidclient->midiFileName()!=NULL))
		kmidclient->song_Play();
};  

void kmidFrame::shuttingDown()
{
   if (kmidclient->isPlaying()==1) 
   {
//      kmidclient->song_Stop();
      kmidclient->shuttingDown();
   };
};

void kmidFrame::saveProperties(KConfig *kcfg)
{
kmidclient->saveCollections();
int play=kmidclient->isPlaying();
kcfg->writeEntry("File",kmidclient->midiFileName());
kcfg->writeEntry("ActiveCollection",kmidclient->getActiveCollection());
kcfg->writeEntry("ActiveSong",kmidclient->getSelectedSong());
kcfg->writeEntry("Playing",play);
};

void kmidFrame::readProperties(KConfig *kcfg)
{
int activecol=kcfg->readNumEntry("ActiveCollection",0);
int activesong=kcfg->readNumEntry("ActiveSong",0);
int wasplaying=kcfg->readNumEntry("Playing",0);
SLManager *slman=kmidclient->getSLManager();
SongList *sl=slman->getCollection(activecol);
sl->setActiveSong(activesong);
kmidclient->setActiveCollection(activecol);
kmidclient->selectSong(activesong-1);

if ((activecol==0)&&(wasplaying)) // It was the temporary collection, 
	{		// surely the user would like to continue 
			// hearing the song
	char *s=new char [strlen((const char *)kcfg->readEntry("File",NULL))+1];
	strcpy(s,(const char *)kcfg->readEntry("File",NULL));
	int c=autoAddSongToCollection(s,1);
  	kmidclient->setActiveCollection(c);

	delete s;
	};


if ((wasplaying)&&(kmidclient->midiFileName()!=NULL)) kmidclient->song_Play();
};

void kmidFrame::options_MidiSetup()
{
if (kmidclient->devman()->checkInit()<0) 
	{
	KMsgBox::message(this,i18n("Error"),i18n("Couldn't open /dev/sequencer to get some info\nProbably there is another program using it"));
	return;
	};
MidiConfigDialog *dlg;

dlg=new MidiConfigDialog(kmidclient->devman(),NULL,"MidiDialog");
if (dlg->exec() == QDialog::Accepted) 
    {
#ifdef GENERAL_DEBUG_MESSAGES
    printf("Device selected : %d\n",MidiConfigDialog::selecteddevice);
#endif
    KConfig *kcfg=KApplication::getKApplication()->getConfig();
    kcfg->setGroup("KMid");
    kcfg->writeEntry("MidiPortNumber",MidiConfigDialog::selecteddevice);
    kmidclient->setMidiDevice(MidiConfigDialog::selecteddevice);
#ifdef GENERAL_DEBUG_MESSAGES
    printf("Midi map : %s\n",MidiConfigDialog::selectedmap);
#endif
    kcfg->setGroup("Midimapper"); 
    kcfg->writeEntry("LoadFile",(const char*)
       (MidiConfigDialog::selectedmap==NULL)? "":MidiConfigDialog::selectedmap);
    kmidclient->setMidiMapFilename(MidiConfigDialog::selectedmap);
    };
};

void kmidFrame::collect_organize()
{
CollectionDialog *dlg;
SLManager *slman=new SLManager(*kmidclient->getSLManager());
int idx=kmidclient->getActiveCollection();

dlg=new CollectionDialog(slman,idx,NULL,"MidiDialog");
if (dlg->exec() == QDialog::Accepted) 
    {
    kmidclient->setSLManager(slman);
    kmidclient->setActiveCollection(CollectionDialog::selectedC);
    kmidclient->selectSong(
     (slman->getCollection(CollectionDialog::selectedC))->getActiveSongID()-1);
    }
   else 
    {
    delete slman;
    };
};


void kmidFrame::rechooseTextEvent()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
if ((kcfg->readNumEntry("AutomaticTextEventChooser",1))==1)
   {
   if (kmidclient->ChooseTypeOfTextEvents()==1)
	options_Text();
       else
	options_Lyrics();
   }
};

void kmidFrame::song_Loop()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
int loop;
if ((kcfg->readNumEntry("Loop",0))==0)
   loop=1;
   else
   loop=0;

kcfg->writeEntry("Loop",loop);
kcfg->sync();
m_song->setItemChecked(7,(loop==1)? TRUE : FALSE);
kmidclient->setSongLoop(loop);
};

void kmidFrame::collect_inOrder()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
kcfg->writeEntry("CollectionPlayMode",0);
kcfg->sync();
m_collections->setItemChecked(2,TRUE);
m_collections->setItemChecked(3,FALSE);
kmidclient->setCollectionPlayMode(0);
};

void kmidFrame::collect_shuffle()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
kcfg->writeEntry("CollectionPlayMode",1);
kcfg->sync();
m_collections->setItemChecked(2,FALSE);
m_collections->setItemChecked(3,TRUE);
kmidclient->setCollectionPlayMode(1);
};

void kmidFrame::collect_autoadd()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
int i=kcfg->readNumEntry("AutoAddToCollection",0);
i=(i==1) ? 0 : 1; 
kcfg->writeEntry("AutoAddToCollection",i);
m_collections->setItemChecked(5,(i==1) ? TRUE : FALSE);
};


void kmidFrame::file_SaveLyrics()
{
char name[200];
name[0]=0;
QString filename;
filename=KFileDialog::getSaveFileName(0,"*.*",this,name);
if (!filename.isNull())
        {
	struct stat statbuf;
	if (stat((const char *)filename, &statbuf)!=-1)
		{
		char *s=new char [filename.length()+strlen(i18n("File %s already exists\nDo you want to overwrite it ?"))+5];
		sprintf(s,i18n("File %s already exists\nDo you want to overwrite it ?"),(const char *)filename);
		if (KMsgBox::yesNo(this,i18n("Warning"),s)==2) return;
		};
	FILE *fh=fopen((const char *)filename,"wt");
	kmidclient->saveLyrics(fh);
	fclose(fh);
	};
};

void kmidFrame::spacePressed()
{
if (!kmidclient->isPlaying()) kmidclient->song_Play();
   else song_Pause();
};
void kmidFrame::options_ShowVolumeBar()
{
KConfig *kcfg=KApplication::getKApplication()->getConfig();
kcfg->setGroup("KMid");
int i=kcfg->readNumEntry("ShowVolumeBar",0);
i=(i==1) ? 0 : 1; 
kcfg->writeEntry("ShowVolumeBar",i);
m_options->setItemChecked(7,(i==1) ? TRUE : FALSE);
if (donttoggle==FALSE) toolbar->toggleButton(9);
kmidclient->visibleVolumeBar(i);
};

void kmidFrame::options_ShowChannelView()
{
    int i=(kmidclient->getChannelView()==NULL)?0:1;
    i=(i==1) ? 0 : 1; 
    m_options->setItemChecked(8,(i==1) ? TRUE : FALSE);
    if (donttoggle==FALSE) toolbar->toggleButton(8);
    kmidclient->visibleChannelView(i);

    if (kmidclient->getChannelView()!=NULL)
        connect (kmidclient->getChannelView(),SIGNAL(destroyMe()),this,SLOT(channelViewDestroyed()));
};

void kmidFrame::channelViewDestroyed()
{
    kmidclient->channelViewDestroyed();
    int i=(kmidclient->getChannelView()==NULL)?0:1;
    m_options->setItemChecked(8,(i==1) ? TRUE : FALSE);
    toolbar->toggleButton(8);
};

void kmidFrame::options_ChannelViewOptions()
{
    ChannelViewConfigDialog *dlg;
    
    dlg=new ChannelViewConfigDialog(NULL,"ChannelViewConfigDialog");
    if (dlg->exec() == QDialog::Accepted) 
    {
        kmidclient->getChannelView()->lookMode(ChannelViewConfigDialog::selectedmode);
    };
};

void kmidFrame::options_KeyConfig()
{
    if ( KKeyDialog::configureKeys( kKeysAccel ) )
    {
	kKeysAccel->changeMenuAccel(m_song,0,"Play/Pause");
    	kKeysAccel->changeMenuAccel(m_song,2,"Stop");
	kKeysAccel->changeMenuAccel(m_song,4,"Previous Song");
	kKeysAccel->changeMenuAccel(m_song,5,"Next Song");
	kKeysAccel->changeMenuAccel(m_options,3,"See Text events");
	kKeysAccel->changeMenuAccel(m_options,4,"See Lyrics events");
    };
};
