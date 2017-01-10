/*************************************************************************

 KSircProcess, sirc controller

 $$Id: ksircprocess.cpp,v 1.54.2.2 1999/02/11 15:11:58 kulow Exp $$

 KSircProcess cerate and controls toplevel widgets and sirc process'.
 Each sirc process has 1 and only 1 KSircProcess to control it.  KSirc
 process passes all IO to IOController which is it's friend.

 Interface:

 public:
     KSircProcess(*server=0L, *parent=0, *name=0)
       server: is the name of the server to connect to.  It must be
       provided or else start sirc will barf. :(
       parent: parent window, this _should_ be null
       name: name, passed to QObject...

     ~KSirProcess: 
       kill the sirc process, and iocontrollller, emit delete_toplevel

     getWindowList:
       returns the TopList, see bellow.

   Signals:
     made_toplevel(server, window)
       made a new("toplevel") toplevel window for the "server" we are connected to
       with "window" as the title.

     dalete_toplevel(server, window)
       delete toplevel with server and called window.  If we emit null
       as the window name it means to destroy all info about the
       server and ksircprocess.

     changeChannel(server, old_name, new_name)
       toplevel with old_name has been changed to new_name and all
      future refrences will use new_name.

   public slots:
     new_toplevel(window):
       create a new("window") window with name window.  This MAY only change the
       name of an existing window that's now idle.

     close_topevel(KsircTopLevel*, window):
       deletes all refrences to window and if needed finds a new
       default toplevel.

     default_window(KSricTopLevel*):
       KSircTopLevel is requesting change to !default.  Be carefull
       with this one.

     recvChangeChannel(old, new):
       window old is changing to new.  emit ChangeChannel with server
       name added.  Without server name we can uniqely id the window. :(

 Implementation:

   Bassic process is to create a new("KSircProcess") KSircProcess and it takes care of
   the rest.  It emits signals for each new("window") window and everytime a
   window is delete so you can update external display (like
   servercontroller uses).

   Startup:

   1. Creates a case insensitive TopList.  This is a list of ALL
   KSircReceivers under control of this server, and includes such
   items as "!all" and "!default".  All !name are control windows.

   2. Forks off a KProcess for sirc and passes it over to IOController
   which grabs and control's it's IO.

   3. It then opens a "!default" window.  This will receive all
   initial input and such.  It WILL change it's name on the first
   join.

   4. The IO broadcast object is created and setup.

   5. everything is put into run mode.


   Operation, see code bellow for inline comments.

*************************************************************************/



#include "ksircprocess.h"
#include "servercontroller.h"
#include "toplevel.h"
#include "ioBroadcast.h"
#include "ioDiscard.h"
#include "ioDCC.h"
#include "ioLAG.h"
#include "ioNotify.h"
#include "baserules.h"
#include "iocontroller.h"
#include "control_message.h"
#include "config.h"
#include "version.h"
#include "../config.h"
#include "objFinder.h"
#include "displayMgr.h"

#include <qmsgbox.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream.h>

extern KApplication *kApp;
extern KConfig *kConfig;
extern global_config *kSircConfig;
extern DisplayMgr *displayMgr;

