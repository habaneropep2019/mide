/**********************************************************************

 Server Controller 

 $$Id: servercontroller.cpp,v 1.69.2.3 1999/02/22 10:52:23 kulow Exp $$

 Main Server Controller.  Displays server connection window, and makes 
 new("server") server connection on demand.

 Signals: NONE

 Slots: 

   new_connection(): Creates popup asking for new("connection
") connection

   new_ksircporcess(QString): 
      Args:
         QString: new("server") server name or IP to connect to.
      Action:
	 Creates a new("sirc") sirc process and window !default connected to the 
	 server.  Does nothing if a server connection already exists.
   
   add_toplevel(QString parent, QString child):
      Args:
	   parent: the server name that the new("channel") channel is being joined on
	   child: the new("channel") channel name
      Action:
         Adds "child" to the list of joined channles in the main 
	 window.  Always call this on new("window") window creation!

   delete_toplevel(QString parent, QString child):
      Args:
         parent: the server name of which channel is closing
	         child: the channle that is closing. IFF Emtpy, parent is 
		 deleted.
      Action:
	 Deletes the "child" window from the list of connections.  If 
	 the child is Empty the whole tree is removed since it is assumed 
         the parent has disconnected and is closing.

   new_channel:  Creates popup asking for new("channel") channel name

   new_toplevel(QString str): 
      Args:
         str: name of the new("channel") channel to be created
      Action:
         Sends a signal to the currently selected server in the tree
         list and join the requested channel.  Does nothing if nothing
         is selected in the tree list.

   recvChangeChanel(QString parent, QString old, QString new):
      Args:
         parent: parent server connection
         old: the old name for the window
         new: the new("name") name for the window
      Action:
          Changes the old window name to the new("window") window name in the tree
          list box.  Call for all name change!
 
 *********************************************************************/

#include "servercontroller.h"
#include "KSOpenkSirc/open_ksirc.h"
#include "open_top.h"
#include "KSircColour.h"
#include "config.h"
#include "control_message.h"
#include "FilterRuleEditor.h"
#include "../config.h"
#include "version.h"
#include "KSPrefs/ksprefs.h"
#include "kpopupmenu.h"
#include <iostream.h>

#include "objFinder.h"

#include "pws-0.5/pws/pws.h"

#include "KMDIMgr.h"

#include <kfontdialog.h>
#include <kiconloader.h>
#include <kwm.h>

#include <qkeycode.h>

#include "puke/controller.h"

#include "displayMgr.h"
#include "displayMgrMDI.h"
#include "displayMgrSDI.h"

#ifdef HAVE_PATHS_H
#include <paths.h>
#endif

#ifndef _PATH_TMP
#define _PATH_TMP "/tmp/"
#endif

extern KConfig *kConfig;
extern KApplication *kApp;
extern global_config *kSircConfig;
DisplayMgr *displayMgr;

