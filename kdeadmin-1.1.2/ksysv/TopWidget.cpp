/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/****************************************************************
**
** KSysV
** Toplevel Widget
**
****************************************************************/

#include <qpopmenu.h>
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qprinter.h>
#include "TopWidget.h"
#include <kapp.h>
#include <kmsgbox.h>
#include <kstdaccel.h>
#include "ksv_core.h"
#include "About.h"
#include "Constants.h"

// include meta-object file
#include "TopWidget.moc"

// define some macros
#define TOLERANCE 50

class Tools {
public:
  enum { Cut = 0, Copy = 1, Paste = 2, Undo = 3,
	 New = 4, Save = 5, Exit = 6,
	 Help = 7, About = 8, Toggle = 9};
};

class Status {
public:
  enum { Changed = 3, Progress = 1, Rest = 2 };
};

class Menu {
public:
  enum { New = 0, Save = 1, Print = 3, PrintLog = 4, Exit = 6,
	 Cut = 0, Copy = 1, Paste = 2, Undo = 4,
	 Help = 0, /* Context = 1, */ About = 2, AboutKDE = 3,
	 Tools = 0, Status = 1, Log = 2, Configure = 4, SaveOptions = 6 };
};

KSVTopLevel::KSVTopLevel(const char* name)
  : KTMainWindow(name),
    _kil(kapp->getIconLoader()),
    conf(KSVCore::getConfig()),
    _menu(menuBar()),
    _tools(toolBar()),
    _status(statusBar()),
    prog(new KProgress(_status, "Progress")),
    _view(new KSVGui(this, "View")),
    _prefs(0),
    clip_filled(false)
{
  initMenu();
  initToolBar();
  initStatusBar();

  setView(_view, TRUE);

  // get the various context menus
  _normalPM = _view->normalPM;
  _backPM = _view->backPM;
  _scriptPM = _view->scriptPM;

  // ensure that the statusbar gets updated correctly
  connect(_view, SIGNAL(sigRun(QString)), this, SLOT(slotUpdateRunning(QString)));
  connect(_view, SIGNAL(sigStop()), this, SLOT(slotClearRunning()));
  connect( _view, SIGNAL(cannotGenerateNumber()), this, SLOT(catchCannotGenerateNumber()) );
  connect(_view, SIGNAL(sigChanged()), this, SLOT(slotChanged()));

  // cut & copy
  connect( _view, SIGNAL(selected(const KSVDragData*)),
	   this, SLOT(dispatchCutCopy(const KSVDragData*)) );
  connect( _view, SIGNAL(selectedScripts(const KSVDragData*)),
	   this, SLOT(dispatchCopyScripts(const KSVDragData*)) );
  setCut(false); // disable cut
  setCopy(false); // & copy on startup

  // paste
  connect( CLIP, SIGNAL(filled(const KSVDragData*)),
	   this, SLOT(dispatchPaste(const KSVDragData*)) );
  setPaste(false); // disable paste on startup

  // init _view according to saved preferences
  slotReadConfig();
  _view->reInit();
  setChanged(FALSE);

  setMinimumSize(600,400);

  // restore size and position
  move(conf->getPosition()); // doesn´t seem to work while unmapped
  resize(conf->getSize());
}

KSVTopLevel::~KSVTopLevel() {
  delete _FileMenu;
  delete _OptionsMenu;
  delete _HelpMenu;
}

