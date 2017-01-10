/*
  $Id: topwidget.cpp,v 1.218.2.9 1999/04/09 18:49:46 pbrown Exp $

	 Requires the Qt and KDE widget libraries, available at no cost at
	 http://www.troll.no and http://www.kde.org respectively
	 
	 Copyright (C) 1997, 1998 
	 Preston Brown (preston.brown@yale.edu)
	 Fester Zigterman (F.J.F.ZigtermanRustenburg@student.utwente.nl)
	 Ian Dawes (iadawes@globalserve.net)
	 Christopher Beard
	 Laszlo Boloni (boloni@cs.purdue.edu)

	 This program is free software; you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation; either version 2 of the License, or
	 (at your option) any later version.
	 
	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
	 GNU General Public License for more details.
	 
	 You should have received a copy of the GNU General Public License
	 along with this program; if not, write to the Free Software
	 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <sys/types.h>
#include <signal.h>

#include <kfiledialog.h>
#include <qfiledlg.h>
#include <qcursor.h>
#include <qmlined.h>
#include <qmsgbox.h>
#include <qtimer.h>
#include <kiconloader.h>
#include <ksimpleconfig.h>
#include <kstdaccel.h>

#include "misc.h"
#include "version.h"
#include "koarchivedlg.h"
#include "komailclient.h"
#include "calprinter.h"
#include "aboutdlg.h"

#include "topwidget.h"
#include "topwidget.moc"	

#define AGENDABUTTON 0x10
#define NOACCEL 0

QList<TopWidget> TopWidget::windowList;

TopWidget::TopWidget(CalObject *cal, QString fn, QWidget *, 
		     const char *name, bool fnOverride) 
  : KTopLevelWidget( name )
{
  searchDlg = 0L;
  recentPop = 0L;
  viewMode = AGENDAVIEW;
  toolBarEnable = statusBarEnable = TRUE;
  setMinimumSize(600,400);	// make sure we don't get resized too small...

  // get calendar from passed argument to the constructor.
  calendar = cal;

  // it is valid to get a NULL calender object
  // this is how TopWidget clones itself now
  if (! calendar)
    calendar = new CalObject;

  // create the main layout frames.
  mainFrame    = new QFrame(this);  
  panner = new KNewPanner(mainFrame, "panner",
			  KNewPanner::Vertical,
			  KNewPanner::Absolute, 164);
  leftFrame = new QFrame(panner);
  rightFrame = new QFrame(panner);

  optionsDlg = new OptionsDialog("KOrganizer Configuration Options",
				 this);
  connect(optionsDlg, SIGNAL(configChanged()),
	  this, SLOT(updateConfig()));


  QVBoxLayout *layout1 = new QVBoxLayout(leftFrame, 1, -1, "layout1");
  dateNavFrame = new QFrame(leftFrame);
  //  dateNavFrame->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  dateNavigator = new KDateNavigator(dateNavFrame, calendar, TRUE,
				     QDate::currentDate(),"DateNavigator");
  dateNavigator->move(2,2);
  dateNavigator->resize(160, 150);
  dateNavFrame->resize(dateNavigator->width()+4, dateNavigator->height()+4);
  dateNavFrame->setMinimumSize(dateNavFrame->size());
  dateNavFrame->setFixedHeight(dateNavigator->height());
  
  layout1->addWidget(dateNavFrame);
  layout1->addSpacing(5);

  // add this instance of the window to the static list.
  windowList.append(this);
  initCalendar(fn, fnOverride);

  // create the main data display views.
  todoList   = new TodoView(calendar, leftFrame);
  layout1->addWidget(todoList);
  layout1->activate();

  todoView   = new TodoView(calendar, mainFrame);

  QVBoxLayout *layout2 = new QVBoxLayout(rightFrame, 0, -1, "layout2");
  agendaView = new KAgenda(calendar, rightFrame);
  layout2->addWidget(agendaView);
  layout2->activate();

  QVBoxLayout *layout3 = new QVBoxLayout(rightFrame, 0, -1, "layout3");
  listView   = new ListView(calendar, rightFrame);
  layout3->addWidget(listView);
  layout3->activate();

  monthView  = new KDPMonthView(calendar, mainFrame);

  panner->activate(leftFrame, rightFrame);

  // setup toolbar, menubar and status bar, NOTE: this must be done
  // after the widget creation, because setting the menubar, toolbar
  // or statusbar will generate a call to updateRects, which assumes
  // that all of them are around.
  initMenus();
  initToolBar();

  sb = new KStatusBar(this, "sb");
  setStatusBar(sb);

  // set up printing object
  calPrinter = new CalPrinter(this, calendar);

  // hook up the signals/slots of all widgets together so communication
  // can happen when things get clicked.
  hookupSignals();
  
  // set up autoSaving stuff
  autoSaveTimer = new QTimer(this);
  connect(autoSaveTimer, SIGNAL(timeout()),
	  this, SLOT(checkAutoSave()));
  if (calendar->autoSave())
    autoSaveTimer->start(1000*60);
  
  // this is required for KTopLevelWidget for geometry management
  setView(mainFrame, TRUE);
  goToday();

  changeView(viewMode);
  goToday();
  changeAgendaView(agendaViewMode);

  setupRollover();

  // this isn't working right now -- it isn't properly turning
  // off the tool/status bars.  Bug in KTMainWindow?
  /*  if (!toolBarEnable)
    toggleToolBar();

  if (!statusBarEnable)
  toggleStatusBar();*/

  // make sure the caption is correct just before showing
  set_title();
}

TopWidget::~TopWidget(void)
{
  hide();

  // clean up our calender object
  calendar->close();
  delete calendar;

  // Free memory allocated for widgets (not children)
  // Take this window out of the window list.
  windowList.removeRef( this );

  //KTLW doesn't delete it's main view
  delete tb;
  delete menubar;
  delete mainFrame;
}


void TopWidget::initCalendar(QString fn, bool fnOverride) 
{
  // read the settings from the config file
  readSettings();
  // cmd line argument for filename overrides config files 
  if(!fn.isEmpty() || fnOverride)
    fileName = fn;

  QApplication::setOverrideCursor(waitCursor);
  // note -- we MAY want to remove the second clause of the following
  // boolean expression.  MAYBE.
  if ((!fileName.isEmpty()) && (windowList.count() == 1)) {
    if (!(calendar->load(fileName))) {
      // while failing to load, the calendar object could 
      // have become partially populated.  Clear it out.
      calendar->close();
      fileName = "";
    }
  }
  unsetModified();
  config->setGroup("General");
  config->writeEntry("Current Calendar", fileName);
  QApplication::restoreOverrideCursor();

}

void TopWidget::updateRects()
{
  KTopLevelWidget::updateRects();
  panner->resize(mainFrame->size());
  /*  rightFrame->resize(mainFrame->width()-(panner->separatorPos()+5), 
		     mainFrame->height());

  agendaView->resize(rightFrame->size());
  
  listView->resize(rightFrame->size());*/

  monthView->move(2,2);
  monthView->resize(mainFrame->width()-4, mainFrame->height()-4);
  todoView->move(2,2);
  todoView->resize(mainFrame->width()-4, mainFrame->height()-4);
}