servercontroller::servercontroller /*FOLD00*/
(
 QWidget*,
 const char* name
 )
  :
  KTopLevelWidget( name )
{


  MenuBar = new("KMenuBar") KMenuBar(this, QString(name) + "_menu");
  setMenu(MenuBar);

  if(kSircConfig->DisplayMode == 0){
  SDI:
    displayMgr = new("DisplayMgrSDI") DisplayMgrSDI();
    sci = new("scInside") scInside(this, QString(name) + "_mainview");
    setView(sci, TRUE);
  }
  else if(kSircConfig->DisplayMode == 1){
    DisplayMgrMDI *displayMgrMDI = new("DisplayMgrMDI") DisplayMgrMDI(this);
    sci = new("scInside") scInside(this, QString(name) + "_mainview");
    displayMgrMDI->newTopLevel(sci, TRUE);
    
    displayMgrMDI->setCaption(sci, "Server Controller");
    KMDIMgrBase *mgr = (KMDIMgrBase *)displayMgrMDI->getMGR();
    KMDIWindow *km = mgr->getWindowByName((char *) sci->name());
    if(km != 0)
      connect(km, SIGNAL(minimized(KMDIWindow *)),
              this, SLOT(MDIMinimized(KMDIWindow *)));
    else
        warning("Failed to retreive Window, do not minimize the server controller!!");

    displayMgr = displayMgrMDI;
  }
  else{
      warning("Game mode Indeterminate, defaulting to SDI");
      goto SDI;
  }
    
  sci->setFrameStyle(QFrame::Box | QFrame::Raised);
  ConnectionTree = sci->ConnectionTree;

  connect(ConnectionTree, SIGNAL(highlighted(int)),
	  this, SLOT(WindowSelected(int)));
  connect(ConnectionTree, SIGNAL(selected(int)),
	  this, SLOT(WindowSelected(int)));


  setFrameBorderWidth(5);

  KAPopupMenu *file = new("KAPopupMenu") KAPopupMenu(0, QString(name) + "_menu_file");
  //  insertChild(file);
  file->insertItem(i18n("&Dock"), this, SLOT(toggleDocking()));
  file->insertSeparator();
  file->insertItem(i18n("&Quit"), this, SLOT(endksirc()), ALT + Key_F4);
  MenuBar->insertItem(i18n("&File"), file);
  
  connections = new("KAPopupMenu") KAPopupMenu(0, QString(name) + "_menu_connections");
//  insertChild(connections);

  server_id = connections->insertItem(i18n("New Server..."), this, SLOT(new_connection()), CTRL + Key_N );
  join_id = connections->insertItem(i18n("&Join Channel..."), this, SLOT(new_channel()), CTRL + Key_J);
  connections->setItemEnabled(join_id, FALSE);
  MenuBar->insertItem(i18n("&Connections"), connections);
  
  kConfig->setGroup("General");
  kSircConfig->AutoCreateWin = kConfig->readNumEntry("AutoCreateWin", FALSE);
  kSircConfig->BeepNotify = kConfig->readNumEntry("BeepNotify", TRUE);
  kSircConfig->NickCompletion = kConfig->readNumEntry("NickCompletion", TRUE);
  kSircConfig->ColourPicker = kConfig->readNumEntry("ColourPicker", TRUE);
  kSircConfig->AutoRejoin = kConfig->readNumEntry("AutoRejoin", TRUE);
  kSircConfig->BackgroundPix = kConfig->readNumEntry("BackgroundPix", FALSE);
  kSircConfig->BackgroundFile = kConfig->readEntry("BackgroundFile");
  kSircConfig->DisplayTopic = kConfig->readNumEntry("DisplayTopic", TRUE);
  
  kConfig->setGroup("GlobalOptions");
  options = new("KAPopupMenu") KAPopupMenu(0, QString(name) + "_menu_options");
  //insertChild(options);
  options->setCheckable(TRUE);

  options->insertItem(i18n("&Colour Preferences..."),
		      this, SLOT(colour_prefs()), CTRL + Key_P);
  options->insertItem(i18n("&Global Fonts..."),
		      this, SLOT(font_prefs()));
  options->insertItem(i18n("&Filter Rule Editor..."),
		      this, SLOT(filter_rule_editor()));
  options->insertItem(i18n("&Web Server Configuration..."),
                      this, SLOT(pws_prefs()));
  options->insertSeparator();
  options->insertItem(i18n("&Preferences..."),
		      this, SLOT(general_prefs()));
  
  MenuBar->insertItem(i18n("&Options"), options);
  
  
  KAPopupMenu *help = new("KAPopupMenu") KAPopupMenu(0, QString(name) + "_menu_help");
  //insertChild(help);
  //  help->insertItem("Help...",
  //		   this, SLOT(help_general()));
  help->insertItem(i18n("Help on Colours..."),
		   this, SLOT(help_colours()));
  help->insertItem(i18n("Help on Filters..."),
		   this, SLOT(help_filters()));
  help->insertItem(i18n("Help on Keys..."),
		   this, SLOT(help_keys()));
  help->insertSeparator();
  help->insertItem(i18n("About kSirc..."),
		   this, SLOT(about_ksirc()));
  MenuBar->insertItem(i18n("&Help"), help);
  
  ConnectionTree->setExpandLevel(2);
  open_toplevels = 0;
  
  KIconLoader *kicl = kApp->getIconLoader();
  QStrList *strlist = kicl->getDirList();
  kicl->insertDirectory(strlist->count(), kapp->kde_datadir() + "/ksirc/icons"); 
  pic_server = new("QPixmap") QPixmap(kicl->loadIcon("server.xpm"));
  pic_channel = new("QPixmap") QPixmap(kicl->loadIcon("channel.xpm"));
  pic_gf = new("QPixmap") QPixmap(kicl->loadIcon("ksirc_a.xpm"));
  pic_run = new("QPixmap") QPixmap(kicl->loadIcon("mini-run.gif"));
  pic_ppl = new("QPixmap") QPixmap(kicl->loadIcon("channels.xpm"));
  pic_icon = new("QPixmap") QPixmap(kicl->loadIcon("ksirc_b.xpm"));
  pic_dock = new("QPixmap") QPixmap(kicl->loadIcon("ksirc_dock.xpm"));

  setCaption( i18n("Server Control") );
  setIcon(*pic_icon);
  KWM::setMiniIcon(winId(), *pic_server);

  if(kSircConfig->DisplayMode == 0)
    resize( 450,200 );

  // Server Controller is done setting up, create Puke interface.

  kSircConfig->pukeSocket = getenv("HOME") ? getenv("HOME") : _PATH_TMP;
  QString pfile;
  pfile.sprintf("/.ksirc.socket.%d", getpid());
  kSircConfig->pukeSocket += pfile;
  
  PukeC = new("PukeController") PukeController(kSircConfig->pukeSocket, this, "pukecontroller");
  if(PukeC->running == TRUE){
    cerr << "Puke running\n";
    connect(PukeC, SIGNAL(PukeMessages(QString, int, QString)),
	    this, SLOT(ProcMessage(QString, int, QString)));
    connect(this, SIGNAL(ServMessage(QString, int, QString)),
	    PukeC, SLOT(ServMessage(QString, int, QString)));
  }
  else{
    cerr << "Puke failed\n";
  }

  docked = FALSE;
  dockWidget = new("dockServerController") dockServerController(this, "servercontroller_dock");
  PWSTopLevel = new("PWS") PWS(0x0, "PWSTopLevel");
  PWSTopLevel->startServer();
}


