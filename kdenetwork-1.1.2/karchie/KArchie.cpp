/***************************************************************************
 *                                KArchie.cpp                              *
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

#include <qfiledlg.h>

//#include "KArchie.h"
#include "KArchie.moc"

#include "KAMenu.h"
#include "KAStatusBar.h"
#include "KASettings.h"
#include "KASettingsTypes.h"
#include "KAView.h"
#include "KAQuery.h"
#include "KAQueryFile.h"

#include <kapp.h>
#include <kconfig.h>
#include <kmsgbox.h> 
#include <kfm.h>
#include <kprocess.h>
#include <kfiledialog.h>

#include <qdstream.h> 

#include <fstream.h>

#define KFM_WINDOW "kfmclient"
#define KFM_OPEN_URL "openURL"

TmpOpenFile::~TmpOpenFile()
{
  

}



KArchie::KArchie(const char *name)
  : KTopLevelWidget (name),
    queryResult( 0 )//,   downloadTmpFileList( 0 )
{
  setMinimumSize(420,200);
  config = KApplication::getKApplication()->getConfig();
  QString currenthost;
  QStrList archiehostlist;
  //  int archiehostlistnumber = 
  KConfigGroupSaver *saveGroup = new KConfigGroupSaver( config, "HostConfig" );

  config->readListEntry( "Hosts", archiehostlist );
  //  QString currenthost = config->readEntry( "CurrentHost", "archie.sura.net" );
  uint currentHostId = config->readUnsignedNumEntry( "CurrentHostId", 0 );
  QString defaulthost = "archie.sura.net" ;
  if ( archiehostlist.isEmpty() ) {
    archiehostlist.append( defaulthost );
    currentHostId = 0;
    //    archiehostlistnumber++;
  }
  else if (archiehostlist.count() < currentHostId) {
    currentHostId = 0;
  }    
  currenthost = archiehostlist.at(currentHostId);
  //  config->setGroup( ConfigEntries::HostConfigGroup );
  //  currenthost = config->readEntry( ConfigEntries::CurrentHostEntry,
  //				   ConfigEntries::CurrentHostDefault );

  //    debug("setup menu");
  menu = new KAMenu( this, "mainmenu" );
  setMenu( menu );
  connect( menu, SIGNAL(sigSettingsAll()),
	   this, SLOT(slotChangeSettings()) );
  connect( menu, SIGNAL(sigFileOpen()),
	   this, SLOT(slotOpenFileSelected()) );
  connect( menu, SIGNAL(sigFileOpenDir()), 
	   this, SLOT(slotOpenDirSelected()) );
  connect( menu, SIGNAL(sigFileGet()), 
	   this, SLOT(slotGetfileSelected()) );
  connect( menu, SIGNAL(sigFileLoadList()), 
	   this, SLOT(slotLoadfilelistSelected()) );
  connect( menu, SIGNAL(sigFileStoreList()), 
	   this, SLOT(slotStorefilelistSelected()) );
  connect( menu, SIGNAL(sigFileWriteList()), 
	   this, SLOT(slotWritefilelistSelected()) );
  connect( menu, SIGNAL(sigQueryFile()), 
	   this, SLOT(slotSearchFile()) );
  connect( menu, SIGNAL(sigQueryPath()), 
	   this, SLOT(slotSearchPath()) );
  connect( menu, SIGNAL(sigQueryHost()), 
	   this, SLOT(slotSearchHost()) );

  connect( menu, SIGNAL(sigSortHostname()), 
	   this, SLOT(slotSortListHostname()) );
  connect( menu, SIGNAL(sigSortDomain()), 
	   this, SLOT(slotSortListDomain()) );  
  connect( menu, SIGNAL(sigSortDate()), 
	   this, SLOT(slotSortListDate()) );
  connect( menu, SIGNAL(sigSortFilesize()), 
	   this, SLOT(slotSortListFilesize()) );
  menu->show();
  menu->setFileGetEnable( FALSE );
  menu->setFileOpenEnable( FALSE );
  menu->setFileOpenDirEnable( FALSE );
  menu->setFileStoreListEnable( FALSE );
  menu->setFileWriteListEnable( FALSE );
  menu->setSortEnable( FALSE );

  //    debug("setup statusbar");
  statbar = new KAStatusBar( this, "statusbar" );
  setStatusBar( statbar );
  statbar->slotChangeHost( currenthost );
  statbar->show();
  connect( menu, SIGNAL(sigArchieHost(const char *)), statbar, SLOT(slotChangeHost(const char *)) );

  delete saveGroup;
  saveGroup = new KConfigGroupSaver( config, "WindowConfig" );

  //  debug( "setup view" );
  view = new KAView( this, "view" );
  setView( view );
  //  view->slotShowFileDiscriptor(config->readBoolEntry("FAttr", true));
  view->show();

  connect( menu, SIGNAL(sigSettingsShowFileDiscription( bool )),
	   view, SLOT(slotShowFileDiscriptor( bool )) );
  connect( &(view->getSearchterm()),
	   SIGNAL(sigTextSelected()),
	   SLOT(slotSearchFile()) );
  connect( &(view->getList()),
	   SIGNAL(sigOpenFileSelected()),
	   SLOT(slotOpenFileSelected()) );
  connect( &(view->getList()),
	   SIGNAL(sigOpenDirSelected()),
	   SLOT(slotOpenDirSelected()) );
  connect( &(view->getList()),
	   SIGNAL(sigGetFileSelected()),
	   SLOT(slotGetfileSelected()) );

  show();

  //  changeSettings();
  delete saveGroup;
}

KArchie::~KArchie()
{
  delete menu;
  delete statbar;
  delete view;
  if (queryResult){ // alte Liste loeschen
    queryResult->setAutoDelete(TRUE);
    delete queryResult;
  }
  /*
  if (downloadTmpFileList) {
    downloadTmpFileList->setAutoDelete(TRUE);
    delete downloadTmpFileList;
  }*/
  //   delete config;
}

