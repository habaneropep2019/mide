/**************************************************************************

    kmidclient.cpp  - The main client widget of KMid
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
#include "kmidclient.h"
#include <qkeycode.h>
#include <stdio.h>
#include <stdlib.h>
#include <qfiledlg.h>
#include <kapp.h>
#include <qstring.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <kmsgbox.h>
#include <signal.h>   // kill is declared on signal.h on bsd, not sys/signal.h
#include <sys/signal.h>
//#include <qlcdnum.h>
#include <kurl.h>
#include <qlabel.h>
#include <qkeycode.h>
//#include <kkeyconf.h>
#include <kcombo.h>

#include "klcdnumber.h"

#include "player/midimapper.h"
#include "player/fmout.h"
#include "player/track.h"
#include "player/midispec.h"
#include "player/deviceman.h"
#include "player/mt32togm.h"
#include "randomlist.h"
#include "channelview.h"
#include "channel.h"
#include "version.h"
#include "rhythmview.h"


kmidClient::kmidClient(QWidget *parent,const char *name)
    :QWidget(parent,name)
{
    midifile_opened=NULL;
    loopsong=0;
    collectionplaymode=0;	
    collectionplaylist=NULL;
    channelView=NULL;
    noteArray=NULL;
    shuttingdown=false;
    
    KApplication *kappl;
    kappl=KApplication::getKApplication();
    KConfig *kconf=kappl->getConfig();
    
    kconf->setGroup("KMid"); 
    char *tmp=getenv("HOME");
    char *tmp2=new char[strlen(tmp)+30];
    sprintf(tmp2,"%s/.kmid_collections",tmp);
    collectionsfile=kconf->readEntry("CollectionsFile",tmp2);
    delete tmp2;
    slman=new SLManager();
    slman->loadConfig(collectionsfile);
    currentsl=NULL;
    //	currentsl=slman->getCollection(activecollection);
    itsme=0;
    playerProcessID=0;
    timebar = new KSlider(0,240000,30000,60000,KSlider::Horizontal,this);
    timebar->setSteps(30000,60000);
    timebar->setValue(0);
    timebar->setGeometry(5,10,width()-5,15);
    timebar->show();
    connect (timebar,SIGNAL(valueChanged(int)),this,SLOT(timebarChange(int)));
    
    timetags = new KSliderTime(timebar,this);
    timetags->setGeometry(5,10+timebar->height(),width()-5,16);
    
    qlabelTempo= new QLabel(i18n("Tempo :"),this,"tempolabel",
                            QLabel::NoFrame);
    qlabelTempo->move(5,10+timebar->height()+timetags->height()+5);
    qlabelTempo->adjustSize();
    
    tempoLCD = new KLCDNumber(true,3,this,"TempoLCD");
    //	tempoLCD->setMode(QLCDNumber::DEC);
    tempoLCD->setValue(120);
    tempoLCD->display(120);
    currentTempo=120;
    tempoLCD->setRange(3,999);
    tempoLCD->setDefaultValue(120);
    tempoLCD->setUserSetDefaultValue(true);
    tempoLCD->setGeometry(5+qlabelTempo->width()+5,10+timebar->height()+timetags->height()+5,83,28);
    connect(tempoLCD,SIGNAL(valueChanged(double)),this,SLOT(changeTempo(double)));


    comboSongs = new KCombo(FALSE,this,"Songs");
    //	fillInComboSongs();
    comboSongs->setGeometry(tempoLCD->x()+tempoLCD->width()+15,tempoLCD->y(),width()-(tempoLCD->x()+tempoLCD->width()+25),tempoLCD->height());
    connect (comboSongs,SIGNAL(activated(int)),this,SLOT(selectSong(int)));


    rhythmview= new RhythmView(this,"RhythmView");
    rhythmview->setGeometry(5,10+timebar->height()+timetags->height()+5+tempoLCD->height()+2,15,7);
    rhythmview->show();

    volumebar = new QSlider(0,200,10,100,QSlider::Vertical,this);
    volumebar->setSteps(10,20);
    volumebar->setValue(100);
    volumebar->setGeometry(5,10+timebar->height()+timetags->height()+5+tempoLCD->height()+10,15,height()-(10+timebar->height()+timetags->height()+5+tempoLCD->height()+15));
    volumebar->setTickmarks(QSlider::Right);
    volumebar->setTickInterval(50);
    visiblevolumebar=0;
    connect (volumebar,SIGNAL(valueChanged(int)),this,SLOT(volumebarChange(int)));
    
    typeoftextevents=1;
    kdispt=new KDisplayText(this,"KaraokeWindow");
    kdispt->move(((visiblevolumebar)?25:5),10+timebar->height()+timetags->height()+5+tempoLCD->height()+10);
    
    timer4timebar=new QTimer(this);	
    connect (timer4timebar,SIGNAL(timeout()),this,SLOT(timebarUpdate()));
    timer4events=new QTimer(this);	
    connect (timer4events,SIGNAL(timeout()),this,SLOT(processSpecialEvent()));
    
    
    fmOut::setFMPatchesDirectory((const char *)
                                 (kapp->kde_datadir()+"/kmid/fm"));
    
    
    sharedMemID=shmget(getpid(),sizeof(PlayerController),0666 | IPC_CREAT);
    if (sharedMemID==-1)
    {
        printf("ERROR : Can't allocate shared memory !!!\n"
               "Please report to antlarr@arrakis.es\n");
	exit(1);
    };
    
    pctl=(PlayerController *)shmat(sharedMemID,NULL,0);
    if (pctl==NULL)
        printf("ERROR : Can't get shared memory !!! "
               "Please report to antlarr@arrakis.es\n");
    pctl->playing=0;
    pctl->gm=1;
    pctl->volumepercentage=100;
    pctl->tempo=500000;
    pctl->ratioTempo=1.0;
    for (int i=0;i<16;i++)
    {
        pctl->forcepgm[i]=0;
        pctl->pgm[i]=0;
    };
    
    
    kconf->setGroup("KMid"); 
    int mididev=kconf->readNumEntry("MidiPortNumber",0);
    
    Midi = new DeviceManager(mididev);
    Midi->initManager();
    Player= new player(Midi,pctl);
    
    kconf->setGroup("Midimapper");
    QString qs=kconf->readEntry("Loadfile","gm.map");
    
#ifdef KMidDEBUG
    printf("Read Config file : %s\n",qs.data());
#endif
    tmp=new char[qs.length()+1];
    strcpy(tmp,qs);
    setMidiMapFilename(tmp);
    delete tmp;
    
    initializing_songs=1;
    kconf->setGroup("KMid"); 
    setActiveCollection(kconf->readNumEntry("ActiveCollection",0));
    initializing_songs=0;
    
    //        setActiveCollection(kconf->readNumEntry("ActiveCollection",0));
};

void kmidClient::resizeEvent(QResizeEvent *) 
{
    //timebar->resize(width()-5,timebar->height());
    timebar->setGeometry(5,10,width()-5,timebar->height());
    timetags->setGeometry(5,10+timebar->height(),width()-5,timetags->getFontHeight());
    comboSongs->setGeometry(tempoLCD->x()+tempoLCD->width()+15,tempoLCD->y(),width()-(tempoLCD->x()+tempoLCD->width()+25),tempoLCD->height());
    rhythmview->setGeometry(5,10+timebar->height()+timetags->height()+5+tempoLCD->height()+2,width()-10,7);
    volumebar->setGeometry(5,10+timebar->height()+timetags->height()+5+tempoLCD->height()+10,15,height()-(10+timebar->height()+timetags->height()+5+tempoLCD->height()+15));
    kdispt->setGeometry(((visiblevolumebar)?25:5),10+timebar->height()+timetags->height()+5+tempoLCD->height()+10,width()-(5+((visiblevolumebar)?25:5)),height()-(10+timebar->height()+timetags->height()+5+tempoLCD->height()+10));
};

kmidClient::~kmidClient()
{
    if (pctl->playing==1)
    {
        song_Stop();
        //    sleep(1);
    }; 
    
    if (playerProcessID!=0)
    {
        kill(playerProcessID,SIGTERM);
        waitpid(playerProcessID, NULL, 0);
        playerProcessID=0;
    };
    
    kdispt->PreDestroyer();
    delete kdispt;
    
    if (midifile_opened!=NULL) delete midifile_opened;
    delete timer4timebar;
    delete timer4events;
    delete tempoLCD;
    delete Player;
    delete Midi;
    if (collectionplaylist!=NULL) delete collectionplaylist;
    
    saveCollections();
    delete slman;

// Let's detach and delete shared memory
    shmdt((char *)pctl);
    shmctl(sharedMemID, IPC_RMID, 0L);
};

char *extractFilename(const char *in,char *out)
{
    char *p=(char *)in;
    char *result=out;
    char *filename=(char *)in;
    while (*p!=0)
    {
        if (*p=='/') filename=p+1;
        p++;
    };
    while (*filename!=0)
    {
        *out=*filename;
        out++;
        filename++;
    };
    *out=0;
    return result;
};

int kmidClient::openFile(char *filename)
{
    pctl->message|=PLAYER_HALT;
    song_Stop();
    int r;
    if ((r=Player->loadSong(filename))!=0)
    {
        char errormsg[200];
        switch (r)
        {
        case (-1) : sprintf(errormsg,
                            i18n("The file %s doesn't exist or can't be opened"),filename);
        break;
        case (-6) :
        case (-2) : sprintf(errormsg,
                            i18n("The file %s is not a midi file"),filename);break;
        case (-3) : sprintf(errormsg,
                            i18n("Ticks per cuarter note is negative, please, send this file to antlarr@arrakis.es"));break;
        case (-4) : sprintf(errormsg,
                            i18n("Not enough memory !!"));break;
        case (-5) : sprintf(errormsg,
                            i18n("This file is corrupted or not well built"));break;
        default : sprintf(errormsg,i18n("Unknown error message"));break;
        };
        KMsgBox::message(this,i18n("Error"),errormsg);
        //	Player->loadSong(midifile_opened);
        if (midifile_opened!=NULL) delete midifile_opened;
        midifile_opened=NULL;
        timebar->setRange(0,240000);
        timebar->setValue(0);
        timetags->repaint(TRUE);
        kdispt->ClearEv();
        kdispt->repaint(TRUE);
        topLevelWidget()->setCaption("KMid");
        
        return -1; 
    };
    
    if (midifile_opened!=NULL) delete midifile_opened;
    midifile_opened=new char[strlen(filename)+1];
    strcpy(midifile_opened,filename);
#ifdef KMidDEBUG
    printf("TOTAL TIME : %g milliseconds\n",Player->Info()->millisecsTotal);
#endif
    //    noteArray=Player->parseNotes();
    noteArray=Player->getNoteArray();
    timebar->setRange(0,(int)(Player->Info()->millisecsTotal));
    timetags->repaint(TRUE);
    kdispt->ClearEv();
    spev=Player->takeSpecialEvents();
    while (spev!=NULL)
    {
        if ((spev->type==1) || (spev->type==5)) 
        {
            kdispt->AddEv(spev);
        };
        spev=spev->next;
    };
    
    kdispt->calculatePositions();
    kdispt->CursorToHome();
    kdispt->repaint(TRUE);
    emit mustRechooseTextEvent();
    tempoLCD->display(tempoToMetronomeTempo(pctl->tempo));
    currentTempo=tempoLCD->getValue();
    tempoLCD->setDefaultValue(tempoToMetronomeTempo(pctl->tempo)*pctl->ratioTempo);
    
    char *fn=new char[strlen(filename)+20];
    extractFilename(filename,fn);
    char *capt=new char[strlen(fn)+20];
    sprintf(capt,"KMid - %s",fn);
    delete fn;
    topLevelWidget()->setCaption(capt);
    delete capt;
    
    timebar->setValue(0);
    return 0;
};

int kmidClient::openURL(char *s)
{
    if (s==NULL) {printf("s == NULL!!! \n");return -1;};
    KURL kurldropped(s);
    if (kurldropped.isMalformed()) {printf("Malformed URL\n");return -1;};
    if (strcmp(kurldropped.protocol(),"file")!=0) {printf("KMid only accepts local files\n");return -1;};
    
    char *filename=(char *)kurldropped.path();
    QString qsfilename(filename);
    KURL::decodeURL(qsfilename);
    filename=(char *)(const char *)qsfilename;
    int r=-1;
    if (filename!=NULL) 
    {
        r=openFile(filename);
    }
    return r;
};

ulong kmidClient::timeOfNextEvent(int *type)
{
    int t=0;
    ulong x=0;
    
    
    if (channelView==NULL)
    {
        if ((spev!=NULL)&&(spev->type!=0))
        {
            t=1;
            x=spev->absmilliseconds;
        };
    }
    else
    {
        if (noteArray!=NULL)
        {
            noteCmd *ncmd=noteArray->get();
            if (ncmd==NULL)
            {
                if ((spev!=NULL)&&(spev->type!=0))
                {
                    t=1;
                    x=spev->absmilliseconds;
                }
            }
            else
            {
                if ((spev==NULL)||(spev->type==0))
                {
                    t=2;
                    x=ncmd->ms;
                }
                else
                {
                    if (spev->absmilliseconds<ncmd->ms)
                    {
                        t=1;
                        x=spev->absmilliseconds;
                    }
                    else
                    {
                        t=2;
                        x=ncmd->ms;
                    };
                    
                };
            };
        };
    };
    
    if (type!=NULL) *type=t;
    return x;
    /*
     
     if (type!=NULL) *type=0;
     if (channelView==NULL)
     {
     if ((spev!=NULL)&&(spev->type!=0))
     {
     if (type!=NULL) *type=1;
     return spev->absmilliseconds;
     }
     else return 0;
     };
     
    if (noteArray==NULL) return 0;
    noteCmd *ncmd=noteArray->get();
    if (ncmd==NULL)
    {
        if ((spev!=NULL)&&(spev->type!=0))
        {
            if (type!=NULL) *type=1;
            return spev->absmilliseconds;
        }
        else return 0;
    }
    else
    {
        if ((spev==NULL)||(spev->type==0))
        {
            if (type!=NULL) *type=2;
            return ncmd->ms;
        }
        else
        {
            if (spev->absmilliseconds<ncmd->ms)
            {
                if (type!=NULL) *type=1;
                return spev->absmilliseconds;
            }
            else
            {
                if (type!=NULL) *type=2;
                return ncmd->ms;
            };
            
        };
    };
    */
};