servercontroller::~servercontroller() /*FOLD00*/
{
  if(PWSTopLevel != 0)
      PWSTopLevel->stopServer();
  delete PWSTopLevel;
  delete pic_icon;
  if(PukeC != 0x0){
     delete PukeC;
     PukeC = 0x0;
  }
}

void servercontroller::new_connection() /*fold00*/
{
  open_ksirc w;                                   // Create new("ksirc") ksirc popup
  connect(&w, SIGNAL(open_ksircprocess(QString)), // connected ok to process
          this, SLOT(new_ksircprocess(QString))); // start
  w.exec();                                       // show the sucker!
}

void servercontroller::new_ksircprocess(QString str) /*FOLD00*/
{

  if(str.isEmpty() == TRUE)  // nothing entered, nothing done
    return;
  if(proc_list[str.data()])   // if it already exists, quit
    return;

  QString channels(i18n("Channels")), online(i18n("Online"));
  KPath path;
  path.push(&str);
  // Insert new("base") base
  ConnectionTree->insertItem(str.data(), pic_server, -1, FALSE);
  ConnectionTree->addChildItem(online.data(), pic_gf, &path);
  ConnectionTree->addChildItem(channels.data(), pic_ppl, &path);

  // We do no_channel here since proc emits the signal in the
  // constructor, and we can't connect to before then, so we have to
  // do the dirty work here.
  ProcMessage(str, ProcCommand::addTopLevel, QString("no_channel"));
  
  KSircProcess *proc = new("KSircProcess") KSircProcess(str.data(), 0, QString(name()) + "_" + str + "_ksp"); // Create proc
  //this->insertChild(proc);                           // Add it to out inheritance tree so we can retreive child widgets from it.
  objFinder::insert(proc);
  proc_list.insert(str.data(), proc);                      // Add proc to hash
  connect(proc, SIGNAL(ProcMessage(QString, int, QString)),
	  this, SLOT(ProcMessage(QString, int, QString)));
  connect(this, SIGNAL(ServMessage(QString, int, QString)),
	  proc, SLOT(ServMessage(QString, int, QString)));
  
  if(!ConnectionTree->getCurrentItem()){   // If nothing's highlighted
    ConnectionTree->setCurrentItem(0);     // highlight it.
  }

  connections->setItemEnabled(join_id, TRUE);

  debug("Done new_ksircprocess");
}