void KSVTopLevel::initMenu() {
  KStdAccel* keys = new KStdAccel(kapp->getConfig());

  // generate menu
  _FileMenu = new QPopupMenu();
  _FileMenu->insertItem(i18n("&New Configuration"), this, SLOT(slotClearChanges()), keys->openNew());
  _FileMenu->insertItem(i18n("&Save Configuration"), this, SLOT(slotAcceptChanges()), keys->save());
  _FileMenu->setId(Menu::New,0);
  _FileMenu->setId(Menu::Save,1);

  _FileMenu->insertSeparator();
  _FileMenu->insertItem(i18n("&Print..."), this, SLOT(print()), keys->print());
  _FileMenu->insertItem(i18n("Print &Log..."), this, SLOT(printLog()), CTRL+Key_L);
  _FileMenu->setId(Menu::Print, 3);
  _FileMenu->setId(Menu::PrintLog, 4);
  _FileMenu->setItemEnabled(Menu::PrintLog, false);
  _FileMenu->setItemEnabled(Menu::Print, false);

  _FileMenu->insertSeparator();
  _FileMenu->insertItem(i18n("E&xit"), this, SLOT(slotCloseMain()), keys->quit());

  _EditMenu = new QPopupMenu();
  _EditMenu->insertItem(_kil->loadIcon("editcut.xpm"),
			i18n("C&ut"), this, SLOT(editCut()), keys->cut());
  _EditMenu->insertItem(_kil->loadIcon("editcopy.xpm"),
			i18n("&Copy"), this, SLOT(editCopy()), keys->copy());
  _EditMenu->insertItem(_kil->loadIcon("editpaste.xpm"),
			i18n("&Paste"), this, SLOT(editPaste()), keys->paste());
//   _EditMenu->insertSeparator();
//   _EditMenu->insertItem(i18n("&Undo"), this, SLOT(editUndo()), keys->undo());

  _EditMenu->setId(Menu::Cut,0);
  _EditMenu->setId(Menu::Copy,1);
  _EditMenu->setId(Menu::Paste,2);
//   _EditMenu->setId(Menu::Undo, 4);
//   setUndo(FALSE);

  _OptionsMenu = new QPopupMenu();
  _OptionsMenu->setCheckable(TRUE);
  _OptionsMenu->insertItem(i18n("Show &Tool Bar"), this, SLOT(toggleTools()));
  _OptionsMenu->insertItem(i18n("Show &Status Bar"), this, SLOT(toggleStatus()));
  _OptionsMenu->insertItem(i18n("Show &Log"), this, SLOT(toggleLog()));
  _OptionsMenu->insertSeparator();
  _OptionsMenu->insertItem(i18n("&Configure..."), this, SLOT(slotShowConfig()));

  _OptionsMenu->insertSeparator();
  _OptionsMenu->insertItem(i18n("&Save Options"), this, SLOT(saveOptions()));
  _OptionsMenu->setId( Menu::Configure, Menu::Configure );
  _OptionsMenu->setId( Menu::Log, Menu::Log );
  _OptionsMenu->setId( Menu::Tools, Menu::Tools );
  _OptionsMenu->setId( Menu::Status, Menu::Status );
  _OptionsMenu->setId( Menu::SaveOptions, Menu::SaveOptions );
  _OptionsMenu->setItemChecked( Menu::Log, conf->getShowLog() );

  _HelpMenu = kapp->getHelpMenu( FALSE, 0 );
  _HelpMenu->setId( Menu::Help, 0);
  _HelpMenu->setId( Menu::About, 2 );
  _HelpMenu->setId( Menu::AboutKDE, 3 );
  _HelpMenu->connectItem( Menu::About, this, SLOT(slotShowAbout()) );

   // remove "Content"
   _HelpMenu->removeItem( Menu::Help );
   _HelpMenu->insertItem(_kil->loadIcon("ksysv_help.xpm"), i18n("&Contents"), Menu::Help, 0);
   _HelpMenu->connectItem( Menu::Help, this, SLOT(invokeHelpContents()));
   _HelpMenu->setAccel(keys->help(), Menu::Help);

  _menu->insertItem(i18n("&File"), _FileMenu);
  _menu->insertItem(i18n("&Edit"), _EditMenu);
  _menu->insertItem(i18n("&Options"), _OptionsMenu);
  _menu->insertSeparator();
  _menu->insertItem(i18n("&Help"), _HelpMenu);

  // if I use a var on the stack or delete the object here,
  // I get segfaults in KApplication's destructor (actually
  // in KKeyConfig's destructor)
  //    delete keys;
  //    keys = 0;
}

