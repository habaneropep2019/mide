/*

   kmidi - a midi to wav converter

   $Id$

   Copyright 1997, 1998 Bernd Johannes Wuebben math.cornell.edu

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

#include "kmidi.h"

#include "bitmaps/playpaus.xbm"
#include "bitmaps/stop.xbm"
#include "bitmaps/repeat.xbm"
#include "bitmaps/nexttrk.xbm"
#include "bitmaps/prevtrk.xbm"
#include "bitmaps/ff.xbm"
#include "bitmaps/rew.xbm"
#include "bitmaps/poweroff.xbm"
#include "bitmaps/eject.xbm"
#include "bitmaps/midilogo.xbm"
#include "bitmaps/shuffle.xbm"
#include "bitmaps/options.xbm"
#include "bitmaps/info.xbm"

#include "filepic.h"
#include "cduppic.h"
#include "folderpic.h"

/*#include <qsocknot.h> */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "config.h"
#include "playlist.h"
#include "constants.h"
#include "output.h"

extern "C" {

int pipe_read_ready();
void pipe_int_write(int c);
void pipe_int_read(int *c);
void pipe_string_read(char *str);
void pipe_string_write(char *str);
}

extern PlayMode *play_mode;
extern int have_commandline_midis;
extern int output_device_open;
extern int32 control_ratio;

enum midistatus{ KNONE, KPLAYING, KSTOPPED, KLOOPING, KFORWARD,
		 KBACKWARD, KNEXT, KPREVIOUS,KPAUSED};

midistatus status;

KApplication * thisapp;
KMidi *kmidi;
int pipenumber;

KMidi::KMidi( QWidget *parent, const char *name ) :
    QDialog( parent, name )
{

    playlistdlg = NULL;
    randomplay = false;
    looping = false;
    driver_error = false;
    status = KNONE;
    song_number = 1;
    starting_up = true;

    makedirs();
    readconfig();
    drawPanel();
    loadBitmaps();
    setColors();
    setToolTips();

    playlist = new QStrList(TRUE);

    if ( !folder_pixmap.loadFromData(folder_bmp_data, folder_bmp_len) ) {
	QMessageBox::message( i18n("Error"), i18n("Could not load folder.bmp") );
    }


    if ( !cdup_pixmap.loadFromData(cdup_bmp_data, cdup_bmp_len) ) {
	QMessageBox::message( i18n("Error"), i18n("Could not load cdup.bmp") );
    }

    if ( !file_pixmap.loadFromData(file_bmp_data, file_bmp_len) ) {
	QMessageBox::message( i18n("Error"), i18n("Could not load file.bmp") );
    }

    timer = new QTimer( this );
    readtimer = new QTimer( this);

    logwindow = new LogWindow(NULL,"logwindow");
    logwindow->hide();

    connect( timer, SIGNAL(timeout()),this, SLOT(PlayCommandlineMods()) );
    timer->start( 100, FALSE );

    connect( playPB, SIGNAL(clicked()), SLOT(playClicked()) );
    connect( stopPB, SIGNAL(clicked()), SLOT(stopClicked()) );
    connect( prevPB, SIGNAL(clicked()), SLOT(prevClicked()) );
    connect( nextPB, SIGNAL(clicked()), SLOT(nextClicked()) );
    connect( fwdPB, SIGNAL(clicked()), SLOT(fwdClicked()) );
    connect( bwdPB, SIGNAL(clicked()), SLOT(bwdClicked()) );
    connect( quitPB, SIGNAL(clicked()), SLOT(quitClicked()) );	
    connect( replayPB, SIGNAL(clicked()), SLOT(replayClicked()) );
    connect( ejectPB, SIGNAL(clicked()), SLOT(ejectClicked()) );
    connect( volSB, SIGNAL(valueChanged(int)), SLOT(volChanged(int)));
    connect( aboutPB, SIGNAL(clicked()), SLOT(aboutClicked()));
    connect( configurebutton, SIGNAL(clicked()), SLOT(aboutClicked()));
    connect( shufflebutton, SIGNAL(clicked()), SLOT(randomClicked()));
    connect( infobutton, SIGNAL(clicked()), SLOT(speedupslot()));

    connect(thisapp,SIGNAL(kdisplayPaletteChanged()),this,SLOT(setColors()));
    connect(this,SIGNAL(play()),this,SLOT(playClicked()));

    srandom(time(0L));
    adjustSize();


    setFixedSize(this->width(),this->height());


}


KMidi::~KMidi(){

}

