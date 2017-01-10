/***************************************************************************
 *                                KAMenu.cpp                               *
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

#include "KAMenu.h"
#include "KAMenu.moc"

#include <kapp.h>
#include <qkeycode.h>
#include <kstring.h>

#include "KASettings.h"

#define SEARCH_EXACT 0
#define SEARCH_SUBSTR 1
#define SEARCH_SUBCASE 2
#define SEARCH_REGEXP 3
#define SEARCH_EXACT_SUBSTR 4
#define SEARCH_EXACT_SUBCASE 5
#define SEARCH_EXACT_REGEXP 6

#define NICE_NORMAL 0
#define NICE_NICE 1
#define NICE_VERY 2
#define NICE_MORE 3
#define NICE_NICEST 4

#define QUERY_FILE 0
#define QUERY_PATH 1
#define QUERY_HOST 2
#define QUERY_STOP 4

#define SORT_HOST 0
#define SORT_DOMAIN 1
#define SORT_DATE 2
#define SORT_SIZE 3



KAMenu::KAMenu( QWidget *parent, const char *name )
  : KMenuBar( parent, name ),
    host_id(0),
    nice_id(0),
    searchmode_id(0)
{ 
  config = kapp->getConfig();

  file = new QPopupMenu();
  CHECK_PTR( file );
  file->insertItem( i18n("O&pen in editor"), this, SLOT(slotFileOpen()), CTRL+Key_O );
  file->insertItem( i18n("&Open Dir"), this, SLOT(slotFileOpenDir()), CTRL+Key_D );
  file->insertItem( i18n("&Download..."), this, SLOT(slotFileGet()), CTRL+Key_G );

  file->insertSeparator();
  file->insertItem( i18n("&Load List..."), this, SLOT(slotFileLoad()), CTRL+Key_L );
  file->insertItem( i18n("&Save List..."), this, SLOT(slotFileSave()), CTRL+Key_S );
  file->insertItem( i18n("Expor&t List..."), this, SLOT(slotFileWrite()), CTRL+Key_R );
  file->insertSeparator();
  file->insertItem( i18n("&Exit"), qApp, SLOT(quit()), CTRL+Key_Q );
 
  /* empty for now
   * will be filled on request with actual
   * Config settings
   */
  host = new QPopupMenu();
  CHECK_PTR( host );
  host->setCheckable( TRUE );
  connect ( host, SIGNAL(activated(int)),
	    this, SLOT(slotSettingsHostname(int)) );
  

  searchmode = new QPopupMenu();
  CHECK_PTR( searchmode );
  searchmode->setCheckable( TRUE );
  searchmode->insertItem( i18n("&Exact"), SEARCH_EXACT, SEARCH_EXACT);
  searchmode->insertItem( i18n("&Substring"), SEARCH_SUBSTR, SEARCH_SUBSTR);
  searchmode->insertItem( i18n("Substring &case insensitive"), SEARCH_SUBCASE, SEARCH_SUBCASE);
  searchmode->insertItem( i18n("&Regular expression"), SEARCH_REGEXP, SEARCH_REGEXP);
  searchmode->insertItem( i18n("Exact, then substring"), SEARCH_EXACT_SUBSTR, SEARCH_EXACT_SUBSTR);
  searchmode->insertItem( i18n("Exact, then substring case ins."), SEARCH_EXACT_SUBCASE, SEARCH_EXACT_SUBCASE);
  searchmode->insertItem( i18n("Exact, then regular expression"), SEARCH_EXACT_REGEXP, SEARCH_EXACT_REGEXP);
  connect ( searchmode, SIGNAL(activated(int)),
	    this, SLOT(slotSettingsSearchmode(int)) );


  nicelevel = new QPopupMenu();
  CHECK_PTR( nicelevel );
  nicelevel->setCheckable( TRUE );
  nicelevel->insertItem( i18n("n&ormal"), NICE_NORMAL, NICE_NORMAL);
  nicelevel->insertItem( i18n("&nice"), NICE_NICE, NICE_NICE);
  nicelevel->insertItem( i18n("&very nice"), NICE_VERY, NICE_VERY);
  nicelevel->insertItem( i18n("&even more nice"), NICE_MORE, NICE_MORE);
  nicelevel->insertItem( i18n("n&icest"), NICE_NICEST, NICE_NICEST);
  connect ( nicelevel, SIGNAL(activated(int)),
	    this, SLOT(slotSettingsNicelevel(int)) );

  settings = new QPopupMenu();
  CHECK_PTR( settings );
  settings->setCheckable( TRUE );
  settings->insertItem( i18n("Show &File discription"), this, SLOT( slotSettingsShowFile() ));
  settings->setId( 0, 0 );
  settings->setItemChecked( 0 , TRUE);
  settings->insertItem( i18n("Archie&server"), host );
  settings->insertItem( i18n("Search&mode"), searchmode );
  settings->insertItem( i18n("&Nicelevel"), nicelevel );
  settings->insertSeparator();
  settings->insertItem( i18n("&All..."), this, SLOT( slotSettingsAll() ));

  //  settings->insertSeparator();
  //  settings->insertItem( i18n("&Save"), this, SLOT( slotSettingsSave() ));

  query = new QPopupMenu();
  CHECK_PTR( query ); 
  query->insertItem( i18n("for &Filename"), QUERY_FILE, QUERY_FILE);
  query->insertItem( i18n("for &Path/Location"), QUERY_PATH, QUERY_PATH);
  query->insertItem( i18n("for &Hostname"), QUERY_HOST, QUERY_HOST);
  query->insertSeparator();
  query->insertItem( i18n("&STOP"), QUERY_STOP, QUERY_STOP);
  slotQueryRunning(FALSE); // no query on initialisation 
  connect ( query, SIGNAL(activated(int)),
	    this, SLOT(slotQuery(int)) );
  // host search and path search doesn't work proper 
  query->setItemEnabled(QUERY_PATH, FALSE);
  query->setItemEnabled(QUERY_HOST, FALSE);

 
  /*
  query->insertItem( i18n("for &Filename"), this, SLOT( slotQueryFile() ));
  query->insertItem( i18n("for &Path/Location"), this, SLOT( slotQueryPath() ));
  query->insertItem( i18n("for &Hostname"), this, SLOT( slotQueryHost() ));
  */

  sort = new QPopupMenu();
  CHECK_PTR( sort );
  sort->insertItem( i18n("by &Hostname"), SORT_HOST, SORT_HOST);
  sort->insertItem( i18n("by &Domain"), SORT_DOMAIN, SORT_DOMAIN);
  sort->insertItem( i18n("by D&ate"), SORT_DATE, SORT_DATE);
  sort->insertItem( i18n("by &Filesize"), SORT_SIZE, SORT_SIZE);
  connect ( sort, SIGNAL(activated(int)),
	    this, SLOT(slotSort(int)) );


  QString aboutText;
  aboutText += i18n("KArchie ver. ");
  aboutText += KARCHIE_VERSION;
  aboutText += i18n("\n\n"
		    "by Jörg Habenicht <j.habenicht@europemail.com>\n\n"
		    "based on the work of:\n"
		    "  - archie prospero client by\n"
		    "      Clifford Neuman and\n"
		    "      Brendan Kehoe (brendan@cygnus.com)\n"
		    "  - and xarchie by\n"
		    "      George Ferguson (ferguson@cs.rochester.edu)\n"
		    "(See README files and copyright.h).\n"
		    "My own code is based on the GPL");
  help = kapp->getHelpMenu( TRUE, aboutText );

  insertItem( i18n("&File"), file);
  insertItem( i18n("&Settings"), settings);
  insertItem( i18n("&Query"), query);
  insertItem( i18n("S&ort"), sort);
  insertSeparator();
  insertItem( i18n("&Help"),help);

  slotHostlistChanged();

}

