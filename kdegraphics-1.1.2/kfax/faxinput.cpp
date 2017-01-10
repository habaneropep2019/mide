/* Fax file input processing
   Copyright (C) 1990, 1995  Frank D. Cringle.

This file is part of viewfax - g3/g4 fax processing software.
     
viewfax is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.
     
This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
     
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include "faxexpand.h"
#include <qstring.h>
#include <kapp.h>

void statusbarupdate(char* name,int width,int height,char* res);
extern void kfaxerror(char* title,char* error);

#define	FAXMAGIC	"\000PC Research, Inc\000\000\000\000\000\000"

/* Enter an argument in the linked list of pages */
struct pagenode *
notefile(char *name, int type)
{
    struct pagenode *newnode = (struct pagenode *) xmalloc(sizeof *newnode);

    *newnode = defaultpage;
    if (firstpage == NULL){
	firstpage = newnode;
	auxpage = firstpage;
    }
    newnode->prev = lastpage;
    newnode->next = NULL;
    if (lastpage != NULL)
	lastpage->next = newnode;
    lastpage = newnode;

    /*printf("Adding new node%ld\n",newnode);*/

    newnode->pathname = (char*) malloc (strlen(name) +1);
    if(!newnode->pathname){
      kfaxerror("Sorry","Out of memory\n");
      exit(1);
    }

    strcpy(newnode->pathname,name);

    newnode->type = type;


    if ((newnode->name = strrchr(newnode->pathname, '/')) != NULL)
	newnode->name++;
    else
	newnode->name = newnode->pathname;

    if (newnode->width == 0)
	newnode->width = 1728;
    if (newnode->vres < 0)
	newnode->vres = !(newnode->name[0] == 'f' && newnode->name[1] == 'n');
    newnode->extra = NULL;

    return newnode;
}

static t32bits
get4(unsigned char *p, int endian)
{
    return endian ? (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3] :
	p[0]|(p[1]<<8)|(p[2]<<16)|(p[3]<<24);
}

static int
get2(unsigned char *p, int endian)
{
    return endian ? (p[0]<<8)|p[1] : p[0]|(p[1]<<8);
}