void kmidClient::song_Play()
{
    if (!Player->isSongLoaded())
    {
        KMsgBox::message(this,i18n("Warning"),
                         i18n("You must load a file before playing it"));
        return;
    };
    if (pctl->playing==1)
    {
        KMsgBox::message(this,i18n("Warning"),
                         i18n("A song is already being played"));
        return;
    };
#ifndef MODE_DEMO_ONLYVISUAL
    if (Midi->checkInit()==-1)
    {
        KMsgBox::message(this,i18n("Error"), i18n("Couldn't open /dev/sequencer\nProbably there is another program using it"));
        return;
    };
#endif
    kdispt->CursorToHome();
    pctl->message=0;
    pctl->playing=0;
    pctl->finished=0;
    pctl->error=0;
    pctl->SPEVplayed=0;
    pctl->SPEVprocessed=0;
#ifdef KMidDEBUG
    passcount=0;
#endif
    noteArray->iteratorBegin();
    
    if ((playerProcessID=fork())==0)
    {
#ifdef KMidDEBUG
        printf("PlayerProcessID : %d\n",getpid());
#endif
        Player->play(0,(void (*)(void))kmidOutput);
#ifdef KMidDEBUG
        printf("End of child process\n");
#endif
        _exit(0);
    };
    pctl->millisecsPlayed=0;


    spev=Player->takeSpecialEvents();
#ifdef KMidDEBUG
    printf("writing SPEV\n");
    Player->writeSPEV();
    printf("writing SPEV(END)\n");
#endif

    while ((pctl->playing==0)&&(pctl->error==0)) ;

#ifndef MODE_DEMO_ONLYVISUAL
    if (pctl->error==1) return;
    beginmillisec=pctl->beginmillisec;
#else
    timeval begintv;
    gettimeofday(&begintv, NULL);
    beginmillisec=begintv.tv_sec*1000+begintv.tv_usec/1000;
#endif


//if ((spev!=NULL)&&(spev->type!=0))
//   timer4events->start(spev->absmilliseconds,TRUE);

    int type;
    ulong x=timeOfNextEvent(&type);
    if (type!=0)
        timer4events->start(x,TRUE);
    
    timer4timebar->start(1000);
    
#ifdef KMidDEBUG
    printf("PlayerProcess : %d . ParentProcessID : %d\n",playerProcessID,getpid());
    printf("******************************-\n");
#endif
};

