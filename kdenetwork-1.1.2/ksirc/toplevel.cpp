/**********************************************************************

 TopLevel IRC Channel/query Window

 $$Id: toplevel.cpp,v 1.91.2.2 1999/02/11 15:12:01 kulow Exp $$

 This is the main window with with the user interacts.  It handles
 both normal channel converstations and private conversations.

 2 classes are defined, the UserControlMenu and KSircToplevel.  The
 user control menu is used as alist of user defineable menus used by
 KSircToplevel.  

 KSircTopLevel:

 Signals: 

 outputLine(QString &): 
 output_toplevel(QString):

 closing(KSircTopLevel *, char *): 

 changeChannel(QString old, QString new): 

 currentWindow(KSircTopLevel *):

 changeSize(): 

 Slots: 
 

 
 *********************************************************************/

#include "toplevel.h"
#include "iocontroller.h"
#include "open_top.h"
#include "control_message.h"
#include "config.h"
#include "KSCutDialog/KSCutDialog.h"
#include "ssfeprompt.h"
#include "estring.h"
#include "displayMgr.h"

#include <iostream.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include <qclipbrd.h> 
#include <qmsgbox.h> 
#include <qtooltip.h>

#include <knewpanner.h>
#include <kiconloader.h>

#include <kwm.h>

extern KConfig *kConfig;
extern KApplication *kApp;
//extern KMDIMgr *MDIMgr;
extern DisplayMgr *displayMgr;
//QPopupMenu *KSircTopLevel::user_controls = 0L;
QList<UserControlMenu> *KSircTopLevel::user_menu = 0L;
QPixmap *KSircTopLevel::pix_info = 0L;
QPixmap *KSircTopLevel::pix_star = 0L;
QPixmap *KSircTopLevel::pix_bball = 0L;
QPixmap *KSircTopLevel::pix_greenp = 0L;
QPixmap *KSircTopLevel::pix_bluep = 0L;
QPixmap *KSircTopLevel::pix_madsmile = 0L;
QPixmap *KSircTopLevel::pix_server = 0L;

KSircTopLevel::KSircTopLevel(KSircProcess *_proc, char *cname, const char * name)  /*FOLD00*/
  : KTopLevelWidget(name),
    KSircMessageReceiver(_proc)
   