void KSVTopLevel::initToolBar() {

  QPixmap _icon = _kil->loadIcon("filenew2.xpm");
  _tools->insertButton(_icon, Tools::New, SIGNAL(clicked(int)),
		       this, SLOT(slotClearChanges()), FALSE,
		       i18n("New"));

  _icon = _kil->loadIcon("filefloppy.xpm");
  _tools->insertButton(_icon, Tools::Save, SIGNAL(clicked(int)),
		       this, SLOT(slotAcceptChanges()), FALSE,
		       i18n("Save"));

  _tools->insertSeparator();

  _icon = _kil->loadIcon("editcut.xpm");
  _tools->insertButton(_icon, Tools::Cut, SIGNAL(clicked(int)),
		       this, SLOT(editCut()), TRUE,
		       i18n("Cut"));

  _icon = _kil->loadIcon("editcopy.xpm");
  _tools->insertButton(_icon, Tools::Copy, SIGNAL(clicked(int)),
		       this, SLOT(editCopy()), TRUE,
		       i18n("Copy"));

  _icon = _kil->loadIcon("editpaste.xpm");
  _tools->insertButton(_icon, Tools::Paste, SIGNAL(clicked(int)),
		       this, SLOT(editPaste()), TRUE,
		       i18n("Paste"));

  _tools->insertSeparator();

  _icon = _kil->loadIcon("toggle.xpm");
  _tools->insertButton(_icon, Tools::Toggle, SIGNAL(clicked(int)),
		       this, SLOT(toggleLog()), TRUE,
		       i18n("Toggle script output"));
  _tools->setToggle(Tools::Toggle, TRUE);

  _tools->insertSeparator();

  _icon = _kil->loadIcon("help.xpm");
  _tools->insertButton(_icon, Tools::Help, SIGNAL(clicked(int)),
		       this, SLOT(invokeHelpContents()), TRUE,
		       i18n("Help"));
}

bool KSVTopLevel::queryExit() {
  int res = 1;

  if (_changed) {
    res = KMsgBox::yesNo(kapp->mainWidget(),
			 i18n("Exit"),
			 i18n("There are unsaved changes.\nAre you sure you want to quit?"),
			 KMsgBox::QUESTION,
			 i18n("Yes, quit!"),
			 i18n("No, don't quit."));

  }

  qApp->processEvents();
  return (res!=1 ? FALSE : TRUE);
}

void KSVTopLevel::slotCloseMain() {
  close();
}

void KSVTopLevel::invokeHelpContents() {
  KSVCore::getCore()->invokeHelp( FALSE );
}

void KSVTopLevel::invokeHelpOnContext() {
  KSVCore::getCore()->invokeHelp( TRUE );
}

void KSVTopLevel::slotShowAbout() {
  AboutDlg about(this);
  about.exec();
}

void KSVTopLevel::slotClearChanges() {
  if (_changed) {
    int _res = KMsgBox::yesNo(kapp->mainWidget(),
			      i18n("Clear Changes"),
			      i18n("Do you really want to clear all unsaved changes?"),
			      KMsgBox::QUESTION);
    if (_res == 1)
      {
	setCut(false); // disable cut
	setCopy(false); // & copy...
	_view->reInit();
	setChanged(false);
      }
  }
}

void KSVTopLevel::slotAcceptChanges() {
  int _res = KMsgBox::yesNo(kapp->mainWidget(),
			    i18n("Save configuration"),
			    i18n("You're about to save the changes made to your init "
				 "configuration.\nPlease realize that wrong settings could "
				 "result in your system being unbootable.\n\n"
				 "Do you wish to continue?"),
			    KMsgBox::EXCLAMATION | KMsgBox::DB_SECOND,
			    i18n("Yes, &save!"),
			    i18n("No, &don't save."));

  if (_res == 1)
    {
      _view->slotWriteSysV();
      _view->reInit();
      setCut(false);  // disable bla bla
      setCopy(false); // since no subwidget has focus
      setChanged(FALSE);
    }
}