void kmidClient::timebarUpdate()
{
    itsme=1;
#ifndef MODE_DEMO_ONLYVISUAL
    if (pctl->playing==0)
    {
        timer4timebar->stop();
    };
#endif
    timeval tv;
    gettimeofday(&tv, NULL);
    ulong currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
    pctl->millisecsPlayed=(currentmillisec-beginmillisec);
    
    timebar->setValue((int)(pctl->millisecsPlayed));
    itsme=0;
    if ((pctl->playing==0)&&(pctl->finished==1))
    {
        waitpid(playerProcessID, NULL, 0);
        if (loopsong)
        {
            song_Play();
            return;
        }
        else
            song_PlayNextSong();
    };
};

void kmidClient::timebarChange(int i)
{
    if (itsme==1) return;
    //if (timebar->draggingSlider()==TRUE) return;
#ifndef MODE_DEMO_ONLYVISUAL
    if (pctl->playing==0) 
    {
        itsme=1;
        timebar->setValue(0);
        itsme=0;
        return;
    };
#endif

    if (pctl->paused) return;

    if (playerProcessID!=0)
    {
        kill(playerProcessID,SIGTERM);
#ifdef KMidDEBUG
        printf("Waiting for Process %d to be killed\n",playerProcessID);
#endif
        waitpid(playerProcessID, NULL, 0);
        playerProcessID=0;
    };
    
    
#ifdef KMidDEBUG
    printf("change Time : %d\n",i);
#endif

    timer4events->stop();
    if (channelView!=NULL) channelView->reset(0);
    
    moveEventPointersTo((ulong)i);
    
    pctl->playing=0;
    pctl->OK=0;
    pctl->error=0;
    pctl->gotomsec=i;
    pctl->message|=PLAYER_SETPOS;
    
    if ((playerProcessID=fork())==0)
    {
#ifdef KMidDEBUG
        printf("Player_ProcessID : %d\n",getpid());
#endif
        
        Player->play(0,(void (*)(void))kmidOutput);
        
#ifdef KMidDEBUG
        printf("End of child process\n");
#endif
        _exit(0);
    };
    
    while ((pctl->playing==0)&&(pctl->error==0)) ;
    
#ifndef MODE_DEMO_ONLYVISUAL
    if (pctl->error==1) return;
    beginmillisec=pctl->beginmillisec-i;
    //timeval tv;
    //gettimeofday(&tv, NULL);
    //ulong currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000; 
    ulong currentmillisec=pctl->beginmillisec;
#else
    timeval tv;
    gettimeofday(&tv, NULL);
    ulong currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
    beginmillisec=currentmillisec-i;
#endif
    int type;
    ulong x=timeOfNextEvent(&type);
    if (type!=0)
        timer4events->start(x-(currentmillisec-beginmillisec),TRUE);
    
    /*
     if (spev==NULL) return;
     ulong delaymillisec=spev->absmilliseconds-(currentmillisec-beginmillisec);
     timer4events->start(delaymillisec,TRUE);
     */

    pctl->OK=0;
/*
    tempoLCD->display(tempoToMetronomeTempo(pctl->tempo));
    currentTempo=tempoLCD->getValue();
    tempoLCD->setDefaultValue(tempoToMetronomeTempo(pctl->tempo)*pctl->ratioTempo);
*/
};

