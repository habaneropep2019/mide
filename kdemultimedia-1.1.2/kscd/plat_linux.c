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
 * $Id$
 *
 * Linux-specific drive control routines.  Very similar to the Sun module.
 */

#ifdef linux

#include <unistd.h>
#include <sys/ioctl.h>    
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>

/* this is for glibc 2.x which defines ust structure in ustat.h not stat.h */
#ifdef __GLIBC__
#include <sys/ustat.h>
#endif

#include <linux/cdrom.h>
#include "struct.h"

#include "config.h"

int	min_volume = MIN_VOLUME; 
int	max_volume = MAX_VOLUME;


#ifdef LINUX_SCSI_PASSTHROUGH
# define SCSI_IOCTL_SEND_COMMAND 1
#endif


#define max(a,b) ((a) > (b) ? (a) : (b))


void *malloc();

extern char	*cd_device;

/*
 * Initialize the drive.  A no-op for the generic driver.
 */
int
gen_init(d)
	struct wm_drive	*d;
{
	return (0);
}

/*
 * Get the number of tracks on the CD.
 */
int
gen_get_trackcount(d, tracks)
	struct wm_drive	*d;
	int		*tracks;
{
	struct cdrom_tochdr	hdr;

	if (ioctl(d->fd, CDROMREADTOCHDR, &hdr))
		return (-1);
	
	*tracks = hdr.cdth_trk1;
	return (0);
}

int
gen_get_trackinfocddb(d, track, min, sec, frm)
	struct wm_drive	*d;
	int    *min,*sec,*frm;
{

	struct cdrom_tocentry	entry;

/* printf("IN get_get_trackinfocddb\n"); */

	entry.cdte_track = track;
	entry.cdte_format = CDROM_MSF;

	if (ioctl(d->fd, CDROMREADTOCENTRY, &entry))
		return (-1);
	
	*min =	entry.cdte_addr.msf.minute;
	*sec =  entry.cdte_addr.msf.second;
	*frm =  entry.cdte_addr.msf.frame;
	
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
	struct cdrom_tocentry	entry;

	entry.cdte_track = track;
	entry.cdte_format = CDROM_MSF;

	if (ioctl(d->fd, CDROMREADTOCENTRY, &entry))
		return (-1);
	
	*startframe =	entry.cdte_addr.msf.minute * 60 * 75 +
			entry.cdte_addr.msf.second * 75 +
			entry.cdte_addr.msf.frame;
	*data = entry.cdte_ctrl & CDROM_DATA_TRACK ? 1 : 0;
	
	return (0);
}

/*
 * Get the number of frames on the CD.
 */
int
gen_get_cdlen(d, frames)
	struct wm_drive	*d;
	int		*frames;
{
	int		tmp;

	return (gen_get_trackinfo(d, CDROM_LEADOUT, &tmp, frames));
}


/* Alarm signal handler. */
/*static void do_nothing(x) int x; { x++; }*/

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
	struct cdrom_subchnl		sc;

#ifdef SBPCD_HACK
	static int prevpos = 0;
#endif
	
	/* If we can't get status, the CD is ejected, so default to that. */
	/*	*mode = EJECTED;*/
	*mode = UNKNOWN;

	/* Is the device open? */
	if (d->fd < 0)
	{
#ifdef SBPCD_HACK
		switch (wmcd_open(d,0)) {
#else
		switch (wmcd_open(d)) {
#endif
		case -1:	/* error */
			return (-1);

		case 1:		/* retry */
			return (0);
		}
	}

	sc.cdsc_format = CDROM_MSF;

	if (ioctl(d->fd, CDROMSUBCHNL, &sc))
		return (0);

	switch (sc.cdsc_audiostatus) {
	case CDROM_AUDIO_PLAY:
		*mode = PLAYING;
		*track = sc.cdsc_trk;
		*index = sc.cdsc_ind;
		*pos = sc.cdsc_absaddr.msf.minute * 60 * 75 +
			sc.cdsc_absaddr.msf.second * 75 +
			sc.cdsc_absaddr.msf.frame;
#ifdef SBPCD_HACK
		if( *pos < prevpos )
                  {
                    if( (prevpos - *pos) < 75 )
                    {
                      *mode = TRACK_DONE;
                    }
                  }
                  
                prevpos = *pos;
#endif
		break;

	case CDROM_AUDIO_PAUSED:
	case CDROM_AUDIO_NO_STATUS:
	case CDROM_AUDIO_INVALID: /**/
		if (oldmode == PLAYING || oldmode == PAUSED)
		{
			*mode = PAUSED;
			*track = sc.cdsc_trk;
			*index = sc.cdsc_ind;
			*pos = sc.cdsc_absaddr.msf.minute * 60 * 75 +
				sc.cdsc_absaddr.msf.second * 75 +
				sc.cdsc_absaddr.msf.frame;
		}
		else
			*mode = STOPPED;
		break;

	case CDROM_AUDIO_COMPLETED:
		*mode = TRACK_DONE; /* waiting for next track. */
		break;

	default:
		*mode = UNKNOWN;
		break;
	}

	return (0);
}