void TopWidget::readSettings()
{
  QString str;

  // read settings from the KConfig, supplying reasonable
  // defaults where none are to be found

  config = korganizer->getConfig();

  int windowWidth = 600;
  int windowHeight = 400;
  viewMode = AGENDAVIEW;

  // add stuff for config file versioning here!
  recentFileList.clear();
  config->setGroup("Recently Opened Files");
  str = config->readEntry("1", "");
  add_recent_file(str);
  str = config->readEntry("2", "");
  add_recent_file(str);
  str = config->readEntry("3", "");
  add_recent_file(str);
  str = config->readEntry("4", "");
  add_recent_file(str);
  str = config->readEntry("5", "");
  add_recent_file(str);
	
  config->setGroup("General");
  str = config->readEntry("Width");
  if (!str.isEmpty())
    windowWidth = str.toInt();
  str = config->readEntry("Height");
  if (!str.isEmpty())
    windowHeight = str.toInt();
  this->resize(windowWidth, windowHeight);

  str = config->readEntry("Separator");
  if (!str.isEmpty())
    panner->setSeparatorPos(str.toInt());

  statusBarEnable = config->readBoolEntry("Status Bar", TRUE);

  toolBarEnable = config->readBoolEntry("Tool Bar", TRUE);

  str = config->readEntry("Current View");
  if (!str.isEmpty())
    viewMode = str.toInt();
  str = config->readEntry("Current Calendar");
  if (!str.isEmpty() && QFile::exists(str.data()))
    fileName = str;

  config->setGroup("Views");
  agendaViewMode = config->readNumEntry("Agenda View", KAgenda::DAY);

  config->setGroup( "Colors" );
  if( config->readBoolEntry( "DefaultColors", TRUE ) == TRUE )
  {
    optionsDlg->setColorDefaults();
    optionsDlg->applyColorDefaults();
  }

  config->sync();
}

void TopWidget::writeSettings()
{
	
  config = korganizer->getConfig();

  config->setGroup("Recently Opened Files");
	
  QString recent_number;
  for(int i = 0; i < (int) recentFileList.count(); i++){
    recent_number.setNum(i+1);
    config->writeEntry(recent_number.data(),recentFileList.at(i));
  }

  QString tmpStr;
  config->setGroup("General");

  tmpStr.sprintf("%d", this->width() );
  config->writeEntry("Width",	tmpStr);
	
  tmpStr.sprintf("%d", this->height() );
  config->writeEntry("Height",	tmpStr);

  tmpStr.sprintf("%d", panner->separatorPos());
  config->writeEntry("Separator", tmpStr);

  tmpStr.sprintf("%s", optionsMenu->isItemChecked(toolBarMenuId) ? 
		 "true" : "false");
  config->writeEntry("Tool Bar", tmpStr);

  tmpStr.sprintf("%s", optionsMenu->isItemChecked(statusBarMenuId) ?
		 "true" : "false");
  config->writeEntry("Status Bar", tmpStr);

  tmpStr.sprintf("%d", viewMode);
  config->writeEntry("Current View", tmpStr);
  config->writeEntry("Current Calendar", fileName);

  config->setGroup("Views");
  config->writeEntry("Agenda View", agendaView->currentView());
  config->sync();

}

void TopWidget::goToday()
{
  dateNavigator->selectDates(QDate::currentDate());
  saveSingleDate = QDate::currentDate();
  updateView(dateNavigator->getSelected());
}

void TopWidget::goNext()
{
  // adapt this to work for other views
  agendaView->slotNextDates();
  // this *appears* to work fine...
  updateView(dateNavigator->getSelected());
}

void TopWidget::goPrevious()
{
  // adapt this to work for other views
  agendaView->slotPrevDates();
  // this *appears* to work fine...
  updateView(dateNavigator->getSelected());
}

void TopWidget::setupRollover()
{
  // right now, this is a single shot (because I am too lazy to code a
  // real one using a real qtimer object).  It will only work for a single
  // day rollover.  I should fix this. :)
  QDate tmpDate = QDate::currentDate().addDays(1);
  QTime tmpTime = QTime(00, 1);
  QDateTime tomorrow(tmpDate, tmpTime);

  QTimer::singleShot(QDateTime::currentDateTime().secsTo(tomorrow)*1000,
		     dateNavigator, SLOT(updateView()));
}

