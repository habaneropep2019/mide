
/*

 KFax -- A G3/G4 Fax Viewer

 Copyrigh (C) 1997 Bernd Johannes Wuebben
 wuebben@math.cornell.edu
 wuebben@kde.org

 Based on:
 
 viewfax - g3/g4 fax processing software.
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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 

*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

//#include <kapp.h>
//#include <klocale.h>

/* NewImage() needs to fiddle with the Display structure */
#define XLIB_ILLEGAL_ACCESS
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/cursorfont.h>

#include <qglobal.h>
#include "faxexpand.h"
#include "version.h"



//#define i18n klocale->translate

#define VIEWFAXVERSION "2.3"


/* If moving the image around with the middle mouse button is jerky or
   slow, try defining USE_MOTIONHINT.  It may help (it may also make
   things worse - it depends on the server implementation) */
#undef USE_MOTIONHINT



/* access the 'extra' field in a pagenode */

#define Pimage(p)	((XImage *)(p)->extra)


/* forward declarations */

XImage *FlipImage(XImage *xi);
XImage *MirrorImage(XImage *xi);
XImage *NewImage(int w, int h, char *data, int bit_order);
XImage *RotImage(XImage *Image);
XImage *ZoomImage(XImage *Big);

void FreeImage(XImage *Image);

static int release(int quit);
/*static char *suffix(char *opt, const char *str);*/


/* X variables */

extern  Cursor WorkCursor;
extern  Cursor ReadyCursor;
extern  Cursor MoveCursor;
extern  Cursor LRCursor;
extern  Cursor UDCursor;

extern bool have_no_fax;
extern Display* qtdisplay ;
extern Window  qtwin;
extern Window Win;
extern int qwindow_width;
extern int qwindow_height;

extern void setstatusbarmem(int);
extern void setfaxtitle(char*);
extern void kfaxerror(char* title,char* error);

struct pagenode *firstpage, *lastpage, *thispage, *helppage, *auxpage;
struct pagenode defaultpage;

static Display *Disp;

static int Default_Screen;
char *ProgName;
int verbose = 0;

int abell = 1;			/* audio bell */
int vbell = 0;			/* visual bell */
int zfactor = 0;			/* zoom factor */
bool have_cmd_opt = FALSE;

size_t Memused = 0;		/* image memory usage */
static size_t Memlimit = 8*1024*1024;	/* try not to exceed */

#undef min
#undef max
#define min(a,b)	((a)<(b)?(a):(b))
#define max(a,b)	((a)>(b)?(a):(b))

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/* OK, OK - this is a dreadful hack.  But it adequately distinguishes
   modern big- and little- endian hosts.  We only need this to set the
   byte order in XImage structures */

static union { t32bits i; unsigned char b[4]; } bo;
#define ByteOrder	bo.b[3]

static char Banner[] =
"KFax version " KFAXVERSION "\tCopyright (C) 1997, Bernd Johannes Wuebben\n";

/*"KFax is based on:\n"
"viewfax " VERSION ":\t\tCopyright (c) 1990,  1995 Frank D. Cringle.\n"
"libtiff v 3.4beta:\tCopyright (c) 1988 - 1955 Sam Leffler\n"
"                  \tCopyright (c) 1991 - 1995 Silicon Graphics, Inc.\n\n" 
"KFax comes with ABSOLUTELY NO WARRANTY; for details see the\n"
"file \"COPYING\" in the distribution directory.\n";*/

static char Usage[] = "usage: %s <flags> file ...\n"\
"\t-f\tfine resolution\n"\
"\t-n\tnormal resolution\n"\
"\t-h\theight (number of fax lines)\n"\
"\t-w\twidth (dots per fax line)\n"\
"\t-l\tturn image 90 degrees (landscape mode)\n"\
"\t-u\tturn image upside down\n"\
"\t-i\tinvert (black/white)\n"\
"\t-m\tmemory usage limit in bytes (M m mega, K k kilo) default:8 megs\n"\
"\t-r\tfax data is packed ls-bit first in input bytes\n"\
"\t-v\tversion information\n"\
"\t-2\traw files are g3-2d\n"\
"\t-4\traw files are g4\n";



