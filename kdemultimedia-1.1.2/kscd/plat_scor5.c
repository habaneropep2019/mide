/*   
   Kscd - A simple cd player for the KDE Project

   Copyright (c) 1997 Bernd Johannes Wuebben math.cornell.edu
   Modified for SCO by Dag Nygren, dag@newtech.fi

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
 * SCO Openserver R5 specific.
 */



#include "config.h"

#if defined(M_UNIX) 


#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/scsi.h>
#include <sys/scsicmd.h>
#include <errno.h>
#include <macros.h>

#include "struct.h"

extern int wm_scsi2_get_trackinfo(struct wm_drive *d, int track, int *data, 
int *startframe);
extern int wm_scsi2_get_trackinfocddb(struct wm_drive *d, int track, int *min, 
int *sec, int *frm);
extern int wm_scsi2_eject(struct wm_drive *d);
extern int wm_scsi2_pause(struct wm_drive *d);
extern int wm_scsi2_resume(struct wm_drive *d);
extern int wm_scsi2_play(struct wm_drive *d, int sframe, int eframe);
extern int wm_scsi2_stop(struct wm_drive *d);
extern int wm_scsi_get_drive_type(struct wm_drive *d, unsigned char *vendor, 
unsigned char *model, unsigned char *rev);
extern int wm_scsi2_get_trackcount(struct wm_drive *d, int *tracks);
extern int wm_scsi2_get_cdlen(struct wm_drive *d, int *frames);
extern int wm_scsi2_get_drive_status(struct wm_drive *d, enum cd_modes oldmod, 
enum cd_modes *mode, int *pos, int *track, int *index);
extern int wm_scsi2_set_volume(struct wm_drive *d, int left, int right);
extern int wm_scsi2_get_volume(struct wm_drive *d, int *left, int *right);

#define SENSE_SZ EXTD_SENSE_LEN

void *malloc();
char *strchr();

int	min_volume = 0;
int	max_volume = 255;

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
	return (wm_scsi2_get_trackcount(d, tracks));
}


int
gen_get_trackinfocddb(d, track, min, sec, frm)
	struct wm_drive	*d;
	int    track,*min,*sec,*frm;
{

	return (wm_scsi2_get_trackinfocddb(d, track, min,sec,frm));

}

/*
 * Get the start time and mode (data or audio) of a track.
 */
int
gen_get_trackinfo(d, track, data, startframe)
	struct wm_drive	*d;
	int		track, *data, *startframe;
{
	return (wm_scsi2_get_trackinfo(d, track, data, startframe));
}

/*
 * Get the number of frames on the CD.
 */
int
gen_get_cdlen(d, frames)
	struct wm_drive	*d;
	int		*frames;
{
	return (wm_scsi2_get_cdlen(d, frames));
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
	return (wm_scsi2_get_drive_status(d, oldmode, mode, pos, track, index));
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
	return (wm_scsi2_set_volume(d, left, right));
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
	return (wm_scsi2_get_volume(d, left, right));
}

/*
 * Pause the CD.
 */
int
gen_pause(d)
	struct wm_drive	*d;
{
	return (wm_scsi2_pause(d));
}

/*
 * Resume playing the CD (assuming it was paused.)
 */
int
gen_resume(d)
	struct wm_drive	*d;
{
	return (wm_scsi2_resume(d));
}

/*
 * Stop the CD.
 */
int
gen_stop(d)
	struct wm_drive	*d;
{
	return (wm_scsi2_stop(d));
}

/*
 * Play the CD from one position to another (both in frames.)
 */
int
gen_play(d, start, end)
	struct wm_drive	*d;
	int		start, end;
{
	return (wm_scsi2_play(d, start, end));
}

/*
 * Eject the current CD, if there is one.
 */
int
gen_eject(d)
	struct wm_drive	*d;
{
	int stat;

	stat = wm_scsi2_eject(d);
	sleep(2);
	return (stat);
}