KSircProcess::KSircProcess( char *_server, QObject * parent, const char * name ) /*FOLD00*/
  : QObject(parent, name)
{

  server = qstrdup(_server);

  QDict<KSircMessageReceiver> nTopList(17, FALSE);
  TopList = nTopList;
  //  TopList.setAutoDelete(TRUE)

  auto_create_really = FALSE;

  // Setup the environment for KSirc
  QString qsNick, qsRealname;
  kConfig->setGroup("StartUp");
  qsNick = kConfig->readEntry("Nick", "");
  qsRealname = kConfig->readEntry("RealName", "");
  // 
  // I use qstrdup, I'm SURE THIS IS NOT NEEDED, but doesn't work otherwise!
  //
  if((qsNick.isEmpty() == FALSE)){
    QString env = "SIRCNICK=" + qsNick;
    putenv(qstrdup(env.data()));
  }
  if((qsRealname.isEmpty() == FALSE)){
    QString env = "SIRCNAME=" + qsRealname;
    putenv(qstrdup(env.data()));
  }

  QString env = "SIRCLIB=" + kapp->kde_datadir() + "/ksirc";
  putenv(qstrdup(env.data()));

  QString sock_env = "PUKE_SOCKET=" +  kSircConfig->pukeSocket;
  putenv(qstrdup(sock_env.data()));

  // Setup the proc now, so iocontroller can use it.  It's latter
  // though. started bellow though.

  proc = new("KProcess") KProcess();
  proc->setName(QString(name) + "_kprocess");
  objFinder::insert(proc);
//  insertChild(proc);

  proc->setExecutable("perl");
  *proc << kapp->kde_bindir() + "/dsirc" << "-8" << "-r" << "-s" << server;

  // Finally start the iocontroller.

  iocontrol = new("KSircIOController") KSircIOController(proc, this);
  iocontrol->setName(QString(name) + "_iocontrol");

  // Create toplevel before iocontroller so it has somewhere to write stuff.

  running_window = TRUE;        // True so we do create the default
  new_toplevel("!no_channel");  // 
  TopList.insert("!default", TopList["!no_channel"]);

  running_window = FALSE;       // set false so next changes the first name
  default_follow_focus = TRUE;

  // Write default commands, and open default windows.

  TopList.insert("!all", new("KSircIOBroadcast") KSircIOBroadcast(this));
  TopList.insert("!discard", new("KSircIODiscard") KSircIODiscard(this));
  KSircIODCC *dcc = new("KSircIODCC") KSircIODCC(this);
  connect(dcc, SIGNAL(outputLine(QString)),
	  iocontrol, SLOT(stdin_write(QString)));
  TopList.insert("!dcc", dcc);
  KSircIOLAG *lag = new("KSircIOLAG") KSircIOLAG(this);
  connect(lag, SIGNAL(outputLine(QString)),
	  iocontrol, SLOT(stdin_write(QString)));
  TopList.insert("!lag", lag);
  KSircIONotify *notify = new("KSircIONotify") KSircIONotify(this);
  connect(notify, SIGNAL(notify_online(QString)),
	  this, SLOT(notify_forw_online(QString)));
  connect(notify, SIGNAL(notify_offline(QString)),
	  this, SLOT(notify_forw_offline(QString)));
  TopList.insert("!notify", notify);

  TopList.insert("!base_rules", new("KSMBaseRules") KSMBaseRules(this));

  // Now that all windows are up, start sirc.

  proc->start(KProcess::NotifyOnExit, KProcess::All);
  // Intial commands to load ASAP.
  // turn on sirc ssfe mode
  QString command = "/eval $ssfe=1\n";
  iocontrol->stdin_write(command);
  
  command = "/eval $version .= \"+4KSIRC/" + QString(KSIRC_VERSION) + "\"\n";
  iocontrol->stdin_write(command);
  command = "/load " + kapp->kde_datadir() + "/ksirc/filters.pl\n";
  iocontrol->stdin_write(command);
  command = "/load " + kapp->kde_datadir() + "/ksirc/ksirc.pl\n";
  iocontrol->stdin_write(command);
  command = "/load " + kapp->kde_datadir() + "/ksirc/puke.pl\n";
  iocontrol->stdin_write(command);
  command = "/load " + kapp->kde_datadir() + "/ksirc/dcc_status.pm\n";
  iocontrol->stdin_write(command);



  // Load all the filter rules.  Must be after /load filtes.pl so all
  // the functions are available

  filters_update();

  // We do this after filters_update() since filters_update loads the
  // require notify filters, etc.

  kConfig->setGroup("NotifyList");
  QString cindex, nick;
  int items = kConfig->readNumEntry("Number");
  command = "/notify ";
  for(int i = 0; i < items; i++){
    cindex.setNum(i);
    nick = "Notify-" + cindex;
    command += kConfig->readEntry(nick) + " ";
  }
  command += "\n";
  iocontrol->stdin_write(command);

  debug("Done ksircprocess");

}