void KMidi::setToolTips()
{
    if(tooltips){
	QToolTip::add( playPB, 		i18n("Play/Pause") );
	QToolTip::add( stopPB, 		i18n("Stop") );
	QToolTip::add( replayPB, 	i18n("Loop Song") );
	//	QToolTip::add( songListCB, 	i18n("Track Selection") );
	QToolTip::add( fwdPB, 		i18n("15 Secs Forward") );
	QToolTip::add( bwdPB, 		i18n("15 Secs Backward") );
	QToolTip::add( nextPB, 		i18n("Next Midi") );
	QToolTip::add( prevPB, 		i18n("Previous Midi") );
	QToolTip::add( quitPB, 		i18n("Exit Kmidi") );
	QToolTip::add( shufflebutton, 	i18n("Random Play") );
        QToolTip::add( configurebutton, i18n("Configure Kmidi") );
	QToolTip::add( ejectPB, 	i18n("Open Playlist") );
	QToolTip::add( infobutton, 	i18n("Show Info Window") );
	QToolTip::add( shufflebutton, 	i18n("Random Play") );
	QToolTip::add( volSB, 		i18n("Volume Control") );
    }
    else{
	QToolTip::remove( playPB);
	QToolTip::remove( stopPB);
	QToolTip::remove( replayPB);
	//	QToolTip::remove( songListCB);
	QToolTip::remove( fwdPB );
	QToolTip::remove( bwdPB);
	QToolTip::remove( nextPB );
	QToolTip::remove( prevPB );
	QToolTip::remove( quitPB );
	QToolTip::remove( configurebutton );
	QToolTip::remove( shufflebutton );
	QToolTip::remove( ejectPB );
	QToolTip::remove( shufflebutton );
	QToolTip::remove( infobutton );
	QToolTip::remove( volSB );
    }

}


void KMidi::volChanged( int vol )
{       	
	
    QString volumetext;
    if (vol == 100){
	volumetext.sprintf(i18n("Vol:%03d%%"),vol);
    }
    else{
	volumetext.sprintf(i18n("Vol:%02d%%"),vol);
    }
    volLA->setText( volumetext );

    pipe_int_write(  MOTIF_CHANGE_VOLUME);
    pipe_int_write(vol*255/100);
    //  mp_volume = vol*255/100;

}



QPushButton *KMidi::makeButton( int x, int y, int w, int h, const char *n )
{
    QPushButton *pb = new QPushButton( n, this );
    pb->setGeometry( x, y, w, h );
    return pb;
}
	
