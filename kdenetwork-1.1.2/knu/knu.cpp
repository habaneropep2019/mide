/*
 *   KDE network utilities (knu.cpp)
 *
 *   Copyright (C) 1997  Bertrand Leconte
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * $Id: knu.cpp,v 1.10 1999/01/10 00:33:05 bieker Exp $
 *
 * $Log: knu.cpp,v $
 * Revision 1.10  1999/01/10 00:33:05  bieker
 * i18n() the about box.
 *
 * Revision 1.9  1998/10/29 21:26:30  leconte
 * Bertrand:
 * . renamed Matt s traceroute to mtr as asked by the new mtr maintainer
 * . mtr-0.25 has the diffs included. No more patch here
 * . updated knu for mtr-0.25
 *
 * Revision 1.8  1998/10/14 19:33:03  leconte
 * Bertrand: Added mtr support (with a patch to mtr-0.21)
 *
 * Revision 1.7  1998/09/23 16:24:15  bieker
 * Use i18n() instead of _().
 *
 * Revision 1.6  1998/06/11 19:20:36  leconte
 * - some accelerators added
 * - strings added to i18n
 *
 * Revision 1.5  1998/06/09 21:18:05  leconte
 * Bertrand: correction of bug #745 (reported by Duncan Haldane):
 * 	arguments are now added to the command line
 *
 * Revision 1.4  1998/03/01 19:30:22  leconte
 * - added a finger tab
 * - internal mods
 *
 * Revision 1.3  1997/12/07 23:44:14  leconte
 * - handle the binary's name modification dynamicaly (problem reported
 *   by Conrad Sanderson)
 * - added browse button to the option dialog (for binary selection)
 * - code clean-up
 * - better fallback to "nslookup" if "host" is not found
 *
 * Revision 1.2  1997/11/23 22:28:17  leconte
 * - Id and Log added in the headers
 * - Patch from C.Czezatke applied (preparation of KProcess new version)
 *
 */

#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include <qmsgbox.h>
#include <kmsgbox.h>
#include <qfileinf.h>

#include <kstdaccel.h>

#include "knu.h"
#include "knu.moc"

#include "version.h"

#define FRAME_BORDER_WIDTH 6


// Static data
QList<TopLevel>  TopLevel::windowList;

QString   CaptionString;

void quit();


/**
 * check to see if this current tab is enabled in the config file
 */
bool
isTabEnabled(QString tabName, KConfig *kc)
{
  kc->setGroup(tabName);
  if (kc->readNumEntry("enable", 1) == 0) {
    return(FALSE);
  } else {
    return(TRUE);
  }
}
 

/**
 * Constructor 
 */