void KSVTopLevel::initStatusBar() {
  prog->setFixedWidth(130);

  // make progress bar update itself
  connect( _view, SIGNAL(initProgress(int, QString)), this, SLOT(initProgress(int, QString)) );
  connect( _view, SIGNAL(advance(int)), prog, SLOT(advance(int)) );
  connect( _view, SIGNAL(endProgress()), this, SLOT(endProgress()) );
  //  connect( this, SIGNAL(resized()), this, SLOT(resizeStatusFields()) );

  _status->setInsertOrder( KStatusBar::RightToLeft );
  _status->insertWidget(prog, 130, 0);
  _status->insertItem("SOME_LABEL_WITH_SOME_SLACK_1", Status::Progress);
  _status->insertItem("CHANGED_=0", Status::Changed);

  //  prog->hide();
  _status->setAlignment( Status::Changed, AlignLeft );
  _status->setAlignment( Status::Progress, AlignRight );
  _status->setAlignment( 0, AlignRight );
// //   _status->insertWidget(prog, 130, 0);
// //   _status->insertItem(" ", 5);
// //   _status->insertItem("SOME_LABEL_WITH_SOME_SLACK_1", Status::Progress);
// //   _status->insertItem("CHANGED_0", Status::Changed);

// //   _status->setAlignment( Status::Changed, KStatusBar::Right );
//   _status->insertItem("CHANGED_0", Status::Changed);
// //   _status->insertItem("SOME_LABEL_WITH_SOME_SLACK_1", Status::Progress);
//   _status->insertWidget(prog, 130, 2);
// //   _status->setAlignment( 2, KStatusBar::Right );
// //   _status->setAlignment( Status::Progress, KStatusBar::Right );

// //   _status->insertItem("", Status::Rest);

  _status->changeItem("", Status::Changed);
  _status->changeItem("", Status::Progress);

  // "disable" progress bar
  endProgress();
}

void KSVTopLevel::slotShowConfig() {
  if (!_prefs)
    (_prefs = new KSVPrefDlg(this, "Preferences"))->hide();

  _prefs->reReadConfig();

  if (_prefs->exec() == QDialog::Accepted) {

    conf->setScriptPath( _prefs->getScriptPath() );
    conf->setRLPath( _prefs->getRLPath() );

    conf->setNewColor( _prefs->getNewColor() );
    conf->setChangedColor( _prefs->getChangedColor() );

    _view->reInit();

    setChanged(FALSE);
  } else
    _prefs->reReadConfig();
}

void KSVTopLevel::slotChanged() {
  setChanged(TRUE);
}

void KSVTopLevel::slotReadConfig() {
  setLog(conf->getShowLog());
  setStatus(conf->getShowStatus());
  setTools(conf->getShowTools());
}

void KSVTopLevel::slotAboutQt() {
  QMessageBox::aboutQt(kapp->mainWidget(), i18n("About Qt"));
}

void KSVTopLevel::toggleLog() {
  const bool value = !conf->getShowLog();
  setLog(value);
}

void KSVTopLevel::toggleStatus() {
  setStatus(!conf->getShowStatus());
}

void KSVTopLevel::toggleTools() {
  setTools(!conf->getShowTools());
}

void KSVTopLevel::saveOptions() {
  conf->writeConfig();
}

void KSVTopLevel::slotUpdateRunning(QString _text) {
  _status->changeItem(_text.data(), 1);
}

void KSVTopLevel::slotClearRunning() {
  _status->clear();
}

void KSVTopLevel::editCut() {
  KSVDragList* tmp = _view->getOrigin();

  if (tmp) {
    CLIP->setClipboard(tmp->selected());
    tmp->removeItem(CLIP->getClipboard());
  }
}

void KSVTopLevel::editCopy() {
  KSVDragList* tmp = _view->getOrigin();

  if (tmp)
    CLIP->setClipboard(tmp->selected());
}

void KSVTopLevel::editPaste() {
  KSVDragList* tmp = _view->getOrigin();

  if (tmp)
    {	
      KSVDragData* o = CLIP->getClipboard();
      const int pos = tmp->focusItem();

      if (o && (tmp->find(o) > NONE ? tmp->find(o)!=pos : TRUE))
	tmp->insertItem(o, pos + 1, TRUE);
    }
  else
    fatal("Bug: could not get origin of \"Paste\" event.\n" \
	  "Please notify the maintainer of this program,\n" \
	  "Peter Putzer <putzer@kde.org>.");
}