{
  /* 
   * Make sure we tell others when we are destroyed
   */
  connect(this, SIGNAL(destroyed()),
	  this, SLOT(iamDestroyed()));

  /*
   * QPopup title bar, does not actually do anything at this time
   */

  proc = _proc;


  // Set the name, and keep the orignal so we can use the MDI manager
  orig_name = qstrdup(QString(QObject::name()) + "_" + "toplevel");
  QString kstl_name = QString(QObject::name()) + "_" + "toplevel";
  setName(kstl_name);

  
  channel_name = qstrdup(cname);
  if(channel_name){
  //    QString s = channel_name;
  //    int pos2 = s.find(' ', 0);
  //    if(pos2 > 0)
  //      channel_name = qstrdup(s.mid(0, pos2).data());
    
    setCaption(channel_name);
    caption = channel_name;
  }
  else
    caption = "";

  LineBuffer = new("QStrListTopLevel") QStrList(TRUE);
  Buffer = FALSE;

  have_focus = 0;
  ticker = 0; // Set the ticker to NULL while it doesn't exist.
  tab_pressed = 0; // Tab (nick completion not pressed yet)
  tab_start = -1;
  tab_end = -1;

  KickWinOpen = false;
  current_size = size();
//  startTimer( 500 ); // Start resize timer


  /*
   * Create the status bar which will hold the ping time and status info
   */
  ktool = new("KToolBar") KToolBar(this, "toolbar");
  ktool->setFullWidth(TRUE);
  ktool->insertWidget(0, 200, new("QFrame") QFrame(ktool));
  ktool->setItemAutoSized(0, TRUE);
  ktool->insertWidget(10, 100, new("QFrame") QFrame(ktool));
  ktool->alignItemRight(10, TRUE);
  addToolBar(ktool);

  ktool->getWidget(10)->setName(QString(QObject::name()) + "_ktoolframe");
  lagmeter = new("QLCDNumber") QLCDNumber(6, ktool->getFrame(10), QString(QObject::name()) + "_lagmeter");
  lagmeter->setFrameStyle(QFrame::NoFrame);
  lagmeter->setFixedHeight(ktool->height() - 2);
  lagmeter->display("      ");
  QToolTip::add(lagmeter, i18n("Lag in seconds to the server"));
  
  file = new("KAPopupMenuFile") KAPopupMenu(0x0, QString(QObject::name()) + "_popup_file");
  file->insertItem(i18n("&New Window..."), this, SLOT(newWindow()), CTRL + Key_N);
  file->insertItem(i18n("&Ticker Mode"), this, SLOT(showTicker()), CTRL + Key_T);
  //  file->insertItem("&Root Window Mode", this, SLOT(toggleRootWindow()), CTRL + Key_Z);
  file->insertSeparator();
  file->insertItem(i18n("&Close"), this, SLOT(terminate()), CTRL + Key_W );

  QFrame *menu_frame = (QFrame *) ktool->getWidget(0); // Lookup the insertWidget, it's a QFrame inserted
  CHECK_PTR(menu_frame);
  menu_frame->setFrameStyle(QFrame::NoFrame); // Turn off the frame style.
  menu_frame->setLineWidth(0);
  
  kmenu = new("QMenuBar") QMenuBar(this, "menubar");
  kmenu->setFrameStyle(QFrame::NoFrame); // Turn off frame style.
  kmenu->setLineWidth(0);  
  kmenu->resize(width(), height() - 2);
  if(style() == MotifStyle)
    kmenu->recreate(menu_frame, 0, QPoint(0,-3));
  else
    kmenu->recreate(menu_frame, 0, QPoint(0,0));
  kmenu->insertItem(i18n("&File"), file, 2, -1);
  kmenu->setAccel(Key_F, 2);

  edit = new("KAPopupMenuEdit") KAPopupMenu();
  edit->insertItem(i18n("&Cut Window..."), this, SLOT(openCutWindow()), CTRL + Key_X);
  edit->insertItem(i18n("&Paste"), this, SLOT(pasteToWindow()), CTRL + Key_V);
  kmenu->insertItem(i18n("&Edit"), edit, -1, -1);


  /*
   * Ok, let's look at the basic widge t "layout"
   * Everything belongs to q QFrame F, this is use so we
   * can give the KApplication a single main client widget, which is needs.
   *
   * A QVbox and a QHbox is used to ctronl the 3 sub widget
   * The Modified QListBox is then added side by side with the User list box.
   * The SLE is then fit bello.
   */

  // kstInside does not setup fonts, etc, it simply handles sizing

  f = new("kstInside") kstInside(this, QString(QObject::name()) + "_" + "kstIFrame");
  setView(f);  // Tell the KApplication what the main widget is.

  if(kSircConfig->colour_background == 0){
    kConfig->setGroup("Colours");
    kSircConfig->colour_background = new("QColor") QColor(kConfig->readColorEntry("Background", new("QColor") QColor(colorGroup().mid())));
  }
  
  // get basic variable

  mainw = f->mainw;
  nicks = f->nicks;
  pan = f->pan;
  linee = f->linee;

  if((channel_name[0] == '!') ||
     (channel_name[0] == '&') ||
     (channel_name[0] == '#')){
  }
  else{
    pan->setSeparatorPos(100);
  }

  connect(mainw, SIGNAL(updateSize()),
          this, SIGNAL(changeSize()));
  connect(mainw, SIGNAL(pasteReq()),
	  this, SLOT(pasteToWindow()));

  mainw->setFont(kSircConfig->defaultfont);
  nicks->setFont(kSircConfig->defaultfont);

  QColorGroup cg = QColorGroup(*kSircConfig->colour_text, colorGroup().mid(), 
    			       colorGroup().light(), colorGroup().dark(),
  			       colorGroup().midlight(), 
  			       *kSircConfig->colour_text, 
			       *kSircConfig->colour_background); 

  mainw->setPalette(QPalette(cg,cg,cg));   // Font it also hard coded
  nicks->setPalette(QPalette(cg,cg,cg));   // HARD CODED COLOURS AGAIN!!!!
  linee->setPalette(QPalette(cg,cg,cg));   // HARD CODED COLOURS AGAIN!!!!

  // setup line editor

  linee->setFocusPolicy(QWidget::StrongFocus);
  linee->setFont(kSircConfig->defaultfont);
  connect(linee, SIGNAL(gotFocus()),
	  this, SLOT(gotFocus()));
  connect(linee, SIGNAL(lostFocus()),
	  this, SLOT(lostFocus()));
  connect(linee, SIGNAL(pasteText()),
	  this, SLOT(pasteToWindow()));
  connect(linee, SIGNAL(notTab()),
	  this, SLOT(lineeNotTab()));

  //  gm->addWidget(linee, 0);                    // No special controls are needed.

  connect(linee, SIGNAL(returnPressed()), // Connect return in sle to send
  	  this, SLOT(sirc_line_return()));// the line to dsirc
 
  linee->setFocus();  // Give SLE focus

  lines = 0;          // Set internal line counter to 0
  contents.setAutoDelete( TRUE ); // Have contents, the line holder nuke everything on exit

  /*
   * Set generic run time variables
   *
   */

  opami = FALSE;
  continued_line = FALSE;
  on_root = FALSE;

  /*
   * Load basic pics and images
   * This should use on of the KDE finder commands
   * CHANGE THIS AWAY FROM HARD CODED PATHS!!!
   */

  if(pix_info == 0){
    KIconLoader *kicl = kApp->getIconLoader();
    QStrList *strlist = kicl->getDirList();
    kicl->insertDirectory(strlist->count(), kapp->kde_datadir() + "/ksirc/icons");
    pix_info = new("QPixmap") QPixmap(kicl->loadIcon("info.gif"));
    pix_star = new("QPixmap") QPixmap(kicl->loadIcon("star.gif"));
    pix_bball = new("QPixmap") QPixmap(kicl->loadIcon("blueball.gif"));
    pix_greenp = new("QPixmap") QPixmap(kicl->loadIcon("greenpin.gif"));
    pix_bluep = new("QPixmap") QPixmap(kicl->loadIcon("bluepin.gif"));
    pix_madsmile = new("QPixmap") QPixmap(kicl->loadIcon("madsmiley.gif"));
    pix_server = new("QPixmap") QPixmap(kicl->loadIcon("mini-edit.gif"));
  }
  setIcon(*pix_server);
  KWM::setMiniIcon(winId(), *pix_server);

  /*
   * Create our basic parser object
   */

  ChanParser = new("ChannelParser") ChannelParser(this);
  

  /* 
   * Create the user Controls popup menu, and connect it with the
   * nicks list on the lefthand side (highlighted()).
   * 
   */

  //  if(!user_controls)
  //    user_controls = new("QPopupMenu") QPopupMenu();

  if(user_menu == 0)
    user_menu = UserControlMenu::parseKConfig();

  user_controls = new("QPopupMenuUserControls") QPopupMenu();
  kmenu->insertItem(i18n("&Users"), user_controls);

  connect(user_controls, SIGNAL(activated(int)), 
	  this, SLOT(UserParseMenu(int)));

  connect(nicks, SIGNAL(rightButtonPress(int)), this,
	  SLOT(UserSelected(int)));


  UserUpdateMenu();  // Must call to update Popup.

  accel = new("QAccel") QAccel(this, "accel");

  accel->connectItem(accel->insertItem(SHIFT + Key_PageUp),
		     this,
		     SLOT(AccelScrollUpPage()));
  accel->connectItem(accel->insertItem(SHIFT + Key_PageDown),
		     this,
                     SLOT(AccelScrollDownPage()));

  /*
   * Pageup/dn
   * Added for stupid wheel mice
   */
  
  accel->connectItem(accel->insertItem(Key_PageUp),
		     this,
		     SLOT(AccelScrollUpPage()));
  accel->connectItem(accel->insertItem(Key_PageDown),
                     this,
                     SLOT(AccelScrollDownPage()));

  accel->connectItem(accel->insertItem(CTRL + Key_Enter),
		     this,
		     SLOT(AccelPriorMsgNick()));

  accel->connectItem(accel->insertItem(CTRL + SHIFT + Key_Enter),
		     this,
		     SLOT(AccelNextMsgNick()));

  accel->connectItem(accel->insertItem(CTRL + Key_Return),
		     this,
		     SLOT(AccelPriorMsgNick()));

  accel->connectItem(accel->insertItem(CTRL + SHIFT + Key_Return),
		     this,
		     SLOT(AccelNextMsgNick()));

  accel->connectItem(accel->insertItem(Key_Tab), // adds TAB accelerator
                     this,                         // connected to the main
                     SLOT(TabNickCompletion()));  // TabNickCompletion() slot
  accel->connectItem(accel->insertItem(CTRL + Key_N),
		     this, SLOT(newWindow()));
  accel->connectItem(accel->insertItem(CTRL + Key_T),
		     this, SLOT(showTicker()));
  accel->connectItem(accel->insertItem(CTRL + Key_Q),
		     this, SLOT(terminate()));
  accel->connectItem(accel->insertItem(ALT + Key_F4),
		     this, SLOT(terminate()));


}


