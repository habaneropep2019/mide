/***************************************************************************
 *                                KAQuery.cpp                              *
 *                            -------------------                          *
 *                         Source file for KArchie                         *
 *                  -A programm to display archie queries                  *
 *                                                                         *
 *                KArchie is written for the KDE-Project                   *
 *                         http://www.kde.org                              *
 *                                                                         *
 *   Copyright (C) Oct 1997 Jörg Habenicht                                 *
 *                  E-Mail: j.habenicht@europemail.com                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          * 
 *                                                                         *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.             *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <qtimer.h>

#include "KAQuery.moc"
#include "KAQueryFile.h"
#include <kapp.h>
#include <qsocknot.h> 

extern "C" {

  //#include "archie.h"

  //#include "rdgram.h" /* was in archie */

  /* from archie.h */
  typedef enum query_type
    {
      NONE = '\0', EXACT = '=', REGEXP = 'R',
      SUBSTRING = 'S', SUBSTRING_CASE = 'C',
      EXACTREGEXP = 'r', EXACTSUBSTRING = 's',
      EXACTSUBSTRING_CASE = 'c'
    } Query;

    /* from dirsend.c */
    typedef char *XtPointer;
    typedef char *XtInputId;
    typedef char *XtIntervalId;

    /* forward decl for aquery.c
     * !!! WARNING: this declaration doesn't represent
     * the declaration in aquery.c:
     * VLINK archie_query(char*,char*,int,int,Query,int(*)(),int);
     *                                                   -^^-
     * but represents the real declaration of the cmp_prg(VLINK,VLINK) */
    extern VLINK archie_query(char*,char*,int,int,Query,int(*)(VLINK,VLINK),int);
    extern int defcmplink(VLINK,VLINK);
    extern int invdatecmplink(VLINK,VLINK);
    extern void vllfree(VLINK);

    /* Definitions for the comparison procedures                           */
#define AQ_DEFCMP       defcmplink
#define AQ_INVDATECMP   invdatecmplink

    /* Flags                                                               */
#define AQ_NOSORT       0x01    /* Don't sort                          */
#define AQ_NOTRANS      0x02    /* Don't translate Archie responses    */
    /* end archie.h */

    /* functions for xarchie 
    void status0() {};
    void status1() {};
    void status2() {};
    */

    /* extern variables defined for archiequery */
    int pwarn;
    char p_warn_string[P_ERR_STRING_SZ];
    int perrno;
    char p_err_string[P_ERR_STRING_SZ];
    int pfs_debug;
    int verbose; /* verbose level: 1 verbose, 0 non-verbose */

    /* from dirsend.c */
    int client_dirsrv_timeout = CLIENT_DIRSRV_TIMEOUT;
    int client_dirsrv_retry = CLIENT_DIRSRV_RETRY; 
    int rdgram_priority = 0;
    int packetCounter;

    /* end extern variables */

    /* needed for functions below */
    extern void timeoutProc(XtPointer,XtIntervalId *);
    extern void readProc(XtPointer,int *,XtInputId *);
    extern void abortDirsend();
    KAQuery *currentQuery;

    /* function called from dirsend.c */
    void
      KAQuery_processOneEvent() {
	qApp->processOneEvent();
      };

    void 
      initSocketNotifier(int socket) {
	currentQuery->initSocketNotifier(socket);
      }

    void
      removeSocketNotifier() {
	currentQuery->removeSocketNotifier();
      }

    void
      initTimeout(int msec) {
	currentQuery->initTimeout(msec);
      }

    void
      removeTimeout() {
	currentQuery->removeTimeout();
      }

    void
      timeoutproc() {
	timeoutProc(0, 0);
      }

    void
      socketevent(int socket) {
      readProc(0, (int*)socket, 0);
    }
    
}

/* Semaphore for doing the query one at a time
 * the function archie_query cant do multithreading */
bool bProcessQuery = FALSE;