void kmidClient::moveEventPointersTo(ulong ms)
{
#ifdef KMidDEBUG
    printf("Move To : %lu\n",ms);
#endif
    spev=Player->takeSpecialEvents();

    ulong tempo=(ulong)(500000 * pctl->ratioTempo);
    int num=4;
    int den=4;
    
    while ((spev!=NULL)&&(spev->absmilliseconds<ms))
    {
        if (spev->type==3) tempo=spev->tempo;
        else if (spev->type==6) {num=spev->num;den=spev->den;};
        spev=spev->next;
    };
    tempoLCD->display(tempoToMetronomeTempo(tempo));
    currentTempo=tempoLCD->getValue();
    tempoLCD->setDefaultValue(tempoToMetronomeTempo(tempo)*pctl->ratioTempo);

    rhythmview->setRhythm(num,den);
    
    kdispt->gotomsec(ms);
//    if (noteArray!=NULL) noteArray->moveIteratorTo(ms);
    if (noteArray!=NULL)
    {
        int pgm[16];
        noteArray->moveIteratorTo(ms,pgm);
        if (channelView!=NULL)
        {
            for (int j=0;j<16;j++)
            {
                if (!pctl->forcepgm[j]) channelView->changeInstrument(j,(pctl->gm==1)?(pgm[j]):(MT32toGM[pgm[j]]));
                else channelView->changeInstrument(j,(pctl->pgm[j]));
            };
        };
    };
    
    /*
     if (noteArray!=NULL)
     {
     noteCmd *ncmd;
     noteArray->iteratorBegin();
     ncmd=noteArray->get();
     while ((ncmd!=NULL)&&(ncmd->ms<ms))
     {
     noteArray->next();
     ncmd=noteArray->get();
     };
     };
     */
};