KSircTopLevel::~KSircTopLevel() /*FOLD00*/
{

  // Cleanup and shutdown
  //  writePopUpMenu();
  //  if(this == proc->getWindowList()["default"])
  //    write(sirc_stdin, "/quit\n", 7); // tell dsirc to close
  //
  //  if(proc->getWindowList()[channel_name])
  //    proc->getWindowList().remove(channel_name);

  //  if((channel_name[0] == '#') || (channel_name[0] == '&')){
  //    QString str = QString("/part ") + channel_name + "\n";
  //    emit outputLine(str);
  //  }

  if((channel_name[0] == '#') || (channel_name[0] == '&')){
    QString str = QString("/part ") + channel_name + "\n";
    emit outputLine(str);
  }

  if(ticker)
    delete ticker;
  ticker = 0;
  //  delete gm; // Deletes everthing bellow it I guess...
  //  delete gm2; 
  //  delete pan; // Should be deleted by gm2
  //  delete linee; // ditto
  delete LineBuffer;
  delete user_controls;
  delete file;
  delete edit;

  QToolTip::remove(lagmeter);

  delete kmenu;
  delete ktool;
  delete ChanParser;
  free(orig_name);
}

void KSircTopLevel::show() /*FOLD00*/
{
  if(ticker){
    ticker->show();
    ticker->raise();
  }
  else{
    KTopLevelWidget::show();
    kmenu->show();
    lagmeter->show();
  }
}

//void KSircTopLevel::sirc_stop(bool STOP = FALSE)
//{
//  if(STOP == TRUE){
//    Buffer = TRUE;
//  }
//  else{
//    Buffer = FALSE;
//    if(LineBuffer->isEmpty() == FALSE)
//      sirc_receive(QString(""));
//  }
//}