void 
KArchie::slotChangeSettings()
{
  //  debug( "KArchie::slotChangeSettings request" );
  changeSettings();
}

void 
KArchie::slotSearchFile()
{ //debug("KArchie::slotSearchFile");
  slotQuerySearch('f'); }
void 
KArchie::slotSearchPath()
{ slotQuerySearch('p'); }
void 
KArchie::slotSearchHost()
{ slotQuerySearch('h'); }

void 
KArchie::slotQuerySearch(char type)
{
  //  debug("KArchie::slotQuerySearch");
  QStrList archiehostlist;
  KConfigGroupSaver *saveGroup = new KConfigGroupSaver( config, "HostConfig" );

  config->readListEntry( "Hosts", archiehostlist );

  uint currentHostId = config->readUnsignedNumEntry( "CurrentHostId", 0 );
  QString defaulthost = "archie.sura.net" ;
  if ( archiehostlist.isEmpty() ) {
    archiehostlist.append( defaulthost );
    currentHostId = 0;
    //    archiehostlistnumber++;
  }
  else if (archiehostlist.count() < currentHostId) {
    currentHostId = 0;
  }    

  QString host(archiehostlist.at(currentHostId));
  int retries = config->readNumEntry("Retries", CLIENT_DIRSRV_RETRY);
  int timeout = config->readNumEntry("Timeout", CLIENT_DIRSRV_TIMEOUT);

  delete saveGroup;

  saveGroup = new KConfigGroupSaver( config, "SearchConfig" );

  const QString searchInput = config->readEntry("Search", "=");
  SearchMode searchmode(((const char*)searchInput)[0]);
  NiceLevel nicelevel = NiceLevel::toLevel(config->readNumEntry("Nice", (int)NiceLevel::norm));
  int maxhits = config->readNumEntry("hits", CLIENT_MAX_HITS);

  delete saveGroup;

  //  debug( host );
  QString search;
  switch (type) {
  case 'p': search = "path"; break;
  case 'h': search = "host"; break;
  case 'f': // fall through
  default:  search = "file"; break;
  }
  //  debug( search );

  query = new KAQuery((const char *)host,
		      view->getSearchterm().getText(),
		      searchmode.getMode(),
		      maxhits,
		      retries,
		      nicelevel.getLevel(),
		      timeout);

  connect( query, SIGNAL(sigQueryRunning()),
	   menu, SLOT(slotQueryBegin()) );
  connect( query, SIGNAL(sigQueryFinished()),
	   menu, SLOT(slotQueryEnd()) );
  connect( query, SIGNAL(sigQueryStatus(const char *)),
	   statbar, SLOT(slotChangeStatus(const char *)) );
  connect( query, SIGNAL(sigQueryTryNum(int)),
	   statbar, SLOT(slotChangeTries(int)) );
  connect( query, SIGNAL(sigQueryResults(int)),
	   statbar, SLOT(slotChangeHits(int)) );
  connect ( query, SIGNAL(sigQueryFinished()),
	    this, SLOT(slotReturnQuery()) );
  connect ( menu, SIGNAL(sigQueryStop()),
	    query, SLOT(slotAbort()) );

  // start periodic windowupdates
  //  startTimer(1000);

  QString tmp(search + " query start");
  statbar->slotChangeStatus( tmp );

  // start timer for doing other things while querying
  queryTimer = startTimer(300);

  query->start();

  /*
  if (bSuccess) {
    if (queryResult){ // alte Liste loeschen
      queryResult->setAutoDelete(TRUE);
      delete queryResult;
    }
    statbar->slotChangeStatus( "Success" );
    debug("getfilelist");
    queryResult = &(query->getFileList());
    statbar->slotChangeHits( queryResult->count() );
    debug("displayfilelist");
    view->newFileList( *queryResult );

    bool listitemhighlighted = -1 != view->getList().currentItem();
    menu->setFileOpenEnable( listitemhighlighted );
    menu->setFileOpenDirEnable( listitemhighlighted );
    menu->setFileGetEnable( listitemhighlighted );
    menu->setFileWriteListEnable( view->getList().count > 0 );
  }
  else { // print status on SLine
    const char *cError = query->getError();
    debug( cError );
    statbar->slotChangeStatus( cError );
    statbar->slotChangeHits( 0 );

    // no changes to the view->viewlist
    // so we dont change the filemenu
  }

  //  killTimers();
  // if there is an old list from previous queries
  // enable/disable the file->get menu
  //  QString tmp;
  //  tmp.setNum( view->getList().currentItem() );
  //  tmp += " current list item";
  //  debug( tmp );
  debug("ready");
  delete query;
  */
}