extern "C"{
extern int  getopt(int argc, char * const argv[],const char *optstring);
extern char *optarg;
extern int optind,opterr,optopt;
}

#define MAXZOOM	10

XEvent Event;
XImage *Image, *Images[MAXZOOM];
XSizeHints size_hints;

Time Lasttime = 0; 

struct pagenode *viewpage = NULL;  

int viewfaxmain(int argc, char **argv){

    int c;
    int err  =0;
    int banner = 0;
    int have_height = 0;


    bo.i = 1;
    defaultpage.vres = -1;
    have_no_fax = TRUE;

    /* TODO Do I need to know this: */
    defaultpage.expander = g31expand;

    opterr = 0;			/* suppress getopt error message */

    ProgName = "KFax";

    while ((c = getopt(argc, argv, "b:d:fg:h:ilm:nruvw:z:24")) != -1)
	switch(c) {
	
	case '2':
	    defaultpage.expander = g32expand;
	    if(!have_height)
	      defaultpage.height = 2339;
	    have_cmd_opt = TRUE;
	    break;
	case 'h':               /* user thinks this is the height */
	  have_height = 1;
	  defaultpage.height = atoi(optarg);
	    have_cmd_opt = TRUE;
	  break;
	case 'i':		/* invert black/white */
	    defaultpage.inverse = 1;
	    have_cmd_opt = TRUE;
	    break;
	case 'l':		/* landscape */
	    defaultpage.orient |= TURN_L;
	    have_cmd_opt = TRUE;
	    break;
	case 'f':		/* fine resolution */
	    defaultpage.vres = 1;
	    have_cmd_opt = TRUE;
	    break;
	case 'n':		/* normal resolution */
	    defaultpage.vres = 0;
	    have_cmd_opt = TRUE;
	    break;
	case 'r':		/* reverse input bits */
	    defaultpage.lsbfirst = 1;
	    have_cmd_opt = TRUE;
	    break;
	case 'u':		/* upside down */
	    defaultpage.orient |= TURN_U;
	    have_cmd_opt = TRUE;
	    break;
	case 'v':		/*version*/
	    banner = 1;
	    break;
	case 'w':		/* user thinks this is the width */
	    defaultpage.width = atoi(optarg);
	    have_cmd_opt = TRUE;
	    break;
	case '4':
	    defaultpage.expander = g4expand;
	    if(!have_height)
	      defaultpage.height = 2155;
	    have_cmd_opt = TRUE;
	    break;
	    /*	case 'z':	
	    c = atoi(optarg);
	    if (c <= 0)
		c = 1;
	    for (zfactor = 1; c > 1; c >>= 1)
		zfactor <<= 1;
	    break;*/
	case 'm':		/* memory usage limit */
	    Memlimit = atoi(optarg);
	    switch(optarg[strlen(optarg)-1]) {
	    case 'M':
	    case 'm':
		Memlimit *= 1024;
	    case 'K':
	    case 'k':
		Memlimit *= 1024;
	    }
	    break;
	default:
	    err++;
	    break;

	    /*
	    defaultpage.vres = 1;
	    defaultpage.height = atoi(optarg);
	    defaultpage.inverse = 1;
	    defaultpage.orient |= TURN_L;
	    Memlimit = 
	    defaultpage.vres = 0;
	    defaultpage.lsbfirst = 1;
	    defaultpage.orient |= TURN_U;
	    verbose = 1;
	    defaultpage.width = atoi(optarg);
	    defaultpage.height = atoi(optarg);
	    c = atoi(optarg);
	    defaultpage.expander = g32expand;
	    defaultpage.expander = g4expand;
	    */

	}

    if (defaultpage.expander == g4expand && defaultpage.height == 0) {
	fputs("-h value is required to interpret raw g4 faxes\n", stderr);
	err++;
    }

    firstpage = lastpage = thispage = helppage = auxpage =  NULL;

    for (; optind < argc; optind++){
	(void) notetiff(argv[optind]);
    }

    if (err ) {
      fprintf(stderr, Usage,ProgName);
      exit(1);
    }
    if (banner ) {
      fprintf(stderr, Banner);
      exit(1);
    }

    if ( firstpage == NULL) {
      have_no_fax = TRUE;
    }
    else{
      have_no_fax = FALSE;
    }

    Disp = qtdisplay;
    Default_Screen = XDefaultScreen(qtdisplay);

    return 1;
}