void KSircTopLevel::TabNickCompletion()  /*FOLD00*/
{
  /* 
   * Gets current text from lined find the last item and try and perform
   * a nick completion, then return and reset the line.
   */

  int start, end;
  QString s;

  if(tab_pressed > 0){
    s = tab_saved.data();
    start = tab_start;
    end = tab_end;
  }
  else{
    s = linee->text();
    tab_saved = s.data();
    end = linee->cursorPosition() - 1;
    start = s.findRev(" ", end, FALSE);
    tab_start = start;
    tab_end = end;

  }

  if(s.length() == 0){
    QString line = tab_nick + ": "; // tab_nick holds the last night since we haven't overritten it yet.
    linee->setText(line);
    linee->setCursorPosition(line.length());
    return;
  }

  if (start == -1) {
    tab_nick = findNick(s.mid(0, end+1), tab_pressed);
    if(tab_nick.isNull() == TRUE){
      tab_pressed = 0;
      tab_nick = findNick(s.mid(0, end+1), tab_pressed);
    }
    s.replace(0, end + 1, tab_nick);
  }
  else {
    tab_nick = findNick(s.mid(start + 1, end - start), tab_pressed);
    if(tab_nick.isNull() == TRUE){
      tab_pressed = 0;
      tab_nick = findNick(s.mid(start + 1, end - start), tab_pressed);
    }
    s.replace(start + 1, end - start, tab_nick);
  }

  int tab = tab_pressed + 1;

  linee->setText(s);

  linee->setCursorPosition(start + tab_nick.length() + 1);
  
  tab_pressed = tab; // setText causes lineeTextChanged to get called and erase tab_pressed

  connect(linee, SIGNAL(notTab()),
	  this, SLOT(lineeNotTab()));
  
}
  
void KSircTopLevel::sirc_receive(QString str) /*FOLD00*/
{

  /* 
   * read and parse output from dsirc.
   * call reader, split the read input into lines, parse the lines
   * then print line by line into the main text area.
   *
   * PROBLEMS: if a line terminates in mid line, it will get borken oddly
   *
   */

  /*
   * If we have to many lines, nuke the top 100, leave us with 100
   */

  int lines = 0;

  if(Buffer == FALSE){
    if(LineBuffer->count() >= 2){
      mainw->setAutoUpdate(FALSE);
    }
    
    if(str.isEmpty() == FALSE){
      LineBuffer->append(str);
    }

    ircListItem *item = 0;
    char *pchar;
    QString string;
    bool update = FALSE;

    for(pchar = LineBuffer->first(); 
	pchar != 0; 
	LineBuffer->removeFirst(),     // Remove the first one
	  pchar=LineBuffer->first()){  // Get the new("first") first one
      // Get the need list box item, with colour, etc all set
      string = pchar;
      item = parse_input(string);
      // If we shuold add anything, add it.
      // Item might be null, if we shuoold ingore the line
      
      if(item){
	// Insert line to the end
	connect(this, SIGNAL(changeSize()),
		item, SLOT(updateSize()));
	connect(this, SIGNAL(freezeUpdates(bool)),
		item, SLOT(freeze(bool)));
	mainw->insertItem(item, -1);
	if(ticker){
	  QString text;
	  int colour = KSPainter::colour2num(*(item->defcolour()));
	  if(colour >= 0){
	    text.setNum(colour);
	    text.prepend("~");
	  }
	  text.append(item->getText());
	  ticker->mergeString(text + "~C // ");
	}
	lines++; // Mode up lin counter
	update = TRUE;
      }
    }
    LineBuffer->clear(); // Clear it since it's been added

    if(mainw->count() > (1.25*kSircConfig->WindowLength)){
        mainw->setAutoUpdate(FALSE);
        update = TRUE;
        while(mainw->count() > kSircConfig->WindowLength)
           mainw->removeItem(0);
    }

    mainw->setAutoUpdate(TRUE);

    mainw->scrollToBottom();
    mainw->repaint(FALSE); // Repaint, but not need to erase and cause fliker.
  }
  else{
    LineBuffer->append(str);
  }
}

void KSircTopLevel::sirc_line_return() /*fold00*/
{

  /* Take line from SLE, and output if to dsirc */

  QString s = linee->text();

  if(s.length() == 0)
    return;

  tab_pressed = 0; // new("line,") line, zero the counter.

  // 
  // Lookup the nick completion
  // Do this before we append the linefeed!!
  //

  int pos2;
  
  if(kSircConfig->NickCompletion == TRUE){
    if(s.find(QRegExp("^[^ :]+: "), 0) != -1){
      pos2 = s.find(": ", 0);
      if(pos2 < 1){
	cerr << "Evil string: " << s << endl;
      }
      else
	s.replace(0, pos2, findNick(s.mid(0, pos2)));
    }
  }

  s += '\n'; // Append a need carriage return :)

  if((uint) nick_ring.at() < (nick_ring.count() - 1))
    nick_ring.next();
  else
    nick_ring.last();

  sirc_write(s);

  linee->setText("");
  
}