void KMidi::drawPanel()
{
    int ix = 0;
    int iy = 0;
    //    int WIDTH = 100;
    int WIDTH = 90;
    //    int HEIGHT = 30;
    int HEIGHT = 27;
    //    int SBARWIDTH = 230;
    int SBARWIDTH = 220; //140

    setCaption( i18n("kmidi") );
    aboutPB = makeButton( ix, iy, WIDTH, 2 * HEIGHT, "About" );

    ix = 0;
    iy += 2 * HEIGHT;
    ejectPB = makeButton( ix, iy, WIDTH/2, HEIGHT, "Eject" );
    infobutton = makeButton( ix +WIDTH/2, iy, WIDTH/2, HEIGHT, "info" );

    iy += HEIGHT;
    quitPB = makeButton( ix, iy, WIDTH, HEIGHT, "Quit" );

    ix += WIDTH;
    iy = 0;

    //	backdrop = new MyBackDrop(this,"mybackdrop");
    backdrop = new QWidget(this,"mybackdrop");
    backdrop->setGeometry(ix,iy,SBARWIDTH, 3* HEIGHT - 1);

    int D = 10;
    ix = WIDTH + 4;

    for (int u = 0; u<=4;u++){
	trackTimeLED[u] = new BW_LED_Number(this );	
	trackTimeLED[u]->setGeometry( ix  + u*18, iy + D, 23 ,  30 );
    }
	
    QString zeros("--:--");
    setLEDs(zeros);

    ix = WIDTH;
    iy += 2 * HEIGHT;
    statusLA = new QLabel( this );
    statusLA->setGeometry( WIDTH -25 +2*SBARWIDTH/3, 6, 44, 15 );
    statusLA->setFont( QFont( "Helvetica", 10, QFont::Bold ) );
    statusLA->setAlignment( AlignLeft );
    statusLA->setText("    ");


	
    looplabel = new QLabel( this );
    looplabel->setGeometry( WIDTH -25 +2*SBARWIDTH/3 +45, 6, 60, 13 );
    looplabel->setFont( QFont( "Helvetica", 10, QFont::Bold ) );
    looplabel->setAlignment( AlignLeft );
    looplabel->setText("");





    properties2LA = new QLabel( this );

    properties2LA->setGeometry( WIDTH -25 + 2*SBARWIDTH/3 , HEIGHT + 20, 100, 13 );
    properties2LA->setFont( QFont( "Helvetica", 10, QFont::Bold ) );
    properties2LA->setAlignment( AlignLeft );
    properties2LA->setText("");

    propertiesLA = new QLabel( this );

    propertiesLA->setGeometry( WIDTH -25 + 2*SBARWIDTH/3 ,  33, 100, 13 );
    propertiesLA->setFont( QFont( "Helvetica", 10, QFont::Bold ) );
    propertiesLA->setAlignment( AlignLeft );
    propertiesLA->setText("");

    ix += SBARWIDTH/2;

    shufflebutton = makeButton( WIDTH + 2*SBARWIDTH/3 ,
				iy+ HEIGHT,SBARWIDTH/6 , HEIGHT, "F" );

    configurebutton = makeButton( WIDTH + 5*SBARWIDTH/6 ,
				  iy+HEIGHT, SBARWIDTH/6 , HEIGHT, "S" );


    lowerBar = new QPushButton(this);
    lowerBar->setGeometry( WIDTH ,
			   7*HEIGHT/2, 2*SBARWIDTH/3 -1, HEIGHT/2 +1 );

    ix = WIDTH ;
    iy += HEIGHT;

    volLA = new QLabel( this );
    volLA->setAlignment( AlignLeft );
    volLA->setGeometry( WIDTH -25 + 2*SBARWIDTH/3 , 20, 60, 13 );

    volLA->setFont( QFont( "helvetica", 10, QFont::Bold) );


    QString volumetext;
    volumetext.sprintf(i18n("Vol:%02d%%"),volume);
    volLA->setText( volumetext );


    /*speedLA = new QLabel( this );
      speedLA->setAlignment( AlignLeft );
    speedLA->setGeometry( WIDTH -25 + 2*SBARWIDTH/3 +45, 24, 80, 13 );
    speedLA->setFont( QFont( "helvetica", 10, QFont::Bold) );
    */

    /*    QString speedtext;
    speedtext.sprintf(i18n("Spd:%3.0f%%"),100.0);
    speedLA->setText( speedtext );
    */

    modlabel = new QLabel( this );
    modlabel->setAlignment( AlignLeft );
    modlabel->setGeometry( WIDTH + 9, HEIGHT + 24 + 10, SBARWIDTH - 26, 12 );
    modlabel->setFont( QFont( "helvetica", 10, QFont::Bold) );
    modlabel->setText( "" );

    totaltimelabel = new QLabel( this );
    totaltimelabel->setAlignment( AlignLeft );
    totaltimelabel->setGeometry( WIDTH + 80, HEIGHT + 20, 30, 14 );
    totaltimelabel->setFont( QFont( "helvetica", 10, QFont::Bold) );
    totaltimelabel->setText( "--:--" );


    song_count_label = new QLabel( this );
    song_count_label->setAlignment( AlignLeft );
    song_count_label->setGeometry( WIDTH + 9, HEIGHT + 20, 60, 15 );
    song_count_label->setFont( QFont( "helvetica", 10, QFont::Bold) );

    song_count_label->setText( i18n("Song --/--") );


    iy += HEIGHT / 2 ;
	
    volSB = new QSlider( 0, 100, 5,  volume, QSlider::Horizontal,
			 this, "Slider" );
    volSB->setGeometry( WIDTH , 3*HEIGHT, 2*SBARWIDTH/3, HEIGHT /2 );

    iy = 0;
    ix = WIDTH + SBARWIDTH;

    playPB = makeButton( ix, iy, WIDTH, HEIGHT, i18n("Play/Pause") );

    iy += HEIGHT;
    stopPB = makeButton( ix, iy, WIDTH/2 , HEIGHT, i18n("Stop") );

    ix += WIDTH/2 ;
    replayPB = makeButton( ix, iy, WIDTH/2 , HEIGHT, i18n("Replay") );

    ix = WIDTH + SBARWIDTH;
    iy += HEIGHT;
    bwdPB = makeButton( ix, iy, WIDTH/2 , HEIGHT, i18n("Bwd") );

    ix += WIDTH/2;
    fwdPB = makeButton( ix, iy, WIDTH/2 , HEIGHT, i18n("Fwd") );

    ix = WIDTH + SBARWIDTH;
    iy += HEIGHT;
    prevPB = makeButton( ix, iy, WIDTH/2 , HEIGHT, i18n("Prev") );

    ix += WIDTH/2 ;
    nextPB = makeButton( ix, iy, WIDTH/2 , HEIGHT, i18n("Next") );

}


void KMidi::loadBitmaps() {

    QBitmap playBmp( playpause_width, playpause_height, playpause_bits,
		     TRUE );
    QBitmap stopBmp( stop_width, stop_height, stop_bits, TRUE );
    QBitmap prevBmp( prevtrk_width, prevtrk_height, prevtrk_bits, TRUE );
    QBitmap nextBmp( nexttrk_width, nexttrk_height, nexttrk_bits, TRUE );
    QBitmap replayBmp( repeat_width, repeat_height, repeat_bits, TRUE );
    QBitmap fwdBmp( ff_width, ff_height, ff_bits, TRUE );
    QBitmap bwdBmp( rew_width, rew_height, rew_bits, TRUE );
    QBitmap ejectBmp( eject_width, eject_height, eject_bits, TRUE );
    QBitmap quitBmp( poweroff_width, poweroff_height, poweroff_bits,
		     TRUE );
    QBitmap aboutBmp( midilogo_width, midilogo_height, midilogo_bits, TRUE );

    QBitmap shuffleBmp( shuffle_width, shuffle_height, shuffle_bits, TRUE );
    QBitmap optionsBmp( options_width, options_height, options_bits, TRUE );
    QBitmap infoBmp( info_width, info_height, info_bits, TRUE );

    playPB->setPixmap( playBmp );
    infobutton->setPixmap( infoBmp );
    shufflebutton->setPixmap( shuffleBmp );
    configurebutton->setPixmap( optionsBmp );
    stopPB->setPixmap( stopBmp );
    prevPB->setPixmap( prevBmp );
    nextPB->setPixmap( nextBmp );
    replayPB->setPixmap( replayBmp );
    fwdPB->setPixmap( fwdBmp );
    bwdPB->setPixmap( bwdBmp );
    ejectPB->setPixmap( ejectBmp );
    quitPB->setPixmap( quitBmp );
    aboutPB->setPixmap( aboutBmp );
}