void kmidClient::volumebarChange(int i)
{
    int autochangemap=0;
    if ((pctl->playing==1)&&(pctl->paused==0)) autochangemap=1;
    
    if (autochangemap)
    {
        song_Pause();
    };
    i=200-i;
    pctl->volumepercentage=i;
    
    if (autochangemap)
    {
        song_Pause(); 
    };
}


void kmidClient::song_PlayPrevSong()
{
    if (currentsl==NULL) return;
    if (collectionplaylist==NULL) generateCPL();
    if (collectionplaylist==NULL) return;
    /*
     if (collectionplaymode==0)
     {
     if (currentsl->getActiveSongID()==1) return;
     currentsl->previous();
     }
     else
     {
     int r;
     while ((r=1+(int) ((double)(currentsl->NumberOfSongs())*rand()/(RAND_MAX+1.0)))==currentsl->getActiveSongID()) ;
     
     currentsl->setActiveSong(r);
     };
     */
    int idx=searchInCPL(currentsl->getActiveSongID());
    if (idx==0) return;
    idx--;
    currentsl->setActiveSong(collectionplaylist[idx]);
    
    if (currentsl->getActiveSongID()==-1)
    {
        //    comboSongs->setCurrentItem(0);
        //    currentsl->setActiveSong(1);
        return;
    };
    
    if (pctl->paused) emit song_stopPause();
    comboSongs->setCurrentItem(currentsl->getActiveSongID()-1);
    if (openURL(currentsl->getActiveSongName())==-1) return;
    song_Play();
    
};

void kmidClient::song_PlayNextSong()
{
    if (currentsl==NULL) return;
    if (collectionplaylist==NULL) generateCPL();
    if (collectionplaylist==NULL) return;
    
    /*if (collectionplaymode==0)
     {
     if (currentsl->getActiveSongID()==currentsl->NumberOfSongs()) return;
     currentsl->next();
     }
     else
     {
     int r;
     while ((r=1+(int) ((double)(currentsl->NumberOfSongs())*rand()/(RAND_MAX+1.0)))==currentsl->getActiveSongID()) ;
     
     #ifdef KMidDEBUG
     printf("random number :%d\n",r);
     #endif
     currentsl->setActiveSong(r);
     };
     */
    int idx=searchInCPL(currentsl->getActiveSongID());
    idx++;
    if (idx==currentsl->NumberOfSongs()) return;
    currentsl->setActiveSong(collectionplaylist[idx]);
    if (currentsl->getActiveSongID()==-1)
    {
        ////    comboSongs->setCurrentItem(0);
        //    currentsl->setActiveSong(1);
        return;
    };
    
    if (pctl->paused) emit song_stopPause();
    comboSongs->setCurrentItem(currentsl->getActiveSongID()-1);
    if (openURL(currentsl->getActiveSongName())==-1) return;
    song_Play();
};

void kmidClient::song_Pause()
{
#ifndef MODE_DEMO_ONLYVISUAL
    if (pctl->playing==0) return;
#endif
#ifdef KMidDEBUG
    printf("song Pause\n");
#endif
    if (pctl->paused==0)
    {
        if (playerProcessID!=0)
        {
            kill(playerProcessID,SIGTERM);
            waitpid(playerProcessID, NULL, 0);
            playerProcessID=0;
        };
        pausedatmillisec=(ulong)pctl->millisecsPlayed;
        pctl->paused=1;
        timer4timebar->stop(); 
        timer4events->stop();
        //    kill(playerProcessID,SIGSTOP);
        //   The previous line doesn't work because it stops the two processes (!?)
    }
    else
    {
        pctl->playing=0;
        pctl->OK=0;
        pctl->error=0;
        pctl->gotomsec=pausedatmillisec;
        pctl->message|=PLAYER_SETPOS;
        if ((playerProcessID=fork())==0)
        {
#ifdef KMidDEBUG
            printf("PlayerProcessID : %d\n",getpid());
#endif
            Player->play(0,(void (*)(void))kmidOutput);
#ifdef KMidDEBUG
            printf("End of child process\n");
#endif
            _exit(0);
        };
        
        while ((pctl->playing==0)&&(pctl->error==0)) ;
        
#ifndef MODE_DEMO_ONLYVISUAL
        if (pctl->error==1) return;
#endif
        pctl->OK=0;
        pctl->paused=0;
        
        beginmillisec=pctl->beginmillisec-pausedatmillisec;
        ulong currentmillisec=pctl->beginmillisec;
        
        int type;
        ulong x=timeOfNextEvent(&type);
        if (type!=0)
            timer4events->start(x-(currentmillisec-beginmillisec),TRUE);
        timer4timebar->start(1000);
        
        if (noteArray!=NULL)
        {
            int pgm[16];
            noteArray->moveIteratorTo(pausedatmillisec,pgm);
            if (channelView!=NULL)
            {
                for (int j=0;j<16;j++)
                {
                    if (!pctl->forcepgm[j]) channelView->changeInstrument(j,(pctl->gm==1)?(pgm[j]):(MT32toGM[pgm[j]]));
                    else channelView->changeInstrument(j,(pctl->pgm[j]));
                };
            };

        };
        
    };
};

void kmidClient::shuttingDown(void)
{
    shuttingdown=true;
    song_Stop();
};

