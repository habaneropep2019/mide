/*
 * 
 * $Id: pppstats.cpp,v 1.8 1999/01/27 01:58:19 porten Exp $
 *
 * History:
 *
 * Bernd Wuebben, wuebben@math.cornell.edu:
 *
 * Much of this  is taken from the pppd sources in particular
 * /pppstat/pppstat.c, and modified to suit the needs of kppp.
 *
 *
 * Here the original history of pppstat.c:
 *
 * perkins@cps.msu.edu: Added compression statistics and alternate 
 *                display. 11/94
 *
 * Brad Parker (brad@cayman.com) 6/92
 *
 * from the original "slstats" by Van Jaconson
 *
 * Copyright (c) 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Van Jacobson (van@helios.ee.lbl.gov), Dec 31, 1989:
 *	- Initial distribution.
 */



#include <ctype.h>
#include <errno.h>

#if !(__GLIBC__ >= 2)
#include <nlist.h>
#endif  

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <qstring.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/ppp_defs.h>
#include <netinet/in.h>

#include "kpppconfig.h"

#ifdef __svr4__
#include <sys/stropts.h>
#include <net/pppio.h>		/* SVR4, Solaris 2, etc. */

#else
#include <sys/time.h>
#include <sys/socket.h>
#include <net/if.h>

#ifndef STREAMS

#if defined(linux) && !(__GLIBC__ >= 2)
#include <linux/if_ppp.h>
#else
#include <net/if_ppp.h>                /* BSD, Linux, NeXT, etc. */
#endif

#else				/* SunOS 4, AIX 4, OSF/1, etc. */
#define PPP_STATS	1	/* should be defined iff it is in ppp_if.c */
#include <sys/stream.h>
#include <net/ppp_str.h>
#endif
#endif

#include "log.h"


#define V(offset) (line % 20? cur.offset - old.offset: cur.offset)
#define W(offset) (line % 20? ccs.offset - ocs.offset: ccs.offset)
#define CRATE(comp, inc, unc) ((unc) == 0? 0.0: 1.0 - (double)((comp) + (inc)) / (unc))

#define FLAGS_GOOD (IFF_UP | IFF_BROADCAST)
#define FLAGS_MASK (IFF_UP | IFF_BROADCAST | IFF_POINTOPOINT | IFF_LOOPBACK  | IFF_NOARP)

int	vflag, rflag, cflag, aflag;
unsigned interval = 5;
int	unit;
int	s = 0;			/* socket file descriptor */
int	signalled;		/* set if alarm goes off "early" */
bool    ppp_stats_available;
int 	ibytes;
int 	ipackets;
int 	compressedin;
int 	uncompressedin;
int 	errorin;
int 	obytes;
int	opackets;
int 	compressed;
int 	packetsunc;
int 	packetsoutunc;
bool    have_local_address;

QString tmp_address;

struct ifreq ifr;
struct sockaddr_in *sinp;
struct ppp_stats cur, old;
struct ppp_comp_stats ccs, ocs;

extern QString local_ip_address;
extern QString remote_ip_address;

extern	char *malloc();

bool get_ppp_stats(struct ppp_stats *curp);
bool get_ppp_stats(struct ppp_stats *curp);
bool get_ppp_cstats(struct ppp_comp_stats *csp);
bool get_ppp_cstats(struct ppp_comp_stats *csp);
bool strioctl( int fd, int cmd, char* ptr,int ilen, int olen);


int if_is_up() {
  int is_up;

#ifdef __svr4__
    if ((s = open("/dev/ppp", O_RDONLY)) < 0) {
	perror("pppstats: Couldn't open /dev/ppp: ");
	return false;
    }
    if (strioctl(s, PPPIO_ATTACH, &unit, sizeof(int), 0) < 0) {
	fprintf(stderr, "pppstats: ppp%d is not available\n", unit);
	return false;
    }
#else
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	perror("Couldn't create IP socket");
	return false;
    }
#endif

    memset(&ifr,0,sizeof(ifr));

    // if you change this you have to change "unit" for 0 to whatever.
    strncpy(ifr.ifr_name, "ppp0",sizeof(ifr.ifr_name)); 


    if(ioctl(s, SIOCGIFFLAGS, &ifr)<0){
    	perror("Couldn't find interface ppp0");
	::close(s);
	s = 0;
	return 0;
    }

    if ((ifr.ifr_flags  & IFF_UP ) != 0L){
	is_up = 1;
	Debug("Interface is up\n");
    } 
    else{
      is_up = 0;
      ::close(s);
      s = 0;
      Debug("Interface is down\n");
    }
    
    return is_up;
}


bool init_stats(){


  unit = 0; // carefull here this is the zero of ppp0


  ibytes = 0;
  ipackets = 0;
  compressedin = 0;
  uncompressedin = 0;
  errorin = 0;
  obytes = 0;
  opackets = 0;
  compressed = 0;
  packetsunc = 0;
  packetsoutunc = 0;

  have_local_address = false;
  tmp_address = "";

  strcpy(ifr.ifr_name, "ppp0"); // if you change this you have to change "unit"
    
  if (ioctl(s, SIOCGIFADDR, &ifr) < 0) {	
  }

  sinp = (struct sockaddr_in*)&ifr.ifr_addr;	
  
  if(sinp->sin_addr.s_addr)
    local_ip_address = inet_ntoa(sinp->sin_addr);	
  else
    local_ip_address = "";
  Debug("Local IP: %s\n",local_ip_address.data());

  if (ioctl(s, SIOCGIFDSTADDR, &ifr) < 0)
    ;  

  sinp = (struct sockaddr_in*)&ifr.ifr_dstaddr;	

  if(sinp->sin_addr.s_addr)
    remote_ip_address = inet_ntoa(sinp->sin_addr);	
  else
    remote_ip_address = "";  
  Debug("Remote IP: %s\n",remote_ip_address.data());    

  memset(&old, 0, sizeof(old));
  memset(&ocs, 0, sizeof(ocs));
    
  return true;

}


