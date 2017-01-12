
/*

  kscdmagic 1.0   Bernd Johannes Wuebben <wuebben@kde.org>
  $Id$

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
  pfh@yoyo.cc.monash.edu.au
  27 Bond St., Mt. Waverley, 3149, Melbourne, Australia

*/

#include <stdlib.h>
#include <stdio.h>

#if defined(__linux__) 

#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <getopt.h>

#include "magicconf.h"
#include "syna.h"

volatile short *data;
unsigned char *output;
int outWidth, outHeight;

static int configUseCD = 1;

const int numRows = 4, rowHeight = 50, leftColWidth = 40, rowMaxWidth = 310,
          sliderBorder = 20, sliderWidth = rowMaxWidth - leftColWidth - sliderBorder*2,
          numberSpacing = 15,
          uiWidth = 320, uiHeight = 200;


static int isExpanded = 0;
static double bright = 1.0;


void setBrightness(double bright) { 

  brightFactor = int(brightness * bright * 8.0); 

} 



static void cleanup( int sig )
{

  (void) sig;
  closeSound();
  exit(0);
}


// make sure the pid file is cleaned up when exiting unexpectedly.

void catchSignals()
{
	signal(SIGHUP, cleanup);		/* Hangup */
	signal(SIGINT, cleanup);		/* Interrupt */
	signal(SIGTERM, cleanup);		/* Terminate */
	signal(SIGPIPE, cleanup);
	signal(SIGQUIT, cleanup);

}

void usage(char*){

  fprintf(stderr, "Valid command line options:\n");
  fprintf(stderr, " -b set brightness (1 - 10)\n");
  fprintf(stderr, " -w set width\n");
  fprintf(stderr, " -h set height\n");
  exit(1);

}
int main(int argc, char **argv) { 

  int windX=10;
  int windY=30;
  int windWidth=320;
  int windHeight=200;
  int c;
  opterr = 0;

  openSound(configUseCD); 
  catchSignals();


  while ((c = getopt(argc, argv, "b:h:w:")) != -1){
    switch (c)
      {

      case '?':
	fprintf(stderr, "%s: unknown option \"%s\"\n", 
		argv[0], argv[optind-1]);
	usage(argv[0]);
	exit(1);	
      case 'b':
	bright = (double) atoi(optarg);
	bright = bright/10;
	break;
      case 'w':
	windWidth = atoi(optarg);
	break;
      case 'h':
	windHeight = atoi(optarg);
	break;
      }
  }

 
  if (bright > 1.0)
    bright = 1.0;
  else if (bright < 0.0)
    bright = 0.0;


  if (windWidth < 1)
    windWidth = 320;
  if (windHeight < 1)
    windHeight = 200;

  setBrightness(bright);

  screenInit(windX,windY,windWidth,windHeight);
  output = new unsigned char[outWidth*outHeight*2];

  coreInit();

  time_t timer = time(NULL);
  
  int frames = 0;
  do {
    screenShow();
    coreGo();
    frames++;
  } while(!processUserInput());

  timer = time(NULL) - timer;
  delete output;
  closeSound();

  if (timer > 10)
    fprintf(stderr,"Frames per second: %f\n", double(frames)/timer);
  
  return 0;
}

void error(char *str) { 
  printf(PROGNAME ": Error %s\n",str); 
  exit(1);
}

void warning(char *str) { printf(PROGNAME ": Possible error %s\n",str); }


int processUserInput() {

  if (sizeUpdate()) {
    isExpanded = 0;
  }

  return 0;
}

#else

int main() { 

  fprintf(stderr,"KSCD Magic works currently only on Linux.\n"\
	  "It should however be trivial to port it to other platforms ...\n");

}

#endif

