/*   
   kmidi - a midi to wav converter
   
   $Id$
 
   Copyright 1997 Bernd Johannes Wuebben math.cornell.edu
  
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

#ifndef KMIDI_PLAYER_H
#define KMIDI_PLAYER_H

#include "version.h"
#include "bwlednum.h"

#include <qfileinf.h>
#include <qdstream.h> 
#include <qfile.h> 
#include <qtabdlg.h> 
#include <qfiledlg.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qapp.h>
#include <qpopmenu.h>
#include <qtimer.h>
#include <qbitmap.h>
#include <qslider.h>
#include <qgrpbox.h>
#include <qcombo.h>
#include <qmsgbox.h>
#include <qscrbar.h>
#include <qtooltip.h>
#include <qregexp.h>
#include <qgrpbox.h>

#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <kapp.h>
#include <kmsgbox.h>

#define PLAYLIST_WIDTH  550
#define PLAYLIST_HEIGHT 440



#include "configdlg.h"
#include "log.h"

class PlaylistDialog;



class KMidi : public QDialog {

	Q_OBJECT
public:

	~KMidi();
	
	// Buttons

	QPushButton	*playPB;
	QPushButton	*stopPB;
	QPushButton	*prevPB;
	QPushButton	*nextPB;
	QPushButton	*fwdPB;
	QPushButton	*bwdPB;
	QPushButton	*quitPB;
	QPushButton	*replayPB;
	QPushButton	*ejectPB;
	QPushButton	*aboutPB;
	QPushButton 	*shufflebutton;
	QPushButton 	*configurebutton;
	QPushButton 	*lowerBar;
	QTabDialog      *tabdialog;
	QPushButton 	*infobutton;
	BW_LED_Number	*trackTimeLED[9];
    
	QPixmap folder_pixmap;
	QPixmap file_pixmap;
	QPixmap cdup_pixmap;

	QTimer*  readtimer;
	

	QLabel	*statusLA;
	QLabel	*volLA;
	QLabel  *titleLA;
	QLabel  *propertiesLA;
	QLabel  *properties2LA;
	QLabel  *modlabel;
	QLabel  *totaltimelabel;
	QLabel  *looplabel;
	QLabel  *speedLA;
	QLabel  *song_count_label;

	QTimer		*timer;

	QComboBox	*songListCB;

	QSlider		*volSB;
	QWidget 	*backdrop;
	QPushButton     *makeButton( int, int, int, int, const char * );

	int		mixerFd;
        bool             StopRequested;
	bool 		loop;
	int 		init_volume;
	int 		Child;
	bool 		driver_error;
	bool 		tooltips;
	bool 		starting_up;
	bool 		looping;
	int		volume;
	int 		max_sec;
	int 		song_number;
	int 		last_sec;

	bool		randomplay;
	QColor		background_color;
	QColor		led_color;
	bool 		playing;
	bool 		blink;

	QString		fileName;
	QStrList	*playlist;

	LogWindow 	*logwindow;
	ConfigDlg       *configdlg;
	PlaylistDialog  *playlistdlg;
	KConfig 	*config;

protected:
	void closeEvent( QCloseEvent *e );  

private:
	QString		getHomeDir();
	void		makedirs();
	void 		display_playmode();
	int		randomSong();
	void 		resetPos();
	void 		setLEDs(QString );
	void		drawPanel();
	void		cleanUp();
	void		loadBitmaps();
	void		initMixer( const char *mixer = "/dev/mixer" );
	void 		playtime();
	void 	        playthemod(QString );


public:

	KMidi( QWidget *parent = 0, const char *name = 0 );


signals:	
	void		play();

public slots:

	void 		loadplaylist();
	void 		randomPlay();
	void 		updateUI();
        void 		randomClicked();
	void 		ReadPipe();
	void 		setToolTips();
	void 		PlayCommandlineMods();
	void            PlayMOD();
	void 		readconfig();
        void 		writeconfig();
	void 		setColors();
	void		playClicked();
	void		stopClicked();
	void 		slowdownslot();
	void 		speedupslot();
	void		prevClicked();
	void		nextClicked();
	void		fwdClicked();
	void		bwdClicked();
	void		quitClicked();
	void		replayClicked();
	void 		cdMode();
	void		ejectClicked();
	void		aboutClicked();
	void		volChanged( int );
};



#endif 
