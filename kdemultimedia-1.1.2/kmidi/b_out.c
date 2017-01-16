/*
 * $Id$
 */

#include "config.h"

#ifdef __FreeBSD__
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BB_SIZE AUDIO_BUFFER_SIZE*8
static unsigned char *bbuf = 0;
static int bboffset = 0, bbcount = 0;

void b_out(int fd, int *buf, int ocount)
{
  int ret;

  if (!bbuf) {
    bbcount = bboffset = 0;
    bbuf = (unsigned char *)malloc(BB_SIZE);
    if (!bbuf) {
	    fprintf(stderr, "malloc output error");
	    #ifdef ADAGIO
	    X_EXIT
	    #else
	    exit(1);
	    #endif
    }
  }

  ret = 0;
  while (bbcount) {
    ret = write(fd, bbuf + bboffset, bbcount);
    if (ret < 0) {
	if (errno == EINTR) continue;
	else if (errno == EWOULDBLOCK) {
	    if (bboffset) {
		memcpy(bbuf, bbuf + bboffset, bbcount);
		bboffset = 0;
	    }
	    if (ocount && bboffset + bbcount + ocount <= BB_SIZE) break;
	    sleep(1);
	}
	else {
	    perror("error writing to dsp device");
	    #ifdef ADAGIO
	    X_EXIT
	    #else
	    exit(1);
	    #endif
	}
    }
    else {
	bboffset += ret;
	bbcount -= ret;
	if (!bbcount) bboffset = 0;
	else if (bbcount < 0 || bboffset + bbcount > BB_SIZE) {
	}
    }
  }

  if (!ocount) return;

  memcpy(bbuf + bboffset + bbcount, buf, ocount);
  bbcount += ocount;
  if (ret >= 0) while (bbcount) {
    ret = write(fd, bbuf + bboffset, bbcount);
    if (ret < 0) {
	if (errno == EINTR) continue;
	else if (errno == EWOULDBLOCK) break;
	else {
	    perror("error writing to dsp device");
	    #ifdef ADAGIO
	    X_EXIT
	    #else
	    exit(1);
	    #endif
	}
    }
    else {
/* Debug: Are we getting output? */
#if 0
int i;
fprintf(stderr,"[%d]",ret);
for (i = 0; i < 20 && i < ret; i++)
fprintf(stderr," %d", (int)*(bbuf+bboffset+i) );
fprintf(stderr,"\n");
#endif
	bboffset += ret;
	bbcount -= ret;
	if (!bbcount) bboffset = 0;
	else if (bbcount < 0 || bboffset + bbcount > BB_SIZE) {
	    fprintf(stderr, "output error");
	    bbcount = bboffset = 0;
	}
    }
  }
}