KAMenu::~KAMenu()
{
  delete file;
  delete settings;
  delete host;
  delete searchmode;
  delete nicelevel;
  delete query;
  delete sort;
  delete help;
}

void 
KAMenu::slotHostlistChanged()
{
  /* die Liste der erreichbaren Archiehosts hat sich
   * veraendert. neu einlesen
   */
  //  KConfig *config = KApplication::getKApplication()->getConfig();
  QStrList archiehostlist;
  //  int archiehostlistnumber = 
  KConfigGroupSaver saveGroup( config, "HostConfig" );

  config->readListEntry( "Hosts", archiehostlist );
  //  QString currenthost = config->readEntry( "CurrentHost", "archie.sura.net" );
  QString defaulthost = "archie.sura.net" ;
  if ( archiehostlist.isEmpty() ) {
    archiehostlist.append( defaulthost );
    //    currentHostId = 0;
  }
  
  host->clear();
  char *tmpStr;
  int i = 0;
  for (tmpStr=archiehostlist.first(); tmpStr; tmpStr=archiehostlist.next()) {
    host->insertItem( tmpStr, i, i);
    i++;
  }

  slotConfigChanged();

  emit sigArchieHost(host->text(host_id));
}

void  
KAMenu::slotConfigChanged()
{
  // effects Nicelevel, hostname, searchmode, "show file attributes"

  int tmp_id;

  KConfigGroupSaver *saveGroup = new KConfigGroupSaver(config, "SearchConfig");
  const QString searchInput = config->readEntry("Search", "=");
  //  debug ("searchmode %c",((const char*)input)[0]);
  SearchMode searchconfig(((const char*)searchInput)[0]);
  tmp_id = searchmode2menu(searchconfig.getMode());
  changeCheckMenu(searchmode, searchmode_id, tmp_id);
  searchmode_id = tmp_id;

  const int niceInput = config->readNumEntry("Nice", 0);
  //  debug("nicelevel %i",niceInput);
  tmp_id = nicelevel2menu(NiceLevel::toLevel(niceInput));
  changeCheckMenu(nicelevel, nice_id, tmp_id);
  nice_id = tmp_id;

  delete saveGroup;

  saveGroup = new KConfigGroupSaver(config, "HostConfig");

  uint currentHostId = config->readUnsignedNumEntry( "CurrentHostId", 0 );
  if (host->count() < currentHostId-1) {
    currentHostId = 0;
  }    
  changeCheckMenu(host, host_id, currentHostId);
  host_id = currentHostId;

  delete saveGroup;

  saveGroup = new KConfigGroupSaver( config, "WindowConfig" );
  settings->setItemChecked( 0,
			    config->readBoolEntry("FAttr", true));

}