/*
 * scale_volume(vol, max)
 *
 * Return a volume value suitable for passing to the CD-ROM drive.  "vol"
 * is a volume slider setting; "max" is the slider's maximum value.
 *
 * On Sun and DEC CD-ROM drives, the amount of sound coming out the jack
 * increases much faster toward the top end of the volume scale than it
 * does at the bottom.  To make up for this, we make the volume scale look
 * sort of logarithmic (actually an upside-down inverse square curve) so
 * that the volume value passed to the drive changes less and less as you
 * approach the maximum slider setting.  The actual formula looks like
 *
 *     (max^2 - (max - vol)^2) * (max_volume - min_volume)
 * v = --------------------------------------------------- + min_volume
 *                           max^2
 *
 * If your system's volume settings aren't broken in this way, something
 * like the following should work:
 *
 *	return ((vol * (max_volume - min_volume)) / max + min_volume);
 */



#ifdef CURVED_VOLUME
static int
scale_volume(vol, max)
	int	vol, max;
{
	return ((max * max - (max - vol) * (max - vol)) *
		(max_volume - min_volume) / (max * max) + min_volume);
}

#else /* LINEAR VOLUME */
static int
scale_volume(vol, max)
	int	vol, max;
{
	return ((vol * (max_volume - min_volume)) / max + min_volume);
}
#endif

/*
 * Set the volume level for the left and right channels.  Their values
 * range from 0 to 100.
 */
int
gen_set_volume(d, left, right)
	struct wm_drive	*d;
	int		left, right;
{
	struct cdrom_volctrl v;

/* Adjust the volume to make up for the CD-ROM drive's weirdness. */
	left = scale_volume(left, 100);
	right = scale_volume(right, 100);

	v.channel0 = v.channel2 = left < 0 ? 0 : left > 255 ? 255 : left;
	v.channel1 = v.channel3 = right < 0 ? 0 : right > 255 ? 255 : right;

	return (ioctl(d->fd, CDROMVOLCTRL, &v));
}

/*
 * Pause the CD.
 */
int
gen_pause(d)
	struct wm_drive	*d;
{
#ifdef DEBUG
printf("CDROMPAUSE\n");
#endif
	return (ioctl(d->fd, CDROMPAUSE));
}

/*
 * Resume playing the CD (assuming it was paused.)
 */
int
gen_resume(d)
	struct wm_drive	*d;
{
#ifdef DEBUG
printf("CDROMRESUME\n");
#endif
	return (ioctl(d->fd, CDROMRESUME));
}

/*
 * Stop the CD.
 */
int
gen_stop(d)
	struct wm_drive *d;
{
#ifdef DEBUG
printf("CDROMRESTOP\n");
#endif
	return (ioctl(d->fd, CDROMSTOP));
}

/*
 * Play the CD from one position to another (both in frames.)
 */
int
gen_play(d, start, end)
	struct wm_drive	*d;
	int		start, end;
{
	struct cdrom_msf		msf;

	msf.cdmsf_min0 = start / (60*75);
	msf.cdmsf_sec0 = (start % (60*75)) / 75;
	msf.cdmsf_frame0 = start % 75;
	msf.cdmsf_min1 = end / (60*75);
	msf.cdmsf_sec1 = (end % (60*75)) / 75;
	msf.cdmsf_frame1 = end % 75;

#ifdef WORKMAN_ORIGINAL
#ifdef DEBUG
printf("CDROMSTART\n");
#endif
	if (ioctl(d->fd, CDROMSTART))
		return (-1);
#endif

#ifdef DEBUG
printf("CDROMPLAYMSF %d %d %d %d %d %d\n",
       msf.cdmsf_min0,
       msf.cdmsf_sec0,
       msf.cdmsf_frame0,
       msf.cdmsf_min1,
       msf.cdmsf_sec1,
       msf.cdmsf_frame1
       );
#endif

	if (ioctl(d->fd, CDROMPLAYMSF, &msf))
		return (-2);
	
	return (0);
}

/*
 * Eject the current CD, if there is one.
 */
int
gen_eject(d)
	struct wm_drive	*d;
{
	struct stat	stbuf;
	struct ustat	ust;

	if (fstat(d->fd, &stbuf) != 0)
		return (-2);

	/* TODO replace this with statfs, Bernd */