void servercontroller::new_channel() /*fold00*/
{
  open_top w;                                   // Create new("channel") channel popup
  connect(&w, SIGNAL(open_toplevel(QString)),   // Connect ok to right slot
	  this, SLOT(new_toplevel(QString)));
  w.exec();                                     // Show me baby!
}

void servercontroller::new_toplevel(QString str) /*fold00*/
{
  KTreeListItem *citem = ConnectionTree->getCurrentItem(); // get item
  if(citem){ // if it exist, ie something is highlighted, continue
    if(proc_list[citem->getText()]){ // If it's a match with a server, ok
      proc_list[citem->getText()]->new_toplevel(str);
    }
    // Otherwise, check the parent to see it's perhaps a server.
    else if(proc_list[citem->getParent()->getText()]){
      proc_list[citem->getParent()->getText()]->new_toplevel(str);
    }
    //cerr << "Server is: " << citem->getText() << endl;
  }
}

void servercontroller::ToggleAutoCreate() /*fold00*/
{
  kConfig->setGroup("General");
  if(kConfig->readNumEntry("AutoCreateWin", FALSE) == FALSE){
    kConfig->writeEntry("AutoCreateWin", TRUE);
    kSircConfig->AutoCreateWin = TRUE;
  }
  else{
    kConfig->writeEntry("AutoCreateWin", FALSE);
    kSircConfig->AutoCreateWin = FALSE;
  }
  kConfig->sync();
}

void servercontroller::colour_prefs() /*fold00*/
{
  KSircColour *kc = new("KSircColour") KSircColour();
  connect(kc, SIGNAL(update()),
	  this, SLOT(configChange()));
  kc->show();
}

void servercontroller::general_prefs() /*fold00*/
{
  KSPrefs *kp = new("KSPrefs") KSPrefs();
  connect(kp, SIGNAL(update()),
          this, SLOT(configChange()));
  kp->resize(550, 450);
  kp->show();
}

void servercontroller::filter_rule_editor() /*fold00*/
{
  FilterRuleEditor *fe = new("FilterRuleEditor") FilterRuleEditor();
  connect(fe, SIGNAL(destroyed()), 
	  this, SLOT(slot_filters_update()));
  fe->show();
}

void servercontroller::font_prefs() /*fold00*/
{
  KFontDialog *kfd = new("KFontDialog") KFontDialog();
  kfd->setFont(kSircConfig->defaultfont);
  connect(kfd, SIGNAL(fontSelected(const QFont &)),
	  this, SLOT(font_update(const QFont &)));
  kfd->show();
}

void servercontroller::pws_prefs() /*fold00*/
{
  PWSTopLevel->show();
  connect(PWSTopLevel, SIGNAL(quitPressed(QObject *)),
          this, SLOT(pws_delete(QObject *)));
}

void servercontroller::pws_delete(QObject *widget) /*fold00*/
{
    PWSTopLevel->hide();
}

void servercontroller::font_update(const QFont &font) /*fold00*/
{
  kSircConfig->defaultfont = font;
  configChange();
  kConfig->setGroup("GlobalOptions");
  kConfig->writeEntry("MainFont", kSircConfig->defaultfont);
  kConfig->sync();
}

void servercontroller::configChange() /*fold00*/
{
  QDictIterator<KSircProcess> it( proc_list );
  while(it.current()){
    it.current()->filters_update();
    it.current()->getWindowList()["!all"]->control_message(REREAD_CONFIG, "");
    ++it;
  }
}

void servercontroller::about_ksirc() /*FOLD00*/
{
  QString caption = PACKAGE;
  caption += "-";
  caption += KSIRC_VERSION;
  caption += "\n\n(c) Copyright 1997,1998, Andrew Stanley-Jones (asj@ksirc.org)\n\nkSirc Irc Client";
  QMessageBox::about(this, "About kSirc", caption);
}