void  
KArchie::slotOpenFileSelected()
{
  QString downloadfile;
  /*
  if (!downloadTmpFileList) {
    downloadTmpFileList = new TmpOpenFileList;
  }*/
  //  KProcess *process = new KProcess;

  //  debug ("file open");
  KURL url("file:/usr/local/kde/bin/kfmclient");
  openFile(url);
}

void  
KArchie::slotOpenDirSelected()
{
  //  debug ("dir open");
  //  KURL url("file:/usr/local/kde/");
  //  openDir(url);
  const KAQueryFile *file = checkViewSelection();
  if (file) {
    slotOpenDirSelected(file);
  }
}

void 
KArchie::slotGetfileSelected()
{
  const KAQueryFile *file = checkViewSelection();
  if (file) {
    slotGetfileSelected(file);
  }
}

void  
KArchie::slotOpenFileSelected(const KAQueryFile *file)
{
  KURL url;
  if (checkFtpFile(url,file))
    openFile(url);
}

void  
KArchie::slotOpenDirSelected(const KAQueryFile *file)
{
  KURL url;
  if (checkFtpPath(url,file))
    openDir(url);
}

void  
KArchie::slotGetfileSelected(const KAQueryFile *file)
{
  KURL url;
  if (checkFtpFile(url,file))
    getFile( url );
}