void TopWidget::initMenus()
{
  QPixmap pixmap;
  //  int itemId;
  KStdAccel stdAccel;

  fileMenu = new QPopupMenu;
  pixmap = Icon("mini/korganizer.xpm");
  fileMenu->insertItem(pixmap,i18n("&New Window"), this,
		       SLOT(newTopWidget()), stdAccel.openNew());
  fileMenu->insertSeparator();
  pixmap = Icon("fileopen.xpm");
  fileMenu->insertItem(pixmap, i18n("&Open"), this,
		       SLOT(file_open()), stdAccel.open());
  recentPop = new QPopupMenu;
  fileMenu->insertItem(i18n("Open &Recent"), recentPop);
  connect( recentPop, SIGNAL(activated(int)), SLOT(file_openRecent(int)));

  // setup the list of recently used files
  recentPop->clear();
  for (int i = 0; i < (int) recentFileList.count(); i++)
    recentPop->insertItem(recentFileList.at(i));
  add_recent_file(fileName);

  fileMenu->insertItem(i18n("&Close"), this,
		       SLOT(file_close()), stdAccel.close());
  fileMenu->insertSeparator();

  pixmap = Icon("filefloppy.xpm");
  fileMenu->insertItem(pixmap, i18n("&Save"), this,
		       SLOT(file_save()), stdAccel.save());
  fileMenu->insertItem(i18n("Save &As"), this,
		       SLOT(file_saveas()));

  fileMenu->insertSeparator();

  fileMenu->insertItem(i18n("&Import From Ical"), this,
  		       SLOT(file_import()));
  fileMenu->insertItem(i18n("&Merge Calendar"), this,
		       SLOT(file_merge()));
  
  fileMenu->setItemEnabled(fileMenu->insertItem(i18n("Archive Old Entries"),
						this,
						SLOT(file_archive())),
			   FALSE);
  

  fileMenu->insertSeparator();
  fileMenu->insertItem(i18n("Print Setup"), this,
		       SLOT(printSetup()));

  pixmap = Icon("fileprint.xpm");
  fileMenu->insertItem(pixmap, i18n("&Print"), this,
		       SLOT(print()), stdAccel.print());
  fileMenu->insertItem(i18n("Print Pre&view"), this,
		       SLOT(printPreview()));
  
  fileMenu->insertSeparator();
  fileMenu->insertItem(i18n("&Quit"), this,
		       SLOT(file_quit()), stdAccel.quit());

  editMenu = new QPopupMenu;
  //put stuff for editing here 
  pixmap = Icon("editcut.xpm");
  editMenu->insertItem(pixmap, i18n("C&ut"), this,
		       SLOT(edit_cut()), stdAccel.cut());
  pixmap = Icon("editcopy.xpm");
  editMenu->insertItem(pixmap, i18n("&Copy"), this,
		       SLOT(edit_copy()), stdAccel.copy());
  pixmap = Icon("editpaste.xpm");
  editMenu->insertItem(pixmap, i18n("&Paste"), this,
		       SLOT(edit_paste()), stdAccel.paste());

  viewMenu = new QPopupMenu;

  pixmap = Icon("listicon.xpm");
  viewMenu->insertItem(pixmap, i18n("&List"), this,
			SLOT( view_list() ) );

  pixmap = Icon("dayicon.xpm");
  viewMenu->insertItem(pixmap, i18n("&Day"), this,
			SLOT( view_day() ) );
  pixmap = Icon("5dayicon.xpm");
  viewMenu->insertItem(pixmap, i18n("W&ork Week"), this,
			SLOT( view_workweek() ) );
  pixmap = Icon("weekicon.xpm");
  viewMenu->insertItem(pixmap, i18n("&Week"), this,
			SLOT( view_week() ) );
  pixmap = Icon("monthicon.xpm");
  viewMenu->insertItem(pixmap, i18n("&Month"), this,
			SLOT( view_month() ) );
  pixmap = Icon("todolist.xpm");
  viewMenu->insertItem(pixmap,i18n("&To-do list"), this,
			SLOT( view_todolist()), FALSE );
  viewMenu->insertSeparator();
  viewMenu->insertItem( i18n("&Update"), this,
			SLOT( update() ) );
	
  actionMenu = new QPopupMenu;
  pixmap = Icon("newevent.xpm");
  actionMenu->insertItem(pixmap, i18n("New &Appointment"), 
			 this, SLOT(apptmnt_new()));
  actionMenu->insertItem(i18n("New E&vent"),
			 this, SLOT(allday_new()));
  actionMenu->insertItem(i18n("&Edit Appointment"), this,
			  SLOT(apptmnt_edit()));
  pixmap = Icon("delete.xpm");
  actionMenu->insertItem(pixmap, i18n("&Delete Appointment"),
			 this, SLOT(apptmnt_delete()));
  actionMenu->insertItem(i18n("Delete To-do"),
			 this, SLOT(action_deleteTodo()));

  actionMenu->insertSeparator();

  pixmap = Icon("search.xpm");
  actionMenu->insertItem(pixmap,i18n("&Search"), this,
			 SLOT(action_search()), stdAccel.find());

  pixmap = Icon("send.xpm");
  actionMenu->insertItem(pixmap, i18n("&Mail Appointment"), this,
				  SLOT(action_mail()));

  actionMenu->insertSeparator();

  pixmap = Icon("todayicon.xpm");
  actionMenu->insertItem(pixmap,i18n("Go to &Today"), this,
			 SLOT(goToday()));

  pixmap = Icon("1leftarrow.xpm");
  actionMenu->insertItem(pixmap, i18n("&Previous Day"), this,
				  SLOT(goPrevious()));

  pixmap = Icon("1rightarrow.xpm");
  actionMenu->insertItem(pixmap, i18n("&Next Day"), this,
				  SLOT(goNext()));

  optionsMenu = new QPopupMenu;
  toolBarMenuId = optionsMenu->insertItem(i18n("Show &Tool Bar"), this,
				   SLOT(toggleToolBar()));
  optionsMenu->setItemChecked(toolBarMenuId, TRUE);

  statusBarMenuId = optionsMenu->insertItem(i18n("Show St&atus Bar"), this,
				   SLOT(toggleStatusBar()));
  optionsMenu->setItemChecked(statusBarMenuId, TRUE);

  optionsMenu->insertSeparator();			  
  optionsMenu->insertItem(i18n("&Edit Options"), 
			  this, SLOT(edit_options()));

  helpMenu = new QPopupMenu;
  helpMenu->insertItem(i18n("&Contents"), this,
		       SLOT(help_contents()),
		       stdAccel.help());
  helpMenu->insertSeparator();
  helpMenu->insertItem(i18n("&About"), this,
		       SLOT(help_about())); 
  helpMenu->insertItem(i18n("Send &Bug Report"), this,
		       SLOT(help_postcard()));
    
  // construct a basic menu
  menubar = new KMenuBar(this, "menubar_0");
  menubar->insertItem(i18n("&File"), fileMenu);
  menubar->insertItem(i18n("&Edit"), editMenu);
  menubar->insertItem(i18n("&View"), viewMenu);
  menubar->insertItem(i18n("&Actions"), actionMenu);
  menubar->insertItem(i18n("&Options"), optionsMenu);
  menubar->insertSeparator();
  menubar->insertItem(i18n("&Help"), helpMenu);

  setMenu(menubar);
}

void TopWidget::initToolBar()
{
  QPixmap pixmap;
  QString dirName;

  tb = new KToolBar(this);

  pixmap = Icon("fileopen.xpm");
  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(file_open()), TRUE, 
		   i18n("Open A Calendar"));
	
  pixmap = Icon("filefloppy.xpm");
  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(file_save()), TRUE, 
		   i18n("Save This Calendar"));

  pixmap = Icon("fileprint.xpm");
  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(print()), TRUE, 
		   i18n("Print"));

  tb->insertSeparator();
//  QFrame *sepFrame = new QFrame(tb);
//  sepFrame->setFrameStyle(QFrame::VLine|QFrame::Raised);
//  tb->insertWidget(0, 10, sepFrame);

  pixmap = Icon("newevent.xpm");
  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(apptmnt_new()), TRUE, 
		   i18n("New Appointment"));
  pixmap = Icon("delete.xpm");

  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(apptmnt_delete()), TRUE,
		   i18n("Delete Appointment"));

  pixmap = Icon("findf.xpm");
  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(action_search()), TRUE,
		   i18n("Search For an Appointment"));

  pixmap = Icon("send.xpm");
  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(action_mail()), TRUE,
		   i18n("Mail Appointment"));

  tb->insertSeparator();
//  sepFrame = new QFrame(tb);
//  sepFrame->setFrameStyle(QFrame::VLine|QFrame::Raised);
//  tb->insertWidget(0, 10, sepFrame);

//  KPTButton *bt = new KPTButton(tb);
//  bt->setText(i18n("Go to Today"));
//  bt->setPixmap(Icon("todayicon.xpm"));
//  connect(bt, SIGNAL(clicked()), SLOT(goToday()));
//  tb->insertWidget(0, bt->sizeHint().width(), bt);
//
// ! replaced the "Go to Today" button with an icon

  pixmap = Icon("todayicon.xpm");
  tb->insertButton(pixmap, 0,
      SIGNAL(clicked()), this,
      SLOT(goToday()), TRUE,
      i18n("Go to Today"));

  pixmap = Icon("1leftarrow.xpm");
  tb->insertButton(pixmap, 0,
      SIGNAL(clicked()),
      this, SLOT(goPrevious()), TRUE,
      i18n("Previous Day"));

  pixmap = Icon("1rightarrow.xpm");
  tb->insertButton(pixmap, 0,
      SIGNAL(clicked()),
      this, SLOT(goNext()), TRUE,
      i18n("Next Day"));

  tb->insertSeparator();