void 
KAMenu::slotQueryRunning(bool runs)
{
  query->setItemEnabled(QUERY_FILE, !runs);
  //  query->setItemEnabled(QUERY_PATH, !runs);
  //  query->setItemEnabled(QUERY_HOST, !runs);
  query->setItemEnabled(QUERY_STOP, runs);
}

void 
KAMenu::changeCheckMenu(QPopupMenu *pop, int old_id, int new_id)
{
  pop->setItemChecked(old_id, FALSE);
  pop->setItemChecked(new_id, TRUE);
}

int  
KAMenu::searchmode2menu(SearchMode::mode _mode)
{
  switch (_mode) {
  case SearchMode::substr : return SEARCH_SUBSTR; break;
  case SearchMode::subcase: return SEARCH_SUBCASE; break;
  case SearchMode::regexp: return SEARCH_REGEXP; break;
  case SearchMode::exactsubstr: return SEARCH_EXACT_SUBSTR; break;
  case SearchMode::exactsubcase: return SEARCH_EXACT_SUBCASE; break;
  case SearchMode::exactregexp: return SEARCH_EXACT_REGEXP; break;
  case SearchMode::exact: // fall through
  default: return SEARCH_EXACT; break;
  }
}

SearchMode::mode  
KAMenu::menu2searchmode(int mode)
{
  switch (mode) {
  case SEARCH_EXACT: return SearchMode::exact; break;
  case SEARCH_SUBSTR: return SearchMode::substr; break;
  case SEARCH_SUBCASE: return SearchMode::subcase; break;
  case SEARCH_REGEXP: return SearchMode::regexp; break;
  case SEARCH_EXACT_SUBSTR: return SearchMode::exactsubstr; break;
  case SEARCH_EXACT_SUBCASE: return SearchMode::exactsubcase; break;
  case SEARCH_EXACT_REGEXP: return SearchMode::exactregexp; break;
  default: return SearchMode::none; break;
  }
}

int 
KAMenu::nicelevel2menu(NiceLevel::mode _mode)
{
  switch (_mode) {
  case NiceLevel::nice : return NICE_NICE; break;
  case NiceLevel::nicer: return NICE_VERY; break;
  case NiceLevel::verynice: return NICE_MORE; break;
  case NiceLevel::extremlynice: return NICE_NICEST; break;
  case NiceLevel::norm: // fall through
  default: return NICE_NORMAL; break;
  }
}

NiceLevel::mode 
KAMenu::menu2nicelevel(int mode)
{
  switch (mode) {
  case NICE_NICE: return NiceLevel::nice; break;
  case NICE_VERY: return NiceLevel::nicer; break;
  case NICE_MORE: return NiceLevel::verynice; break;
  case NICE_NICEST: return NiceLevel::extremlynice; break;
  case NICE_NORMAL: // fall through
  default: return NiceLevel::norm; break;
  }
}