void KSVTopLevel::setChanged( bool val ) {
  _changed = val;
  // for session management
  setUnsavedData(val);

  // set discard changes enabled
  _tools->setItemEnabled( Tools::New, val );
  _FileMenu->setItemEnabled( Menu::New, val );

  // set save changes enabled
  _tools->setItemEnabled( Tools::Save, val );
  _FileMenu->setItemEnabled( Menu::Save, val );

  // update statusbar
  QString barTxt = val ? i18n("Changed") : "";
  _status->changeItem(barTxt, Status::Changed);

  // clear messages

  _status->clear();
  //  prog->hide();
}

void KSVTopLevel::properties() {
  KSVDragList* tmp = _view->getOrigin();

  if (tmp)
    _view->infoOnData(tmp->selected());
}

void KSVTopLevel::scriptProperties()
{
  KSVDragList* tmp = _view->getOrigin();

  if (tmp)
    _view->slotScriptProperties(tmp->selected(), QPoint());
}

void KSVTopLevel::editUndo() {
  debug("UNDO: not implemented yet");
}

void KSVTopLevel::setCut( bool val ) {
  _EditMenu->setItemEnabled(Menu::Cut, val);
  _tools->setItemEnabled(Tools::Cut, val);
  _normalPM->setItemEnabled(PopMenu::Cut, val);
}

void KSVTopLevel::setCopy( bool val ) {
  _EditMenu->setItemEnabled(Menu::Copy, val);
  _tools->setItemEnabled(Tools::Copy, val);
  _scriptPM->setItemEnabled(Menu::Copy, val);
  _normalPM->setItemEnabled(PopMenu::Copy, val);
}

void KSVTopLevel::setPaste( bool val ) {
  _EditMenu->setItemEnabled(Menu::Paste, val);
  _tools->setItemEnabled(Tools::Paste, val);
  _normalPM->setItemEnabled(PopMenu::Paste, val);
  _backPM->setItemEnabled(PopMenu::Paste, val);
}

void KSVTopLevel::setUndo( bool val ) {
  _EditMenu->setItemEnabled(Menu::Undo, val);
  _tools->setItemEnabled(Tools::Undo, val);
}

void KSVTopLevel::setLog( bool val ) {
  conf->setShowLog(val);

  _OptionsMenu->setItemChecked( Menu::Log, val );
  _tools->setButton( Tools::Toggle, val );

  _view->setDisplayScriptOutput(val);
}

void KSVTopLevel::setStatus( bool val ) {
  conf->setShowStatus(val);

  _OptionsMenu->setItemChecked( Menu::Status, val );

  enableStatusBar(val ? KStatusBar::Show : KStatusBar::Hide);
}

void KSVTopLevel::setTools( bool val ) {
  conf->setShowTools(val);

  _OptionsMenu->setItemChecked( Menu::Tools, val );
  enableToolBar( val ? KToolBar::Show : KToolBar::Hide );
}

void KSVTopLevel::initProgress( int max, QString label ) {
  // clear any messages
  _status->clear();

  _status->changeItem( label, Status::Progress );
  prog->setRange(0, max);

  // "enable" progress bar
  prog->setBackgroundColor(prog->palette().normal().base());
}

void KSVTopLevel::endProgress() {
  _status->changeItem( "", Status::Progress );
  prog->setValue(0);

  // clear any messages
  _status->clear();

  // "disable" progress bar again
  prog->setBackgroundColor(prog->palette().disabled().background());
}

void KSVTopLevel::print() {
 debug("Not implemented.");
}

void KSVTopLevel::printLog() {
 debug("Not implemented.");
//  QPrinter prt;
//  if ( prt.setup(this) ) {
//    QPainter p;
//    p.begin( &prt );
//    p.setFont( QFont("times", 20, QFont::Bold) );
//    p.drawText(0,0, "TEST");
//    p.drawText(50,50, _view->getLog());
//    p.end();
//  }
}

void KSVTopLevel::catchCannotGenerateNumber() {
  _status->message(i18n("Could not generate sorting number. Please change manually."), 5);
}