/* return mismatching suffix of option name */
/*static char *suffix(char *opt, const char *prefix){

    while (*opt && *opt == *prefix) {
	opt++; prefix++;
    }
    return opt;
}
*/

/* Change orientation of all following pages */
void TurnFollowing(int How, struct pagenode *pn)
{
    while (pn) {
	if (Pimage(pn)) {
	    FreeImage(Pimage(pn));
	    pn->extra = NULL;
	}
	pn->orient ^= How;
	pn = pn->next;
    }
}

static void
drawline(pixnum *run, int LineNum, struct pagenode *pn)
{
    t32bits *p, *p1;		/* p - current line, p1 - low-res duplicate */
    pixnum *r;			/* pointer to run-lengths */
    t32bits pix;		/* current pixel value */
    t32bits acc;		/* pixel accumulator */
    int nacc;			/* number of valid bits in acc */
    int tot;			/* total pixels in line */
    int n;

    LineNum += pn->stripnum * pn->rowsperstrip;
    p = (t32bits *) (Pimage(pn)->data + LineNum*(2-pn->vres)*Pimage(pn)->bytes_per_line);
    p1 =(t32bits *)( pn->vres ? NULL : p + Pimage(pn)->bytes_per_line/sizeof(*p));
    r = run;
    acc = 0;
    nacc = 0;
    pix = pn->inverse ? ~0 : 0;
    tot = 0;
    while (tot < pn->width) {
	n = *r++;
	tot += n;
	if (pix)
	    acc |= (~(t32bits)0 >> nacc);
	else if (nacc)
	    acc &= (~(t32bits)0 << (32 - nacc));
	else
	    acc = 0;
	if (nacc + n < 32) {
	    nacc += n;
	    pix = ~pix;
	    continue;
	}
	*p++ = acc;
	if (p1)
	    *p1++ = acc;
	n -= 32 - nacc;
	while (n >= 32) {
	    n -= 32;
	    *p++ = pix;
	    if (p1)
		*p1++ = pix;
	}
	acc = pix;
	nacc = n;
	pix = ~pix;
    }
    if (nacc) {
	*p++ = acc;
	if (p1)
	    *p1++ = acc;
    }
}

static int
GetPartImage(struct pagenode *pn, int n)
{
    unsigned char *Data = getstrip(pn, n);

    if (Data == NULL)
	return 3;
    pn->stripnum = n;
    (*pn->expander)(pn, drawline);
    free(Data);
    return 1;
}

int GetImage(struct pagenode *pn){
    int i;

    if (pn->strips == NULL) {

      /*printf("RAW fax file\n");*/

	/* raw file; maybe we don't have the height yet */
	unsigned char *Data = getstrip(pn, 0);
	if (Data == NULL){

	  return 0;
	}
	pn->extra = NewImage(pn->width, pn->vres ?
			     pn->height : 2*pn->height, NULL, 1);

//printf("height = %d\n",pn->height);
//printf("setting height to %d\n", pn->vres ? pn->height : 2*pn->height);

	if(pn->extra == NULL)
	  return 0;

	(*pn->expander)(pn, drawline);
    }
    else {
	/* multi-strip tiff */
      /*printf("MULTI STRIP TIFF fax file\n");*/

	pn->extra = NewImage(pn->width, pn->vres ?
			     pn->height : 2*pn->height, NULL, 1);

	if(pn->extra == NULL)
	  return 0;
	pn->stripnum = 0;
	for (i = 0; i < pn->nstrips; i++){

	  int k =GetPartImage(pn, i); 
	  
	  if ( k == 3 ){
	    FreeImage(Pimage(pn));
	    return k;
	  }

	}
    }
    if (pn->orient & TURN_U)
	pn->extra = FlipImage(Pimage(pn));
    if (pn->orient & TURN_M)
	pn->extra = MirrorImage(Pimage(pn));
    if (pn->orient & TURN_L)
	pn->extra = RotImage(Pimage(pn));
    if (verbose) printf("\tmemused = %d\n", Memused);

/*
if(pn->extra)
  printf("pn->extra !=0 %s\n",pn->name);
else
  printf("pn->extra ==0 %s\n",pn->name);
  */

    return 1;
}