void KSircTopLevel::sirc_write(QString str) /*FOLD00*/
{
  /*
   * Parse line forcommand we handle
   */

  str.detach();

  if((strncmp(str, "/join ", 6) == 0) ||
     (strncmp(str, "/j ", 3) == 0) ||
     (strncmp(str, "/query ", 7) == 0)){
    str = str.lower();
    int pos1 = str.find(' ', 0) + 1;
    if(pos1 == -1)
      return;
    while(str[pos1] == ' ')
      pos1++;
    int pos2 = str.find(' ', pos1) - 1;
    if(pos2 < 0)
      pos2 = str.length() - 1; // -1 don't include the enter
    if(pos1 > 2){
      QString name = str.mid(pos1, pos2 - pos1); // make sure to remove line feed
      emit open_toplevel(name);
      if(name[0] != '#'){
	linee->setText("");
	return;
      }
      // Finish sending /join
    }
  }
  else if(strncmp(str, "/server ", 6) == 0){
    QString command = "/eval &print(\"*E* Use The Server Controller\\n\");\n";
    sirc_write(command);
    linee->setText("");
    return;
  }
  else if((strncmp(str, "/part", 5) == 0) ||
	  (strncmp(str, "/leave", 6) == 0) ||
	  (strncmp(str, "/hop", 4) == 0) ||
	  (strncmp(str, "/bye", 4) == 0) ||
	  (strncmp(str, "/quit", 5) == 0)){
    QApplication::postEvent(this, new("QCloseEvent") QCloseEvent()); // WE'RE DEAD
    linee->setText("");
    str.truncate(0);
    return;
  }

  //
  // Look at the command, if we're assigned a channel name, default
  // messages, etc to the right place.  This include /me, etc
  //

  if(channel_name[0] != '!'){
    str.detach();
    if(str[0] != '/'){
      str.prepend(QString("/msg ") + channel_name + QString(" "));
    }
    else if(strnicmp(str, "/me", 3) == 0){
      str.remove(0, 3);
      str.prepend(QString("/de ") + channel_name);
    }
  }
  
  // Write out line

  //  proc->stdin_write(str);
  mainw->scrollToBottom(TRUE);
  emit outputLine(str);

}

ircListItem *KSircTopLevel::parse_input(QString string) /*FOLD00*/
{

  /* 
   * Parsing routines are broken into 3 majour sections 
   *
   * 1. Search and replace evil characters. The string is searched for
   * each character in turn (evil[i]), and the character string in
   * evil_rep[i] is replaced.  
   *
   * 2. Parse control messages, add pixmaps, and colourize required
   * lines.  Caption is also set as required.
   *
   * 3. Create and return the ircListItem.
   *
   */

  /*
   * c: main switch char, used to id *'s and ssfe commands
   * s2: temporary string pointer, used is parsing title string
   * evil[]: list of eveil characters that should be removed from 
   *         the data stream.
   * evil_rep[][], list of characters to fill replace evil[] with
   * s3: temp Qstring
   * s4: temp QString
   * pos: start position indicator, used for updating nick info
   * pos2: end position indicator, tail end for nick info
   * color: colour for ListBox line entry
   * pixmap: pixmap for left hand side of list box
   * */


  QString s3, s4, channel;
  QColor *color = kSircConfig->colour_text;
  QPixmap *pixmap = NULL;

  /*
   * No-output get's set to 1 if we should ignore the line
   */

  /*
   * This is the second generation parsing code.
   * Each line type is determined by the first 3 characters on it.
   * Each line type is then sent to a parsing function.
   */
  try {
    ChanParser->parse(string);
  }
  catch(parseSucc &item){
    if(item.string.length() > 0)
      return new("ircListItem") ircListItem(item.string,item.colour,mainw,item.pm);
    else
      return NULL;
  }
  catch(parseError &err){
    if(err.err.isEmpty() == FALSE){
      QString format = err.err + ": %s";
      warning(format.data(), string.data());
    }
    if(err.str.isEmpty() == FALSE){
      return new("ircListItem") ircListItem(err.str, kSircConfig->colour_error, mainw, pix_madsmile);
    }
    return NULL;
  }
  catch(estringOutOfBounds &err){
      warning("esParsing failed on: %s", err.str.data());
      return NULL;
  }
  
  /*
   * We've fallen out of the loop
   */
  return new("ircListItem") ircListItem(string,color,mainw,pixmap);
}

void KSircTopLevel::UserSelected(int index) /*fold00*/
{
  if(index >= 0)
    user_controls->popup(this->cursor().pos());
}

void KSircTopLevel::UserParseMenu(int id) /*FOLD00*/
{
  if(nicks->currentItem() < 0){
    QMessageBox::warning(this, "Warning, dork at the helm Captain!\n",
			 "Warning, dork at the helm Captain!\nTry Selecting a nick first!");
    return;
  }
  QString s;
  s = QString("/eval $dest_nick='") + 
    QString(nicks->text(nicks->currentItem())) + 
    QString("';\n");
  sirc_write(s);
  s = QString("/eval &docommand(eval{\"") + 
    QString(user_menu->at(id)->action) +
    QString("\"});\n");
  s.replace(QRegExp("\\$\\$"), "$");
  sirc_write(s);
}