void kmidClient::song_Stop()
{
    if (!shuttingdown)
    {
        for (int i=0;i<16;i++) pctl->forcepgm[i]=FALSE;
        if (channelView!=NULL) channelView->reset();
        tempoLCD->display(tempoToMetronomeTempo(pctl->tempo));
        currentTempo=tempoLCD->getValue();
        tempoLCD->setDefaultValue(tempoToMetronomeTempo(pctl->tempo)*pctl->ratioTempo);
    }
    
#ifndef MODE_DEMO_ONLYVISUAL
    if (pctl->playing==0) return;
#endif
    if (pctl->paused) return;
#ifdef KMidDEBUG
    printf("song Stop\n");
#endif
    if (playerProcessID!=0)
    {
        kill(playerProcessID,SIGTERM);
#ifdef KMidDEBUG
	printf("Killing\n");
#endif
        waitpid(playerProcessID, NULL, 0);
        playerProcessID=0;
    };
    pctl->playing=0;
    ////////pctl->OK=0;
    ////////pctl->message|=PLAYER_HALT;
    timer4timebar->stop();
    timer4events->stop();
    //pctl->playing=0;
    //pctl->paused=0;
    ////////while (pctl->OK==0) ;
};

void kmidClient::song_Rewind()
{
    if ((pctl->playing)&&(!pctl->paused))
    {
        timebar->subtractPage();
        timebarChange(timebar->value());
    };
};

void kmidClient::song_Forward()
{
    if ((pctl->playing)&&(!pctl->paused))
    {
        timebar->addPage();
        timebarChange(timebar->value());
    };
};


void kmidClient::kmidOutput(void)
{
// Should do nothing
    /*
    Midi_event *ev=pctl->ev;
    
    timeval tv;
    gettimeofday(&tv, NULL);
    ulong currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
    
    if ((ev->command==MIDI_SYSTEM_PREFIX)&&((ev->command|ev->chn)==META_EVENT))
    {
        if ((ev->d1==5)||(ev->d1==1))
        {
            char *text=new char[ev->length+1];
            strncpy(text,(char *)ev->data,ev->length);
            text[ev->length]=0;
#ifdef KMidDEBUG
            printf("%s , played at : %ld\n",text,currentmillisec-beginmillisec);
#endif
        }
        else if (ev->d1==ME_SET_TEMPO)
        {
            int tempo=(ev->data[0]<<16)|(ev->data[1]<<8)|(ev->data[2]);
            //	    printf("Change tempo : %d , %g, played at :%ld\n",tempo,tempoToMetronomeTempo(tempo),currentmillisec-beginmillisec);
        };
        
    }; 
     */
};


void kmidClient::processSpecialEvent()
{
/*
    if (spev==NULL)
    {
        printf("SPEV == NULL !!!!!\n");
        return;
    };
*/

//#ifdef KMidDEBUG
//    printf(":::: %ld",passcount++);
//    printf("%d %s %ld",spev->type,spev->text,spev->absmilliseconds);
//#endif

    int processNext=1;
    int type;
    ulong x;

    long delaymillisec=~0;
    
    while (processNext)
    {
        /*
         timeval tv;
         gettimeofday(&tv, NULL);
         ulong currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
         */

        x=timeOfNextEvent(&type);

        if (type==0) return;
        if (type==1)
        {
            if ((spev->type==1) || (spev->type==5))
            {
                kdispt->PaintIn(spev->type);
            }
            else if (spev->type==3)
            {
                tempoLCD->display(tempoToMetronomeTempo(spev->tempo));
#ifdef KMidDEBUG
                printf("Changing lcd tempo : spev->tempo : %d , ratio : %.9g\n",spev->tempo,pctl->ratioTempo);
                printf("Result : %g %.9g %d\n",tempoToMetronomeTempo(spev->tempo),tempoToMetronomeTempo(spev->tempo),(int)tempoToMetronomeTempo(spev->tempo));
#endif
                currentTempo=tempoLCD->getValue();
                tempoLCD->setDefaultValue(tempoToMetronomeTempo(spev->tempo)*pctl->ratioTempo);
            }
            else if (spev->type==6)
            {
                rhythmview->setRhythm(spev->num,spev->den);
            }
            else if (spev->type==7)
            {
#ifdef KMidDEBUG
                printf("Beat : %d/%d\n",spev->num,spev->den);
#endif
                rhythmview->Beat(spev->num);
            };
            pctl->SPEVprocessed++;
            spev=spev->next;
        };
        if (type==2)
        {
            noteCmd *ncmd=noteArray->get();
            if (ncmd==NULL) {printf("ncmd is NULL !!!");return;};
            if (channelView!=NULL)
            {
                if (ncmd->cmd==1) channelView->noteOn(ncmd->chn,ncmd->note);
                else if (ncmd->cmd==0) channelView->noteOff(ncmd->chn,ncmd->note);
                else if (ncmd->cmd==2)
                    if (!pctl->forcepgm[ncmd->chn]) channelView->changeInstrument(ncmd->chn,(pctl->gm==1)?(ncmd->note):(MT32toGM[ncmd->note]));
                    else channelView->changeInstrument(ncmd->chn,(pctl->pgm[ncmd->chn]));

                noteArray->next();
            };
        };
        processNext=0;

        x=timeOfNextEvent(&type);

        if (type==0) return;
        
        timeval tv;
        ulong currentmillisec;
        gettimeofday(&tv, NULL);
        currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
        delaymillisec=x-(currentmillisec-beginmillisec);
        if (delaymillisec<10) processNext=1;
    }

    if (delaymillisec!=~(long)0) timer4events->start(delaymillisec,TRUE);

};