void  
KArchie::slotLoadfilelistSelected()
{
  QString loadfile = KFileDialog::getOpenFileName( 0, "*", this );
  if ( !loadfile.isEmpty() ) {
    // datei oeffnen die Liste abspeichern
    KAQueryFile *qrfile;
    QFile file(loadfile);

    if ( file.open(IO_ReadOnly)) {
      int i, count; // = queryResult->count();
      QDataStream data(&file);
      data >> count;
      //debug("loading %i items", count);
      //debug("queryResult is %ld", (long)queryResult);
      if ( queryResult ) {
	//debug("enable autodelete querylist");
	queryResult->setAutoDelete( TRUE );
	//debug("deleting querylist");
	delete queryResult;
      }
      queryResult = new KAQueryList;
      for ( i=0; i<count; i++ ) {
	//    fprintf(stderr,"[%i] ", queryResult->at());
	//      qrfile = queryResult->at(i);
	qrfile = new KAQueryFile;
	data >> *qrfile;
	//debug ("Item %s%s%s", (const char *)qrfile->getHost(), (const char *)qrfile->getPath(), (const char *)qrfile->getFile());
	queryResult->append(qrfile);
      }
      file.close();
      //debug("put new filelist");
      view->newFileList(*queryResult);
      //debug("setting menu:");
      if (view->getList().count() > 0) {
	// list contains items
	//debug("\tsetstore");
	menu->setFileStoreListEnable( TRUE );
	//debug("\tsetwrite");
	menu->setFileWriteListEnable( TRUE );
	//debug("\tsetsort");
	menu->setSortEnable( TRUE );

	if (-1 != view->getList().currentItem()) {
	  // list has item selected/highlighted
	  //debug("\tsetopen");
	  menu->setFileOpenEnable( TRUE );
	  //debug("\tsetopendir");
	  menu->setFileOpenDirEnable( TRUE );
	  //debug("\tsetget");
	  menu->setFileGetEnable( TRUE );
	}
      }
      //debug ("loading ready");
    }
    else {
      QString errstr = i18n("Couldn't open \n\"%s\"");
      QString tmp(loadfile.length() + errstr.length() + 1);

      tmp.sprintf((const char *)errstr, (const char *)loadfile);
      warning("%s", (const char *)tmp);
      KMsgBox::message( this, i18n("KArchie: bad file"), tmp,
			KMsgBox::EXCLAMATION, i18n("OK") );
    }
  }  
}

void  
KArchie::slotStorefilelistSelected()
{
  if ( !queryResult )
    return;

  QString storefile = KFileDialog::getSaveFileName( 0, "*", this );
  if ( !storefile.isEmpty() ) {
    if (!checkSaveFile(storefile))
	return; // please don't overwrite

    // datei oeffnen die Liste abspeichern

    KAQueryFile *qrfile;
    QFile file(storefile);
    int count = queryResult->count();

    if ( file.open(IO_WriteOnly | IO_Truncate)) {
      QDataStream data(&file);

      data << count;
      for ( qrfile = queryResult->first();
	    qrfile;
	    qrfile = queryResult->next() ) {
	//    fprintf(stderr,"[%i] ", queryResult->at());
	//      qrfile = queryResult->at(i);
	data << *qrfile;
      }
      file.close();
    }
    else {
      QString errstr = i18n("Couldn't open \n\"%s\"");
      QString tmp(storefile.length() + errstr.length() + 1);

      tmp.sprintf((const char *)errstr, (const char *)storefile);
      warning("%s", (const char *)tmp);
      KMsgBox::message( this, i18n("KArchie: bad file"), tmp,
			KMsgBox::EXCLAMATION, i18n("OK") );
    }
  }  
}

void 
KArchie::slotWritefilelistSelected()
{
  if ( !queryResult )
    return;

  QString savefile = KFileDialog::getSaveFileName( 0, "*.txt", this );
  if ( !savefile.isEmpty() ) {
    if (!checkSaveFile(savefile))
	return; // please don't overwrite

    // datei oeffnen die Liste abspeichern
    KAQueryFile *qrfile;
    ofstream file(savefile);
    int i, count = queryResult->count();

    for ( i=0; i<count; i++ ) {
      //    fprintf(stderr,"[%i] ", queryResult->at());
      qrfile = queryResult->at(i);
      file << qrfile->getHost() << qrfile->getPath()
	   << qrfile->getFile() << endl;
    }
    file.close();
  }  
}

void 
KArchie::slotSortListHostname()
{
  // mache eine neue Liste aus der alten
  // und gib sie der Viewlist zur Darstellung.
  // danach löschen.

  if (queryResult) {
    KAHostList *sortList = new KAHostList;
    KAQueryFile *file;

    int i, count = queryResult->count();
    for ( i=0; i<count; i++ ) {
      file = queryResult->at( i );
      // binaer einsortieren
      sortList->inSort( file );
    }
    KAQueryList *tmp = queryResult;
    queryResult = sortList;
    tmp->setAutoDelete(FALSE);
    delete tmp;

    view->newFileList( *sortList );
  }
}