void KMidi::setLEDs(QString symbols){

    for(int i=0;i<=4;i++){
	trackTimeLED[i]->display(symbols.data()[i]);
    }

}


QString KMidi::getHomeDir() {

  struct passwd *pwd;
  pwd = getpwuid(getuid());

  if(pwd == NULL)
    return QString("/");

  QString s = pwd->pw_dir;
  if(s.right(1) != "/")
    s += "/";
  return s;

}

void KMidi::makedirs(){

  QDir dir;
  QString d;

  d = getHomeDir();

  d += "/.kde";
  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

  d += "/share";
  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

  d += "/apps";
  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

  d += "/kmidi" ;

  dir.setPath(d.data());
  if(!dir.exists()){
    dir.mkdir(d.data());
    chown(d.data(),getuid(),getgid());
    chmod(d.data(),S_IRUSR | S_IWUSR | S_IXUSR);
  }

}
int KMidi::randomSong(){

    int j;
    j=1+(int) (((double)playlist->count()) *rand()/(RAND_MAX+1.0));
    return j;
}


void KMidi::randomClicked(){
    if(playlist->count() == 0)
        return;

    looping = FALSE;
    if(randomplay == TRUE){
	randomplay = FALSE;
	looplabel->setText("");
	return;
    }

    randomplay = TRUE;
    updateUI();
    setLEDs("OO:OO");
    statusLA->setText(i18n("Playing"));
    looplabel->setText(i18n("Random"));

    int index = randomSong() - 1;
    song_number = index + 1;
    pipe_int_write(MOTIF_PLAY_FILE);
    pipe_string_write(playlist->at(index));

    status = KPLAYING;

}

void KMidi::randomPlay(){

    randomplay = TRUE;
    updateUI();
    setLEDs("OO:OO");
    statusLA->setText(i18n("Playing"));
    looplabel->setText(i18n("Random"));

    int index = randomSong() - 1;
    song_number = index + 1;
    pipe_int_write(MOTIF_PLAY_FILE);
    pipe_string_write(playlist->at(index));

    status = KPLAYING;

}

void KMidi::playClicked()
{

  if(!output_device_open){
    return;
  }

  int index;
  index = song_number -1;

  if(status == KPLAYING){
    status = KPAUSED;
    pipe_int_write(MOTIF_PAUSE);
    statusLA->setText(i18n("Paused"));
    return;
  }

  if(status == KPAUSED){
    status = KPLAYING;
    pipe_int_write(MOTIF_PAUSE);
    statusLA->setText(i18n("Playing"));
    return;
  }

  if(((int)playlist->count()  > index) && (index >= 0)){
    setLEDs("OO:OO");
    statusLA->setText(i18n("Playing"));

    pipe_int_write(MOTIF_PLAY_FILE);
    pipe_string_write(playlist->at(index));

    status = KPLAYING;
  }

}


void KMidi::stopClicked()
{
     looping = false;
     randomplay = false;
     pipe_int_write(MOTIF_PAUSE);
     status = KSTOPPED;
     statusLA->setText(i18n("Ready"));
     looplabel->setText("");
     setLEDs("00:00");
}



void KMidi::prevClicked(){

    song_number --;

    if (song_number < 1)
      song_number = playlist->count();

    if(status == KPLAYING)
      setLEDs("OO:OO");

    statusLA->setText(i18n("Playing"));

    pipe_int_write(MOTIF_PLAY_FILE);
    pipe_string_write(playlist->at(song_number-1));
    status = KPLAYING;
}

void KMidi::slowdownslot(){



}

void KMidi::speedupslot(){

    if(!logwindow)
	return;

    if(logwindow->isVisible())
	logwindow->hide();
    else
	logwindow->show();

}

void KMidi::nextClicked(){


    if(playlist->count() == 0)
	return;
    song_number = (randomplay) ? randomSong() : song_number + 1;
    if(song_number > (int)playlist->count())
      song_number = 1;

    if(status == KPLAYING)
      setLEDs("OO:OO");

    statusLA->setText(i18n("Playing"));

    pipe_int_write(MOTIF_PLAY_FILE);
    pipe_string_write(playlist->at(song_number-1));
    status = KPLAYING;
}

void KMidi::fwdClicked(){

    pipe_int_write(MOTIF_CHANGE_LOCATOR);
    pipe_int_write((last_sec + 15) * 100);

}

