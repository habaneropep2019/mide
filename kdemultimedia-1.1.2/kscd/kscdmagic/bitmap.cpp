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



#if defined(__linux__) 

#include "bitmap.h"
#include "symbol.h"
#include "syna.h"

static int span[SymbolCount][2] = {
  {1,25},
  {32,55},
  {63,78},
  {86,109},
  {116,140},
  {147,172},
  {176,201},
  {206,216},
  {224,246},
  {266,289},
  {300,332},
  {338,373},

  {524,538},
  {386,391},
  {401,415},
  {419,430},
  {432,446},
  {447,461},
  {462,476},
  {480,492},
  {493,507},
  {508,522},

  {545,552},
  {559,568},
  {575,584}
};

struct Entry {

  unsigned char red;
  unsigned char blue;

  Entry(unsigned char b,unsigned char r) : red(r), blue(b) { }

};

Entry maxBlue(unsigned char src,Entry dest) { 
    return Entry(src > dest.blue ? src : dest.blue,dest.red); 
}

Entry halfBlue(unsigned char src,Entry dest) { 
    return Entry(src/2 > dest.blue ? src/2 : dest.blue,dest.red); 
}

Entry maxRed(unsigned char src,Entry dest) { 
    return Entry(dest.blue,src > dest.red ? src : dest.red); 
}

Entry halfRed(unsigned char src,Entry dest) { 
  return Entry(dest.blue,src/2 > dest.red ? src/2 : dest.red); 
}

template <Entry (*transfer)(unsigned char,Entry)>

struct PutSymbol {
 
    static void go(int x,int y,int id) {

	BitmapPut<unsigned char,Entry,transfer>::go(
	    Symbols,SYMBOLSWIDTH,SYMBOLSHEIGHT,
            (Entry*)output,outWidth,outHeight,
            span[id][0],0,
            x-(span[id][1]-span[id][0]>>1),y,
            span[id][1]-span[id][0],SYMBOLSHEIGHT);
  }

};

void putSymbol(int x,int y,int id,TransferType typ) {

    switch(typ) {

    case HalfBlue  : PutSymbol <halfBlue>::go(x,y,id); break;
    case MaxBlue   : PutSymbol <maxBlue> ::go(x,y,id); break;
    case HalfRed   : PutSymbol <halfRed> ::go(x,y,id); break;
    case MaxRed    : PutSymbol <maxRed>  ::go(x,y,id); break;

    default      : error("Invalid transfer operation.\n");

    }
}

#endif