TopLevel::TopLevel(QWidget *, const char *name)
  : KTopLevelWidget(name)
{
  KConfig         *kc = kapp->getConfig();
  PingDlg         *pd;
  TracerouteDlg   *td;
  HostDlg         *hd;
  FingerDlg       *fd;
  MtrDlg          *md;

  windowList.setAutoDelete(FALSE);
  windowList.append(this);

  /*
   * Set caption string
   */
  setCaption(CaptionString);
  
  kconfig = kc;
    
  /*
   * Create MenuBar
   */
  createMenu();
  setMenu(menuBar);

  
  /*
   * Create the tabctrl widget
   */
  tabCtrl = new KTabCtl(this);
  CHECK_PTR(tabCtrl);
  connect(tabCtrl, SIGNAL(tabSelected(int)), 
	  this, SLOT(slotTabChanged(int)));
  pagesNumber = 0;

  /*********************
   * Don't forget the slotConfig function while adding tabs
   *********************/

  // ping tab
  if (isTabEnabled("Ping", kc)) {
    pd = new PingDlg("ping", tabCtrl, i18n("&Ping"));
    CHECK_PTR(pd);
    tabCtrl->addTab(pd, pd->name());
    pages[pagesNumber] = pd;
    pagesNumber++;
  }

  // traceroute tab 
  if (isTabEnabled("Traceroute", kc)) {
    td = new TracerouteDlg("traceroute", tabCtrl, i18n("&Traceroute"));
    CHECK_PTR(td);
    tabCtrl->addTab(td, td->name());
    pages[pagesNumber] = td;
    pagesNumber++;
  }

  // host tab
  if (isTabEnabled("Host resolution", kc)) {
    hd = new HostDlg("host", tabCtrl, i18n("Host &resolution"));
    /* 
     * the "host" command name can be modified by 
     * HostDlg::search_for_binary if host is not found
     */
    CHECK_PTR(hd);
    tabCtrl->addTab(hd, hd->name());
    pages[pagesNumber] = hd;
    pagesNumber++;
  }

  // finger tab 
  if (isTabEnabled("Finger", kc)) {
    fd = new FingerDlg("finger", tabCtrl, i18n("F&inger"));
    CHECK_PTR(fd);
    tabCtrl->addTab(fd, fd->name());
    pages[pagesNumber] = fd;
    pagesNumber++;
  }

  // mtr tab 
  if (isTabEnabled("Mtr", kc)) {
    md = new MtrDlg("mtr", tabCtrl, i18n("&mtr"));
    CHECK_PTR(md);
    tabCtrl->addTab(md, md->name());
    pages[pagesNumber] = md;
    pagesNumber++;
  }

  if (pagesNumber == 0) {
    // There is *no* command enabled ! 

    QMessageBox::critical(this, caption(),
			  i18n("There is no command enabled\n"
			  "in the configuration file.\n\n"
			  "So I can't do anything...\n"));
    exit(-1);
  }
  setFrameBorderWidth(FRAME_BORDER_WIDTH);
  //tabCtrl->adjustSize();
  //tabCtrl->setMinimumSize(tabCtrl->size());
  setView(tabCtrl);		// for KTopLevelWidget
  currentTab = -1;

  /*
   * Enable File->Close if needed
   */
  if (windowList.count() > 1) {
    TopLevel *toplevel = windowList.first();
    while (toplevel) {
      toplevel->fileMenu->setItemEnabled(toplevel->closeIndex, TRUE);
      toplevel = windowList.next();
    }
  } else {
    fileMenu->setItemEnabled(closeIndex, FALSE);
  }

  menuBar->show();
  adjustSize();

  setMinimumSize(QMAX(tabCtrl->minimumSize().width()
		      +2*FRAME_BORDER_WIDTH, 500), 
		 QMAX(tabCtrl->minimumSize().height()
		      +2*FRAME_BORDER_WIDTH, 300));
}


/**
 * Create the KMenuBar
 */
void
TopLevel::createMenu()
{
  int mi;
  KStdAccel key = KStdAccel(kapp->getConfig());

  fileMenu = new QPopupMenu;
  CHECK_PTR(fileMenu);
  fileMenu->insertItem(i18n("&New window"), 
		       this, SLOT(slotNew()), key.openNew());
  fileMenu->insertSeparator();
  closeIndex = fileMenu->insertItem(i18n("&Close window"), 
				    this, SLOT(slotClose()), 
				    key.close());
  fileMenu->insertItem(i18n("&Quit"), this, SLOT(slotQuit()), 
		       key.quit());
  
  editMenu = new QPopupMenu;
  CHECK_PTR(editMenu);
  mi = editMenu->insertItem(i18n("&Copy"), this, SLOT(slotCopy()), 
			    key.copy());
  editMenu->setItemEnabled(mi, FALSE);
  editMenu->insertItem(i18n("Select &all"), 
		       this, SLOT(slotSelectAll()));
  editMenu->insertSeparator();
  editMenu->insertItem(i18n("C&lear output window"), 
		       this, SLOT(slotClear()));
  editMenu->insertSeparator();
  configIndex = editMenu->insertItem(i18n("P&references..."), 
				     this, SLOT(slotConfig()));

  
#if 0
  // In the future
  helpMenu = getHelpMenu(true, at.data());
#endif

  helpMenu = new QPopupMenu;
  CHECK_PTR(helpMenu);
  helpMenu->insertItem(i18n("&Contents"),
		       this, SLOT(slotHelp()), key.help());
  helpMenu->insertSeparator();
  QString str;
  str.sprintf(i18n("&About %s"), KNU_APPNAME);
  helpMenu->insertItem(str, this, SLOT(slotAbout()), SHIFT+Key_F1);
  helpMenu->insertItem(i18n("About &Qt"), this, SLOT(slotAboutQt()));
  
  menuBar = new KMenuBar(this);
  CHECK_PTR(menuBar);
  menuBar->insertItem(i18n("&File"), fileMenu);
  menuBar->insertItem(i18n("&Edit"), editMenu);
  menuBar->insertSeparator();
  menuBar->insertItem(i18n("&Help"), helpMenu);
}