void kmidClient::repaintText(int type)
{
    kdispt->ChangeTypeOfTextEvents(type);
    typeoftextevents=type;
    kdispt->repaint(TRUE);
};

int kmidClient::ChooseTypeOfTextEvents(void)
{
    return kdispt->ChooseTypeOfTextEvents();
};

void kmidClient::songType(int i)
{
    int autochangetype=0;
    if ((pctl->playing==1)&&(pctl->paused==0)) autochangetype=1;
    
    if (autochangetype) 
    {
        song_Pause();
    };
    pctl->gm=i;

    if (autochangetype)
    {
        song_Pause();
    };
    
};


QFont * kmidClient::getFont(void)
{
return kdispt->getFont();
};

void  kmidClient::fontChanged(void)
{
    kdispt->fontChanged();
};

void kmidClient::setMidiDevice(int i)
{
    Midi->setDefaultDevice(i);
};

void kmidClient::setMidiMapFilename(char *mapfilename)
{
    MidiMapper *Map=new MidiMapper(mapfilename);
    if (Map->OK()==-1)
    {
        char *tmp=new char [strlen(mapfilename)+
                            strlen(KApplication::kde_datadir())+20];
        sprintf(tmp,"%s/kmid/maps/%s",
                (const char *)KApplication::kde_datadir(),mapfilename);
        delete Map;
        Map=new MidiMapper(tmp);
        delete tmp;
        if (Map->OK()!=1)
        {
            delete Map;
            Map=new MidiMapper(NULL);
        };
    };
    int autochangemap=0;
    if ((pctl->playing==1)&&(pctl->paused==0)) autochangemap=1;
    
    if (autochangemap)
    {
        song_Pause();
    };
    Midi->setMidiMap(Map); 
    if (autochangemap) 
    {
        song_Pause(); 
    };
}; 

void kmidClient::setSLManager(SLManager *slm)
{
    if (slman!=NULL) delete slman;
    slman=slm;
};

void kmidClient::setActiveCollection(int i)
{
    activecollection=i;
    KApplication *kappl;
    kappl=KApplication::getKApplication();
    KConfig *kconf=kappl->getConfig();
    
    kconf->setGroup("KMid"); 
    kconf->writeEntry("ActiveCollection",activecollection);
    currentsl=slman->getCollection(activecollection);
    generateCPL();
    initializing_songs=1;
    fillInComboSongs();
    initializing_songs=0;
};

void kmidClient::fillInComboSongs(void)
{
    //int oldselected=comboSongs->currentItem();
    comboSongs->clear();
    //comboSongs->setCurrentItem(-1);
    if (currentsl==NULL) return;
    currentsl->iteratorStart();
    char temp[300];
    char temp2[300];
    QString qs;
    while (!currentsl->iteratorAtEnd())
    {
	qs=currentsl->getIteratorName();
	KURL::decodeURL(qs);
        sprintf(temp,"%d - %s",currentsl->getIteratorID(),
                extractFilename(qs.data(),temp2));
        comboSongs->insertItem(temp);
        currentsl->iteratorNext();
    };
    if (currentsl->getActiveSongID()==-1) return;
    comboSongs->setCurrentItem(currentsl->getActiveSongID()-1);
    /*
     if (oldselected==currentsl->getActiveSongID()-1)
     {
     selectSong(currentsl->getActiveSongID()-1);
     };
     */
    selectSong(currentsl->getActiveSongID()-1);
};

void kmidClient::selectSong(int i)
{
    if (currentsl==NULL) return;
    i++;
    if ((i<=0))  // The collection may be empty, or it may be just a bug :-)
    {
#ifdef KMidDEBUG
        printf("Empty\n"); 
#endif
        emit song_stopPause();
        if (pctl->playing) song_Stop();
        if (midifile_opened!=NULL) delete midifile_opened;
        midifile_opened=NULL;
        Player->removeSong();
        timebar->setRange(0,240000);
        timebar->setValue(0);
        timetags->repaint(TRUE);
        kdispt->ClearEv();
        kdispt->repaint(TRUE);
        comboSongs->clear();
        comboSongs->repaint(TRUE);
        topLevelWidget()->setCaption("KMid");
        return;
    };
    
    if ((i==currentsl->getActiveSongID())&&(!initializing_songs)) return;
    int play=0;
    if (pctl->playing==1) play=1;
    
    if (pctl->paused) emit song_stopPause();
    if (/*(i!=currentsl->getActiveSongID())&&*/(play==1)) song_Stop();
    currentsl->setActiveSong(i);
    if (openURL(currentsl->getActiveSongName())==-1) return;
    if (play) song_Play();
    
};


int kmidClient::getSelectedSong(void)
{
    if (currentsl==NULL) return -1;
    return currentsl->getActiveSongID();
};


void kmidClient::setSongLoop(int i)
{
    loopsong=i;
};


void kmidClient::generateCPL(void)
{
    if (collectionplaylist!=NULL) delete collectionplaylist;
    collectionplaylist=NULL;
    
    if (currentsl==NULL) return;
    
    if (collectionplaymode==0)
        collectionplaylist=generate_list(currentsl->NumberOfSongs());
    else
        collectionplaylist=generate_random_list(currentsl->NumberOfSongs());
};


void kmidClient::setCollectionPlayMode(int i)
{
    collectionplaymode=i;
    generateCPL();
};

void kmidClient::saveCollections(void)
{
    if (slman==NULL) return;
#ifdef KMidDEBUG
    printf("Saving collections in : %s\n",(const char *)collectionsfile);
#endif
    slman->saveConfig((const char *)collectionsfile);
};