KAQuery::KAQuery( const char *host, const char *search, SearchMode::mode smode, int max_hits, int max_retries, NiceLevel::mode nlevel, int time_out, Sort sorting, Flags flags )
  : host( host ),
    search( search ),
    max_hits( max_hits ),
    max_retries( max_retries ),
    timeout( time_out ),
    searchmode( smode ),
    nicelevel( nlevel ),
    flags( flags ),
    sorting( sorting ),
    processed_matches( 0 ),
    queryresult( 0 ),
    socketNotifier( 0 )
{
  verbose = 0;
}


void 
KAQuery::start()  // get the first "max_hits" results
{
  processed_matches = 0;
  startover();
}


void 
KAQuery::startover() // get the next "max_hits" results
{
  emit sigQuerySetup();
  emit sigQueryStatus(i18n("Query setup"));

  current_try = 1; // erster Versuch
  QTimer::singleShot( 300, this, SLOT(slotProcessQueryNow()) );

  //  bool result = FALSE;
  //  int i;

  // solange kein ergebniss vorliegt
  // && der fehler ein timeout war
  // && wir noch nicht die max. wiederholungen haben
  // probieren wir nochmal
  /*  for ( i=1; i<=max_retries; i++ ) {
    emit sigQueryTryNum( i );
    result = doQuery();
    if ( result || 5!=perrno )
      break;
  }
  
  // check the result
  errno2str();
  //sError = p_err_string;
  //  debug( p_err_string );
  nError = perrno;
  //  QString tmp;
  //  debug( (const char*)tmp.setNum(nError) );
  //  sWarning = p_warn_string;
  //  debug( p_warn_string );
  nWarning = pwarn;
  //debug( tmp.setNum(nWarning) );
  return result;
  */
}

int
KAQuery::getProcessedResults() const
{
  return packetCounter;
}

KAQueryList &
KAQuery::getFileList() const
{
  KAQueryFile *file;
  KAQueryList *filelist = new KAQueryList;
  QDate date;
  QTime time;
  //  QDateTime datetime;
  VLINK tmplink = queryresult;
  /* (from archie/procquery.c)         */
  PATTRIB     ap;
  int         size = 0;
  char        *modes = "";
  char        *gt_date = "";
  int         gt_year = 0;
  int         gt_mon = 0;
  int         gt_day = 0;
  int         gt_hour = 0;
  int         gt_min = 0;

  while (tmplink != 0) {
    /* Parse the attibutes of this link
     * (from archie/procquery.c)         */
    for (ap = tmplink->lattrib; ap; ap = ap->next) {
	if (strcmp(ap->aname,"SIZE") == 0) {
#ifdef MSDOS
	  sscanf(ap->value.ascii,"%lu",&size);
#else
	  sscanf(ap->value.ascii,"%d",&size);
#endif
	} else if(strcmp(ap->aname,"UNIX-MODES") == 0) {
	    modes = ap->value.ascii;
	} else if(strcmp(ap->aname,"LAST-MODIFIED") == 0) {
	    gt_date = ap->value.ascii;
	    sscanf(gt_date,"%4d%2d%2d%2d%2d", &gt_year,
		   &gt_mon, &gt_day, &gt_hour, &gt_min);
	    /*
	    if ((12 * (presenttime->tm_year + 1900 - gt_year) + 
					presenttime->tm_mon - gt_mon) > 6) 
		sprintf(archie_date,"%s %2d %4d",month_sname(gt_mon),
			gt_day, gt_year);
	    else
		sprintf(archie_date,"%s %2d %02d:%02d",month_sname(gt_mon),
			 gt_day, gt_hour, gt_min);
			 */
	}
    }
    
    date.setYMD(gt_year, gt_mon, gt_day);
    time.setHMS(gt_hour, gt_min, 0);
    //datetime.setTime( time );
    //datetime.setDate( date );
    QString host(tmplink->host);
    QString path(tmplink->filename);
    int slash_index = path.findRev('/');
    QString filename( path.right( path.length() - slash_index -1 ));
    path.resize( slash_index +2 );
    file = new KAQueryFile( host, path, filename, size, modes, date, time );
    filelist->append( file );
    tmplink = tmplink->next;
  }

  return *filelist;
}

