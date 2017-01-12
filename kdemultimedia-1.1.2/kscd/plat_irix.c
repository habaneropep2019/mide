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

/*
 * @(#)plat_irix.c	1.2	26/9/97
 *
 * IRIX specific.
 * If you want audio to come out of your SGI, define CDDA.
 * You must have a CD drive capable of doing CDDA as well.
 *
 * If you turn off the CDDA define, you can connect your
 * CD headphone to the line-in jack and turn on the rec
 * monitor to listen to the music. You will have to set the
 * input rate to 44100 Hz.  The volume control will work from
 * kscd.
 *
 * Paul Kendall
 * paul@orion.co.nz, or
 * paul@kcbbs.gen.nz
 */


#include "config.h"

#ifdef sgi
/*#define CDDA*/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sigfpe.h>
#include <dmedia/cdaudio.h>
#include <dmedia/audio.h>
#include <errno.h>

#include "struct.h"


void *malloc();
char *strchr();

int	min_volume = 0;
int	max_volume = 255;

extern char	*cd_device;

#ifdef CDDA
static int playing = STOPPED;
static CDPLAYER *icd;
static CDPARSER *icdp;
static CDFRAME cdbuf[12];
static ALport audioport;
static ALconfig aconfig;
static struct itimerval audiotimer = { {0,0}, {0,25000} };
static int cdtrack=0;
static int cdframe=0;
static int cdstopframe=0;

cbprognum(void *arg, CDDATATYPES type, CDPROGNUM* prognum)
{
	cdtrack = prognum->value;
}

cbabstime(void *arg, CDDATATYPES type, struct cdtimecode* atime)
{
	cdframe = CDtctoframe(atime);
	if( cdframe == cdstopframe )
		playing = STOPPED;
}

cbplayaudio(void *arg, CDDATATYPES type, short* audio)
{
	if(playing != PLAYING) return;
	ALwritesamps(audioport, audio, CDDA_NUMSAMPLES);
}

static void alarmsignal()
{
	int n, i;
	if(playing != PLAYING) return;
	if( ALgetfilled(audioport) < CDDA_NUMSAMPLES*8 ) {
		/* Only get more samples and play them if we're getting low
		 * this ensures that the CD stays close to the sound
		 */
		n = CDreadda(icd, cdbuf, 12);
		if( n == 0 ) return;
		for( i=0 ; i<12 ; i++ )
			CDparseframe(icdp, &cdbuf[i]);
	}
	signal(SIGALRM, alarmsignal);
	setitimer(ITIMER_REAL, &audiotimer, NULL);
}
#endif

/*
 * Initialize the drive.  A no-op for the generic driver.
 */
int
gen_init(d)
	struct wm_drive	*d;
{
#ifdef CDDA
	long Param[4];
	/* Set the audio rate to 44100Hz 16bit 2s-comp stereo */
	aconfig = ALnewconfig();
	ALsetwidth(aconfig, AL_SAMPLE_16);
	ALsetsampfmt(aconfig, AL_SAMPFMT_TWOSCOMP);
	ALsetchannels(aconfig, 2);
	Param[0] = AL_OUTPUT_RATE;      Param[1] = AL_RATE_44100;
	Param[2] = AL_CHANNEL_MODE;     Param[3] = AL_STEREO;
	ALsetparams(AL_DEFAULT_DEVICE, Param, 4);
	audioport = ALopenport("KDE KSCD Audio", "w", aconfig);

	/* setup cdparser */
	icdp = CDcreateparser();
	CDaddcallback(icdp, cd_audio, (CDCALLBACKFUNC)cbplayaudio, 0);
	CDaddcallback(icdp, cd_pnum, (CDCALLBACKFUNC)cbprognum, 0);
	CDaddcallback(icdp, cd_atime, (CDCALLBACKFUNC)cbabstime, 0);

	/* Lets handle those floating point exceptions expeditiously. */
	sigfpe_[_UNDERFL].repls = _ZERO;
	handle_sigfpes(_ON, _EN_UNDERFL, NULL, _ABORT_ON_ERROR, NULL);
#endif
	return 0;
}

/*
 * Get the number of tracks on the CD.
 */
int
gen_get_trackcount(d, tracks)
	struct wm_drive	*d;
	int		*tracks;
{
	CDSTATUS s;
	if( CDgetstatus(d->daux, &s)==0 )
		return -1;
	*tracks = s.last;
	return 0;
}


int
gen_get_trackinfocddb(d, track, min, sec, frm)
	struct wm_drive	*d;
	int    *min,*sec,*frm;
{

	
	CDTRACKINFO i;
	int ret = CDgettrackinfo(d->daux, track, &i);
	if( ret == 0 )
		return -1;

	*min =	i.start_min;
	*sec =  i.start_sec;
	*frm =  i.start_frame;
	
	return (0);
}

/*
 * Get the start time and mode (data or audio) of a track.
 */
int
gen_get_trackinfo(d, track, data, startframe)
	struct wm_drive	*d;
	int		track, *data, *startframe;
{
	CDTRACKINFO i;
	int ret = CDgettrackinfo(d->daux, track, &i);
	if( ret == 0 )
		return -1;
	*data = 0;
	*startframe = CDmsftoframe(i.start_min,i.start_sec,i.start_frame);
	return 0;
}

/*
 * Get the number of frames on the CD.
 */
int
gen_get_cdlen(d, frames)
	struct wm_drive	*d;
	int		*frames;
{
	CDSTATUS s;
	if( CDgetstatus(d->daux, &s)==0 )
		return -1;
	*frames = CDmsftoframe(s.total_min,s.total_sec,s.total_frame);
	return 0;
}