bool do_stats()
{

  if(! get_ppp_stats(&cur)){
    return false;
  }

  ppp_stats_available = true;

  // "in"  "pack"  "comp"  "uncomp"  "err"
  // IN    PACK    VJCOMP  VJUNC     VJERR

  ibytes =   cur.p.ppp_ibytes; 			// bytes received
  ipackets =   cur.p.ppp_ipackets; 		// packets recieved
  compressedin =  cur.vj.vjs_compressedin; 	// inbound compressed packets
  uncompressedin =   cur.vj.vjs_uncompressedin; // inbound uncompressed packets
  errorin =  cur.vj.vjs_errorin; 		//receive errors

  //  "out"  "pack"  "comp"  "uncomp"  "ip"
  // OUT       PACK   JCOMP   VJUNC    NON-VJ

  obytes =  cur.p.ppp_obytes; 		       	// raw bytes sent
  opackets =  cur.p.ppp_opackets; 		// packets sent
  compressed =  cur.vj.vjs_compressed; 		//outbound compressed packets

  // outbound packets - outbound compressed packets
  packetsunc =  cur.vj.vjs_packets - cur.vj.vjs_compressed;

  // packets sent - oubount compressed
  packetsoutunc = cur.p.ppp_opackets - cur.vj.vjs_packets; 

  return true;
    
}


#ifndef __svr4__
bool get_ppp_stats(struct ppp_stats *curp){

    struct ifpppstatsreq req;

    if(s==0)
      return false;

#ifdef linux
    req.stats_ptr = (caddr_t) &req.stats;
#undef ifr_name
#define ifr_name ifr__name
#endif

    sprintf(req.ifr_name, "ppp%d", unit);
    if (ioctl(s, SIOCGPPPSTATS, &req) < 0) {
	if (errno == ENOTTY)
	    fprintf(stderr, "pppstats: kernel support missing\n");
	else
	    perror("ioctl(SIOCGPPPSTATS)");
	return false;
    }
    *curp = req.stats;
    return true;
}

bool get_ppp_cstats(    struct ppp_comp_stats *csp){

    struct ifpppcstatsreq creq;

    memset (&creq, 0, sizeof (creq));

#ifdef linux
    creq.stats_ptr = (caddr_t) &creq.stats;
#undef  ifr_name
#define ifr_name ifr__name
#endif

    sprintf(creq.ifr_name, "ppp%d", unit);
    if (ioctl(s, SIOCGPPPCSTATS, &creq) < 0) {
	if (errno == ENOTTY) {
	    fprintf(stderr, "pppstats: no kernel compression support\n");
	    if (cflag)
		return false;
	    rflag = 0;
	} else {
	    perror("ioctl(SIOCGPPPCSTATS)");
	    return false;
	}
    }

#ifdef linux
    if (creq.stats.c.bytes_out == 0)
	creq.stats.c.ratio = 0.0;
    else
	creq.stats.c.ratio = (double) creq.stats.c.in_count /
			     (double) creq.stats.c.bytes_out;

    if (creq.stats.d.bytes_out == 0)
	creq.stats.d.ratio = 0.0;
    else
	creq.stats.d.ratio = (double) creq.stats.d.in_count /
			     (double) creq.stats.d.bytes_out;
#endif

    *csp = creq.stats;
    return true;
}

#else	/* __svr4__ */
bool get_ppp_stats( struct ppp_stats *curp){

    if (strioctl(s, PPPIO_GETSTAT, curp, 0, sizeof(*curp)) < 0) {
	if (errno == EINVAL)
	    fprintf(stderr, "pppstats: kernel support missing\n");
	else
	    perror("pppstats: Couldn't get statistics");
	return false;
    }
}

bool get_ppp_cstats(    struct ppp_comp_stats *csp){

    if (strioctl(s, PPPIO_GETCSTAT, csp, 0, sizeof(*csp)) < 0) {
	if (errno == ENOTTY) {
	    fprintf(stderr, "pppstats: no kernel compression support\n");
	    if (cflag)
	      return false;
	    rflag = 0;
	} else {
	    perror("pppstats: Couldn't get compression statistics");
	    return false;
	}
    }
}

bool strioctl(int fd, int cmd, char* ptr, int ilen, int olen){

    struct strioctl str;

    str.ic_cmd = cmd;
    str.ic_timout = 0;
    str.ic_len = ilen;
    str.ic_dp = ptr;
    if (ioctl(fd, I_STR, &str) == -1)
	return false;
    if (str.ic_len != olen)
	fprintf(stderr, "strioctl: expected %d bytes, got %d for cmd %x\n",
	       olen, str.ic_len, cmd);
    return true;
}
#endif /* __svr4__ */