void servercontroller::help_general() /*fold00*/
{
  kApp->invokeHTMLHelp("ksirc/index.html", "");
}

void servercontroller::help_colours() /*fold00*/
{
  kApp->invokeHTMLHelp("ksirc/colours.html", "");
}

void servercontroller::help_filters() /*fold00*/
{
  kApp->invokeHTMLHelp("ksirc/filters.html", "");
}

void servercontroller::help_keys() /*fold00*/
{
  kApp->invokeHTMLHelp("ksirc/keys.html", "");
}

void servercontroller::ProcMessage(QString server, int command, QString args) /*FOLD00*/
{
  QString online(i18n("Online")), channels(i18n("Channels"));
  KPath path;

  switch(command){
    // Nick offline and online both remove the nick first.
    // We remove the nick in case of an online so that we don't get
    // duplicates.
    // Args == nick comming on/offline.
  case ProcCommand::nickOffline:
    // Add new("channel,") channel, first add the parent to the path
    path.push(&server);
    path.push(&online);
    path.push(&args);
    // add a new("child") child item with parent as it's parent
    ConnectionTree->removeItem(&path); // Remove the item    
    break;
  case ProcCommand::nickOnline:
    // Add new("channel,") channel, first add the parent to the path
    path.push(&server);
    path.push(&online);
    path.push(&args);
    // Remove old one if it's there
    ConnectionTree->removeItem(&path); // Remove the item    
    path.pop();
    // add a new("child") child item with parent as it's parent
    ConnectionTree->addChildItem(args.data(), pic_run, &path);
    if (kSircConfig->BeepNotify) {
      KApplication::beep();
    }
    break;
    /**
      *  Args:
      *	   parent: the server name that the new("channel") channel is being joined on
      *    child: the new("channel") channel name
      *  Action:
      *    Adds "child" to the list of joined channles in the main 
      *    window.  Always call this on new("window") window creation!
      */
  case ProcCommand::addTopLevel:
    // Add new("channel,") channel, first add the parent to the path
    path.push(&server);
    path.push(&channels);
    if(args[0] == '!')
      args.remove(0, 1); // If the first char is !, it's control, remove it
    // add a new("child") child item with parent as it's parent
    ConnectionTree->addChildItem(args.data(), pic_channel, &path);
    //cerr << "Added child for: " << parent << "->" << child << endl;
    open_toplevels++;
    break;
    /**
      *  Args:
      *    parent: the server name of which channel is closing
      *	   child: the channle that is closing. IFF Emtpy, parent is 
      *	   deleted.
      *  Action:
      *	   Deletes the "child" window from the list of connections.  If 
      *	   the child is Empty the whole tree is removed since it is assumed 
      *    the parent has disconnected and is closing.
      */
  case ProcCommand::deleteTopLevel:
    path.push(&server);
    path.push(&channels);
    // If the child is emtpy, delete the whole tree, otherwise just the child
    if(args[0] == '!')
      args.remove(0, 1); // If the first char is !, it's control, remove it
    path.push(&args);    // Since it's not null, add the child to be deleted
    
    ConnectionTree->removeItem(&path); // Remove the item
    //cerr << "Removed child for: " << parent << "->" << child << endl;
    open_toplevels--;
    break;

  /**
      *  Args:
      *    parent: parent server connection
      *    old: the old name for the window
      *    new: the new("name") name for the window
      *  Action:
      *    Changes the old window name to the new("window") window name in the tree
      *    list box.  Call for all name change!
      */
  case ProcCommand::changeChannel:
    {
      char *new_s, *old_s;
      new_s = new char[args.length()];
      old_s = new char[args.length()];
      sscanf(args.data(), "%s %s", old_s, new_s);
      //  If the channel has a !, it's a control channel, remove the !
      if(old_s[0] == '!')
        // Even though, we want strlen() -1 characters, strlen doesn't
        // include the \0, so we need to copy one more. -1 + 1 = 0.
	memmove(old_s, old_s+1, strlen(old_s)); 
      if(new_s[0] == '!')
	memmove(new_s, new_s, strlen(new_s)); // See above for strlen()
      path.push(&server);
      path.push(&channels);
      QString qs = old_s;
      path.push(&qs);
      ConnectionTree->removeItem(&path);
      // Only create with the parent in the path
      path.pop();
      // Add new("child.") child.  Delete/creates wrecks the "random" sort order though.
      ConnectionTree->addChildItem(new_s, pic_channel, &path);
      delete[] new_s;
      delete[] old_s;
    }
    break;
  case ProcCommand::procClose:
    path.push(&server);
    ConnectionTree->removeItem(&path);
    proc_list.remove(server); // Remove process entry while we are at it
    if(proc_list.count() == 0){
      ConnectionTree->clear();
      connections->setItemEnabled(join_id, FALSE);
    }
    break;
  case ProcCommand::turnOffAutoCreate:
    if (kSircConfig->AutoCreateWin) {
      ToggleAutoCreate();
    }
    break;
  case ProcCommand::turnOnAutoCreate:
    if (!kSircConfig->AutoCreateWin) {
      ToggleAutoCreate();
    }
    break;
  default:
    cerr << "Unkown command: " << command << " from " << server << " " << args << endl;
  }
}