	/* Is this a mounted filesystem? */
	if (ustat(stbuf.st_rdev, &ust) == 0)
		return (-3);

	  if (ioctl(d->fd, CDROMEJECT))
	    return (-1);
	  
	  return (0);
}

/*
 * Keep the CD open all the time.
 */
void
keep_cd_open()
{
	int	fd;
	struct flock	fl;
	extern	end;

	for (fd = 0; fd < 256; fd++)
		close(fd);

	if (fork())
		exit(0);

	if ((fd = open("/tmp/cd.lock", O_RDWR | O_CREAT, 0666)) < 0)
		exit(0);
	fl.l_type = F_WRLCK;
	fl.l_whence = 0;
	fl.l_start = 0;
	fl.l_len = 0;
	if (fcntl(fd, F_SETLK, &fl) < 0)
		exit(0);

	if (open(cd_device, 0) >= 0)
	{
		brk(&end);
		pause();
	}

	exit(0);
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
	/* Suns, HPs, Linux, NEWS can't read the volume; oh well */
	*left = *right = -1;

	return (0);
}

/*
 * Send an arbitrary SCSI command to a device.
 */
int
wm_scsi(d, cdb, cdblen, retbuf, retbuflen, getreply)
	struct wm_drive	*d;
	unsigned char	*cdb;
	int		cdblen;
	void		*retbuf;
	int		retbuflen;
	int		getreply;
{
#ifdef LINUX_SCSI_PASSTHROUGH

        char *cmd;
	int cmdsize;

	cmdsize = 2 * sizeof(int);
	if (retbuf)
	  {
	    if (getreply) cmdsize += max(cdblen, retbuflen);
	    else cmdsize += (cdblen + retbuflen);
	  }
	else cmdsize += cdblen;
	  
	cmd = malloc(cmdsize);
	if (cmd == NULL)
	  return (-1);

	((int*)cmd)[0] = cdblen + ((retbuf && !getreply) ? retbuflen : 0);
	((int*)cmd)[1] = ((retbuf && getreply) ? retbuflen : 0);

	memcpy(cmd + 2*sizeof(int), cdb, cdblen);
	if (retbuf && !getreply)
	  memcpy(cmd + 2*sizeof(int) + cdblen, retbuf, retbuflen);
	
	if (ioctl(d->fd, SCSI_IOCTL_SEND_COMMAND, cmd))
	  {
	    free(cmd);
	    return (-1);
	  }
	
	if (retbuf && getreply)
	  memcpy(retbuf, cmd + 2*sizeof(int), retbuflen);

	free(cmd);
	return 0;

#else
	return (-1);
#endif
}

/*
 * Open the CD device and figure out what kind of drive is attached.
 * new parameter "i" introduced for Linux platform with some special drivers
 * inserted by milliByte@DeathsDoor.com 1997-06-10
 */
int
#ifdef SBPCD_HACK
wmcd_open(d, i)
#else
wmcd_open(d)
#endif
	struct wm_drive	*d;
#ifdef SBPCD_HACK
	int i;
#endif
{
	int		fd;
	static int	warned = 0;

#ifdef LINUX_SCSI_PASSTHROUGH
	char vendor[9], model[17], rev[5];
#endif

	if (cd_device == NULL){
	  fprintf(stderr,"cd_device string empty\n");
	  return (-1);
	}


	if (d->fd >= 0)		/* Device already open? */
#ifdef SBPCD_HACK
             if( i == 1)     /* ... but re-open it, please */
                close( d->fd );
             else
#endif
		return (0);
	
	d->fd = open(cd_device, O_RDONLY | O_NONBLOCK);
	if (d->fd < 0)
	{
		if (errno == EACCES)
		{
			if (!warned)
			{
				fprintf(stderr,
		"As root, please run\n\nchmod 666 %s\n\n%s\n", cd_device,
		"to give yourself permission to access the CD-ROM device.");
				warned++;
			}
		}
		else if (errno != ENXIO)
		{
			perror(cd_device);
		}

		/* CD device couldn't be opened */
		return (-1);
	}

	if (warned)
	{
		warned = 0;
		fprintf(stderr, "Thank you.\n");
	}

	/* Now fill in the relevant parts of the wm_drive structure. */
	fd = d->fd;

#ifdef LINUX_SCSI_PASSTHROUGH
  
	/* Can we figure out the drive type? */
	vendor[0] = model[0] = rev[0] = '\0';
	wm_scsi_get_drive_type(d, vendor, model, rev);

	*d = *(find_drive_struct(vendor, model, rev));
	/*	about_set_drivetype(d->vendor, d->model, rev);*/

#else
 
	*d = *(find_drive_struct("", "", ""));
 
#endif

	d->fd = fd;

	(d->init)(d);

	return (0);
}

#endif /* linux */