void KSircTopLevel::UserUpdateMenu() /*FOLD00*/
{
  int i = 0;
  UserControlMenu *ucm;
  //  QPopupMenu *umenu;

  user_controls->clear();
  for(ucm = user_menu->first(); ucm != 0; ucm = user_menu->next(), i++){
    if(ucm->type == UserControlMenu::Seperator){
      user_controls->insertSeparator();
    }
    else{
      user_controls->insertItem(i18n(ucm->title), i);
      if(ucm->accel)
	user_controls->setAccel(i, ucm->accel);
      if((ucm->op_only == TRUE) && (opami == FALSE))
	user_controls->setItemEnabled(i, FALSE);
    }
  }
  //  writePopUpMenu();
}

void KSircTopLevel::AccelScrollDownPage() /*fold00*/
{
    mainw->pageDown();
}

void KSircTopLevel::AccelScrollUpPage() /*fold00*/
{
    mainw->pageUp();
}
void KSircTopLevel::AccelPriorMsgNick() /*fold00*/
{
  linee->setText(QString("/msg ") + nick_ring.current() + " ");

  if(nick_ring.at() > 0)
    nick_ring.prev();

}

void KSircTopLevel::AccelNextMsgNick() /*fold00*/
{
  if(nick_ring.at() < ((int) nick_ring.count() - 1) )
    linee->setText(QString("/msg ") + nick_ring.next() + " ");
}

void KSircTopLevel::newWindow()  /*fold00*/
{ 
  open_top *w = new("open_top") open_top(); 
  connect(w, SIGNAL(open_toplevel(QString)),
	  this, SIGNAL(open_toplevel(QString)));
  w->show();
}

void KSircTopLevel::closeEvent(QCloseEvent *) /*FOLD00*/
{
  // Let's not part the channel till we are acutally delete.
  // We should always get a close event, *BUT* we will always be deleted.
  //  if((channel_name[0] == '#') || (channel_name[0] == '&')){
  //    QString str = QString("/part ") + channel_name + "\n";
  //    emit outputLine(str);
  //  }

  // Hide ourselves until we finally die
  hide();
  // Let's say we're closing, what ever connects to this should delete us.
  emit closing(this, channel_name); // This should call "delete this".
  // This line is NEVER reached.
}

void KSircTopLevel::resizeEvent(QResizeEvent *e) /*FOLD00*/
{
  bool update = mainw->autoUpdate();
  mainw->setAutoUpdate(FALSE);
  KTopLevelWidget::resizeEvent(e);
  // If we are talking too a person, don't show the nick list
  if((channel_name[0] == '!') ||
     (channel_name[0] == '&') ||
     (channel_name[0] == '#')){
    //    pan->setSeparatorPos(85);
  }
  else{
    //    pan->setSeparatorPos(100);
  }

  mainw->setAutoUpdate(update);

//  debug("Finished main window resize event");
  // The ListBox will get an implicit size change

  // Delete QPopup menus
  //delete file;
}

void KSircTopLevel::gotFocus() /*FOLD00*/
{
  if(isVisible() == TRUE){
    if(have_focus == 0){
      if(channel_name[0] == '#'){
	QString str = "/join " + QString(channel_name) + "\n";
	emit outputLine(str);
      }
      have_focus = 1;
      emit currentWindow(this);
      //    cerr << channel_name << " got focusIn Event\n";
    }
  }
}

void KSircTopLevel::lostFocus() /*fold00*/
{
  if(have_focus == 1){
    have_focus = 0;
    //    cerr << channel_name << " got focusIn Event\n";
  }

}

