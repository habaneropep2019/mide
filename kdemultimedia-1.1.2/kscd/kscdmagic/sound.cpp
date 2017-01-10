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

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <linux/soundcard.h>
#include <time.h>

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "syna.h"
#include "magicconf.h"


static int configUseCD, device;


void openSound(int param) {

  configUseCD = param;
  int format, stereo, fragment, fqc;
 
  attempt(device = open("/dev/dsp",O_RDONLY),"opening /dev/dsp");
    
  //Probably not needed
  //attemptNoDie(ioctl(device,SNDCTL_DSP_RESET,0),"reseting dsp");

  format = AFMT_S16_LE;
  fqc = frequency;
  stereo = 1;
  fragment = 0x00020000 + (m-overlap+1); 

  // 2 fragments of size 2*(2^(m-overlap+1)) bytes
  // Was 0x00010000 + m; 
    
  attemptNoDie(ioctl(device,SNDCTL_DSP_SETFRAGMENT,&fragment),"setting fragment");
  attempt(ioctl(device,SNDCTL_DSP_SETFMT,&format),"setting format");

  if (format != AFMT_S16_LE) error("setting format (2)");
  attempt(ioctl(device,SNDCTL_DSP_STEREO,&stereo),"setting stereo");
  attemptNoDie(ioctl(device,SNDCTL_DSP_SPEED,&fqc),"setting frequency");
   
  data = new short[n*2];  
}

void closeSound() {

  delete data;
  close(device);

}

void getNextFragment(void) {

  read(device,(char*)data+n*4-recSize*4, recSize*4);

  if (recSize != n)
    memmove((char*)data,(char*)data+recSize*4,(n-recSize)*4);

}

#endif