/**
 * Edit->Preferences menu entry
 */
void 
TopLevel::slotConfig()
{
  CommandCfgDlg   *configPages[10];
  OptionsDlg      *options;
  int n = 0;

  /*
   * Create options dialogBox
   */
  
  /* ping */
  CommandCfgDlg *ccd = new CommandCfgDlg(i18n("&Ping"), 0, "ping_cfg");
  CHECK_PTR(ccd);
  configPages[n] = ccd;
  n++;
  
  /* traceroute */
  ccd = new CommandCfgDlg(i18n("&Traceroute"), 0, "traceroute_cfg");
  CHECK_PTR(ccd);
  configPages[n] = ccd;
  n++;
  
  /* host resolution */
  HostCfgDlg *hcd = new HostCfgDlg(i18n("Host &resolution"), 0, "host_cfg");
  CHECK_PTR(hcd);
  configPages[n] = hcd;
  n++;

  /* finger */
  ccd = new CommandCfgDlg(i18n("&Finger"), 0, "finger_cfg");
  CHECK_PTR(ccd);
  configPages[n] = ccd;
  n++;

  /* mtr */
  ccd = new CommandCfgDlg(i18n("&mtr"), 0, "mtr_cfg");
  CHECK_PTR(ccd);
  configPages[n] = ccd;
  n++;

  options = new OptionsDlg(configPages, n, 0);
  CHECK_PTR(options);

  if (options->exec()) {
    kapp->getConfig()->sync();
  } else {
    // nothing to do
  }
  
  // distribute the new configs values among all the toplevel windows
  TopLevel *toplevel = TopLevel::windowList.first();
  
  while (toplevel) {
    toplevel->checkBinaryAndDisplayWidget();
    toplevel = TopLevel::windowList.next();
  }
}

/**
 * Pass checkBinaryAndDisplayWidget to each tab
 */
void 
TopLevel::checkBinaryAndDisplayWidget()
{
  int i;
  for (i=0; i<pagesNumber; i++) {
    pages[i]->checkBinaryAndDisplayWidget();
  }
}


/**
 * Destructor
 */
TopLevel::~TopLevel()
{
  windowList.removeRef(this);
  delete(menuBar);
}


/**
 * called when a tab is changed
 */
void
TopLevel::slotTabChanged(int newTab)
{
  if (newTab != currentTab) {
    if (currentTab >= 0) {
      pages[currentTab]->tabDeselected();
    }
    pages[newTab]->tabSelected();
    currentTab = newTab;
  }
}

/**
 * File->Quit menu entry
 */
void 
TopLevel::slotQuit()
{
  ::quit();
}

/**
 * Edit->Copy menu entry
 */
void 
TopLevel::slotCopy()
{
}

/**
 * Edit->Clear output window menu entry
 */
void 
TopLevel::slotClear()
{
  pages[currentTab]->clearOutput();
}

/**
 * File->New window menu entry
 */
void 
TopLevel::slotNew()
{
  TopLevel *toplevel = new TopLevel;
  toplevel->show();
}

/**
 * File->Close window menu entry
 */
void 
TopLevel::slotClose()
{
  TopLevel *toplevel;

  if (windowList.count()>1) {
    delete this;

    if (windowList.count() == 1) {
      toplevel = windowList.first();
      toplevel->fileMenu->setItemEnabled(toplevel->closeIndex, FALSE);
    }
  } else {
    ::quit();
  }
}