//  sepFrame = new QFrame(tb);
//  sepFrame->setFrameStyle(QFrame::VLine|QFrame::Raised);
//  tb->insertWidget(0, 10, sepFrame);

  QPopupMenu *agendaViewPopup = new QPopupMenu();
  agendaViewPopup->insertItem( Icon("dayicon.xpm"), "Show one day", KAgenda::DAY );
  agendaViewPopup->insertItem( Icon("5dayicon.xpm"), "Show a work week", KAgenda::WORKWEEK );
  agendaViewPopup->insertItem( Icon("weekicon.xpm"), "Show a week", KAgenda::WEEK );
  connect( agendaViewPopup, SIGNAL( activated(int) ), this , SLOT( changeAgendaView(int) ) );

  pixmap = Icon("listicon.xpm");
  tb->insertButton(pixmap, 0, SIGNAL(clicked()), this,
		   SLOT(view_list()), TRUE,
		   i18n("List View"));

  pixmap = Icon("agenda.xpm");
  tb->insertButton(pixmap, AGENDABUTTON, SIGNAL(clicked()),
		   this, SLOT( nextAgendaView()), TRUE,
		   i18n("Schedule View"));
  tb->setDelayedPopup( AGENDABUTTON, agendaViewPopup );
  
  pixmap = Icon("monthicon.xpm");
  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(view_month()), TRUE,
		   i18n("Month View"));
  
  pixmap = Icon("todolist.xpm");
  tb->insertButton(pixmap, 0,
		   SIGNAL(clicked()), this,
		   SLOT(view_todolist()), TRUE,
		   i18n("To-do list view"));

  addToolBar(tb);
}

void TopWidget::hookupSignals()
{
  // SIGNAL/SLOTS FOR DATE SYNCHRO
    
  connect(listView, SIGNAL(datesSelected(const QDateList)),
	  dateNavigator, SLOT(selectDates(const QDateList)));
  connect(agendaView, SIGNAL(datesSelected(const QDateList)),
	  dateNavigator, SLOT(selectDates(const QDateList)));
  connect(monthView, SIGNAL(datesSelected(const QDateList)),
	  dateNavigator, SLOT(selectDates(const QDateList)));    
  connect(dateNavigator, SIGNAL(datesSelected(const QDateList)),
	  this, SLOT(updateView(const QDateList)));
    
  // SIGNALS/SLOTS FOR LIST VIEW
  connect(listView, SIGNAL(editEventSignal(KDPEvent *)),
	  this, SLOT(editEvent(KDPEvent *)));
  connect(listView, SIGNAL(deleteEventSignal(KDPEvent *)), 
	  this, SLOT(deleteEvent(KDPEvent *)));

  // SIGNALS/SLOTS FOR DAY/WEEK VIEW
  connect(agendaView,SIGNAL(newEventSignal(QDateTime)),
		this, SLOT(newEvent(QDateTime)));
  connect(agendaView, SIGNAL(editEventSignal(KDPEvent *)),
	  this, SLOT(editEvent(KDPEvent *)));
  connect(agendaView, SIGNAL(deleteEventSignal(KDPEvent *)), 
	  this, SLOT(deleteEvent(KDPEvent *)));

  // SIGNALS/SLOTS FOR MONTH VIEW
  connect(monthView, SIGNAL(newEventSignal(QDate)),
	  this, SLOT(newEvent(QDate)));
  connect(monthView, SIGNAL(editEventSignal(KDPEvent *)),
	  this, SLOT(editEvent(KDPEvent *)));
  connect(monthView, SIGNAL(deleteEventSignal(KDPEvent *)),
	  this, SLOT(deleteEvent(KDPEvent *)));


  // SIGNALS/SLOTS FOR TODO LIST
  connect(todoList, SIGNAL(editEventSignal(KDPEvent *)),
	  this, SLOT(editEvent(KDPEvent *)));
  connect(todoView, SIGNAL(editEventSignal(KDPEvent *)),
	  this, SLOT(editEvent(KDPEvent *)));

  // CONFIGURATION SIGNALS/SLOTS

  // need to know about changed in configuration.
  connect(this, SIGNAL(configChanged()), calendar, SLOT(updateConfig()));
  connect(this, SIGNAL(configChanged()), agendaView, SLOT(updateConfig()));
  connect(this, SIGNAL(configChanged()), monthView, SLOT(updateConfig()));
  connect(this, SIGNAL(configChanged()), listView, SLOT(updateConfig()));
  connect(this, SIGNAL(configChanged()), calPrinter, SLOT(updateConfig()));
  connect(this, SIGNAL(configChanged()), dateNavigator, SLOT(updateConfig()));
  connect(this, SIGNAL(configChanged()), todoView, SLOT(updateConfig()));
  connect(this, SIGNAL(configChanged()), todoList, SLOT(updateConfig()));

  // MISC. SIGNALS/SLOTS
  connect(calendar, SIGNAL(calUpdated(KDPEvent *)), this, SLOT(setModified()));
}

// most of the changeEventDisplays() right now just call the view's
// total update mode, but they SHOULD be recoded to be more refresh-efficient.
void TopWidget::changeEventDisplay(KDPEvent *which, int action)
{
  dateNavigator->updateView();
  if (searchDlg)
    searchDlg->updateView();

  switch(viewMode) {
  case LISTVIEW:
    listView->changeEventDisplay(which, action);
    break;
  case AGENDAVIEW:
    agendaView->changeEventDisplay(which, action);
    break;
  case MONTHVIEW:
    monthView->changeEventDisplay(which, action);
    break;
  default:
    debug(i18n("we don't handle updates for that view, yet.\n"));
    qApp->beep();
    return;
  }
}

void TopWidget::changeAgendaView( int newView )
{
  QPixmap px;
  
  switch( newView ) {
  case KAgenda::DAY: {
    QDateList tmpList(FALSE);
    tmpList = dateNavigator->getSelected();
    if (saveSingleDate != *tmpList.first()) {
      dateNavigator->selectDates(saveSingleDate);
      updateView(dateNavigator->getSelected());
    }    
    px = Icon("dayicon.xpm" );
    tb->setButtonPixmap( AGENDABUTTON, px);
    break;
  }
  // if its a workweek view, calculate the dates and emit
  case KAgenda::WORKWEEK:
    px = Icon("5dayicon.xpm" );
    tb->setButtonPixmap( AGENDABUTTON, px);
    break;
    // if its a week view, calculate the dates and emit
  case KAgenda::WEEK:
    px = Icon("weekicon.xpm" );
    tb->setButtonPixmap( AGENDABUTTON, Icon("weekicon.xpm" ) );
    break;
    // if its a list view, update the list properties.
  case KAgenda::LIST:
    // we want to make sure that this is up to date.
    px = Icon("agenda.xpm" );
    tb->setButtonPixmap( AGENDABUTTON, Icon("agenda.xpm" ) );
    break;
  }
  agendaView->slotViewChange( newView );
}

void TopWidget::nextAgendaView()
{
  int view;
  
  if( viewMode == AGENDAVIEW ) {
    view = agendaView->currentView() + 1;
    if ((view >= KAgenda::DAY) && ( view < KAgenda::LIST))
      changeAgendaView( view );
    else 
      changeAgendaView( KAgenda::DAY );
  } else {
    changeAgendaView( agendaView->currentView() );
    changeView( AGENDAVIEW );
  }

}

void TopWidget::changeView(int newViewMode)
{
  viewMode = newViewMode;
  QPixmap px;
  
  updateView(dateNavigator->getSelected());

  switch(viewMode) {
  case AGENDAVIEW:
    todoView->hide();  
    monthView->hide();
    listView->hide();
    agendaView->show();
    panner->show();    
    break;
  case LISTVIEW:
    todoView->hide();  
    monthView->hide();
    agendaView->hide();
    listView->show();
    panner->show();
    break;
  case MONTHVIEW:
    todoView->hide();  
    panner->hide();
    monthView->show();
    break; 
  case TODOVIEW:
    panner->hide();
    monthView->hide();
    todoView->show();
    break;
  default:
    debug(i18n("we don't handle that view yet.\n"));
    break;
  }
  updateView();
}

