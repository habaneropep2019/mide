/* Synaesthesia - program to display sound graphically
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
  or
    27 Bond St., Mt. Waverley, 3149, Melbourne, Australia
*/

#if defined(__linux__) || defined(__svr4__)

template <
  class SrcType,
  class DestType,
  DestType (*transfer)(SrcType,DestType)
>

struct BitmapPut {

static void go(
  SrcType *src, int srcWidth, int srcHeight, 
  DestType *dest, int destWidth, int destHeight,
  int srcX,int srcY,int destX, int destY, int width,int height) {

  if (srcX < 0) {
    destX -= srcX;
    width += srcX;
    srcX   = 0;
  }
  if (srcY < 0) {
    destY -= srcY;
    height+= srcY;
    srcY   = 0;
  }
  if (destX < 0) {
    srcX  -= destX;
    width += destX;
    destX  = 0; 
  }
  if (destY < 0) {
    srcY  -= destY;
    height+= destY;
    destY  = 0;
  }

  if (srcX+width > srcWidth) {
    width = srcWidth - srcX;
  }
  if (srcY+height > srcHeight) {
    height = srcHeight - srcY;
  }
  if (destX+width > destWidth) {
    width = destWidth - destX; 
  }
  if (destY+height > destHeight) {
    height = destHeight - destY;
  }

  if (width <= 0 || height <= 0) return;

  int srcSkip = srcWidth - width, destSkip = destWidth - width;
  int i;
  src  += srcX  + srcY  * srcWidth;
  dest += destX + destY * destWidth;

  for(;height--;src+=srcSkip,dest+=destSkip)
    for(i=width;i--;src++,dest++)
      *dest = transfer(*src,*dest);
}};

#endif