void 
KAQuery::slotAbort()
{
  abortDirsend();
  emit sigQueryStatus(i18n("Query aborted"));
}

void 
KAQuery::errno2str()
{
  switch(perrno) {
  case 0: sError = i18n("Success"); break;
  case 1: sError = i18n("Port unknown"); break;
  case 2: sError = i18n("Can't open local UDP port"); break;
  case 3: sError = i18n("Can't resolve hostname"); break;
  case 4: sError = i18n("Didn't send entire message"); break;
  case 5: sError = i18n("Timed out"); break;
  case 6: sError = i18n("Recvfrom failed"); break;
  case 11: sError = i18n("Reply: sendto failed"); break;
  case 22: sError = i18n("Link exists with same name"); break;
  case 21:
  case 25: sError = i18n("Link already exists"); break;
  case 26: sError = i18n("Replacing existing link"); break;
  case 27: sError = i18n("Prv entry not in dir->ulinks"); break;
  case 41: sError = i18n("Temporary not found"); break;
  case 42: sError = i18n("Namespace not closed w/ object::"); break;
  case 43: sError = i18n("No alias for namespace NS#:"); break;
  case 44: sError = i18n("Specified namespace not found"); break;
  case 51: sError = i18n("File access method not supported"); break;
  case 55: sError = i18n("Delete cached copy on close"); break;
  case 56: sError = i18n("Unable to retrieve file"); break;
  case 71:
  case 61: sError = i18n("Directory already exists"); break;
  case 72:
  case 62: sError = i18n("Link with name already exists"); break;
  case 65: sError = i18n("Not a virtual system"); break;
  case 66: sError = i18n("Can't find directory"); break;
  case 111: sError = i18n("Not a directory name"); break;
  case 121: sError = i18n("Object not found"); break;
  case 122: sError = i18n("Object has moved"); break;
  case 230: sError = i18n("File not found"); break;
  case 231: sError = i18n("Directory in path not found"); break;
  case 232: sError = i18n("Symbolic links nested too deep"); break;
  case 233: sError = i18n("Can't read environment"); break;
  case 234: sError = i18n("Can't traverse an external file"); break;
  case 235: sError = i18n("Exceeded max forward depth"); break;
  case 242: sError = i18n("Authentication required"); break;
  case 243: sError = i18n("Not authorized"); break;
  case 244: sError = i18n("Not found"); break;
  case 245: sError = i18n("Bad version number"); break;
  case 246: sError = i18n("Not a directory"); break;
  case 247: sError = i18n("Identical link already exists"); break;
  case 248: sError = i18n("Link with name already exists"); break;
  case 251: sError = i18n("Command not implemented on server"); break;
  case 252: sError = i18n("Bad format for response"); break;
  case 253: sError = i18n("Protocol error"); break;
  case 254: sError = i18n("Unspecified server failure"); break;
  case 255:
  default: sError = i18n("Generic Failure"); break;
  }

  switch(pwarn) {
  case 0: sWarning = i18n("No warning"); break;
  case 1: sWarning = i18n("You are using an old version of this program"); break;
  case 2: sWarning = i18n("From server: "); sWarning += p_warn_string; break;
  case 3:
  default: sWarning = i18n("Unrecognized line in response from server"); break;
  }
}