void TopWidget::updateView(const QDateList selectedDates)
{
  QDateList tmpList(FALSE);
  tmpList = selectedDates;
  int numView;
  KConfig *config = kapp->getConfig();
  config->setGroup("Time & Date");
  bool weekStartsMonday = config->readBoolEntry("Week Starts Monday", TRUE);
  QPixmap px;

  // if there are 5 dates and the first is a monday, we have a workweek.
  if ((tmpList.count() == 5) &&
      (tmpList.first()->dayOfWeek() == 1) &&
      (tmpList.first()->daysTo(*tmpList.last()) == 4)) {
    numView = KAgenda::WORKWEEK;
  // if there are 7 dates and the first date is a monday, we have a regular week.
  } else if ((tmpList.count() == 7) &&
	   (tmpList.first()->dayOfWeek() == (weekStartsMonday ? 1 : 7)) &&
	   (tmpList.first()->daysTo(*tmpList.last()) == 6)) {
    numView = KAgenda::WEEK;

  } else if (tmpList.count() == 1) {
    numView = KAgenda::DAY;
    saveSingleDate = *tmpList.first();

  } else {
    // for sanity, set viewtype to LIST for now...
    numView = KAgenda::LIST;

  }

  switch( numView )
    {
    case KAgenda::DAY:
      px = Icon("dayicon.xpm" );
      tb->setButtonPixmap( AGENDABUTTON, px);
      break;
      // if its a workweek view, calculate the dates and emit
    case KAgenda::WORKWEEK:
      px = Icon("5dayicon.xpm" );
      tb->setButtonPixmap( AGENDABUTTON, px);
      break;
      // if its a week view, calculate the dates and emit
    case KAgenda::WEEK:
      px = Icon("weekicon.xpm" );
      tb->setButtonPixmap( AGENDABUTTON, Icon("weekicon.xpm" ) );
      break;
      // if its a list view, update the list properties.
    case KAgenda::LIST:
      // we want to make sure that this is up to date.
      px = Icon("agenda.xpm" );
      tb->setButtonPixmap( AGENDABUTTON, Icon("agenda.xpm" ) );
      break;
    }
  
  switch (viewMode) {
  case AGENDAVIEW:
      agendaView->selectDates(selectedDates);
    break;
  case LISTVIEW:
      listView->selectDates(selectedDates);
    break;
  case MONTHVIEW:
      monthView->selectDates(selectedDates);
    break;
  default:
    break;
  }
  todoList->updateView();
  todoView->updateView();
}

void TopWidget::updateView()
{
  // update the current view with the current dates from the date navigator
  QDateList tmpList(FALSE); // we want a shallow copy
  tmpList = dateNavigator->getSelected();

  // if no dates are supplied, we should refresh the dateNavigator too...
  dateNavigator->updateView();
  updateView(tmpList);
}

QString TopWidget::file_getname(int open_save)
  {
  QString    fileName;
  QString    defaultPath;

  // Be nice and tidy and have all the files in a nice easy to find place.
  defaultPath = KApplication::localkdedir();
  defaultPath += "/share/apps/korganizer/";
  
  switch (open_save) {
  case 0 :
    // KFileDialog works?
    //fileName = QFileDialog::getOpenFileName(defaultPath, "*.vcs", this);
    fileName = KFileDialog::getOpenFileName(defaultPath, "*.vcs", this);
    break;
  case 1 :
    // KFileDialog works?
    //fileName = QFileDialog::getSaveFileName(defaultPath, "*.vcs", this);
    fileName = KFileDialog::getSaveFileName(defaultPath, "*.vcs", this);
    break;
  default :
    debug("Internal error: TopWidget::file_getname(): invalid paramater");
    return "";
  } // switch

  // If file dialog box was cancelled or blank file name
  if (fileName.isEmpty()) {
    if(!fileName.isNull())
      QMessageBox::warning(this, i18n("KOrganizer Error"), 
			   i18n("You did not specify a valid file name."));
    return "";
  }
  
  QFileInfo tf("", fileName);

  if(tf.isDir()) {
    QMessageBox::warning(this, i18n("KOrganizer Error"),
			 i18n("The file you specified is a directory,\n"
			      "and cannot be opened. Please try again,\n"
			      "this time selecting a valid calendar file."));
    return "";
  }

  if (open_save == 1) {
    // Force the extension for consistency.
    if(fileName.size() > 3) {
      QString e = fileName.right(4);
      // Extension ending in '.vcs' or anything else '.???' is cool.
      if(e != ".vcs" && e.right(1) != ".")
      // Otherwise, force the default extension.
      fileName += ".vcs";
    }
  }

  return fileName;
}
  
int TopWidget::msgCalModified()
{
  // returns:
  //  0: "Yes"
  //  1: "No"
  //  2: "Cancel"
  return QMessageBox::warning(this,
			      i18n("KOrganizer Warning"),
			      i18n("This calendar has been modified.\n"
				   "Would you like to save it?"),
			      i18n("&Yes"), i18n("&No"), i18n("&Cancel"));
}

int TopWidget::msgItemDelete()
{
  // returns:
  //  0: "OK"
  //  1: "Cancel"
  return QMessageBox::warning(this,
			      i18n("KOrganizer Confirmation"),
			      i18n("This item will be permanently deleted."),
			      i18n("&OK"), i18n("&Cancel"));
}

void TopWidget::file_open()
{
  int       whattodo = 0; // the same as button numbers from QMessageBox return

  if (isModified() && (fileName.isEmpty() || !calendar->autoSave())) {
    whattodo = msgCalModified();
  } else if (isModified()) {
    whattodo = 0; // save if for sure
  } else {
    whattodo = 1; // go ahead and just open a new one
  }

  switch (whattodo) {
  case 0: // Save
    if (file_save()) // bail on error
      return;
  
  case 1: { // Open
    QString newFileName = file_getname(0);
    
    if (newFileName == "")
      return;
    
    QApplication::setOverrideCursor(waitCursor);
    // child windows no longer valid
    emit closingDown();
    
    calendar->close();
    fileName = "";
    
    if(calendar->load(newFileName)) {
      fileName = newFileName;
      add_recent_file(newFileName);
    }
    
    unsetModified();
    updateView();

    QApplication::restoreOverrideCursor();
    break;
  } // case 1
  } // switch
}
  
void TopWidget::file_openRecent(int i)
{
  int       whattodo = 0; // the same as button numbers from QMessageBox return
  
  if (isModified() && (fileName.isEmpty() || !calendar->autoSave())) {
    whattodo = msgCalModified();
  } else if (isModified()) {
    whattodo = 0; // save if for sure
  } else {
    whattodo = 1; // go ahead and just open a new one
  }

  switch (whattodo) {
  case 0: // save ("Yes")
    if (file_save())
      return;
    
  case 1: { // open ("No")
    QString newFileName = recentFileList.at(i);

    // this should never happen
    ASSERT(newFileName != "");

    QApplication::setOverrideCursor(waitCursor);
    // child windows no longer valid
    emit closingDown();
    calendar->close();
    
    if(calendar->load(newFileName)) {
      fileName = newFileName;
      add_recent_file(newFileName);
    }

    unsetModified();
    updateView();
    
    QApplication::restoreOverrideCursor();
    break;
  } // case 1
  } // switch
}					

