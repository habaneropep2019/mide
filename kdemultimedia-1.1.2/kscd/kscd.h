/*
   Kscd - A simple cd player for the KDE Project

   $Id$

   Copyright (c) 1997 Bernd Johannes Wuebben math.cornell.edu

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



#ifndef __KSCD__
#define __KSCD__

#include "bwlednum.h"

#include <qpushbt.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qapp.h>
#include <qtimer.h>
#include <qbitmap.h>
#include <qcombo.h>
#include <qmsgbox.h>
#include <qscrbar.h>
#include <qslider.h>
#include <qtabdlg.h>
#include <qtooltip.h>
#include <qpopmenu.h>

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

/* this is for glibc 2.x which the ust structure in ustat.h not stat.h */
#ifdef __GLIBC__
#include <sys/ustat.h>
#endif

#ifdef __FreeBSD__
#include <sys/param.h>
#include <sys/ucred.h>
#include <sys/mount.h>
#endif

#ifdef linux
#include <mntent.h>
#define KSCDMAGIC 1
#endif

#include "cddb.h"
#include "smtpconfig.h"
#include "smtp.h"
#include "ledlamp.h"
#include "CDDialog.h"
#include "CDDBSetup.h"
#include <kapp.h>
#include <kprocess.h>

struct configstruct{

  QColor led_color;
  QColor background_color;
  bool   tooltips;
  QString cd_device;
  QString mailcmd;
  QString browsercmd;
  bool	use_kfm;
  bool	docking;
  bool  autoplay;
  bool autodock;
  bool stopexit;
  bool ejectonfinish;
};

struct mgconfigstruct{

  int width;
  int height;
  int brightness;
};


class KSCD : public QWidget {

	Q_OBJECT

      // time display modes
      enum { TRACK_SEC = 0, TRACK_REM = 1, TOTAL_SEC = 2, TOTAL_REM = 3};

public slots:

    void smtpMessageSent(void);
    void smtpError(int);
    void magicdone(KProcess*);
    void magicslot(int);
    void togglequeryled();
    void cddb_done();
    void cddb_timed_out();
    void cddb_ready();
    void cddb_failed();
    void setToolTips();
    void randomSelected();
    void readSettings();
    void writeSettings();
    void setColors();
    void playClicked();
    void initCDROM();
    void stopClicked();
    void prevClicked();
    void nextClicked();
    void fwdClicked();
    void bwdClicked();
    void quitClicked();
    void loopClicked();
    void cdMode();
    void ejectClicked();
    void trackSelected( int );
    void aboutClicked();
    void volChanged( int );
    void mycddb_inexact_read();
    void cddb_no_info();
    void led_on();
    void led_off();
    void titlelabeltimeout();
    void CDDialogSelected();
    void cycleplaytimemode();
    void cycletimeout();
    void get_cddb_info(bool);
    void CDDialogDone();
    void getCDDBservers();
    void getCDDBserversDone();
    void getCDDBserversFailed();
    void updateCurrentCDDBServer();
    void performances(int);
    void purchases(int);
    void information(int);
    void showPopup();
    void dockClicked();

    void doSM();

private:
    CDDBSetup       *setup;

    SMTPConfig      *smtpconfig;
    SMTPConfig::SMTPConfigData  smtpConfigData;

    CDDialog        *cddialog;
    QPushButton	    *playPB;
    QPushButton	    *stopPB;
    QPushButton	    *prevPB;
    QPushButton	    *nextPB;
    QPushButton	    *fwdPB;
    QPushButton	    *bwdPB;
    QPushButton	    *dockPB;
    QPushButton	    *replayPB;
    QPushButton	    *ejectPB;
    QPushButton	    *aboutPB;
    QPushButton	    *infoPB;
    QPopupMenu	    *mainPopup;
    QPopupMenu	    *perfPopup;
    QPopupMenu	    *purchPopup;
    QPopupMenu	    *infoPopup;

    QColor 		background_color;
    QColor 		led_color;
    BW_LED_Number	*trackTimeLED[6];
    QLabel 		*statuslabel;
    QLabel 		*titlelabel;
    QLabel 		*artistlabel;
    QLabel 		*volumelabel;
    QLabel 		*tracklabel;
    QLabel 		*totaltimelabel;
    QLabel		*nLEDs;
    QPushButton 	*optionsbutton;
    QPushButton 	*shufflebutton;
    QPushButton 	*cddbbutton;
    QPushButton	*volLA;
    QTimer		*timer;
    QTimer		*titlelabeltimer;
    QTimer		*queryledtimer;
    QTimer		*initimer;
    QTimer		*cycletimer;
    QComboBox	*songListCB;
    QSlider		*volSB;

    KProcess*             magicproc;
    int		volChnl;
    int		mixerFd;
    int 	        volume;
    int magic_width;
    int magic_height;
    int magic_brightness;
    QFrame 		*backdrop;
    LedLamp        *queryled;
    KConfig 	*config;
    bool 		tooltips;
    bool 		randomplay ;
    bool 		looping;
    bool		cycle_flag;
    QString		cycle_str;
    bool 		volstartup;
    bool 		cddrive_is_ok;
    int 		time_display_mode;
    QString cd_device_str;

    QPushButton     *makeButton( int, int, int, int, const char * );

    void		initWorkMan();
    //	void		checkMount();

    void		drawPanel();
    void		cleanUp();
    void		loadBitmaps();
    void 	 	setLEDs(QString symbols);
    int 	        randomtrack();

//TODO get rid of the mixe stuff

    void		initMixer( const char *mixer = "/dev/mixer" );

// These are the variables from workbone

	int 		sss;
	int 		sel_stat;
	int 		dly;
	int 		fastin;
	int 		scmd;
	int 		tmppos;
	int 		save_track;
	struct timeval 	mydelay;
	struct mntent 	*mnt;
	FILE 		*fp;

	CDDB 		cddb;
	QStrList        querylist;
	QStrList        pathlist;
	QStrList        discidlist;
	QStrList	tracktitlelist;
	QStrList	playlist;
	int		playlistpointer;
	QStrList	extlist;
	int		revision;
	QString	        category;
	QString	        xmcd_data;
	QString         cddbbasedir;
	QStrList	cddbserverlist;
	QString		current_server;
	QString	        mailcmd;
        QString  	submitaddress;
        QStrList cddbsubmitlist;
	QString  	browsercmd;
	bool		cddb_remote_enabled;
	bool 		use_kfm;
        bool            docking;
        bool            autoplay;
        bool autodock;
        bool stopexit;
        bool ejectonfinish;

	bool 		cddb_inexact_sentinel;
        bool            updateDialog;
        bool ejectedBefore;
        bool currentlyejected;
public:

	KSCD( QWidget *parent = 0, const char *name = 0 );


protected:

    void closeEvent( QCloseEvent *e );
    bool event( QEvent *e );
    //    void focusOutEvent(QFocusEvent *e);
    void playtime();
    void setupPopups();
    void startBrowser(char* querystring);
    bool getArtist(QString& artist);
    void get_pathlist(QStrList& _patlist);

};



#endif