void KMidi::bwdClicked(){

    if( (last_sec -15 ) >= 0){
	pipe_int_write(MOTIF_CHANGE_LOCATOR);
	pipe_int_write((last_sec -15) * 100);
    }
    else{
	pipe_int_write(MOTIF_CHANGE_LOCATOR);
	pipe_int_write(0);

    }
}

void KMidi::quitClicked(){

    setLEDs("--:--");
    statusLA->setText("");

    writeconfig();
    pipe_int_write(MOTIF_QUIT);

    if(output_device_open == 0){
      thisapp->processEvents();
      thisapp->flushX();
      usleep(100000);
      thisapp->quit();
    }

}

void KMidi::replayClicked(){

    if(status != KPLAYING)
	return;

    if(looping == false){
	looping = true;
	randomplay = false;
	looplabel->setText(i18n("Looping"));
    }
    else{
	looping = false;
	looplabel->setText("");
    }

}

void KMidi::ejectClicked(){

    /*  if(driver_error){ // we couldn't initialize the driver
	return;         // this happens e.g. when I still have NAS running
	}

	if(playing)
	return;*/

    if(!playlistdlg)
	playlistdlg = new PlaylistDialog( NULL , "Compose Play List",playlist);

    if(playlistdlg->exec()){

      updateUI();
      status = KSTOPPED;
      timer->start( 200, TRUE );  // single shot
    }


}

void KMidi::PlayMOD(){

    // this method is called "from" ejectClicked once the timer has fired
    // this is done to allow the user interface to settle a bit
    // before playing the mod

    if (!playlist->isEmpty())
	playClicked();


}



void KMidi::aboutClicked()
{

  QTabDialog * tabdialog;

  tabdialog = new QTabDialog(0,"tabdialog",TRUE);
  tabdialog->setCaption( i18n("kmidi Configuraton") );
  tabdialog->resize( 350, 350 );
  tabdialog->setCancelButton( i18n("Cancel") );

  QWidget *about = new QWidget(tabdialog,"about");

  QGroupBox *box = new QGroupBox(about,"box");

  QLabel  *label = new QLabel(box,"label");
  box->setGeometry(10,10,320,260);

  box->setTitle(i18n("About"));


  label->setGeometry(140,40,160,170);

  QString labelstring;
  labelstring.sprintf(i18n("KMidi %s\n"\
		   "Copyright (c) 1997-98\nBernd Johannes Wuebben\n"\
		   "wuebben@kde.org\n\n"\
		   "KMidi contains code from:\n"
		   "TiMidity version 0.2.i\n"
		   "Copyright (c) \nTuukka Toivonen\n"\
		   "toivonen@clinet.fi\n"), KMIDIVERSION);

  label->setAlignment(AlignLeft|WordBreak|ExpandTabs);
  label->setText(labelstring.data());

  QString pixdir = thisapp->kde_datadir() + "/kmidi/pics/";


  QPixmap pm((pixdir + "kmidilogo.xpm").data());
  QLabel *logo = new QLabel(box);
  logo->setPixmap(pm);
  logo->setGeometry(30, 50, pm.width(), pm.height());

  ConfigDlg* dlg;
  struct configstruct config;
  config.background_color = background_color;
  config.led_color = led_color;
  config.tooltips = tooltips;

  dlg = new ConfigDlg(tabdialog,&config,"configdialg");

  tabdialog->addTab(dlg,i18n("Configure"));
  tabdialog->addTab(about,i18n("About"));

  if(tabdialog->exec() == QDialog::Accepted){

    background_color = dlg->getData()->background_color;
    led_color = dlg->getData()->led_color;
    tooltips = dlg->getData()->tooltips;
    setColors();
    setToolTips();
  }

  delete dlg;
  delete about;
  delete tabdialog;
}


void KMidi::PlayCommandlineMods(){

    // If all is O.K this method is called precisely once right after
    // start up when the timer fires. We check whether we have mods
    // to play that were specified on the command line.
    // if  there was a driver error we well not stop the timer
    // and the timer will continue to call this routine. We check each
    // time whether the driver is finally ready. This is handy for people
    // like me who usually use NAS but forgot to turn it off.


  timer->changeInterval(1000);

  connect(readtimer, SIGNAL(timeout()),this,SLOT(ReadPipe()));
  readtimer->start(10);

  if(!output_device_open){

    // we couldn't initialize the driver
    // this happens e.g. when I still have NAS running
    // let's blink a bit.

    pipe_int_write(TRY_OPEN_DEVICE);

    if (blink){
      blink = false;
      statusLA->setText("           ");
      modlabel->setText(""); // clear the error message
	}
    else{
      blink = true;
      statusLA->setText(i18n("ERROR"));
      modlabel->setText(i18n("Can't open Output Device."));
      song_count_label->setText( i18n("Song --/--") );
    }


    return;
  }

  modlabel->setText(""); // clear the error message
  song_count_label->setText( i18n("Song --/--") );

  // O.K all clear -- the driver is ready.

  timer->stop();

  disconnect( timer, SIGNAL(timeout()),this, SLOT(PlayCommandlineMods()) );
  connect( timer, SIGNAL(timeout()),this, SLOT(PlayMOD()) );
  thisapp->processEvents();
  thisapp->flushX();

  display_playmode();

  thisapp->processEvents();
  thisapp->flushX();


  statusLA->setText(i18n("Ready"));

  //  readtimer->start(10);

}