void KSVTopLevel::closeEvent( QCloseEvent* e )
{
  if (queryExit()) // ah I found the bug...
    // e was always accepted in default implementation because
    // I did not override queryClose and
    // this is qApp´s mainWidget
    {
      e->accept();
      hide(); // delete takes to long
      delete this;
      kapp->quit();
    }
}

void KSVTopLevel::redrawEntry( const KSVDragData* d )
{
  KSVDragList* tmp = _view->getOrigin();

  if (tmp)
    {
      const int pos = tmp->find(d);
      tmp->redraw( pos, pos+1 );
    }
}

void KSVTopLevel::dispatchCutCopy( const KSVDragData* d )
{
  if (!d)
    {
      setCopy(false);
      setCut(false);
    }
  else
    {
      setCopy(true);
      setCut(true);
    }

  if (clip_filled)
    setPaste(true);
}

void KSVTopLevel::dispatchCopyScripts( const KSVDragData* d )
{
  if (!d)
    setCopy(false);
  else
    setCopy(true);

  setCut(false);
  setPaste(false);
}

void KSVTopLevel::dispatchPaste( const KSVDragData* d )
{
  if (!d)
    {
      setPaste(false);
      clip_filled = false;
    }
  else
    {
      setPaste(true);
      clip_filled = true;
    }
}

// void KSVTopLevel::setEnabled( bool val )
// {
//   if (val)
//     installEventFilter(this);
//   else
//     removeEventFilter(this);
// }

// bool KSVTopLevel::eventFilter( QObject* o, QEvent *e )
// {
//   bool res = false;

//   switch (e->type())
//     {

//     case Event_KeyPress:
//     case Event_MouseButtonPress:
//     case Event_MouseButtonRelease:
//     case Event_MouseButtonDblClick:
//       debug("ate event");
//       res = true;
//       break;

//     default:
//       res = false;

//     }

//   return res;
// }


// void KSVTopLevel::setEnabled( bool val )
// {
//   static bool newM, saveM, printM, printLogM, exitM;
//   static bool cutM, copyM, pasteM;
//   static bool showToolsM, showStatusM, showLogM, configM, saveOptionsM;
//   static bool newT, saveT, cutT, copyT, pasteT, showLogT;
//   static bool called = false;

//   KToolBar* t = toolBar();
//   KMenuBar* m = menuBar();

//   if (!val)
//     {
//       newT = t->getButton(Tools::New)->isEnabled();
//       saveT = t->getButton(Tools::Save)->isEnabled();
//       cutT = t->getButton(Tools::Cut)->isEnabled();
//       copyT = t->getButton(Tools::Copy)->isEnabled();
//       pasteT = t->getButton(Tools::Paste)->isEnabled();
//       showLogT = t->getButton(Tools::Toggle)->isEnabled();

//       disableToolbar(t);
//       disableMenubar(m);

//       called = true;
//     }
//   else if (!called)
//     {
//       newT = t->getButton(Tools::New)->isEnabled();
//       saveT = t->getButton(Tools::Save)->isEnabled();
//       cutT = t->getButton(Tools::Cut)->isEnabled();
//       copyT = t->getButton(Tools::Copy)->isEnabled();
//       pasteT = t->getButton(Tools::Paste)->isEnabled();
//       showLogT = t->getButton(Tools::Toggle)->isEnabled();

//       called = true;
//     }
//   else
//     {
//       t->setItemEnabled(Tools::New, newT);
//       t->setItemEnabled(Tools::Save, saveT);
//       t->setItemEnabled(Tools::Cut, cutT);
//       t->setItemEnabled(Tools::Copy, copyT);
//       t->setItemEnabled(Tools::Paste, pasteT);
//       t->setItemEnabled(Tools::Toggle, showLogT);
//     }

// }

// void KSVTopLevel::disableToolbar(KToolBar* t)
// {
//   t->setItemEnabled(Tools::New, false);
//   t->setItemEnabled(Tools::Save, false);
//   t->setItemEnabled(Tools::Cut, false);
//   t->setItemEnabled(Tools::Copy, false);
//   t->setItemEnabled(Tools::Paste, false);
//   t->setItemEnabled(Tools::Toggle, false);
// }

// void KSVTopLevel::disableMenubar(KMenuBar* m)
// {

// }