void TopWidget::file_import()
{
  // eventually, we will need a dialog box to select import type, etc.
  // for now, hard-coded to ical file, $HOME/.calendar.
  int retVal;
  QString progPath;
  char *tmpFn;

  QString homeDir;
  homeDir.sprintf("%s/.calendar",getenv("HOME"));
		  
  if (!QFile::exists(homeDir.data())) {
    QMessageBox::critical(this, i18n("KOrganizer Error"),
			  i18n("You have no ical file in your home directory.\n"
			       "Import cannot proceed.\n"));
    return;
  }
  tmpFn = tmpnam(0);
  progPath.sprintf("%s/korganizer/ical2vcal %s",
		   KApplication::kde_datadir().data(), tmpFn);
  retVal = system(progPath.data());
  
  if (retVal >= 0 && retVal <= 2) {
    // now we need to MERGE what is in the iCal to the current calendar.
    calendar->load(tmpFn);
    if (!retVal)
      QMessageBox::information(this, i18n("KOrganizer Info"),
			       i18n("KOrganizer succesfully imported and "
				    "merged your\n.calendar file from ical "
				    "into the currently\nopened calendar.\n"),
			       QMessageBox::Ok);
    else
      QMessageBox::warning(this, i18n("ICal Import Successful With Warning"),
			   i18n("KOrganizer encountered some unknown fields while\n"
				"parsing your .calendar ical file, and had to\n"
				"discard them.  Please check to see that all\n"
				"your relevant data was correctly imported.\n"));
  } else if (retVal == -1) {
    QMessageBox::warning(this, i18n("KOrganizer Error"),
			 i18n("KOrganizer encountered some error parsing your\n"
			      ".calendar file from ical.  Import has failed.\n"));
  } else if (retVal == -2) {
    QMessageBox::warning(this, i18n("KOrganizer Error"),
			 i18n("KOrganizer doesn't think that your .calendar\n"
			      "file is a valid ical calendar. Import has failed.\n"));
  }

  updateView();
}

void TopWidget::file_merge()
{
  QString mergeFileName;

  mergeFileName = file_getname(0);

  // If file dialog box was cancelled (trap for null) 
  if(mergeFileName.isEmpty())
    return;

  if(calendar->load(mergeFileName)) {
    setModified();
    updateView();
  }
}

void TopWidget::file_archive()
{
  ArchiveDialog ad;

  if (ad.exec()) {
    // ok was pressed.
  }
}

int TopWidget::file_saveas()
{
  QString newFileName = file_getname(1);

  if (newFileName == "")
    return 1;

  if (calendar->save(newFileName))
    return 1;

  fileName = newFileName;
  add_recent_file(newFileName);
  unsetModified();

  // keep saves on a regular interval
  if (calendar->autoSave()) {
    autoSaveTimer->stop();
    autoSaveTimer->start(1000*60);
  } 

  return 0;
}

void TopWidget::file_close()
{
  // a better place to put this?
  // this seems a little weird though - should we really do this???
  if( windowList.count() > 1 ) {
    file_quit();
    return;
  }

  int       whattodo = 0; // the same as button numbers from QMessageBox return

  if (isModified() && (fileName.isEmpty() || !calendar->autoSave())) {
    whattodo = msgCalModified();
  } else if (isModified()) {
    whattodo = 0; // save if for sure
  } else {
    whattodo = 1; // go ahead and just open a new one
  }

  switch (whattodo) {
  case 0: // save ("Yes")
    if (file_save())
      return;

  case 1: // open ("No", or wasn't modified)
    // child windows no longer valid
    emit closingDown();
    calendar->close();
    fileName = "";
    unsetModified();
    updateView();
		set_title();

    break;

  } // switch
}

int TopWidget::file_save()
{
  // has this calendar been saved before?
  if (fileName.isEmpty())
    return file_saveas();

  if (calendar->save(fileName))
    return 1;

  unsetModified();

  // keep saves on a regular interval
  if (calendar->autoSave()) {
    autoSaveTimer->stop();
    autoSaveTimer->start(1000*60);
  }

  return 0;
}

void TopWidget::file_quit()
{
  int       whattodo = 0; // the same as button numbers from QMessageBox return

  if (isModified() && (fileName.isEmpty() || !calendar->autoSave())) {
    whattodo = msgCalModified();
  } else if (isModified()) {
    whattodo = 0; // save if for sure
  } else {
    whattodo = 1; // go ahead and just open a new one
  }

  switch (whattodo) {
  case 0: // save ("Yes")
    if (file_save())
      return;

  case 1: // open ("No", or wasen't modified)
    // child windows no longer valid
    emit closingDown();
    calendar->close();

    if (windowList.count() == 1) {
      writeSettings();

      delete this;

      kapp->quit();
    } else {
      delete this;
    }

    break;

  } // switch
}

void TopWidget::edit_cut()
{
  KDPEvent *anEvent;
  switch(viewMode) {
  case LISTVIEW:
    anEvent = listView->getSelected();
    break;
  case AGENDAVIEW:
    anEvent = agendaView->getSelected();
    break;
  case MONTHVIEW:
    anEvent= monthView->getSelected();
    break;
  default:
    QMessageBox::warning(this,i18n("KOrganizer error"),
			 i18n("Unfortunately, we don't handle cut/paste for\n"
			      "that view yet.\n"));
    return;
  }
  if (!anEvent) {
    qApp->beep();
    return;
  }
  calendar->cutEvent(anEvent);
  changeEventDisplay(anEvent, EVENTDELETED);
}

void TopWidget::edit_copy()
{
  KDPEvent *anEvent;
  switch(viewMode) {
  case LISTVIEW:
    anEvent = listView->getSelected();
    break;
  case AGENDAVIEW:
    anEvent = agendaView->getSelected();
    break;
  case MONTHVIEW:
    anEvent = monthView->getSelected();
    break;
  default:
    QMessageBox::warning(this,i18n("KOrganizer error"),
			 i18n("Unfortunately, we don't handle cut/paste for\n"
			      "that view yet.\n"));
    return;
  }
  if (!anEvent) {
    qApp->beep();
    return;
  }
  calendar->copyEvent(anEvent);
}

void TopWidget::edit_paste()
{
  KDPEvent *pastedEvent;
  QDateList tmpList(FALSE);

  tmpList = dateNavigator->getSelected();
  pastedEvent = calendar->pasteEvent(tmpList.first());
  changeEventDisplay(pastedEvent, EVENTADDED);
}

void TopWidget::edit_options()
{
//  OptionsDialog *optionsDlg;
  
  optionsDlg->exec();
}

// Configuration changed as a result of the options dialog.
// I wanted to break this up, in order to avoid inefficiency 
// introduced as we were ALWAYS updating configuration
// in multiple widgets regardless of changed in information.
void TopWidget::updateConfig()
{
  emit configChanged();
  readSettings(); // this is the best way to assure that we have them back
  if (calendar->autoSave() && !autoSaveTimer->isActive()) {
    checkAutoSave();
    autoSaveTimer->start(1000*60);
  }
  if (!calendar->autoSave())
    autoSaveTimer->stop();

  // static slot calls here
  KDPEvent::updateConfig();

  set_title();
}

/****************************************************************************/