void 
KArchie::slotSortListDomain()
{
  if (queryResult) {
    KADomainList *sortList = new KADomainList;
    KAQueryFile *file;

    int i, count = queryResult->count();
    for ( i=0; i<count; i++ ) {
      file = queryResult->at( i );
      // binaer einsortieren
      sortList->inSort( file );
    }
    KAQueryList *tmp = queryResult;
    queryResult = sortList;
    tmp->setAutoDelete(FALSE);
    delete tmp;

    view->newFileList( *sortList );
  }
}

void 
KArchie::slotSortListDate()
{
  if (queryResult) {
    KADateList *sortList = new KADateList;
    KAQueryFile *file;

    int i, count = queryResult->count();
    for ( i=0; i<count; i++ ) {
      file = queryResult->at( i );
      // binaer einsortieren
      sortList->inSort( file );
    }
    KAQueryList *tmp = queryResult;
    queryResult = sortList;
    tmp->setAutoDelete(FALSE);
    delete tmp;

    view->newFileList( *sortList );
  }
}

void 
KArchie::slotSortListFilesize()
{
  if (queryResult) {
    KASizeList *sortList = new KASizeList;
    KAQueryFile *file;

    int i, count = queryResult->count();
    for ( i=0; i<count; i++ ) {
      file = queryResult->at( i );
      // binaer einsortieren
      sortList->inSort( file );
    }
    KAQueryList *tmp = queryResult;
    queryResult = sortList;
    tmp->setAutoDelete(FALSE);
    delete tmp;

    view->newFileList( *sortList );
  }
}

void 
KArchie::timerEvent( QTimerEvent * )
{
  //  repaint();
  //  statbar->slotChangeHits(query->getProcessedResults());
}

void 
KArchie::changeSettings()
{
  KASettings *dialog = new KASettings( this, "setup", TRUE );
  //  connect(dialog, SIGNAL(applyButtonPressed()), menu, SLOT(slotConfigChanged()) );
  //  dialog->setMinimumSize( 520, 460 );
  if (dialog->exec()) {
    statbar->slotConfigChanged();
    menu->slotConfigChanged();
  }

  delete dialog;
}

void 
KArchie::getFile(const KURL &url)
{
  KURL myurl(url);
  QString tmp;
  tmp = "./";
  tmp += myurl.filename();
  QString savefile = KFileDialog::getSaveFileName( tmp, 0, this );
  if ( !savefile.isEmpty() ) {
    if (!checkSaveFile(savefile))
	return; // please don't overwrite
    // ok, lets copy ftp://url to file://savefile
    KFM kfm;
    QString tmp( "file://" );
    tmp += savefile;
    kfm.copy( url.url(), tmp );
  }
}

void  
KArchie::openFile(const KURL &url)
{
  // currently doesn't work
  KFM kfm;
  kfm.openURL(url.url());
}

void  
KArchie::openDir(const KURL &url)
{
  // Prozess erstellen
  KProcess p;
  // .. mit Daten fuettern
  //  debug("starting process: %s %s %s", KFM_WINDOW, KFM_OPEN_URL, (const char*)url.url());
  p << KFM_WINDOW << KFM_OPEN_URL << (const char*)url.url();
  // .. und durchstarten
  p.start(KProcess::DontCare, KProcess::NoCommunication);

  // hoping the process is still running, when I get out of here
}

bool  
KArchie::checkFtpFile(KURL &url, const KAQueryFile *file)
{
  QString tmpstr("ftp://");
  tmpstr += file->getHost();
  tmpstr += file->getPath();
  tmpstr += file->getFile();

  // make an url of the data and check for correctness
  url = tmpstr;
  return checkUrl(url);

}

bool  
KArchie::checkFtpPath(KURL &url, const KAQueryFile *file)
{
  QString tmpstr("ftp://");
  tmpstr += file->getHost();
  tmpstr += file->getPath();
  //  tmpstr += file->getFile();

  // make an url of the data and check for correctness
  url = tmpstr;
  return checkUrl(url);

}

