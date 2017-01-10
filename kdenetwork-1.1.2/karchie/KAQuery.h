/* -*- c++ -*- */
/***************************************************************************
 *                                 KAQuery.h                               *
 *                            -------------------                          *
 *                         Header file for KArchie                         *
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

#ifndef KAQUERY_H
#define KAQUERY_H

/* includes from the archie source */
#include "perrno.h"
#include "pfs.h"

//#ifndef KARCHIE_H
//#include "rdgram.h"
//#endif
/* end archie includes */

#include <qobject.h>
#include <qdatetm.h>
//#include <qlist.h>
//#include <unistd.h>


#include "KAQueryList.h"
#include "KASettingsTypes.h"

#define CLIENT_MAX_HITS 95

class QSocketNotifier;

class KAQuery : public QObject
/* Query class to provide an archie query
 * result is a list of KAQueryFile
 */
{
Q_OBJECT
private:
  QString sError, sWarning;
  int nError, nWarning;

public:
  //  enum QueryType{ exact, substring, substringcase, regexp, exactsubstring,
  //		  exactsubstringcase, exactregexp };
  enum Flags{ none, notranslate };
  enum Sort{ nosort, host_filename, inv_date };

KAQuery( const char *host,
	 const char *search,
	 SearchMode::mode searchmode = SearchMode::exact,
	 int max_hits = CLIENT_MAX_HITS,
	 int max_retries = CLIENT_DIRSRV_RETRY,
	 NiceLevel::mode nicelevel = NiceLevel::norm,
	 int time_out = CLIENT_DIRSRV_TIMEOUT,
	 Sort sorting = nosort,
	 Flags flags = none );

  /* methods for starting the query
   * result is true for no errors or warnings */
  void start(); // get the first "max_hits" results
  void startover(); // get the next "max_hits" results

  inline const QString &getHost() const;
  inline const QString &getSearchString() const;
  inline int            getMaxHits() const;
  inline int            getMaxRetries() const;
  inline int            getCurrentTry() const;
  inline int            getTimeout() const;
  inline const QString &getWarning() const;
  inline const QString &getError() const;
  inline int            getErrorNo() const;
  inline bool           getSuccess() const;
  inline int            getProcessedResults() const;

  inline void setHost(QString &rHost);
  inline void setSearchString(QString &rSearch);
  inline void setQueryType(SearchMode::mode smode);
  inline void setNiceLevel(NiceLevel::mode nlevel);
  inline void setFlags(Flags eFlags);
  inline void setMaxHits(int maxhits);
  inline void setMaxRetries(int retries);
  inline void setTimeout(int time_out);

  // gives a deep copy of the searchresults
  // so we don't have to bother about deleting
  KAQueryList &getFileList() const;

signals:
  void sigQuerySetup();
  void sigQueryTryNum(int);
  void sigQueryResults(int);
  void sigQueryRunning();
  void sigQueryFinished();
  //  void sigNoHost();
  //  void sigHostNotFound();
  //  void sigNoQuerystring();
  //  void sigQueryNotFound();
  void sigWaitForQueryentry(); // emitted if more then one query running same time
  void sigQueryStatus(const char *);

public slots:
  inline void slotStart();
  inline void slotStartOver();
         void slotAbort();

private:
  /* set the errorNumbers to Strings in sError/sWarning
   * Strings from archie/perrno.h and archie/perrmessg.c */
  void errno2str();
  /* prozess one query */
  bool doQuery();

  
  QString host, search;
  int max_hits, max_retries, current_try, timeout;
  SearchMode searchmode;
  NiceLevel  nicelevel;
  Flags flags;
  Sort sorting;
  
  // for the query
  int processed_matches;
  VLINK queryresult;

private slots:
  void slotProcessQueryNow();


  /* The following is needed by dirsend.c
   * because of having only one query a time - using functions
   * doesn't allow for more than one query - these methods
   * can be static. in fact they have to, or else i have to have
   * a static KAQuery::this pointer
   * Not to use from other objects */
public:
  void initSocketNotifier(int socket);
  void removeSocketNotifier();
  void initTimeout(int msec);
  void removeTimeout();
protected:
  virtual void timerEvent (QTimerEvent *);
private slots:
  void slotSocketEvent(int socket);
private:
  QSocketNotifier *socketNotifier;
  int              timerID;
};

const QString &
KAQuery::getHost() const { return host; }

const QString &
KAQuery::getSearchString() const { return search; }

int
KAQuery::getMaxHits() const { return max_hits; }

int
KAQuery::getMaxRetries() const { return max_retries; }

int
KAQuery::getCurrentTry() const { return current_try; }

int
KAQuery::getTimeout() const { return timeout; }

const QString &     
KAQuery::getWarning() const { return sWarning; }

const QString &     
KAQuery::getError() const { return sError; }

int       
KAQuery::getErrorNo() const
 { return nError; }

bool      
KAQuery::getSuccess() const
 { return (nError==0); }

void   
KAQuery::setHost(QString &rHost)
 { host = rHost; }

void   
KAQuery::setSearchString(QString &rSearch)
 { search = rSearch; }

void   
KAQuery::setQueryType(SearchMode::mode smode)
 { searchmode = smode; }

void   
KAQuery::setNiceLevel(NiceLevel::mode nlevel)
 { nicelevel = nlevel; }

void   
KAQuery::setFlags(Flags eFlags)
 { flags = eFlags; }

void   
KAQuery::setMaxHits(int maxhits)
 { max_hits = maxhits; }

void   
KAQuery::setMaxRetries(int retries)
 { max_retries = retries; }

void   
KAQuery::setTimeout(int time_out)
 { timeout = time_out; }

void  
KAQuery::slotStart()
{ start(); }

void  
KAQuery::slotStartOver()
{ startover(); }

#endif