void TopWidget::newEvent(QDateTime fromHint, QDateTime toHint)
{
  // create empty event win
  EditEventWin *eventWin = new EditEventWin( calendar );

  // put in date hint
  eventWin->newEvent(fromHint, toHint);

  // connect the win for changed events
  connect(eventWin, SIGNAL(eventChanged(KDPEvent *, int)), 
  	  SLOT(changeEventDisplay(KDPEvent *, int)));
  connect(eventWin, SIGNAL(closed(QWidget *)),
	  SLOT(cleanWindow(QWidget *)));
  connect(this, SIGNAL(closingDown()),
	  eventWin, SLOT(cancel()));

  // show win
  eventWin->show();
}

void TopWidget::newEvent(QDateTime fromHint, QDateTime toHint, bool allDay)
{
  // create empty event win
  EditEventWin *eventWin = new EditEventWin( calendar );

  // put in date hint
  eventWin->newEvent(fromHint, toHint, allDay);

  // connect the win for changed events
  connect(eventWin, SIGNAL(eventChanged(KDPEvent *, int)), 
  	  SLOT(changeEventDisplay(KDPEvent *, int)));
  connect(eventWin, SIGNAL(closed(QWidget *)),
	  SLOT(cleanWindow(QWidget *)));
  connect(this, SIGNAL(closingDown()),
	  eventWin, SLOT(cancel()));

  // show win
  eventWin->show();
}

void TopWidget::apptmnt_new()
{
  QDate from, to;

  QDateList tmpList(FALSE);
  tmpList = dateNavigator->getSelected();
  from = *tmpList.first();
  to = *tmpList.last();
  
  ASSERT(from.isValid());
  if (!from.isValid()) { // dateNavigator sometimes returns GARBAGE!
    from = QDate::currentDate();
    to = from;
  }

  config = korganizer->getConfig();
  config->setGroup("Time & Date");
  QString confStr(config->readEntry("Default Start Time", "12:00"));
  int pos = confStr.find(':');
  if (pos >= 0)
    confStr.truncate(pos);
  int fmt = confStr.toUInt();

  newEvent(QDateTime(from, QTime(fmt,0,0)),
	   QDateTime(to, QTime(fmt+1,0,0)));
}

void TopWidget::allday_new()
{

  QDate from, to;
  QDateList tmpList(FALSE);
  tmpList = dateNavigator->getSelected();

  from = *tmpList.first();
  to = *tmpList.last();

  ASSERT(from.isValid());
  if (!from.isValid()) {
    from = QDate::currentDate();
    to = from;
  }
  
  newEvent(QDateTime(from, QTime(12,0,0)),
	   QDateTime(to, QTime(12,0,0)), TRUE);
}

void TopWidget::editEvent(KDPEvent *anEvent)
{
  QDateList tmpList(FALSE);
  QDate qd;

  tmpList = dateNavigator->getSelected();
  if(anEvent!=0)    {
      qd = *tmpList.first();
        EventWin *eventWin;
        if (anEvent->getTodoStatus()) {
            // this is a todo
            eventWin = new TodoEventWin(calendar);
            eventWin->editEvent(anEvent, qd);
            connect(eventWin, SIGNAL(eventChanged(KDPEvent *, int)),
                    todoList, SLOT(updateView()));
            connect(eventWin, SIGNAL(eventChanged(KDPEvent *, int)),
                    todoView, SLOT(updateView()));
        } else { // this is an event
            eventWin = new EditEventWin(calendar);
            eventWin->editEvent(anEvent, qd);
        }
        // connect for changed events, common for todos and events
        connect(eventWin, SIGNAL(eventChanged(KDPEvent *, int)),
                SLOT(changeEventDisplay(KDPEvent *, int)));
        connect(eventWin, SIGNAL(closed(QWidget *)),
                SLOT(cleanWindow(QWidget *)));
        connect(this, SIGNAL(closingDown()),
                eventWin, SLOT(cancel()));

        eventWin->show();
    } else {
        qApp->beep();
    }
}



void TopWidget::apptmnt_edit()
{
  KDPEvent *anEvent;
  
  switch(viewMode) {
  case LISTVIEW:
    anEvent = listView->getSelected();
    break;
  case AGENDAVIEW:
    anEvent = agendaView->getSelected();
    break;
  case MONTHVIEW:
    anEvent = monthView->getSelected();
    break;
  default:
    debug("we don't handle that view yet.");
    return;
  }
  if (!anEvent) {
    qApp->beep();
    return;
  }

  editEvent(anEvent);
}

void TopWidget::apptmnt_delete()
{
  KDPEvent *anEvent;

  switch(viewMode) {
  case LISTVIEW:
    anEvent = listView->getSelected();
    break;
  case AGENDAVIEW:
    anEvent = agendaView->getSelected();
    break;
  case MONTHVIEW:
    anEvent = monthView->getSelected();
    break;
  default:
    debug(i18n("don't handle deletes from that view, yet.\n"));
    return;
  }
  if (!anEvent) {
    qApp->beep();
    return;
  }

  deleteEvent(anEvent);
}

void TopWidget::action_deleteTodo()
{
  KDPEvent *aTodo;
  TodoView *todoList2 = (viewMode == TODOVIEW ? todoView : todoList); 

  aTodo = todoList2->getSelected();
  if (!aTodo) {
    qApp->beep();
    return;
  }
  
  KConfig *config(kapp->getConfig());
  config->setGroup("General");
  if (config->readBoolEntry("Confirm Deletes") == TRUE) {
    switch(msgItemDelete()) {
    case 1: // OK
      calendar->deleteTodo(aTodo);
      todoList2->changeEventDisplay(aTodo, EVENTDELETED);
      break;

    } // switch
  } else {
    calendar->deleteTodo(aTodo);
    todoList2->changeEventDisplay(aTodo, EVENTDELETED);
  }
}

void TopWidget::deleteEvent(KDPEvent *anEvent)
{
  if (!anEvent) {
    qApp->beep();
    return;
  }

  if (anEvent->doesRecur()) {

  switch ( QMessageBox::warning(this,
				i18n("KOrganizer Confirmation"),
				i18n("This event recurs over multiple dates.\n"
				     "Are you sure you want to delete the\n"
				     "selected event, or just this instance?\n"),
				i18n("&All"), i18n("&This"),
				i18n("&Cancel"))) {

    case 0: // all
      calendar->deleteEvent(anEvent);
      changeEventDisplay(anEvent, EVENTDELETED);
      break;

    case 1: // just this one
      {
        QDate qd;
        QDateList tmpList(FALSE);
        tmpList = dateNavigator->getSelected();
        qd = *(tmpList.first());
        if (!qd.isValid()) {
          debug(i18n("no date selected, or invalid date\n"));
          qApp->beep();
          return;
        }
	while (!anEvent->recursOn(qd))
	  qd.addDays(1);
        anEvent->addExDate(qd);
        changeEventDisplay(anEvent, EVENTDELETED);
        break;
      }

    } // switch
  } else {
    KConfig *config(kapp->getConfig());
    config->setGroup("General");
    if (config->readBoolEntry("Confirm Deletes") == TRUE) {
      switch (msgItemDelete()) {
      case 0: // OK
        calendar->deleteEvent(anEvent);
        changeEventDisplay(anEvent, EVENTDELETED);
        break;

      } // switch
    } else {
      calendar->deleteEvent(anEvent);
      changeEventDisplay(anEvent, EVENTDELETED);
    }
  } // if-else
}

/*****************************************************************************/

