
/*

  kscdmagic 1.0   Bernd Johannes Wuebben <wuebben@kde.org>
  $ Id: $
  based on:

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
    phar6@student.monash.edu.au
  or
    27 Bond St., Mt. Waverley, 3149, Melbourne, Australia
*/

#ifndef __SYNA_H__
#define __SYNA_H__

#if defined(__linux__) || defined(__svr4__)

#include "magicconf.h"


#define PROGNAME "kscdmagic"

void error(char *str);
#define attempt(x,y) if ((int)(x) == -1) error(y)
void warning(char *str);
#define attemptNoDie(x,y) if ((int)(x) == -1) warning(y)

#define n (1<<m)
#define recSize (1<<m-overlap)

/* core */
extern volatile short *data;
extern unsigned char *output;
extern int outWidth, outHeight;
extern int brightFactor;

void coreInit();
void coreGo();
void coreGonew();

void screenInit(int xHint,int yHint,int widthHint,int heightHint);
void screenEnd(void);
void screenShow(void);
void sizeRequest(int width,int height);
int sizeUpdate(void);
void mouseUpdate(void);
int mouseGetX(void);
int mouseGetY(void);
int mouseGetButtons(void);

int processUserInput(void); //True == abort now
void showOutput(void);

void error(char *str); //Display error and exit
void warning(char *str); //Display error


/* bitmap */
enum SymbolID {
  Bulb = 0, 
  Speaker = 1, Play = 2, Pause = 3, Stop = 4, SkipFwd = 5, SkipBack = 6,
  Handle = 7, Pointer = 8, Open = 9, NoCD = 10, Exit = 11,
  Zero = 12, One = 13, Two = 14, Three = 15, Four = 16,
  Five = 17, Six = 18, Seven = 19, Eight = 20, Nine = 21, Ten = 22,
  Colon = 22, Slider = 23, Selector = 24,
  SymbolCount = 25
};

enum TransferType {
  HalfBlue, HalfRed, MaxBlue, MaxRed
};

void putSymbol(int x,int y,int id,TransferType typ);


/* sound */
void cdOpen(void);
void cdClose(void);
void cdGetStatus(int &track, int &frames, SymbolID &state);
void cdPlay(int track); 
void cdStop(void);
void cdPause(void);
void cdResume(void);
void cdEject(void);
void cdCloseTray(void);
int cdGetTrackCount(void);
int cdGetTrackFrame(int track);
void openSound(int configUseCD);
void closeSound();
void setupMixer(double &loudness);
void setVolume(double loudness);
void getNextFragment(void);

#endif

#endif
