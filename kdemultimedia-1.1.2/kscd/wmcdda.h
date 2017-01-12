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
 * @(#)wmcdda.h	1.4 12 Sep 1995
 */

/*
 * Information about a particular block of CDDA data.
 */
struct cdda_block {
	unsigned char	status;		/* see below */
	unsigned char	track;
	unsigned char	index;
	unsigned char	minute;
	unsigned char	second;
	unsigned char	frame;

	/* Average volume levels, for level meters */
	unsigned char	lev_chan0;
	unsigned char	lev_chan1;

	/* Current volume setting (0-255) */
	unsigned char	volume;

	/* Current balance setting (0-255, 128 = balanced) */
	unsigned char	balance;
};

/*
 * cdda_block status codes.
 */
#define WMCDDA_ERROR	0	/* Couldn't read CDDA from disc */
#define WMCDDA_OK	1	/* Read this block successfully (raw data) */
#define WMCDDA_PLAYED	2	/* Just played the block in question */
#define WMCDDA_STOPPED	3	/* Block data invalid; we've just stopped */
#define WMCDDA_ACK	4	/* Acknowledgement of command from parent */
#define WMCDDA_DONE	5	/* Chunk of data is done playing */
#define WMCDDA_EJECTED	6	/* Disc ejected or offline */

/*
 * Enable or disable CDDA building depending on platform capabilities, and
 * determine endianness based on architecture.  (Gross!)
 */

#ifdef sun
# ifdef SYSV
#  include <sys/types.h>
#  include <sys/cdio.h>
#  ifndef CDROMCDDA
#   undef BUILD_CDDA
#  endif
#  ifdef i386
#   define LITTLE_ENDIAN
#  else
#   define BIG_ENDIAN
#  endif
# else
#  undef BUILD_CDDA
# endif
#endif