KSircProcess::~KSircProcess() /*FOLD00*/
{
  
  if(TopList["!default"]){
    TopList.remove("!default"); // remove default so we don't delete it twice.
  }

  QDictIterator<KSircMessageReceiver> it(TopList);
  while(it.current()){
    delete it.current();
    ++it;
  }

  emit ProcMessage(QString(server), ProcCommand::procClose, QString());

  // Do closing down commands, this should release all puke widgets
  QString quit_cmd = "/eval &dohooks(\"quit\");\n";
  proc->writeStdin(quit_cmd.data(), quit_cmd.length());
  sleep(1);
  if(proc->isRunning()){
      proc->kill(SIGTERM);
      sleep(1);
  }
  
  delete proc;               // Delete process, seems to kill sirc, good.
  delete iocontrol;          // Take out io controller
  delete server;
}

bool KSircProcess::eventFilter ( QObject *obj, QEvent *e ){ /*FOLD00*/
    debug("Got event %d", e->type());
    return TRUE;
}
void KSircProcess::new_toplevel(QString str) /*FOLD00*/
{
  static time_t last_window_open = 0;
  static int number_open = 0;
  static bool flood_dlg = FALSE;

  debug("Creating toplevel for: -%s-", str.data());

  if(running_window == FALSE){ // If we're not fully running, reusing
			       // !default window for next chan.
    running_window = TRUE;
    // insert and remove is done as a side effect of the control_message call
    // TopList.insert(str, TopList["!no_channel"]); 
    // TopList.remove("!no_channel"); // We're no longer !no_channel
    TopList["!no_channel"]->control_message(CHANGE_CHANNEL, QString(server) + "!!!" + str);
  }
  else if(TopList.find(str.data()) == 0x0){ // If the window doesn't exist, continue
    // If AutoCreate windows is on, let's make sure we're not being flooded.
    if(kSircConfig->AutoCreateWin == TRUE){
      time_t current_time = time(NULL);
      if((current_time - last_window_open) < 5){
        if(number_open > 4 && flood_dlg == FALSE){
          flood_dlg = TRUE;
	  switch(QMessageBox::warning(0, "Flood warning",
				      "5 Channel windows were opened\n"
				      "in less than 5 seconds.  Someone\n"
				      "maybe trying to flood your X server\n"
				      "with windows.\n\n"
				      "Should I turn off AutoCreate windows?\n",
				      "Yes", "No", (char *) 0, 0, 0)){

          case 0:
	    emit ProcMessage(QString(server), ProcCommand::turnOffAutoCreate, QString());
	  }
	  last_window_open = current_time;
	  number_open = 0;
	}
	else{
	  number_open++;
        }
        flood_dlg = FALSE;
      }
      else{
	last_window_open = current_time;
      }
    }

    // Create a new("toplevel,") toplevel, and add it to the toplist.  
    // TopList is a list of KSircReceivers so we still need wm.
//    KSircMessageReceiver *faker = new("KSircMessageReceiver") KSircMessageReceiver(this);
    //    TopList.insert(str, faker); // Insert place holder since the constructor for kSircTopLevel may parse the event queue which will cause us to try and create trhe window several times!!!
    debug("Calling new toplevel for: -%s-", str.data());
    KSircTopLevel *wm = new("KSircTopLevel") KSircTopLevel(this, str.data(), QString(server) +"_" + str);
    //    insertChild(wm); // Keep ineheratence going so we can find children
    installEventFilter(wm);
    TopList.insert(str, wm);
//    TopList.replace(str, wm);
//    delete faker;
    // Connect needed signals.  For a message window we never want it
    // becomming the default so we ignore focusIn events into it.
    connect(wm, SIGNAL(outputLine(QString)),
	    iocontrol, SLOT(stdin_write(QString)));
    connect(wm, SIGNAL(open_toplevel(QString)),
	    this,SLOT(new_toplevel(QString)));
    connect(wm, SIGNAL(closing(KSircTopLevel *, char *)),
	    this,SLOT(close_toplevel(KSircTopLevel *, char *)));
    connect(wm, SIGNAL(currentWindow(KSircTopLevel *)),
	    this,SLOT(default_window(KSircTopLevel *)));
    connect(wm, SIGNAL(changeChannel(QString, QString)),
            this,SLOT(recvChangeChannel(QString, QString)));
    connect(wm, SIGNAL(objDestroyed(KSircTopLevel *)),
            this,SLOT(clean_toplevel(KSircTopLevel *)));
    default_window(wm); // Set it to the default window.
    emit ProcMessage(QString(server), ProcCommand::addTopLevel, str);

    displayMgr->newTopLevel(wm, TRUE);
    displayMgr->setCaption(wm, str);
    displayMgr->show(wm);
  }
  else{
    debug("Window %s already exists", str.data());
  }
}