void kmidClient::saveLyrics(FILE *fh)
{
    if (kdispt!=NULL) kdispt->saveLyrics(fh);
};

int kmidClient::searchInCPL(int song)
{
    if (currentsl==NULL) return -1;
    int i=0;
    int n=currentsl->NumberOfSongs();
    while ((i<n)&&(collectionplaylist[i]!=song)) i++;
    if (i<n) return i;
    return -1;
};

void kmidClient::visibleVolumeBar(int i)
{
    visiblevolumebar=i;
    if (visiblevolumebar)
        volumebar->show();
    else
        volumebar->hide();
    resizeEvent(NULL);
};

void kmidClient::visibleChannelView(int )
{
    if (channelView==NULL)
    {
        channelView=new ChannelView();
        if (noteArray!=NULL)
        {
            int pgm[16],j;
            noteArray->moveIteratorTo((ulong)pctl->millisecsPlayed,pgm);
            for (j=0;j<16;j++)
            {
                if (!pctl->forcepgm[j]) channelView->changeInstrument(j,(pctl->gm==1)?(pgm[j]):(MT32toGM[pgm[j]]));
                else channelView->changeInstrument(j,(pctl->pgm[j]));
                channelView->changeForceState(j,pctl->forcepgm[j]);
            };
        };
        channelView->show();
        connect(channelView,SIGNAL(signalToKMidClient(int *)),this,SLOT(communicationFromChannelView(int *)));
	connect(kapp,SIGNAL(shutDown()),parentWidget(),SLOT(shuttingDown()));

    }
    else
    {
        delete channelView;
        channelView=NULL;
        
    };
    rethinkNextEvent();
};

void kmidClient::channelViewDestroyed()
{
    channelView=NULL;
    rethinkNextEvent();
};


void kmidClient::rethinkNextEvent(void)
{
    if (pctl->playing==0) return;
    timer4events->stop();
    
    int type;
    ulong delaymillisec;
    ulong x=timeOfNextEvent(&type);
    
    if (type==0) return;
    
    timeval tv;
    ulong currentmillisec;
    gettimeofday(&tv, NULL);
    currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
    delaymillisec=x-(currentmillisec-beginmillisec);
    
    timer4events->start(delaymillisec,TRUE);
};

void kmidClient::communicationFromChannelView(int *i)
{
    if (i==NULL) return;
    int autocontplaying=0;
    if ((i[0]==CHN_CHANGE_PGM)||((i[0]==CHN_CHANGE_FORCED_STATE)&&(i[3]==1)))
    {
        if ((pctl->playing==1)&&(pctl->paused==0)) autocontplaying=1;
        
        if (autocontplaying)
        {
            song_Pause();
        };
    };
    if (i[0]==CHN_CHANGE_PGM)
        pctl->pgm[i[1]-1]=i[2];
    else if (i[0]==CHN_CHANGE_FORCED_STATE)
        pctl->forcepgm[i[1]-1]=i[2];
    if ((i[0]==CHN_CHANGE_PGM)||((i[0]==CHN_CHANGE_FORCED_STATE)&&(i[3]==1)))
    {
        if (autocontplaying)
        {
            song_Pause();
        };
    };
    
};

void kmidClient::changeTempo(double value)
{
    if (!Player->isSongLoaded())
    {
        tempoLCD->display(120);
        currentTempo=120;
        tempoLCD->setDefaultValue(120);
        return;
    };

#ifdef KMidDEBUG
    printf("Change tempo to %g\n",value);
#endif
    int autocontplaying=0;

    if ((pctl->playing==1)&&(pctl->paused==0)) autocontplaying=1;
    

    if (autocontplaying)
    {
        song_Pause();
    }

//    double ratio=(tempoToMetronomeTempo(pctl->tempo)*pctl->ratioTempo)/(value);
//    double ratio=(tempoLCD->getOldValue()*pctl->ratioTempo)/(value);
    double ratio=(currentTempo*pctl->ratioTempo)/value;

    char s[20];
    sprintf(s,"%g",ratio);
    if (strcmp(s,"1")!=0) tempoLCD->setLCDColor (255,100,100);
    else tempoLCD->setLCDColor (100,255,100);
#ifdef KMidDEBUG
    printf("ratio : (%.9g = %g ) tempo now : %g , new tempo %g\n",ratio,ratio,tempoToMetronomeTempo(pctl->tempo),value);
    printf("OldValue : %g , value %g\n",tempoLCD->getOldValue(),value);
#endif
    
    if (pctl->paused==1)
    {
        pausedatmillisec=(long)(((double)pausedatmillisec/pctl->ratioTempo)*ratio);
#ifdef KMidDEBUG
        printf("pausedat : %ld\n",pausedatmillisec);
#endif
    }
    Player->changeTempoRatio(ratio);

    timebar->setRange(0,(int)(Player->Info()->millisecsTotal));
    timebar->setValue(pausedatmillisec);
    timetags->repaint(TRUE);

    kdispt->ClearEv(false);

    noteArray=Player->getNoteArray();
    spev=Player->takeSpecialEvents();
    currentTempo=value;
    
    while (spev!=NULL)
    {
        if ((spev->type==1) || (spev->type==5)) 
        {
            kdispt->AddEv(spev);
        };
        spev=spev->next;
    };
    
    kdispt->calculatePositions();
    kdispt->CursorToHome();
    if (pctl->paused==1)
        moveEventPointersTo(pausedatmillisec);
    
    if (autocontplaying)
    {
        song_Pause();
    };
    
};