void 
KAMenu::slotFileOpen()
{
  debug( "File open selected" );
  emit sigFileOpen();
}

void 
KAMenu::slotFileOpenDir()
{
  //  debug( "File open dir selected" );
  emit sigFileOpenDir();
}

void 
KAMenu::slotFileGet()
{
  //  debug( "File get selected" );
  emit sigFileGet();
}

void 
KAMenu::slotFileSave()
{
  //  debug( "File save selected" );
  emit sigFileStoreList();
}

void
KAMenu::slotFileLoad()
{
  //  debug( "File load selected" );
  emit sigFileLoadList();
}

void 
KAMenu::slotFileWrite()
{
  //  debug( "File write selected" );
  emit sigFileWriteList();
}

void 
KAMenu::slotSettings(int menu)
{
  debug( "Settings selected %i",menu );
}

void 
KAMenu::slotSettingsHostname(int menu)
{
  //  debug( "Settings hostname selected %i",menu );
  changeCheckMenu(host, host_id, menu);
  host_id = menu;
  // Eintrag in der Config aendern
  KConfigGroupSaver saveGroup( config, "HostConfig" );
  config->writeEntry("CurrentHostId", menu, true, false, false);
  config->sync();
  emit sigArchieHost(host->text(menu));
}

void 
KAMenu::slotSettingsSearchmode(int mode)
{
  //  debug( "Settings seachtype selected %i", mode );
  changeCheckMenu(searchmode, searchmode_id, mode);
  searchmode_id = mode;
  SearchMode currentmode(menu2searchmode(mode));

  // Eintrag in der Config aendern
  KConfigGroupSaver saveGroup( config, "SearchConfig" );
  char input[2]; //= new char[2];
  input[0] = currentmode.getChar();
  input[1] = '\0';
  config->writeEntry("Search", input, true, false, false);
  config->sync();
  //  delete input;

  emit (currentmode.getMode());
}

/*
void 
KAMenu::slotSettingsSorttype()
{
  debug( "Settings sorttype selected" );
}
*/

void 
KAMenu::slotSettingsNicelevel(int menuitem)
{
  //  debug( "Settings nicelevel selected %i", menuitem );
  changeCheckMenu(nicelevel, nice_id, menuitem);
  nice_id = menuitem;
  NiceLevel nicelevel(menu2nicelevel(menuitem));

  // Eintrag in der Config aendern
  KConfigGroupSaver saveGroup( config, "SearchConfig" );
  config->writeEntry("Nice", nicelevel.toLevel(), true, false, false);
  config->sync();

  emit sigNicelevel(nicelevel.getLevel());
}

void 
KAMenu::slotSettingsShowFile()
{
  //  debug( "Settings showfile selected" );
  const bool itemChecked = !settings->isItemChecked(0);
  // toggle the showfilemenuitem
  settings->setItemChecked( 0, itemChecked);

  // save it
  KConfigGroupSaver saveGroup( config, "WindowConfig" );
  config->writeEntry("FAttr", itemChecked, true, false, false);
  config->sync();
  
  // call it to the world..
  emit sigSettingsShowFileDiscription( itemChecked );
}

void 
KAMenu::slotSettingsAll()
{
  //  debug( "Settings all selected" );
  emit sigSettingsAll();
}

void 
KAMenu::slotSettingsSave()
{
  debug( "Settings save selected" );
}

void
KAMenu::slotQuery(int item)
{
  switch(item) {
  case QUERY_FILE: emit sigQueryFile(); break;
  case QUERY_PATH: emit sigQueryPath(); break;
  case QUERY_HOST: emit sigQueryHost(); break;
  case QUERY_STOP: emit sigQueryStop(); break;
  default: break;
  }
}

void 
KAMenu::slotSort(int item)
{
  switch (item) {
  case SORT_HOST: emit sigSortHostname(); break;
  case SORT_DOMAIN: emit sigSortDomain(); break;
  case SORT_DATE: emit sigSortDate(); break;
  case SORT_SIZE: emit sigSortFilesize(); break;
  default: break;
  }
}


/*
void 
KAMenu::slotHelpUsingKA()
{
}

void 
KAMenu::slotHelpAbout()
{
}
*/