/**
 * Edit->Select all menu entry
 */
void 
TopLevel::slotSelectAll()
{
  pages[currentTab]->selectAll();
}

/**
 * Help->About menu entry
 */
void 
TopLevel::slotAbout()
{
  QString about;
  about.sprintf(i18n("Knu\nVersion %s\n\nBertrand
  Leconte\n<B.Leconte@mail.dotcom.fr>\n"), KNU_VERSION);

  QMessageBox::about( this, i18n("About..."),
		      about);
}


/**
 * Help->AboutQt menu entry
 */
void 
TopLevel::slotAboutQt()
{
  QMessageBox::aboutQt(this);
}

/**
 * This is called when the application is closed by the wm.
 * we make this act like the File->Quit menu entry
 */
void 
TopLevel::closeEvent (QCloseEvent *)
{
  TopLevel *toplevel;

  if (windowList.count()>1) {
    delete this;		// I _know_ that I used new...
    if (windowList.count() == 1) {
      // disable other window Close menu entry
      toplevel = windowList.first();
      toplevel->fileMenu->setItemEnabled(toplevel->closeIndex, FALSE);
    }
  } else {
    ::quit();
  }
}

/**
 * Help->Contents menu entry
 */
void 
TopLevel::slotHelp()
{
  kapp->invokeHTMLHelp("", "");
}


/*
 * Called when the user is login out.
 */
void
TopLevel::saveProperties(KConfig *kc)
{
  kc->writeEntry("CurrentTab", currentTab);
}

void
TopLevel::readProperties(KConfig *kc)
{
  //debug("TopLevel::readProperties(KConfig*)");
  int entry = kc->readNumEntry("CurrentTab", -1);
  //debug("readNumEntry = %d", entry);
  if (entry >= 0) {
    //tabCtrl->setCurrentTab(entry);
    // This is not implemented yet (bug somewhere. Qt ?)
  }
}



/**
 * Application quit function
 */
void
quit()
{
  // we have to scan the whole list and delete toplevel
  TopLevel *toplevel = TopLevel::windowList.first();
  
  while (toplevel) {
    delete toplevel;
    toplevel = TopLevel::windowList.next();
  }
  kapp->quit();
}


/**
 * Test if a filename is executable.
 * If there is not / in the filename, search in the path.
 */
bool
test_for_exec(QString filename)
{
  bool rc = FALSE;

  if (strchr(filename, '/')) {
    // It's a full path
    if (access(filename, X_OK)) {
      rc = FALSE;
    } else {
      rc = TRUE;
    }
  } else {
    // Check in the path
    char *pathEnv = getenv("PATH");
    char *p;
    char *path = (char *)malloc(strlen(pathEnv)+1);

    strcpy(path, pathEnv);

    p = strtok(path, ":");
    while (p != 0) {
      //debug("  ELEM = \"%s\"", p);
      if (test_for_exec((QString)p + "/" + filename)) {
	rc = TRUE;
	break;
      }
      p = strtok(NULL, ":");
    }

  }
  return(rc);
}


/**
 * Application main function
 */
int
main(int argc, char **argv)
{
  const QString  appname = KNU_APPNAME;
  KApplication  *a = new KApplication(argc, argv, appname);

  CaptionString = i18n("Network utilities");

  if (strcmp(a->getCaption(), KNU_APPNAME)) {
    CaptionString = a->getCaption();
  }

  // Session Management
  if (a->isRestored()) {
    int n = 1;
    while (KTopLevelWidget::canBeRestored(n)) {
      TopLevel *toplevel = new TopLevel((QWidget*)0, (const char*) appname);
      CHECK_PTR(toplevel);
      toplevel->restore(n);
      n++;
    } 
  } else {
    TopLevel *toplevel = new TopLevel((QWidget*)0, (const char*) appname);
    CHECK_PTR(toplevel);
    
    toplevel->show();
  }
  return a->exec();
}