/* generate pagenodes for the images in a tiff file */
int
notetiff(char *name)
{

    FILE *tf;
    unsigned char header[8];
    static const char littleTIFF[5] = "\x49\x49\x2a\x00";
    static const char bigTIFF[5] = "\x4d\x4d\x00\x2a";
    int endian;
    t32bits IFDoff;
    struct pagenode *pn = NULL;
    QString str;


    if ((tf = fopen(name, "r")) == NULL) {
        QString mesg;
	mesg.sprintf(i18n("Unable to open:\n%s\n"),name);
	kfaxerror("Sorry",mesg.data());
	return 0;
    }

    if (fread(header, 8, 1, tf) == 0) {
    nottiff:
	fclose(tf);
	(void) notefile(name,FAX_RAW);
	return 0;
    }
    if (memcmp(header, &littleTIFF, 4) == 0)
	endian = 0;
    else if (memcmp(header, &bigTIFF, 4) == 0)
	endian = 1;
    else
	goto nottiff;
    IFDoff = get4(header+4, endian);
    if (IFDoff & 1)
	goto nottiff;
    do {			/* for each page */
	unsigned char buf[8];
	unsigned char *dir = NULL , *dp = NULL;
	int ndirent;
	pixnum iwidth = defaultpage.width ? defaultpage.width : 1728;
	pixnum iheight = defaultpage.height ? defaultpage.height : 2339;
	int inverse = defaultpage.inverse;
	int lsbfirst = 0;
	int t4opt = 0, comp = 0;
	int orient = defaultpage.orient;
	double yres = defaultpage.vres ? 196.0 : 98.0;
	struct strip *strips = NULL;
	unsigned long rowsperstrip = 0;
	t32bits nstrips = 1;

	if (fseek(tf, IFDoff, SEEK_SET) < 0) {
	realbad:
	  str.sprintf("             Invalid tiff file:         \n%s\n",name);
	  kfaxerror("Sorry",str.data());
	bad:
	    if (strips)
		free(strips);
	    if (dir)
		free(dir);
	    fclose(tf);
	    return 1;
	}
	if (fread(buf, 2, 1, tf) == 0)
	    goto realbad;
	ndirent = get2(buf, endian);
	dir = (unsigned char *) xmalloc(12*ndirent+4);
	if (fread(dir, 12*ndirent+4, 1, tf) == 0)
	    goto realbad;
	for (dp = dir; ndirent; ndirent--, dp += 12) {
	    /* for each directory entry */
	    int tag, ftype;
	    t32bits count, value = 0;
	    tag = get2(dp, endian);
	    ftype = get2(dp+2, endian);
	    count = get4(dp+4, endian);
	    switch(ftype) {	/* value is offset to list if count*size > 4 */
	    case 3:		/* short */
		value = get2(dp+8, endian);
		break;
	    case 4:		/* long */
		value = get4(dp+8, endian);
		break;
	    case 5:		/* offset to rational */
		value = get4(dp+8, endian);
		break;
	    }
	    switch(tag) {
	    case 256:		/* ImageWidth */
		iwidth = value;
		break;
	    case 257:		/* ImageLength */
		iheight = value;
		break;
	    case 259:		/* Compression */
		comp = value;
		break;
	    case 262:		/* PhotometricInterpretation */
		inverse ^= (value == 1);
		break;
	    case 266:		/* FillOrder */
		lsbfirst = (value == 2);
		break;
	    case 273:		/* StripOffsets */
		nstrips = count;
		strips = (struct strip *) xmalloc(count * sizeof *strips);
		if (count == 1 || (count == 2 && ftype == 3)) {
		    strips[0].offset = value;
		    if (count == 2)
			strips[1].offset = get2(dp+10, endian);
		    break;
		}
		if (fseek(tf, value, SEEK_SET) < 0)
		    goto realbad;
		for (count = 0; count < nstrips; count++) {
		    if (fread(buf, (ftype == 3) ? 2 : 4, 1, tf) == 0)
			goto realbad;
		    strips[count].offset = (ftype == 3) ?
			get2(buf, endian) : get4(buf, endian);
		}
		break;
	    case 274:		/* Orientation */
		switch(value) {
		default:	/* row0 at top,    col0 at left   */
		    orient = 0;
		    break;
		case 2:		/* row0 at top,    col0 at right  */
		    orient = TURN_M;
		    break;
		case 3:		/* row0 at bottom, col0 at right  */
		    orient = TURN_U;
		    break;
		case 4:		/* row0 at bottom, col0 at left   */
		    orient = TURN_U|TURN_M;
		    break;
		case 5:		/* row0 at left,   col0 at top    */
		    orient = TURN_M|TURN_L;
		    break;
		case 6:		/* row0 at right,  col0 at top    */
		    orient = TURN_U|TURN_L;
		    break;
		case 7:		/* row0 at right,  col0 at bottom */
		    orient = TURN_U|TURN_M|TURN_L;
		    break;
		case 8:		/* row0 at left,   col0 at bottom */
		    orient = TURN_L;
		    break;
		}
		break;
	    case 278:		/* RowsPerStrip */
		rowsperstrip = value;	
		break;
	    case 279:		/* StripByteCounts */
		if (count != nstrips) {
		  str.sprintf("In file %s\nStrpisPerImage tag 273=%ls,tag279=%ld\n",
			      name, nstrips,(long int) count);
		  kfaxerror("Message",str.data());
		  goto realbad;
		}
		if (count == 1 || (count == 2 && ftype == 3)) {
		    strips[0].size = value;
		    if (count == 2)
			strips[1].size = get2(dp+10, endian);
		    break;
		}
		if (fseek(tf, value, SEEK_SET) < 0)
		    goto realbad;
		for (count = 0; count < nstrips; count++) {
		    if (fread(buf, (ftype == 3) ? 2 : 4, 1, tf) == 0)
			goto realbad;
		    strips[count].size = (ftype == 3) ?
			get2(buf, endian) : get4(buf, endian);
		}
		break;
	    case 283:		/* YResolution */
		if (fseek(tf, value, SEEK_SET) < 0 ||
		    fread(buf, 8, 1, tf) == 0)
		    goto realbad;
		yres = get4(buf, endian) / get4(buf+4, endian);
		break;
	    case 292:		/* T4Options */
		t4opt = value;
		break;
	    case 293:		/* T6Options */
		/* later */
		break;
	    case 296:		/* ResolutionUnit */
		if (value == 3)
		    yres *= 2.54;
		break;
	    }
	}
	IFDoff = get4(dp, endian);
	free(dir);
	dir = NULL;
	if (comp < 2 || comp > 4) {
	  kfaxerror("Sorry","This version can only handle Fax files\n");
	    goto bad;
	}
	pn = notefile(name,FAX_TIFF);
	pn->nstrips = nstrips;
	pn->rowsperstrip = nstrips > 1 ? rowsperstrip : iheight;
	pn->strips = strips;
	pn->width = iwidth;
	pn->height = iheight;
	pn->inverse = inverse;
	pn->lsbfirst = lsbfirst;
	pn->orient = orient;
	pn->vres = (yres > 150); /* arbitrary threshold for fine resolution */
	if (comp == 2)
	    pn->expander = MHexpand;
	else if (comp == 3)
	    pn->expander = (t4opt & 1) ? g32expand : g31expand;
	else
	    pn->expander = g4expand;
    } while (IFDoff);
    fclose(tf);
    return 1;
}