void KSircProcess::close_toplevel(KSircTopLevel *wm, char *name) /*FOLD00*/
{

  bool is_default = FALSE; // Assume it's no default

  if(auto_create_really == TRUE)
      turn_on_autocreate();

  // Do this now or we get junk left on the screen
  displayMgr->removeTopLevel(wm);
  /*
  if(kSircConfig->MDIMode == TRUE && MDIMgr != 0x0){
    KMDIWindow *km = MDIMgr->getWindowByName(wm->original_name());
    if(km != 0x0){
      MDIMgr->removeWindow(km);
      km->hide();
    }
    }
    */

  if(TopList.count() <= 8){ // If this is the last window shut down
    QString command = "/quit\n";
    iocontrol->stdin_write(command); // kill sirc
    delete this; // Delete ourself, WARNING MUST RETURN SINCE WE NO
		 // LONGER EXIST!!!!
    return;      // ^^^^^^^^^^^^^^^
  }
  else if(wm == TopList["!default"]){ // Are we the current default?
    is_default = TRUE;
  }

  if(TopList[name]){  // Delete window
    delete TopList[name];
    while(TopList[name]) // In case multiple copies exist remove them all
      TopList.remove(name); // Though, this should never happend....
  }
  else // If the window wasn't found warn, something's going wrong.
    cerr << "DID NOT DELETE THE WINDOW: " << name << " !!!!\n";

  //
  // Ok, now if we just deleted the default we have a problem, we need
  // a new("default.") default.  BUT don't make the default "!all" or !message.
  // So let's go grab a default, and make sure it's not "!" control
  // object.
  //

  if(is_default == TRUE){
    QDictIterator<KSircMessageReceiver> it(TopList);
    it.toFirst();
    char *key = (char*) it.currentKey();
    if(key[0] == '!')
      for(;(key[0] == '!') && it.current(); ++it)
	key = (char*) it.currentKey();

    if(it.current())
      TopList.replace("!default", it.current());
    else{
      cerr << "NO MORE WINDOWS?\n"; // We're out of windows with > 3
				    // huh open, huh?
      TopList.remove("!default");   // let's not blow up to badly
      QString command = "/signoff\n";  // close this server connetion then
      iocontrol->stdin_write(command); // kill sirc
      delete this; // Delete ourself, WARNING MUST RETURN SINCE WE NO
                   // LONGER EXIST!!!!
      return;      // ^^^^^^^^^^^^^^^

    }
  }
  // Let's let em know she's deleted!
  if(kSircConfig->AutoCreateWin == TRUE){
      emit ProcMessage(QString(server), ProcCommand::turnOffAutoCreate, QString());
      QTimer::singleShot(5000, this, SLOT(turn_on_autocreate()));
      auto_create_really = TRUE;
  }
  else{
      auto_create_really = FALSE;
  }
  
  emit ProcMessage(QString(server), ProcCommand::deleteTopLevel,
                   QString(name));
}

