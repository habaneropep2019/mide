
/* 
  Synaesthesia - program to display sound graphically
  Copyright (C) 1997  Paul Francis Harrison

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  675 Mass Ave, Cambridge, MA 02139, USA.

  The author may be contacted at:
  pfh@yoyo.cc.monash.edu.au
  27 Bond St., Mt. Waverley, 3149, Melbourne, Australia

*/


#if defined(__linux__) 

#include <string.h>

extern "C" {

#include "xlib.h"

}

#include "magicconf.h"
#include "syna.h"

static xlibparam xparams = { 0, 0, 0 };
static xdisplay *d;

static int lowColor;
static unsigned char mapping[64];

int setPalette(int ,int r,int g,int b) {
  return xalloc_color(d,r*257,g*257,b*257,0);
}

void screenInit(int xHint,int yHint,int widthHint,int heightHint) {

  d = xalloc_display("KSCD Magic",xHint,yHint,widthHint,heightHint,&xparams);

  if (d == 0) 
    error("setting up a window");

  if (!alloc_image(d)) 
    error("allocating window buffer");

  outWidth = widthHint;
  outHeight = heightHint;

  // XMoveWindow(d->display,d->window,xHint,yHint);

#define BOUND(x) ((x) > 255 ? 255 : (x))
#define PEAKIFY(x) BOUND((x) - (x)*(255-(x))/255/2)

  int i;
  lowColor = (d->depth <= 8);

  if (!lowColor) { 
   
    for(i=0;i<256;i++)
      attempt(setPalette(i,PEAKIFY((i&15*16)),
			 PEAKIFY((i&15)*16+(i&15*16)/4),
			 PEAKIFY((i&15)*16)),
              " in X: could not allocate sufficient palette entries");
  } 
  else {

    for(i=0;i<64;i++)
      mapping[i] =setPalette(i,PEAKIFY((i&7*8)*4),
				       PEAKIFY((i&7)*32+(i&7*8)*2),
				       PEAKIFY((i&7)*32));
	
  }
}

void screenEnd() {

  xfree_colors(d);
  xfree_display(d);

}


int sizeUpdate() {

  int newWidth;
  int newHeight;

  if (xsize_update(d,&newWidth,&newHeight)) {

    if (newWidth == outWidth && newHeight == outHeight)
      return 0;

    delete[] output;

    outWidth = newWidth;
    outHeight = newHeight;

    output = new unsigned char [outWidth*outHeight*2];

    memset(output,32,outWidth*outHeight*2);
    return 1;

  }

  return 0;
}

void sizeRequest(int width,int height) {

  if (width != outWidth || height != outHeight) {

    xsize_set(d,width,height);
    XFlush(d->display);

  }
}

void mouseUpdate() {

  xmouse_update(d);

}
int mouseGetX() {

  return xmouse_x(d);

}
int mouseGetY() {

  return xmouse_y(d);

}
int mouseGetButtons() {

  return xmouse_buttons(d);

}
 
void screenShow(void) {

  register unsigned long *ptr2 = (unsigned long*)output;

  unsigned long *ptr1 = (unsigned long*)d->back;

  int i = outWidth*outHeight/4;

  // Asger Alstrup Nielsen's (alstrup@diku.dk)
  // optimized 32 bit screen loop

  if (lowColor)

    do {

      register unsigned int const r1 = *(ptr2++);
      register unsigned int const r2 = *(ptr2++);
    
      if (r1 || r2) {

        register unsigned int const v = 
             mapping[((r1&0xe0)>>5)|((r1&0xe000)>>10)]
            |mapping[((r1&0xe00000)>>21)|((r1&0xe0000000)>>26)]*256U; 

        *(ptr1++) = v | 
             mapping[((r2&0xe0)>>5)|((r2&0xe000)>>10)]*65536U
            |mapping[((r2&0xe00000)>>21)|((r2&0xe0000000)>>26)]*16777216U; 

	/* 
	   ( ((r2 & 0x000000e0) << 0-5 + 16)

          | ((r2 & 0x0000e000) << 3-13 + 16)

          | ((r2 & 0x00e00000) << 8-21 + 16)

          | ((r2 & 0xe0000000) >> 29-12 - 16));

	*/
      } 

      else 
	ptr1++;

    } while (--i); 

  else
    do {

      register unsigned int const r1 = *(ptr2++);
      register unsigned int const r2 = *(ptr2++);
    
      if (r1 || r2) {

        register unsigned int const v = 
            ((r1 & 0x000000f0) >> 4)
          | ((r1 & 0x0000f000) >> 8)
          | ((r1 & 0x00f00000) >> 12)
          | ((r1 & 0xf0000000) >> 16);

        *(ptr1++) = v | 
          ( ((r2 & 0x000000f0) << 12)
          | ((r2 & 0x0000f000) << 8)
          | ((r2 & 0x00f00000) << 4)
          | ((r2 & 0xf0000000)));
      } 
      else 
	ptr1++;

    } while (--i); 
  
  xflip_buffers(d);
  draw_screen(d);
  XFlush(d->display);

}

#endif