void KSircTopLevel::control_message(int command, QString str) /*FOLD00*/
{
  switch(command){
  case CHANGE_CHANNEL: // 001 is defined as changeChannel
    {
      QString server, chan;
      int bang;
      bang = str.find("!!!");
      if(bang < 0){
          chan = str;
          QString mname = name();
          int end = mname.find('_');
          if(end < 0){
              warning("Change channel message was invalid: %s", str.data());
              break;
          }
          server = mname.mid(0, end);
      }
      else{
          server = str.mid(0, bang);
          chan = str.mid(bang + 3, str.length() - (bang + 3));
      }
      emit changeChannel(channel_name, chan.data());
      if(channel_name)
	delete channel_name;
      channel_name = qstrdup(chan.data());
      setName(server + "_" + QString(channel_name) + "_" + "toplevel");
      f->setName(QString(QString(QObject::name()) + "_" + "kstIFrame"));
      ktool->getFrame(10)->setName(QString(QObject::name()) + "_ktoolframe");
      lagmeter->setName(QString(QObject::name()) + "_lagmeter");
      have_focus = 0;
      this->setCaption(channel_name);
      mainw->scrollToBottom();
      emit currentWindow(this);
      break;
    }
  case STOP_UPDATES:
    Buffer = TRUE;
    break;
  case RESUME_UPDATES:
    Buffer = FALSE;
    if(LineBuffer->isEmpty() == FALSE)
      sirc_receive(QString(""));
    break;
  case REREAD_CONFIG:
    emit freezeUpdates(TRUE); // Stop the list boxes update
    mainw->setUpdatesEnabled(FALSE); // Let's try and reduce the flicker
    nicks->setUpdatesEnabled(FALSE); // just a little bit.
    linee->setUpdatesEnabled(FALSE);
    setUpdatesEnabled(FALSE);
    mainw->setFont(kSircConfig->defaultfont);
    mainw->updateTableSize();
    nicks->setFont(kSircConfig->defaultfont);
    linee->setFont(kSircConfig->defaultfont);
    linee->resize(width(), linee->fontMetrics().lineSpacing() + 8);
    //    linee->setMinimumHeight(linee->fontMetrics().lineSpacing() + 8);
    resize(size()); // Make the Layout manager make everything fit right.
    //    emit changeSize();
    {
      QColorGroup cg = QColorGroup(*kSircConfig->colour_text, colorGroup().mid(), 
				   colorGroup().light(), colorGroup().dark(),
				   colorGroup().midlight(), 
				   *kSircConfig->colour_text, 
				   *kSircConfig->colour_background); 
      mainw->setPalette(QPalette(cg, cg, cg));
      nicks->setPalette(QPalette(cg, cg, cg));
      linee->setPalette(QPalette(cg, cg, cg));
    }
    UserUpdateMenu();  // Must call to update Popup.
    mainw->setUpdatesEnabled(TRUE);
    nicks->setUpdatesEnabled(TRUE);
    linee->setUpdatesEnabled(TRUE);
    setUpdatesEnabled(TRUE);
    emit freezeUpdates(FALSE); // Stop the list boxes update
    emit changeSize(); // Have the ist box update correctly.
    mainw->scrollToBottom();
    repaint(TRUE);
    break;
  case SET_LAG:
    if(str.isNull() == FALSE){
      bool ok = TRUE;
      str.detach();
      str.truncate(6);
      double lag = str.toDouble(&ok);
      if(ok == TRUE){
      	lag -= (lag*100.0 - int(lag*100.0))/100.0;
	lagmeter->display(lag);
      }
      else{
	lagmeter->display(str);
      }
    }
    break;
  default:
    cerr << "Unkown control message: " << str << endl;
  }
}

void KSircTopLevel::showTicker() /*FOLD00*/
{
  myrect = geometry();
  mypoint = pos();
  ticker = new("KSTicker") KSTicker(0, "ticker", WStyle_NormalBorder);
  ticker->setCaption(caption);
  kConfig->setGroup("TickerDefaults");
  ticker->setFont(kConfig->readFontEntry("font", new("QFont") QFont("fixed")));
  ticker->setSpeed(kConfig->readNumEntry("tick", 30), 
		   kConfig->readNumEntry("step", 3));
  QColorGroup cg = QColorGroup(*kSircConfig->colour_text, colorGroup().mid(), 
                               colorGroup().light(), colorGroup().dark(),
                               colorGroup().midlight(), 
                               *kSircConfig->colour_text, 
                               *kSircConfig->colour_background); 
  ticker->setPalette(QPalette(cg,cg,cg));
  ticker->setBackgroundColor( *kSircConfig->colour_background );
  connect(ticker, SIGNAL(doubleClick()), 
	  this, SLOT(unHide()));
  connect(ticker, SIGNAL(closing()), 
          this, SLOT(terminate()));

  this->recreate(0, 0, QPoint(0,0));
  displayMgr->removeTopLevel(this);
  
  if(tickerrect.isEmpty() == FALSE){
    ticker->setGeometry(tickerrect);
    ticker->recreate(0, 0, tickerpoint, TRUE);
  }
  for(int i = 5; i > 0; i--)
    ticker->mergeString(QString(mainw->text(mainw->count()-i)) + " // ");

  ticker->show();
}

void KSircTopLevel::unHide() /*FOLD00*/
{
  tickerrect = ticker->geometry();
  tickerpoint = ticker->pos();
  int tick, step;
  ticker->speed(&tick, &step);
  kConfig->setGroup("TickerDefaults");
  kConfig->writeEntry("font", ticker->font());
  kConfig->writeEntry("tick", tick);
  kConfig->writeEntry("step", step);
  kConfig->sync();
  delete ticker;
  ticker = 0;
  displayMgr->newTopLevel(this, TRUE);
//  this->setGeometry(myrect);
//  this->recreate(0, getWFlags(), mypoint, TRUE);
//  this->show();
  linee->setFocus();  // Give SLE focus
}

QString KSircTopLevel::findNick(QString part, uint which) /*FOLD00*/
{
  QStrList matches;
  for(uint i=0; i < nicks->count(); i++){
    if(strlen(nicks->text(i)) >= part.length()){
      if(strnicmp(part, nicks->text(i), part.length()) == 0){
        kConfig->setGroup("StartUp");
        QString qsNick = kConfig->readEntry("Nick", "");
        if(strcmp(nicks->text(i), qsNick) != 0){ // Don't match your own nick
          matches.append(nicks->text(i));
        }
      }
    }
  }
  if(matches.count() > 0){
    if(which < matches.count())
      return matches.at(which);
    else
      return 0;
  }
  return part;
    
}