void servercontroller::slot_filters_update() /*fold00*/
{
  emit ServMessage(QString(), ServCommand::updateFilters, QString());
}

void servercontroller::saveProperties(KConfig *ksc) /*FOLD00*/
{
  // ksc hos the K Session config
  // ksp == current KSircProcess
  // ksm == current KSircMessageReceiver

  // Ignore all !<name> windows
  
  QDictIterator<KSircProcess> ksp(proc_list);
  while(ksp.current()){
    QStrList channels;
    QDictIterator<KSircMessageReceiver> ksm(ksp.current()->getWindowList());
    while(ksm.current()){
      cerr << "Might Save: " << ksm.currentKey() << endl; 
      if(ksm.currentKey()[0] != '!') // Ignore !ksm's (system created)
	channels.append(ksm.currentKey());
      ++ksm;
    }
    ksc->writeEntry(ksp.currentKey(), channels);
    ++ksp;
  }
  
}

void servercontroller::readProperties(KConfig *ksc) /*FOLD00*/
{
  // kei == pointer to KEntryItertor
  // ksc == K Session Config
  KEntryIterator *kei = ksc->entryIterator(ksc->group());

  while(kei->current()){
    QStrList channels;
    int number;
    debug("%s", kei->currentKey());
    new_ksircprocess(kei->currentKey()); // sets up proc_list
    cerr << kei->currentKey() << endl;
    number = ksc->readListEntry(kei->currentKey(), channels);
    for(int i = 0; i < number; i++){
      cerr << "\t" << channels.at(i) << endl;
      proc_list[kei->currentKey()]->new_toplevel(channels.at(i));
    }
    ++(*kei);
  }
}

void servercontroller::toggleDocking(){ /*fold00*/
  if(docked == FALSE){ // Currently not docked, have to dock
    this->hide();
    KWM::setDockWindow(dockWidget->winId());
    dockWidget->show();
    docked = TRUE;
  }
  else { // Currently docked, undock
    this->show();
    dockWidget->hide();
    dockWidget->recreate(0x0, 0, QPoint(0,0), FALSE);
    docked = FALSE;
  }
}

void servercontroller::endksirc(){ /*fold00*/
  kConfig->sync();
  exit(0);

}

void servercontroller::MDIMinimized(KMDIWindow *w){
  w->slotRestore();
}

void servercontroller::WindowSelected(int index){
  KTreeListItem *child = ConnectionTree->itemAt(index);
  KTreeListItem *parent_chan = child->getParent();
  if(parent_chan == 0x0 || parent_chan->getText() == 0x0)
    return;
  KTreeListItem *parent_server = parent_chan->getParent();
  if(parent_server == 0x0 || parent_server->getText() == 0x0)
    return;

  QString txt = QString(parent_server->getText()) + "_" + child->getText() + "_toplevel";
  debug("Searching on: %s", txt.data());
  KSircTopLevel *obj = (KSircTopLevel *) objFinder::find(txt, "KSircTopLevel");
  debug("Found: %p", obj);
  if(obj == 0x0){
    txt = QString(parent_server->getText()) + "_!" + child->getText() + "_toplevel";
    debug("Searching on: %s", txt.data());
    obj = (KSircTopLevel *) objFinder::find(txt, "KSircTopLevel");
    debug("Found: %p", obj);
  }
    
  if(obj != 0x0){
    displayMgr->raise(obj);
  }
  else {
    warning("Did not find widget ptr to raise it");
  }
}