void KMidi::loadplaylist(){


    QString home = QDir::homeDirPath();
    home = home + "/.kde/share/apps/kmidi";

    QDir savedir(home.data());

    if(!savedir.exists()){
	savedir.mkdir(home.data());
    }

    QString defaultlist;
    defaultlist = home + "/" + "default";

    QFile f(defaultlist.data());

    f.open( IO_ReadWrite | IO_Translate);

    char buffer[1024];
    QString tempstring;

    while(!f.atEnd()){
      QFileInfo file;

      buffer[0] = (char) 0;
      f.readLine(buffer,511);

      tempstring = buffer;
      tempstring = tempstring.stripWhiteSpace();

      if (!tempstring.isEmpty()){
	file.setFile(tempstring.data());
	if (file.isReadable())
	  playlist->append(tempstring.data());
	}	
    }

    f.close();

}

void KMidi::ReadPipe(){

    int message;

    if(pipe_read_ready()){

	pipe_int_read(&message);

	switch (message)
	    {
	    case REFRESH_MESSAGE : {
	      /*		printf("REFRESH MESSAGE IS OBSOLETE !!!\n");*/
	    }
	    break;

	    case DEVICE_NOT_OPEN:
	      output_device_open = 0;
	      break;

	    case DEVICE_OPEN:

	      output_device_open = 1;
	      if(have_commandline_midis && output_device_open)
		emit play();
	      break;

	    case TOTALTIME_MESSAGE : {
		int cseconds;
		int minutes,seconds;
		char local_string[20];

		pipe_int_read(&cseconds);
	
		seconds=cseconds/100;
		minutes=seconds/60;
		seconds-=minutes*60;
		sprintf(local_string,"%02i:%02i",minutes,seconds);
		totaltimelabel->setText(local_string);
		/*		printf("GUI: TOTALTIME %s\n",local_string);*/
		max_sec=cseconds;
	    }
	    break;
	
	    case MASTERVOL_MESSAGE: {
		int volume;
	
		pipe_int_read(&volume);

	    }
	    break;
	
	    case FILENAME_MESSAGE : {
		char filename[255];
		char *pc;

		pipe_string_read(filename);
		/*		printf("RECEIVED: FILENAME_MESSAGE:%s\n",filename);*/
		/* Extract basename of the file */

		pc=strrchr(filename,'/');
		if (pc==NULL)
		    pc=filename;
		else
		    pc++;
	
		fileName = pc;
		updateUI();
	    }
	    break;
	
	    case FILE_LIST_MESSAGE : {
		char filename[255];
		int i, number_of_files;
		/*		printf("RECEIVED: FILE_LIST_MESSAGE\n");*/
	
		/* reset the playing list : play from the start */
		song_number = 1;

		pipe_int_read(&number_of_files);

		if(number_of_files > 0 )
		    playlist->clear();	

		
		QFileInfo file;
		have_commandline_midis = false;

		for (i=0;i<number_of_files;i++)
		  {
		    pipe_string_read(filename);
		    file.setFile(filename);
		    if( file.isReadable()){
		      playlist->append(filename);
		      have_commandline_midis = true;
		    }
		    else{
		      QString string;
		      string.sprintf(i18n("%s\nis not readable or doesn't exist."),filename);
		      KMsgBox::message(NULL, i18n("KMidi Warning"), string.data(),
				       KMsgBox::EXCLAMATION);

		    }
		  }	
		
		if( !have_commandline_midis) {
		  /*don't have cmd line midis to play*/
		  /*or none of them was readable */
		    loadplaylist();
		}


		if(playlist->count() > 0){
	
		    fileName = playlist->at(0);
		    int index = fileName.findRev('/',-1,TRUE);
		    if(index != -1)
			fileName = fileName.right(fileName.length() -index -1);
		    if(output_device_open)
		      updateUI();

		}
		if(have_commandline_midis && output_device_open)
		  emit play();

	    }
	    break;
	
	    case NEXT_FILE_MESSAGE :
	    case PREV_FILE_MESSAGE :
	    case TUNE_END_MESSAGE :{

	      /*		printf("RECEIVED: NEXT/PREV/TUNE_MESSAGE\n");*/
	
		/* When a file ends, launch next if auto_next toggle */
		/*	    if ((message==TUNE_END_MESSAGE) &&
			    !XmToggleButtonGetState(auto_next_option))
			    return;
			    */

		if(starting_up){
		    starting_up = false;
		    return;
		}

	

		setLEDs("OO:OO");
		if(randomplay){
		    randomPlay();
		    return;
		}

		if(looping){
		    status = KNEXT;
		    playClicked();
		    return;
		}

		/*
		if (message==PREV_FILE_MESSAGE)
		    song_number--;
		else {
		    song_number++;
		}
		*/

		if (song_number < 1)
		    {
			song_number = 1;
		    }
	
	
		if (song_number > (int)playlist->count() )
		    {
			song_number = 1 ;
			setLEDs("--:--");
			looplabel->setText("");
			statusLA->setText(i18n("Ready"));
			QString str;
			str.sprintf(i18n("Song: --/%02d"),playlist->count());
			song_count_label->setText(str.data() );
			modlabel->setText("");
			totaltimelabel->setText("--:--");
			status = KSTOPPED;
			return;
		    }
		else{
		    status = KPLAYING;
		    nextClicked();
		}
	

	    }
	    break;

	    case CURTIME_MESSAGE : {
		int cseconds;
		int  sec,seconds, minutes;
		int nbvoice;
		char local_string[20];

		/*		printf("RECEIVED: CURTIME_MESSAGE\n");*/
		pipe_int_read(&cseconds);
		pipe_int_read(&nbvoice);
	
		sec=seconds=cseconds/100;
	
		/* To avoid blinking */
		if (sec!=last_sec)
		    {
			minutes=seconds/60;
			seconds-=minutes*60;
		
			sprintf(local_string,"%02d:%02d",
				minutes, seconds);
			//		    printf("GUI CURTIME %s\n",local_string);	
			setLEDs(local_string);
	
		    }

		last_sec=sec;

	    }
	    break;
	    /*	
	    case NOTE_MESSAGE : {
		int channel;
		int note;
	
		pipe_int_read(&channel);
		pipe_int_read(&note);
		printf("NOTE chn%i %i\n",channel,note);
	    }
	    break;
	
	    case    PROGRAM_MESSAGE :{
		int channel;
		int pgm;
	
		pipe_int_read(&channel);
		pipe_int_read(&pgm);
		printf("NOTE chn%i %i\n",channel,pgm);
	    }
	    break;
	
	    case VOLUME_MESSAGE : {
		int channel;
		int volume;
	
		pipe_int_read(&channel);
		pipe_int_read(&volume);
		printf("VOLUME= chn%i %i \n",channel, volume);
	    }
	    break;
	
	
	    case EXPRESSION_MESSAGE : {
		int channel;
		int express;
	
		pipe_int_read(&channel);
		pipe_int_read(&express);
		printf("EXPRESSION= chn%i %i \n",channel, express);
	    }
	    break;
	
	    case PANNING_MESSAGE : {
		int channel;
		int pan;
	
		pipe_int_read(&channel);
		pipe_int_read(&pan);
		printf("PANNING= chn%i %i \n",channel, pan);
	    }
	    break;
	
	    case  SUSTAIN_MESSAGE : {
		int channel;
		int sust;
	
		pipe_int_read(&channel);
		pipe_int_read(&sust);
		printf("SUSTAIN= chn%i %i \n",channel, sust);
	    }
	    break;
	
	    case  PITCH_MESSAGE : {
		int channel;
		int bend;
	
		pipe_int_read(&channel);
		pipe_int_read(&bend);
		printf("PITCH BEND= chn%i %i \n",channel, bend);
	    }
	    break;
	
	    case RESET_MESSAGE : {
		printf("RESET_MESSAGE\n");
	    }
	    break;
	    */

	    case CLOSE_MESSAGE : {
		//	    printf("CLOSE_MESSAGE\n");
		writeconfig();
		thisapp->quit();
	    }
	    break;
	
	    case CMSG_MESSAGE : {
		char strmessage[2024];
		int type = 0;

		pipe_int_read(&type);
		/*		printf("got a CMSG_MESSAGE of type %d\n", type);*/
		pipe_string_read(strmessage);
		/*		printf("RECEIVED %s %d\n",strmessage,strlen(strmessage));*/

		logwindow->insertStr(strmessage);

	    }
	    break;

	      	case CMSG_ERROR : {
		char strmessage[2024];
	
		pipe_string_read(strmessage);
		logwindow->insertStr(strmessage);


		}

	    break;
	    default:
		fprintf(stderr,"Kmidi: unknown message %i\n",message);
	
	    }
    }
}