void KSircTopLevel::openCutWindow() /*FOLD00*/
{
  KSCutDialog *kscd = new("KSCutDialog") KSCutDialog();
  QString buffer;
  for(uint i = 0; i < mainw->count(); i++){
    buffer += mainw->text(i);
    buffer += "\n";
  }
  kscd->setText(buffer);
  kscd->scrollToBot();
  kscd->show();
  // kscd deletes it self.
}

void KSircTopLevel::pasteToWindow() /*fold00*/
{
  QString text = kApp->clipboard()->text();
  text += "\n";
  if((text.contains("\n") > 4) || (text.length() > 300)){
      switch( QMessageBox::warning(this, "Large Paste Requested",
				   "You are about to paste a very \nlarge number of lines,\ndo you really want to do this?",
				   "Yes", "No", 0, 0, 1)){
      case 0:
	break;
      default:
	linee->setText("");
	return;
      }
  }
  if(text.contains("\n") > 1){
    linee->setUpdatesEnabled(FALSE);
    for(QString line = strtok(text.data(), "\n");
	line.isNull() == FALSE;
	line = strtok(NULL, "\n")){
      QString hold = linee->text();
      hold += line;
      linee->setText(hold);
      sirc_line_return();
      linee->setText("");
    }
    linee->setText("");
    linee->setUpdatesEnabled(TRUE);
    linee->update();
  }
  else{
    text.replace(QRegExp("\n"), "");
    QString line = linee->text();
    line += text;
    linee->setText(line);
  }
}

void KSircTopLevel::lineeNotTab() /*fold00*/
{
  tab_pressed = 0;
  disconnect(linee, SIGNAL(notTab()),
	     this, SLOT(lineeNotTab()));

}

void KSircTopLevel::toggleRootWindow() /*fold00*/
{
}

void KSircTopLevel::iamDestroyed() /*fold00*/
{
  emit objDestroyed(this);
}

#undef BLAH
#ifdef BLAH
void KSircTopLevel::timerEvent( QTimerEvent * ){ /*FOLD00*/
//  debug("Tick:  current size: %d %d, real size: %d %d",
//	current_size.width(), current_size.height(),
//	size().width(), size().height());
  XWindowAttributes xwa;
  memset(&xwa, 0, sizeof(XWindowAttributes));
  XGetWindowAttributes(qt_xdisplay(), this->winId(), &xwa);
  current_size.setWidth(xwa.width);
  current_size.setHeight(xwa.height);
//  debug("Tick2: current size: %d %d, real size: %d %d",
//	current_size.width(), current_size.height(),
//	size().width(), size().height());
  if(size() != current_size){
    resize(current_size);
  }
}
#endif

void KSircTopLevel::gotMDIFocus(KMDIWindow *win)
{
  setFocus();
  f->setFocus();
  linee->setFocus();
  emit currentWindow(this);
}

void KSircTopLevel::setCaption(const char *str)
{
  displayMgr->setCaption(this, str);
  KTopLevelWidget::setCaption(str);
}

kstInside::kstInside ( QWidget * parent, const char * name, WFlags f,  /*fold00*/
		       bool allowLines )
  : QFrame(parent, name, f, allowLines)
{
  
  pan = new("KNewPanner") KNewPanner(this, "", KNewPanner::Vertical);

  mainw = new("KSircListBox") KSircListBox(pan, "");
  mainw->setFocusPolicy(QWidget::NoFocus);
//  mainw->setEnabled(FALSE);
  mainw->setSmoothScrolling(TRUE);       

  nicks = new("aListBox") aListBox(pan, "");
  nicks->setFocusPolicy(QWidget::NoFocus);

  linee = new("aHistLineEdit") aHistLineEdit(this, "");

  pan->activate(mainw, nicks);

  pan->setSeparatorPos(85);

  setName(name);

}

kstInside::~kstInside() /*fold00*/
{
  delete mainw;
  delete nicks;
  delete pan;
  delete linee;
}


void kstInside::resizeEvent(QResizeEvent *e) /*fold00*/
{
  QFrame::resizeEvent(e);

  int linee_height =  linee->fontMetrics().height() + 8;
  int real_height = this->height();
  linee->setGeometry(5, real_height - linee_height - 5,
		     width() - 10, linee_height);
  pan->setGeometry(5, 5,
                   width() - 10, real_height - linee_height - 15);
}

void kstInside::setName(const char *name) /*fold00*/
{
  QObject::setName(name);
  my_name = name;
  panner_name = my_name + "_" + "Panner";
  mainw_name = my_name + "_" + "MainIrc";
  nicks_name = my_name + "_" + "NickListBox";
  linee_name = my_name + "_" + "LineEnter";

  pan->setName(panner_name);
  mainw->setName(mainw_name);
  nicks->setName(nicks_name);
  linee->setName(linee_name);
}
#include "toplevel.moc"