void TopWidget::action_search()
{
  if (!searchDlg) {
    searchDlg = new SearchDialog(calendar);
    connect(searchDlg, SIGNAL(closed(QWidget *)),
	    this, SLOT(cleanWindow(QWidget *)));
    connect(searchDlg, SIGNAL(editEventSignal(KDPEvent *)),
	    this, SLOT(editEvent(KDPEvent *)));
    connect(searchDlg, SIGNAL(deleteEventSignal(KDPEvent *)), 
	    this, SLOT(deleteEvent(KDPEvent *)));
    connect(this, SIGNAL(closingDown()),
	      searchDlg, SLOT(cancel()));
  }
  // make sure the widget is on top again
  searchDlg->hide();
  searchDlg->show();
  
}

void TopWidget::action_mail()
{
  KDPEvent *anEvent;
  KoMailClient mailobject(calendar);

  switch(viewMode) {
  case LISTVIEW:
    anEvent = listView->getSelected();
    break;
  case AGENDAVIEW:
    anEvent = agendaView->getSelected();
    break;
  case MONTHVIEW:
    anEvent= monthView->getSelected();
    break;
  default:
    QMessageBox::warning(this,i18n("KOrganizer error"),
			 i18n("Can't generate mail from this view\n"));
    return;
  }
  if (!anEvent) {
    qApp->beep();
    return;
  }
  if(anEvent->attendeeCount() == 0 ) {
    qApp->beep();
    QMessageBox::warning(this,i18n("KOrganizer error"),
			 i18n("Can't generate mail:\n No attendees defined!\n"));
    return;
  }
  mailobject.emailEvent(anEvent,this);
}

void TopWidget::add_recent_file(QString recentFileName)
{
  const char *rf;
  QFileInfo tf("",recentFileName);

  // if it is not a file or is not readable bail.
  if (!tf.isFile() || !tf.isReadable())
    return;

  // sanity check
  if (recentFileName.isEmpty())
    return;

  // check to see if it is already in the list.
  for (rf = recentFileList.first(); rf;
       rf = recentFileList.next()) {
    if (strcmp(rf, recentFileName.data()) == 0)
      return;  
  }

  if( recentFileList.count() < 5)
    recentFileList.insert(0,recentFileName.data());
  else {
    recentFileList.remove(4);
    recentFileList.insert(0,recentFileName.data());
  }
  if (recentPop) {
    recentPop->clear();
    for ( int i =0 ; i < (int)recentFileList.count(); i++)
      {
	recentPop->insertItem(recentFileList.at(i));
      }
    if (recentFileList.count() == 0) {
      // disable the "Open Recent" option on the file menu if necessary
      fileMenu->setItemEnabled(1, FALSE);
    }
  }
}

void TopWidget::help_contents()
{
  korganizer->invokeHTMLHelp("korganizer/korganizer.html","");
}

void TopWidget::help_about()
{
  AboutDialog *ad = new AboutDialog(this);
  ad->show();
  delete ad;
}

void TopWidget::help_postcard()
{
  PostcardDialog *pcd = new PostcardDialog(this);
  pcd->show();
  delete pcd;
}

void TopWidget::newTopWidget()
{
  (new TopWidget(NULL, QString("")))->show();
}

void TopWidget::view_list()
{
  changeView(LISTVIEW);
}

void TopWidget::view_day()
{
  QDateList tmpList(FALSE);
  tmpList = dateNavigator->getSelected();
  if (saveSingleDate != *tmpList.first()) {
    dateNavigator->selectDates(saveSingleDate);
    updateView(dateNavigator->getSelected());
  }

  agendaView->slotViewChange( KAgenda::DAY );
  changeView(AGENDAVIEW);
}

void TopWidget::view_workweek()
{
  agendaView->slotViewChange(KAgenda::WORKWEEK);
  changeView(AGENDAVIEW);
}

void TopWidget::view_week()
{
  agendaView->slotViewChange(KAgenda::WEEK );
  changeView(AGENDAVIEW);
}

void TopWidget::view_month()
{
  changeView(MONTHVIEW);
}

void TopWidget::view_todolist()
{
  changeView(TODOVIEW);
}

void TopWidget::setModified()
{
  if (!modifiedFlag) {
    modifiedFlag = true;
    set_title();
  }
}

void TopWidget::unsetModified()
{
  if (modifiedFlag) {
    modifiedFlag = false;
    set_title();
  }

  signalAlarmDaemon();
}

bool TopWidget::isModified()
{
  return modifiedFlag;
}

void TopWidget::set_title()
{
  QString tmpStr;

  if (!fileName.isEmpty())
    tmpStr = fileName.mid(fileName.findRev('/')+1, fileName.length());
  else
    tmpStr = i18n("New Calendar");

  // display the modified thing in the title
  // if auto-save is on, we only display it on new calender (no file name)
  if (modifiedFlag && (fileName.isEmpty() || ! calendar->autoSave())) {
    tmpStr += " (";
    tmpStr += i18n("modified");
    tmpStr += ")";
  }

  tmpStr += " - ";
  tmpStr += i18n("KOrganizer");

  setCaption(tmpStr);
}

void TopWidget::closeEvent( QCloseEvent * )
{
  file_quit();
}

void TopWidget::signalAlarmDaemon()
{
  QFile pidFile;
  QString tmpStr;
  pid_t pid;
  char pidStr[25];

  tmpStr.sprintf("%s/share/apps/korganizer/alarmd.pid",
		 KApplication::localkdedir().data());
  pidFile.setName(tmpStr.data());

  // only necessary if the file actually is opened
  if(pidFile.open(IO_ReadOnly)) {
    pidFile.readLine(pidStr, 24);
    pidFile.close();
    pid = atoi(pidStr);
    if (pid > 0)
      kill(pid, SIGHUP);
  }
}

void TopWidget::checkAutoSave()
{
  // has this calendar been saved before? 
  if (calendar->autoSave() && !fileName.isEmpty()) {
    add_recent_file(fileName);
    calendar->save(fileName);
    unsetModified();
  }
}

void TopWidget::printSetup()
{
  calPrinter->setupPrinter();
}

void TopWidget::print() 
{
  QDateList tmpDateList(FALSE);

  tmpDateList = dateNavigator->getSelected();
  calPrinter->print(CalPrinter::Month, 
		    *tmpDateList.first(), *tmpDateList.last());
}

void TopWidget::printPreview()
{
  QDateList tmpDateList(FALSE);

  tmpDateList = dateNavigator->getSelected();

  switch(viewMode) {
  case AGENDAVIEW:
    if (tmpDateList.count() == 1)
      calPrinter->preview(CalPrinter::Day,
			  *tmpDateList.first(), *tmpDateList.last());
    else if (tmpDateList.count() <= 7)
      calPrinter->preview(CalPrinter::Week,
			  *tmpDateList.first(), *tmpDateList.last());
    else
      calPrinter->preview(CalPrinter::Month,
			  *tmpDateList.first(), *tmpDateList.last());
    break;
  case LISTVIEW:
    calPrinter->preview(CalPrinter::Day,
			*tmpDateList.first(), *tmpDateList.last());
    break;
  case MONTHVIEW:
  default:
    calPrinter->preview(CalPrinter::Month, 
			*tmpDateList.first(), *tmpDateList.last());
    break;
  }
}

void TopWidget::cleanWindow(QWidget *widget)
{
  widget->hide();
  delete widget;

  // some widgets are stored in TopWidget
  if (widget == searchDlg)
    searchDlg = NULL;
}