void KMidi::readconfig(){

    // let's set the defaults

    volume  = 40;
    randomplay = false;
    int tooltipsint  = 1;
    /*	int randomplayint = 0;*/

    //////////////////////////////////////////

    QString str;
	
    config = thisapp->getConfig();

    str = config->readEntry("Volume");
    if ( !str.isNull() )
	volume = atoi(str.data());

    str = config->readEntry("ToolTips");
    if ( !str.isNull() )
	tooltipsint =  atoi(str.data());

    if (tooltipsint == 1)
	tooltips = TRUE;
    else
	tooltips = FALSE;

    /*	str = config->readEntry("RandomPlay");
	if ( !str.isNull() )
	randomplayint =  atoi(str.data());

	if (randomplayint == 1)
	randomplay = TRUE;
	else
	randomplay = FALSE;*/

    QColor defaultback = black;
    QColor defaultled = QColor(226,224,255);


    background_color = config->readColorEntry("BackColor",&defaultback);	
    led_color = config->readColorEntry("LEDColor",&defaultled);




}

void KMidi::writeconfig(){

		
    config = thisapp->getConfig();
	
    ///////////////////////////////////////////////////

    if(tooltips)
	config->writeEntry("ToolTips", 1);
    else
	config->writeEntry("ToolTips", 0);

    /*	if(randomplay)
	config->writeEntry("RandomPlay", 1);
	else
	config->writeEntry("RandomPlay", 0);
	*/

    config->writeEntry("Volume", volume);
    config->writeEntry("BackColor",background_color);
    config->writeEntry("LEDColor",led_color);
    config->sync();
}