/* run this region through perl to generate the zoom table:
$lim = 1;
@c = ("0", "1", "1", "2");
print "static unsigned char Z[] = {\n";
for ($i = 0; $i < 16; $i++) {
    for ($j = 0; $j < 16; $j++) {
	$b1 = ($c[$j&3]+$c[$i&3]) > $lim;
	$b2 = ($c[($j>>2)&3]+$c[($i>>2)&3]) > $lim;
	printf " %X,", ($b2 << 1) | $b1;
    }
    print "\n";
}
print "};\n";
*/

static unsigned char Z[] = {
 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 2, 2, 2, 3,
 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 2, 3, 3, 3,
 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 2, 3, 3, 3,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3,
 0, 0, 0, 1, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3,
 0, 1, 1, 1, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
 0, 1, 1, 1, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
 0, 0, 0, 1, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3,
 0, 1, 1, 1, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
 0, 1, 1, 1, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3,
 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3,
 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

#define nib(n,w)	(((w)>>((n)<<2))&15)
#define zak(a,b)	Z[(a<<4)|b]

/* 2 -> 1 zoom, 4 pixels -> 1 pixel
   if #pixels <= $lim (see above), new pixel is white,
   else black.
*/

XImage *ZoomImage(XImage *Big){

    XImage *Small;
    int w, h;
    int i, j;

    XDefineCursor(Disp, Win, WorkCursor);
    XFlush(Disp);
    w = (Big->width+1) / 2;
    h = (Big->height+1) / 2;
    Small = NewImage(w, h, NULL, Big->bitmap_bit_order);
    if(Small == NULL)
      return NULL;

    Small->xoffset = (Big->xoffset+1)/2;
    for (i = 0; i < Big->height; i += 2) {
	t32bits *pb0 = (t32bits *) (Big->data + i * Big->bytes_per_line);
	t32bits *pb1 = pb0 + ((i == Big->height-1) ? 0 : Big->bytes_per_line/4);
	t32bits *ps = (t32bits *) (Small->data + i * Small->bytes_per_line / 2);
	for (j = 0; j < Big->bytes_per_line/8; j++) {
	    t32bits r1, r2;
	    t32bits t0 = *pb0++;
	    t32bits t1 = *pb1++;
	    r1 = (zak(nib(7,t0),nib(7,t1))<<14) |
		 (zak(nib(6,t0),nib(6,t1))<<12) |
		 (zak(nib(5,t0),nib(5,t1))<<10) |
		 (zak(nib(4,t0),nib(4,t1))<<8) |
		 (zak(nib(3,t0),nib(3,t1))<<6) |
		 (zak(nib(2,t0),nib(2,t1))<<4) |
		 (zak(nib(1,t0),nib(1,t1))<<2) |
		 (zak(nib(0,t0),nib(0,t1)));
	    t0 = *pb0++;
	    t1 = *pb1++;
	    r2 = (zak(nib(7,t0),nib(7,t1))<<14) |
		 (zak(nib(6,t0),nib(6,t1))<<12) |
		 (zak(nib(5,t0),nib(5,t1))<<10) |
		 (zak(nib(4,t0),nib(4,t1))<<8) |
		 (zak(nib(3,t0),nib(3,t1))<<6) |
		 (zak(nib(2,t0),nib(2,t1))<<4) |
		 (zak(nib(1,t0),nib(1,t1))<<2) |
		 (zak(nib(0,t0),nib(0,t1)));
	    *ps++ = (Big->bitmap_bit_order) ?
		(r1<<16)|r2 : (r2<<16)|r1;
	}
	for ( ; j < Small->bytes_per_line/4; j++) {
	    t32bits r1;
	    t32bits t0 = *pb0++;
	    t32bits t1 = *pb1++;
	    r1 = (zak(nib(7,t0),nib(7,t1))<<14) |
		 (zak(nib(6,t0),nib(6,t1))<<12) |
		 (zak(nib(5,t0),nib(5,t1))<<10) |
		 (zak(nib(4,t0),nib(4,t1))<<8) |
		 (zak(nib(3,t0),nib(3,t1))<<6) |
		 (zak(nib(2,t0),nib(2,t1))<<4) |
		 (zak(nib(1,t0),nib(1,t1))<<2) |
		 (zak(nib(0,t0),nib(0,t1)));
	    *ps++ = (Big->bitmap_bit_order) ?
		(r1<<16) : r1;
	}
    }
    XDefineCursor(Disp, Win, ReadyCursor);
    return Small;
}

XImage *FlipImage(XImage *Image){

    XImage *New = NewImage(Image->width, Image->height,
			   Image->data, !Image->bitmap_bit_order);
    if(New == NULL)
      return NULL;

    t32bits *p1 = (t32bits *) Image->data;
    t32bits *p2 = (t32bits *) (Image->data + Image->height *
			     Image->bytes_per_line - 4);

    /* the first shall be last ... */
    while (p1 < p2) {
	t32bits t = *p1;
	*p1++ = *p2;
	*p2-- = t;
    }

    /* let Xlib twiddle the bits */
    New->xoffset = 32 - (Image->width & 31) - Image->xoffset;
    New->xoffset &= 31;

    Image->data = NULL;
    FreeImage(Image);
    return New;
}

XImage *MirrorImage(XImage *Image){

    int i;
    XImage *New = NewImage(Image->width, Image->height,
			   Image->data, !Image->bitmap_bit_order);
    if(New == NULL)
      return NULL;

    /* reverse order of 32-bit words in each line */
    for (i = 0; i < Image->height; i++) {
	t32bits *p1 = (t32bits *) (Image->data + Image->bytes_per_line * i);
	t32bits *p2 = p1 + Image->bytes_per_line/4 - 1;
	while (p1 < p2) {
	    t32bits t = *p1;
	    *p1++ = *p2;
	    *p2-- = t;
	}
    }

    /* let Xlib twiddle the bits */
    New->xoffset = 32 - (Image->width & 31) - Image->xoffset;
    New->xoffset &= 31;

    Image->data = NULL;
    FreeImage(Image);
    return New;
}

XImage *RotImage(XImage *Image){

    XImage *New;
    int w = Image->height;
    int h = Image->width;
    int i, j, k, shift;
    int order = Image->bitmap_bit_order;
    int offs = h+Image->xoffset-1;

    New = NewImage(w, h, NULL, 1);
    if (New == NULL)
      return NULL;

    k = (32 - Image->xoffset) & 3;
    for (i = h - 1; i && k; i--, k--) {
	t32bits *sp = (t32bits *) Image->data + (offs-i)/32;
	t32bits *dp = (t32bits *) (New->data+i*New->bytes_per_line);
	t32bits d0 =0;
	shift = (offs-i)&31;
	if (order) shift = 31-shift;
	for (j = 0; j < w; j++) {
	    t32bits t = *sp;
	    sp += Image->bytes_per_line/4;
	    d0 |= ((t >> shift) & 1);
	    if ((j & 31) == 31)
		*dp++ = d0;
	    d0 <<= 1;;
	}
	if (j & 31)
	    *dp++ = d0<<(31-j);
    }
    for ( ; i >= 3; i-=4) {
	t32bits *sp = (t32bits *) Image->data + (offs-i)/32;
	t32bits *dp0 = (t32bits *) (New->data+i*New->bytes_per_line);
	t32bits *dp1 = dp0 - New->bytes_per_line/4;
	t32bits *dp2 = dp1 - New->bytes_per_line/4;
	t32bits *dp3 = dp2 - New->bytes_per_line/4;
	t32bits d0=0 , d1=0, d2 =0, d3 =0;
	shift = (offs-i)&31;
	if (order) shift = 28-shift;
	for (j = 0; j < w; j++) {
	    t32bits t = *sp >> shift;
	    sp += Image->bytes_per_line/4;
	    d0 |= t & 1; t >>= 1;
	    d1 |= t & 1; t >>= 1;
	    d2 |= t & 1; t >>= 1;
	    d3 |= t & 1; t >>= 1;
	    if ((j & 31) == 31) {
		if (order) {
		    *dp0++ = d3;
		    *dp1++ = d2;
		    *dp2++ = d1;
		    *dp3++ = d0;
		}
		else {
		    *dp0++ = d0;
		    *dp1++ = d1;
		    *dp2++ = d2;
		    *dp3++ = d3;
		}
	    }
	    d0 <<= 1; d1 <<= 1; d2 <<= 1; d3 <<= 1;
	}
	if (j & 31) {
	    if (order) {
		*dp0++ = d3<<(31-j);
		*dp1++ = d2<<(31-j);
		*dp2++ = d1<<(31-j);
		*dp3++ = d0<<(31-j);
	    }
	    else {
		*dp0++ = d0<<(31-j);
		*dp1++ = d1<<(31-j);
		*dp2++ = d2<<(31-j);
		*dp3++ = d3<<(31-j);
	    }
	}
    }
    for (; i >= 0; i--) {
	t32bits *sp = (t32bits *) Image->data + (offs-i)/32;
	t32bits *dp = (t32bits *) (New->data+i*New->bytes_per_line);
	t32bits d0=0;
	shift = (offs-i)&31;
	if (order) shift = 31-shift;
	for (j = 0; j < w; j++) {
	    t32bits t = *sp;
	    sp += Image->bytes_per_line/4;
	    d0 |= ((t >> shift) & 1);
	    if ((j & 31) == 31)
		*dp++ = d0;
	    d0 <<= 1;;
	}
	if (j & 31)
	    *dp++ = d0<<(31-j);
    }
    FreeImage(Image);
    return New;
}

/* release some non-essential memory or abort */
#define Try(n)								\
    if (n && n != thispage && n->extra) {				\
	FreeImage((XImage*)n->extra);	\
	n->extra = NULL;						\
	return 1;							\
    }

static int
release(int quit)
{
  (void) quit;

    struct pagenode *pn;

    if (thispage) {
	/* first consider "uninteresting" pages */
	for (pn = firstpage->next; pn; pn = pn->next)
	    if (pn->extra && pn != thispage && pn != thispage->prev &&
		pn != thispage->next && pn != lastpage) {
		FreeImage(Pimage(pn));
		pn->extra = NULL;
		return 1;
	    }
	Try(lastpage);
	Try(firstpage);
	Try(thispage->prev);
	Try(thispage->next);
    }

    return 0;

}

XImage *NewImage(int w, int h, char *data, int bit_order){

    XImage *newimage;
    /* This idea is taken from xwud/xpr.  Use a fake display with the
       desired bit/byte order to get the image routines initialised
       correctly */
    Display fake;

    fake = *Disp;
    if (data == NULL)
	data = xmalloc(((w + 31) & ~31) * h / 8);
    fake.byte_order = ByteOrder;
    fake.bitmap_bit_order = bit_order;

    int returncode = -1;
    while ((newimage = XCreateImage(&fake, DefaultVisual(Disp, Default_Screen),
				    1, XYBitmap, 0, data, w, h, 32, 0)) == NULL ){
      
      returncode = release(1);
      if (returncode == 0)
	break;
    }

    if (returncode == 0){
      kfaxerror("Sorry","Can not allocate Memory for a new Fax Image\n");
      return NULL;
    }

    Memused += newimage->bytes_per_line * newimage->height;
    /*printf("allocating %d bytes for %ld\n",
       newimage->bytes_per_line * newimage->height,
       newimage);*/


    return newimage;
}

void FreeImage(XImage *Image){

  if (Image->data){
	Memused -= Image->bytes_per_line * Image->height;
/*printf("deallocating %d bytes for %ld\n",
       Image->bytes_per_line * Image->height,
       Image);*/
  }
    XDestroyImage(Image);
    setstatusbarmem(Memused);

}

#ifndef xmalloc
char *
xmalloc(unsigned int size)
{
    char *p;

    while (Memused + size > Memlimit && release(0))
	;
    while ((p = (char*) malloc(size)) == NULL)
	(void) release(1);
    return p;
}
#endif