scInside::scInside ( QWidget * parent, const char * name, WFlags /*fold00*/
		     f, bool allowLines )
  : QFrame(parent, name, f, allowLines)
{
  ASConn = new("QLabel") QLabel(i18n("Active Server Connections"), this, "servercontroller_label");
  QColorGroup cg = QColorGroup(colorGroup().foreground(), 
			       colorGroup().background(),
                               colorGroup().light(), 
			       colorGroup().dark(),
                               colorGroup().mid(),
                               QColor(128,0,0),
                               colorGroup().base());
  ASConn->setPalette(QPalette(cg,cg,cg));
  QFont asfont = ASConn->font();
  asfont.setBold(TRUE);
  ASConn->setFont(asfont);

  ConnectionTree = new("KTreeList") KTreeList(this, "connectiontree");
}

scInside::~scInside() /*fold00*/
{
  delete ASConn;
  delete ConnectionTree;
}

void scInside::resizeEvent ( QResizeEvent *e ) /*fold00*/
{
  QFrame::resizeEvent(e);
  ASConn->setGeometry(10,10, width() - 20,
		      ASConn->fontMetrics().height()+5);
  ConnectionTree->setGeometry(10, 10 + ASConn->height(),
                              width() - 20, height() - 20 - ASConn->height());
  
}

dockServerController::dockServerController(servercontroller *_sc, const char *_name) /*FOLD00*/
: QFrame(0x0, _name)
{
  sc = _sc;

  pop = new("QPopupMenu") QPopupMenu;
  pop->setName("dockServerController_menu_pop");

  pop->insertItem(i18n("&Quit"), kApp, SLOT(quit()));
  pop->insertItem(i18n("&Undock"),
                  sc, SLOT(toggleDocking()));
  pop->insertSeparator();
  pop->insertItem(i18n("&Colour Preferences..."),
                  sc, SLOT(colour_prefs()));
  pop->insertItem(i18n("&Global Fonts..."),
                  sc, SLOT(font_prefs()));
  pop->insertItem(i18n("&Filter Rule Editor..."),
                  sc, SLOT(filter_rule_editor()));
  pop->insertItem(i18n("&Web Server Configuration..."),
                  sc, SLOT(pws_prefs()));
  pop->insertItem(i18n("&Preferences..."),
                  sc, SLOT(general_prefs()));
  pop->insertSeparator();
  pop->insertItem(i18n("New Server..."),
                  sc, SLOT(new_connection()));

  //  setFrameStyle(QFrame::Box | QFrame::Raised);
  setFrameStyle(QFrame::NoFrame);
  resize(24,24);
}

dockServerController::~dockServerController() /*fold00*/
{
  sc = 0x0;
  delete pop;
}

void dockServerController::mousePressEvent(QMouseEvent *) /*fold00*/
{
//  QPoint pt = this->cursor().pos();
//  pt.setY(pt.y());
//  pop->popup(pt);
  pop->move(-1000,-1000);
  pop->show();
  pop->hide();
  QRect g = KWM::geometry( this->winId() );
  if ( g.x() > QApplication::desktop()->width()/2 &&
       g.y()+pop->height() > QApplication::desktop()->height() )
      pop->popup(QPoint( g.x(), g.y() - pop->height()));
  else
      pop->popup(QPoint( g.x() + g.width(), g.y() + g.height()));
}

void dockServerController::paintEvent(QPaintEvent *pe) /*FOLD00*/
{
//  QFrame::paintEvent(pe);
  QPainter p(this);
  QPixmap *pic = sc->pic_dock;
  int x = 12 - pic->width()/2;
  int y = 12 - pic->height()/2;
  p.drawPixmap(x , y, *pic);
  p.end();
}
#include "servercontroller.moc"