void KMidi::setColors(){



    backdrop->setBackgroundColor(background_color);

    QColorGroup colgrp( led_color, background_color, yellow,yellow , yellow,
                        led_color, white );

    statusLA->setPalette( QPalette(colgrp,colgrp,colgrp) );
    looplabel->setPalette( QPalette(colgrp,colgrp,colgrp) );
    //    titleLA->setPalette( QPalette(colgrp,colgrp,colgrp) );
    propertiesLA->setPalette( QPalette(colgrp,colgrp,colgrp) );
    properties2LA->setPalette( QPalette(colgrp,colgrp,colgrp) );
    volLA->setPalette( QPalette(colgrp,colgrp,colgrp) );
    //    speedLA->setPalette( QPalette(colgrp,colgrp,colgrp) );
    totaltimelabel->setPalette( QPalette(colgrp,colgrp,colgrp) );
    modlabel->setPalette( QPalette(colgrp,colgrp,colgrp) );
    song_count_label->setPalette( QPalette(colgrp,colgrp,colgrp) );


    for (int u = 0; u<=4;u++){
	trackTimeLED[u]->setLEDoffColor(background_color);
	trackTimeLED[u]->setLEDColor(led_color,background_color);
    }

}

void KMidi::display_playmode(){

  if(!play_mode)
    return;

    QString properties;
    QString properties2;

    properties.sprintf(i18n("%d bit %s %s"),
		       play_mode->encoding & PE_16BIT ? 16:8,
		       play_mode->encoding & PE_SIGNED ? i18n("sig"):i18n("usig"),
		       play_mode->encoding &PE_ULAW ? i18n("uLaw"):i18n("Linear"));

    properties2.sprintf(i18n("%d Hz %s"),
		       play_mode->rate,
		       play_mode->encoding & PE_MONO ? i18n("Mono"):i18n("Stereo"));

    propertiesLA->setText(properties);
    properties2LA->setText(properties2);

}

void KMidi::updateUI(){

  QString filenamestr;
  filenamestr = fileName.data();

  filenamestr = filenamestr.replace(QRegExp("_"), " ");

  if(filenamestr.length() > 4){
  if(filenamestr.right(4) == QString(".mid") ||filenamestr.right(4) == QString(".MID"))
    filenamestr = filenamestr.left(filenamestr.length()-4);
  }


    modlabel->setText(filenamestr.data());
    QString songstr;

    if(playlist->count() >0)
	songstr.sprintf(i18n("Song: %02d/%02d"),song_number,playlist->count());
    else
	songstr = i18n("Song: --/--");

    song_count_label->setText( songstr.data() );

    display_playmode();

}

void KMidi::cdMode(){

}

void KMidi::playtime(){


}



void KMidi::closeEvent( QCloseEvent *e ){

    e->ignore();
    quitClicked();
}

#include "kmidi.moc"

extern "C" {

    void createKApplication(int *argc, char **argv){
	thisapp = new KApplication(*argc, argv, "kmidi");
    }
    
    int Launch_KMidi_Process(int _pipenumber){

	pipenumber = _pipenumber;

	kmidi = new KMidi;
       	thisapp->enableSessionManagement(true);
	thisapp->setWmCommand("kmidi");
	thisapp->setTopWidget(kmidi);
	kmidi->setCaption(kapp->getCaption());
	kmidi->show();
	thisapp->exec();

	return 0;
    }

}


/*
static void cleanup( int sig ){

  catchSignals();

}

void catchSignals()
{
signal(SIGHUP, cleanup);	
signal(SIGINT, cleanup);		
signal(SIGTERM, cleanup);		
//	signal(SIGCHLD, cleanup);

signal(SIGABRT, cleanup);
//	signal(SIGUSR1, un_minimize);
signal(SIGALRM, cleanup);
signal(SIGFPE, cleanup);
signal(SIGILL, cleanup);
//	signal(SIGPIPE, cleanup);
signal(SIGQUIT, cleanup);
//	signal(SIGSEGV, cleanup);

#ifdef SIGBUS
signal(SIGBUS, cleanup);
#endif
#ifdef SIGPOLL
signal(SIGPOLL, cleanup);
#endif
#ifdef SIGSYS
signal(SIGSYS, cleanup);
#endif
#ifdef SIGTRAP
signal(SIGTRAP, cleanup);
#endif
#ifdef SIGVTALRM
signal(SIGVTALRM, cleanup);
#endif
#ifdef SIGXCPU
signal(SIGXCPU, cleanup);
#endif
#ifdef SIGXFSZ
signal(SIGXFSZ, cleanup);
#endif
}

*/