bool  
KArchie::checkFtpHost(KURL &url, const KAQueryFile *file)
{
  QString tmpstr("ftp://");
  tmpstr += file->getHost();
  //  tmpstr += file->getPath();
  //  tmpstr += file->getFile();

  // make an url of the data and check for correctness
  url = tmpstr;
  return checkUrl(url);

}

bool  
KArchie::checkUrl(const KURL &url)
{
  if (url.isMalformed() ) {
    QString tmp( i18n("Malformed url"));
    tmp += "\n\"";
    tmp += url.url();
    tmp += "\"\n";
    tmp += i18n("Maybe bad list entry, please select an other file");
    KMsgBox::message( this, i18n("KArchie: bad URL"), tmp,
		      KMsgBox::EXCLAMATION, i18n("OK") );
    return FALSE;
  }
  return TRUE;
}

bool 
KArchie::checkSaveFile(QString filename)
{
  QFileInfo file(filename);
  
  if (!file.exists()) {
    return TRUE; // Datei nicht vorhanden: kann angelegt werden.
  }
  // file exists..
  
  if (!file.isWritable())  {
    KMsgBox::message(this,
		     i18n("KArchie: Wrong fileselection"),
		     i18n("The selected file is not writeable"),
		     KMsgBox::EXCLAMATION,
		     i18n("OK"));
    return FALSE;
  }
  // file exists AND is writeable -> ask for permission

  return (2 == KMsgBox::yesNo(this, i18n("KArchie: Overwrite file ?"),
			      i18n("The selected file is not write protected\n"
				   "Should it be overwritten ?"),
			      KMsgBox::QUESTION,
			      i18n("No"), i18n("Yes")));
}


const KAQueryFile *
KArchie::checkViewSelection()
{
  // get the selected file from the viewlist
  int selected = view->getList().currentItem();
  if (-1 == selected) {
    KMsgBox::message( this, i18n("KArchie: bad selection"),
		      i18n("No file selected !\n"
			   "Please select a file in the list of queryresults\n"
			   "or do a query."),
		      KMsgBox::EXCLAMATION, i18n("OK") );
    return 0;
  }

  // get the fileentry from the filelist
  //  KAQueryFile *file = 
  return queryResult->at( selected );

}


void 
KArchie::slotReturnQuery()
{
  // no need to look for queryhits anymore
  killTimer(queryTimer);

  if (query->getSuccess()) {
    setCaption("KArchie - "+query->getSearchString());
    if (queryResult){ // alte Liste loeschen
      queryResult->setAutoDelete(TRUE);
      delete queryResult;
    }
    statbar->slotChangeStatus( i18n("Success") );
    //    debug("getfilelist");
    queryResult = &(query->getFileList());
    statbar->slotChangeHits( queryResult->count() );
    //    debug("displayfilelist");
    view->newFileList( *queryResult );

    if (view->getList().count() > 0) {
      // list contains items
      menu->setFileStoreListEnable( TRUE );
      menu->setFileWriteListEnable( TRUE );
      menu->setSortEnable( TRUE );

      if (-1 != view->getList().currentItem()) {
	// list has item selected/highlighted
	menu->setFileOpenEnable( TRUE );
	menu->setFileOpenDirEnable( TRUE );
	menu->setFileGetEnable( TRUE );
      }
    }
  }
  else { // print status on SLine
    const char *cError = query->getError();
    debug( cError );
    statbar->slotChangeStatus( cError );
    statbar->slotChangeHits( 0 );

    // no changes to the view->viewlist
    // so we dont change the filemenu
  }

  //  killTimers();
  // if there is an old list from previous queries
  // enable/disable the file->get menu
  //  QString tmp;
  //  tmp.setNum( view->getList().currentItem() );
  //  tmp += " current list item";
  //  debug( tmp );

  //  debug("ready");
  delete query;
  query = 0;

}

/************************MAIN****************************/

int main (int argc, char **argv)
{  
  KApplication *archie;
  KArchie      *window;

  // set main application
  //  debug("setup application");
  archie = new KApplication(argc, argv,"karchie");
  //archie->setFont(QFont("Helvetica",12),true);

  //  debug("setup window");
  window = new KArchie("main");

  archie->setMainWidget( window );
  window->setCaption("KArchie");
  //  debug("goto event loop");
  return archie->exec();
}