/*
 * Get the current status of the drive: the current play mode, the absolute
 * position from start of disc (in frames), and the current track and index
 * numbers if the CD is playing or paused.
 */
int
gen_get_drive_status(d, oldmode, mode, pos, track, index)
	struct wm_drive	*d;
	enum cd_modes	oldmode, *mode;
	int		*pos, *track, *index;
{
#ifdef CDDA
	*mode = playing;
	*track = cdtrack;
	*pos = cdframe;
	*index = 0;
#else
	CDSTATUS s;
	if( CDgetstatus(d->daux, &s)==0 )
		return -1;
	*pos = CDmsftoframe(s.min,s.sec,s.frame);
	*track = s.track;
	*index = 0;
	switch( s.state )
	{
		case CD_READY:	*mode = STOPPED;
						break;
		case CD_STILL:
		case CD_PAUSED: *mode = PAUSED;
						break;
		case CD_PLAYING: *mode = PLAYING;
						break;
		default:		*mode = UNKNOWN;
	}
#endif
	return 0;
}

/*
 * Set the volume level for the left and right channels.  Their values
 * range from 0 to 100.
 */
int
gen_set_volume(d, left, right)
	struct wm_drive	*d;
	int		left, right;
{
	long Param[4];
	Param[0] = AL_LEFT_SPEAKER_GAIN;      Param[1] = left*255/100;
	Param[2] = AL_RIGHT_SPEAKER_GAIN;     Param[3] = right*255/100;
	ALsetparams(AL_DEFAULT_DEVICE, Param, 4);
	return 0;
}

/*
 * Read the initial volume from the drive, if available.  Each channel
 * ranges from 0 to 100, with -1 indicating data not available.
 */
int
gen_get_volume(d, left, right)
	struct wm_drive	*d;
	int		*left, *right;
{
	long Param[4];
	Param[0] = AL_LEFT_SPEAKER_GAIN;      Param[1] = 0;
	Param[2] = AL_RIGHT_SPEAKER_GAIN;     Param[3] = 0;
	ALgetparams(AL_DEFAULT_DEVICE, Param, 4);
	*left = Param[1] * 100 / 255;
	*right = Param[3] * 100 / 255;
	return 0;
}

/*
 * Pause the CD.
 */
int
gen_pause(d)
	struct wm_drive	*d;
{
#ifdef CDDA
	playing = PAUSED;
#else
    CDSTATUS s;
    if( CDgetstatus(d->daux, &s)==0 )
        return -1;
    if(s.state == CD_PLAYING)
	    CDtogglepause(d->daux);
#endif
	return 0;
}

/*
 * Resume playing the CD (assuming it was paused.)
 */
int
gen_resume(d)
	struct wm_drive	*d;
{
#ifdef CDDA
	playing = PLAYING;
	signal(SIGALRM, alarmsignal);
	setitimer(ITIMER_REAL, &audiotimer, NULL);
#else
    CDSTATUS s;
    if( CDgetstatus(d->daux, &s)==0 )
        return -1;
    if(s.state == CD_PAUSED)
	    CDtogglepause(d->daux);
#endif
	return 0;
}

/*
 * Stop the CD.
 */
int
gen_stop(d)
	struct wm_drive	*d;
{
#ifdef CDDA
	playing = STOPPED;
#else
	CDstop(d->daux);
#endif
	return 0;
}

/*
 * Play the CD from one position to another (both in frames.)
 */
int
gen_play(d, start, end)
	struct wm_drive	*d;
	int		start, end;
{
#ifdef CDDA
	int m, s, f;
	CDframetomsf(start, &m, &s, &f);
	CDseek(icd, m, s, f);
	cdstopframe = end;
	playing = PLAYING;
	signal(SIGALRM, alarmsignal);
	setitimer(ITIMER_REAL, &audiotimer, NULL);
#else
	int m, s, f;
	CDframetomsf(start, &m, &s, &f);
	CDplayabs(d->daux, m, s, f, 1);
#endif
	return 0;
}

/*
 * Eject the current CD, if there is one.
 */
int
gen_eject(d)
	struct wm_drive	*d;
{
#ifdef CDDA
	playing = STOPPED;
#endif
	CDeject(d->daux);
	return 0;
}

/*
 * Open the CD and figure out which kind of drive is attached.
 */
int
wmcd_open(d)
	struct wm_drive	*d;
{
	int	fd;
	CDSTATUS s;

	if (d->fd < 0)		/* Device already open? */
	{

	if (cd_device == NULL){
	  fprintf(stderr,"cd_device string empty\n");
	  return (-1);
	}

		d->fd = 1;

		/* Now fill in the relevant parts of the wm_drive structure. */
		fd = d->fd;
		*d = *(find_drive_struct("", "", ""));
		d->fd = fd;
		(d->init)(d);

		d->daux = CDopen(cd_device,"r");
		if (d->daux == 0)
		{
			perror(cd_device);
			exit(1);
		}
#ifdef CDDA
		icd = d->daux;
#endif
	}

	CDgetstatus(d->daux, &s);
	if( s.state==CD_NODISC || s.state==CD_ERROR )
		return 1;

	return (0);
}

void
keep_cd_open() { }

/*
 * Send a SCSI command out the bus.
 */
int 
wm_scsi(d, xcdb, cdblen, retbuf, retbuflen, getreply)
	struct wm_drive *d;
	unsigned char *xcdb;
	int cdblen;
	int getreply;
	char *retbuf;
	int retbuflen;
{
	return -1;
}

#endif