/* report error and remove bad file from the list */
static void
badfile(struct pagenode *pn)
{
    struct pagenode *p;

    if (errno)
	perror(pn->pathname);
    if (pn == firstpage) {
      if (pn->next == NULL){
	  kfaxerror("Sorry","             Bad Fax File          ");
	  return;
      }
      else{
	firstpage = thispage = firstpage->next;
	firstpage->prev = NULL;
      }
    }
    else
	for (p = firstpage; p; p = p->next)
	    if (p->next == pn) {
		thispage = p;
		p->next = pn->next;
		if (pn->next)
		    pn->next->prev = p;
		break;
	    }
    if (pn) free(pn);
    pn = NULL;
}

/* rearrange input bits into t16bits lsb-first chunks */
static void
normalize(struct pagenode *pn, int revbits, int swapbytes, size_t length)
{
    t32bits *p = (t32bits *) pn->data;

    switch ((revbits<<1)|swapbytes) {
    case 0:
	break;
    case 1:
	for ( ; length; length -= 4) {
	    t32bits t = *p;
	    *p++ = ((t & 0xff00ff00) >> 8) | ((t & 0x00ff00ff) << 8);
	}
	break;
    case 2:
	for ( ; length; length -= 4) {
	    t32bits t = *p;
	    t = ((t & 0xf0f0f0f0) >> 4) | ((t & 0x0f0f0f0f) << 4);
	    t = ((t & 0xcccccccc) >> 2) | ((t & 0x33333333) << 2);
	    *p++ = ((t & 0xaaaaaaaa) >> 1) | ((t & 0x55555555) << 1);
	}
	break;
    case 3:
	for ( ; length; length -= 4) {
	    t32bits t = *p;
	    t = ((t & 0xff00ff00) >> 8) | ((t & 0x00ff00ff) << 8);
	    t = ((t & 0xf0f0f0f0) >> 4) | ((t & 0x0f0f0f0f) << 4);
	    t = ((t & 0xcccccccc) >> 2) | ((t & 0x33333333) << 2);
	    *p++ = ((t & 0xaaaaaaaa) >> 1) | ((t & 0x55555555) << 1);
	}
    }
}


/* get compressed data into memory */
unsigned char *
getstrip(struct pagenode *pn, int strip)
{
    int fd;
    size_t offset, roundup;
    struct stat sbuf;
    unsigned char *Data;
    union { t16bits s; unsigned char b[2]; } so;
    QString str;

#define ShortOrder	so.b[1]

    so.s = 1;
    if ((fd = open(pn->pathname, O_RDONLY, 0)) < 0) {
	badfile(pn);
	return NULL;
    }

    if (pn->strips == NULL) {
	if (fstat(fd, &sbuf) != 0) {
	    close(fd);
	    badfile(pn);
	    return NULL;
	}
	offset = 0;
	pn->length = sbuf.st_size;
    }
    else if (strip < pn->nstrips) {
	offset = pn->strips[strip].offset;
	pn->length = pn->strips[strip].size;
    }
    else {
      str.sprintf("Trying to expand too many strips\n%s\n",pn->pathname);
      kfaxerror("Warning",str.data());
      return NULL;
    }

    /* round size to full boundary plus t32bits */
    roundup = (pn->length + 7) & ~3;

    Data = (unsigned char *) xmalloc(roundup);
    /* clear the last 2 t32bits, to force the expander to terminate
       even if the file ends in the middle of a fax line  */
    *((t32bits *) Data + roundup/4 - 2) = 0;
    *((t32bits *) Data + roundup/4 - 1) = 0;

    /* we expect to get it in one gulp... */
    if (lseek(fd, offset, SEEK_SET) < 0 ||
	(uint) read(fd, Data, pn->length) != pn->length) { 
	badfile(pn);
	free(Data);
	close(fd);
	return NULL;
    }
    close(fd);

    pn->data = (t16bits *) Data;
    if (pn->strips == NULL && memcmp(Data, FAXMAGIC, sizeof(FAXMAGIC)) == 0) {
	/* handle ghostscript / PC Research fax file */
      if (Data[24] != 1 || Data[25] != 0){
	str.sprintf(
        "Only the first page of the PC Research multipage file\n%s\nwill be shown\n",
	pn->pathname);
	kfaxerror("Message",str.data());
      }
	pn->length -= 64;
	pn->vres = Data[29];
	pn->data += 32;
	roundup -= 64;
    }

    normalize(pn, !pn->lsbfirst, ShortOrder, roundup);
    if (pn->height == 0)
	pn->height = G3count(pn, pn->expander == g32expand);
    if (pn->height == 0) {

      str.sprintf("No fax found in file:\n%s\n",pn->pathname);
      kfaxerror("Sorry",str.data());
      errno = 0;
      badfile(pn);
      free(Data);
      return NULL;
    }
    if (pn->strips == NULL)
	pn->rowsperstrip = pn->height;
    if (verbose && strip == 0)
	printf("%s:\n\twidth = %d\n\theight = %d\n\tresolution = %s\n",
	       pn->name, pn->width, pn->height, pn->vres ?
	       "fine" : "normal");
//    statusbarupdate(pn->name,pn->width,pn->height,pn->vres ? "fine" : "normal");
    return Data;
}