static int
create_cdrom_node(char *dev_name)
{
	char pass_through[100];
	int file_des;
	struct stat sbuf;
	int err;
	int ccode;


	strcpy(pass_through, dev_name);
	strcat(pass_through, "p" );

	if (setreuid(-1,0) < 0)
	{
		perror("setregid/setreuid/access");
		exit(1);
	}

	ccode = access(pass_through, F_OK);
	
	if (ccode < 0)
	{

		if (stat(dev_name, &sbuf) < 0)
		{
			perror("Call to get pass-through device number failed");
			return -1;
		}

		if (mknod(pass_through, (S_IFCHR | S_IREAD | S_IWRITE),
			sbuf.st_rdev) < 0)
		{
			perror("Unable to make pass-through node");
			return -1;
		}

        	if (chown(pass_through, 0 , 0) < 0)
		{
			perror("chown");
			return -1;
		}

		if (chmod(pass_through, 0660 ) < 0)
		{
			perror("chmod");
			return -1;
		}
	}
	
	file_des = open( pass_through, O_RDONLY);
	err = errno;

/*	if ( (setreuid(-1,getuid()) < 0) || (setregid(-1,getgid()) < 0) )
	{
		perror("setreuid/setregid");
		exit(1);
	}
*/
	errno = err;
	return file_des;
}

/*
 * Open the CD and figure out which kind of drive is attached.
 */
int
wmcd_open(d)
	struct wm_drive	*d;
{
	int		fd;
	static int	warned = 0;
	char		vendor[9], model[17], rev[5];

	if (d->fd >= 0)		/* Device already open? */
		return (0);
	
	if (cd_device == NULL){
	  fprintf(stderr,"cd_device string empty\n");
	  return (-1);
	}


	d->fd = create_cdrom_node(cd_device); /* this will do open */

	if (d->fd < 0)
	{
		if (errno == EACCES)
		{
			if (! warned)
			{
				fprintf(stderr,"Cannot access %s\n",cd_device);
				warned++;
			}
		}
		else if (errno != EINTR)
		{
			perror(cd_device);
		}

		/* can not acces CDROM device */
		return (-1);
	}

	if (warned)
	{
		warned = 0;
		fprintf(stderr, "Thank you.\n");
	}

	/* Now fill in the relevant parts of the wm_drive structure. */

	fd = d->fd;

	if (wm_scsi_get_drive_type(d, vendor, model, rev) < 0)
	{
		perror("Cannot inquiry drive for it's type");
		return (-1);
	}
	*d = *(find_drive_struct(vendor, model, rev));
	/*	about_set_drivetype(d->vendor, d->model, rev);*/

	d->fd = fd;

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
	int ccode;
	int file_des = d->fd;
	unsigned char sense_buffer[ SENSE_SZ ];

	/* getreply == 1 is read, == 0 is write */
	
	struct scsicmd2 sb;	/* Use command with automatic sense */

	if (cdblen > SCSICMDLEN)
	{
		fprintf(stderr,"Cannot handle longer commands than %d bytes.\n", SCSICMDLEN);
		exit(-1);
	}

	/* Get the command */
	memcpy(sb.cmd.cdb, xcdb, cdblen);
	sb.cmd.cdb_len = cdblen;

	/* Point to data buffer */
	sb.cmd.data_ptr = retbuf;
	sb.cmd.data_len = retbuflen;

	/* Is this write or read ? */
	sb.cmd.is_write = (getreply==1) ? 0 : 1;

	/* Zero out return status fields */
	sb.cmd.host_sts = 0;
	sb.cmd.target_sts = 0;

	/* Set up for possible sense info */

	sb.sense_ptr = sense_buffer;
	sb.sense_len = sizeof(sense_buffer);

	ccode =  ioctl(file_des, SCSIUSERCMD2,  &sb);

	if ( sb.cmd.target_sts != 02 )
		return ccode;

	return 0;
}

#endif /* M_UNIX */