bool 
KAQuery::doQuery()
{
  char *cHostname = qstrdup( host ); // some "backups" because archie_query
  char *cSearch = qstrdup( search ); // has no const parameters
  int offset = processed_matches;
  int aqflag = 0; // nosort or notranslate ?
  int (*sortfunc)(VLINK,VLINK);
  Query aquery = (Query)searchmode.getChar();

  //  debug("KAQuery::doQuery\nHostname: %s\nSearchstring: %s\nMax Hits: %i\nQueryoffset: %i\nSearchmode: %c",cHostname, cSearch, max_hits, offset, aquery);
  /*
  switch (searchmode) {
  case exactsubstring:
  case substring: aquery = SUBSTRING; break;
  case exactsubstringcase:
  case substringcase: aquery = SUBSTRING_CASE; break;
  case exactregexp:
  case regexp: aquery = REGEXP; break;
  case exact: aquery = EXACT; break;
  }
  */

  switch (flags) {
  case none: aqflag = 0x0; break;
    //case nosort: aqflag = AQ_NOSORT; break;
  case notranslate: aqflag = AQ_NOTRANS; break;
    //  case nosorttranslate: aqflag = AQ_NOSORT|AQ_NOTRANS; break;
  }

  switch (sorting) {
  case host_filename: sortfunc = AQ_DEFCMP; break;
  case inv_date: sortfunc = AQ_INVDATECMP; break;
  case nosort: // fall through
  default: aqflag = aqflag | AQ_NOSORT; sortfunc = AQ_DEFCMP; break;
  }

  rdgram_priority = nicelevel.toLevel();
  client_dirsrv_timeout = timeout;
  client_dirsrv_retry = max_retries;

  // if there is an old list, delete the entire list
  if (queryresult)
    vllfree(queryresult);

  if (bProcessQuery) {
    debug(" another query running");
    emit sigWaitForQueryentry();
    while (bProcessQuery) sleep (1);
  }
  currentQuery = this;
  bProcessQuery = TRUE;
  emit sigQueryTryNum( current_try );
  emit sigQueryRunning();
  emit sigQueryStatus(i18n("Query running"));
  queryresult = archie_query( cHostname, cSearch, max_hits, offset, aquery, sortfunc, aqflag );

  bProcessQuery = FALSE;

  delete cHostname;
  delete cSearch;
  if ( queryresult ) {
    processed_matches = packetCounter; // save the number of returned entries
    return TRUE;
  }
  // query has failed or has been aborted.
  processed_matches = 0;
  emit sigQueryResults(0);
  return FALSE;
}

void 
KAQuery::slotProcessQueryNow()
{
  /* used to link a timer event to this slot
   * needed for asynchronous query
   */
  if ( !doQuery() && 5==perrno && current_try<=max_retries ) {
    errno2str();
    emit sigQueryStatus( sError );
    current_try++;
    QTimer::singleShot( 300, this, SLOT(slotProcessQueryNow()) );
    return;
  }

  // check the result
  errno2str();
  //sError = p_err_string;
  //  debug( p_err_string );
  nError = perrno;
  //  QString tmp;
  //  debug( (const char*)tmp.setNum(nError) );
  //  sWarning = p_warn_string;
  //  debug( p_warn_string );
  nWarning = pwarn;
  //debug( tmp.setNum(nWarning) );

  emit sigQueryFinished();
  emit sigQueryStatus(i18n("Query finished"));
}

void  
KAQuery::initSocketNotifier(int socket)
{
  socketNotifier = new QSocketNotifier(socket, QSocketNotifier::Read, this);
  connect(socketNotifier, SIGNAL(activated(int)), SLOT(slotSocketEvent(int)));
}

void  
KAQuery::removeSocketNotifier()
{
  if (socketNotifier) {
    delete socketNotifier;
    socketNotifier = 0;
  }
}

void  
KAQuery::initTimeout(int msec)
{
  timerID = startTimer(msec);
}

void  
KAQuery::removeTimeout()
{
  killTimer(timerID);
}

void 
KAQuery::timerEvent (QTimerEvent *)
{
  killTimer(timerID);
  timeoutproc();
}

void
KAQuery::slotSocketEvent(int socket)
{
  socketevent(socket);
  emit sigQueryResults(getProcessedResults());
}