void KSircProcess::clean_toplevel(KSircTopLevel *clean){ /*FOLD00*/
  if(clean == 0x0){
    warning("Passed null to cleaner!!");
    return;
  }
  bool cont = FALSE;
  do{
    cont = FALSE;
    QDictIterator<KSircMessageReceiver> it(TopList);
    while(it.current() != 0x0){
      if(it.current() == clean){
        char *key = strdup(it.currentKey());
        while(TopList[key] != 0x0){
          TopList.remove(key);
        }
        cont = TRUE;
        break;
      }
      ++it;
    }
  } while(cont == TRUE);
}

void KSircProcess::default_window(KSircTopLevel *w) /*FOLD00*/
{

  //
  // If we want to track the default as it goes arround, change the
  // window on focus changes.
  //

  if(w && (default_follow_focus == TRUE))
    TopList.replace("!default", w);

}

void KSircProcess::recvChangeChannel(QString old_chan, QString /*fold00*/
				     new_chan)
{
  //
  // Channel changed name, add our own name and off we go.
  // ServerController needs our name so it can have a uniq handle for
  // the window name.
  //
  if(TopList[old_chan])
    TopList.insert(new_chan, TopList.take(old_chan));
  emit ProcMessage(QString(server), ProcCommand::changeChannel,
		   old_chan + " " + new_chan);
}

void KSircProcess::filters_update() /*FOLD00*/
{
  QString command, next_part, key, data;
  command = "/crule\n";
  iocontrol->stdin_write(command);
  QDictIterator<KSircMessageReceiver> it(TopList);
  KSircMessageReceiver *cur, *br;
  filterRuleList *frl;
  filterRule *fr;
  cur = TopList["!base_rules"];
  br = cur;
  while(cur){
    frl = cur->defaultRules();
    for ( fr=frl->first(); fr != 0; fr=frl->next() ){
      command.truncate(0);
      command += "/ksircappendrule DESC==";
      command += fr->desc;
      command += " !!! SEARCH==";
      command += fr->search;
      command += " !!! FROM==";
      command += fr->from;
      command += " !!! TO==\"";
      command += fr->to;
      command += "\"\n";
      iocontrol->stdin_write(command);
    }
    delete frl;
    ++it;
    cur = it.current();
    if(cur == br){
      ++it;
      cur = it.current();
    }
  }
  kConfig->setGroup("FilterRules");
  int max = kConfig->readNumEntry("Rules", 0);
  for(int number = 1; number <= max; number++){
    command.truncate(0);
    key.sprintf("name-%d", number);
    next_part.sprintf("/ksircappendrule DESC==%s !!! ", kConfig->readEntry(key).data());
    command += next_part;
    key.sprintf("search-%d", number);
    next_part.sprintf("SEARCH==%s !!! ", kConfig->readEntry(key).data());
    command += next_part;
    key.sprintf("from-%d", number);
    next_part.sprintf("FROM==%s !!! ", kConfig->readEntry(key).data());
    command += next_part;
    key.sprintf("to-%d", number);
    next_part.sprintf("TO==\"%s\"\n", kConfig->readEntry(key).data());
    command += next_part;
    iocontrol->stdin_write(command);
  }
}


void KSircProcess::notify_forw_online(QString nick) /*fold00*/
{
  emit ProcMessage(QString(server), ProcCommand::nickOnline, nick);
}

void KSircProcess::notify_forw_offline(QString nick) /*fold00*/
{
  emit ProcMessage(QString(server), ProcCommand::nickOffline, nick);
}

void KSircProcess::ServMessage(QString dst_server, int command, QString args) /*FOLD00*/
{
  if(dst_server.isEmpty() || (dst_server == QString(server))){
    switch(command){
    case ServCommand::updateFilters:
      filters_update();
      break;
    default:
      cerr << "Unkown command: " << command << " to " << command << " args " << args << endl;
      break;
    }
  }
}

void KSircProcess::turn_on_autocreate()
{
    emit ProcMessage(QString(server), ProcCommand::turnOnAutoCreate, QString());
    auto_create_really = FALSE;
}

#include "ksircprocess.moc"
