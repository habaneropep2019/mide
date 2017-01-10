// qps.C
//
// qps -- Qt-based visual process status monitor
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <signal.h>
#include <errno.h>
#include <sched.h>

#include "qps.h"
#include "dialogs.h"
#include "scheddlg.h"

#include <qpopmenu.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qapp.h>
#include <qfont.h>
#include <qpainter.h>
#include <qaccel.h>
#include <qmsgbox.h>
#include <qbitmap.h>
#include <qregexp.h>
#include <qwmatrix.h>

#include <ktopwidget.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kiconloader.h>
#include <kwm.h>
#include "locale.h"

#define QPS_VERSION "1.3"
#define KPM_VERSION "6"

const int Qps::load_update_period = 4800;	// in ms
const int Qps::icon_width = 48;
const int Qps::icon_height = 48;


// default values of settings, overridden by $HOME/.qps-settings if present
bool Qps::show_cmd_path = TRUE;
bool Qps::show_infobar = TRUE;
bool Qps::show_mem_bar = TRUE;
bool Qps::show_swap_bar = TRUE;
bool Qps::show_cpu_bar = TRUE;
bool Qps::show_load_graph = TRUE;
bool Qps::phosphor_graph = TRUE;
bool Qps::solid_graph = TRUE;
bool Qps::load_in_icon = TRUE;
bool Qps::auto_save_options = TRUE;
bool Qps::old_style_tables = FALSE;
bool Qps::hostname_lookup = FALSE;
bool Qps::panel_docking = FALSE;
bool Qps::show_toolbar = TRUE;

KTopLevelWidget *tl;
KMenuBar *kdemenu;

DockWidget::DockWidget() : QLabel((QWidget*)0) {
}

void DockWidget::mousePressEvent(QMouseEvent *e) {
  if(e->button() == LeftButton)
    emit clicked();
}

DockWidget *docking = 0;
QPixmap *dockicon = 0;

Qps::Qps(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  int id;

  connect(kapp, SIGNAL(saveYourself()), this, SLOT(saveYourself()));

  m_process = new QPopupMenu;
  m_process->insertItem(TRANS("Renice..."), MENU_RENICE);
  m_process->connectItem(MENU_RENICE, this, SLOT(menu_renice()));
  m_process->setAccel(CTRL + Key_R, MENU_RENICE);
  m_process->insertItem(TRANS("Change Scheduling..."), MENU_SCHED);
  m_process->connectItem(MENU_SCHED, this, SLOT(menu_sched()));
  m_process->setAccel(CTRL + Key_S, MENU_SCHED);
  m_process->insertSeparator();
  m_process->insertItem(TRANS("Socket Info"), MENU_SOCK);
  m_process->connectItem(MENU_SOCK, this, SLOT(menu_info()));
  m_process->setAccel(CTRL + Key_I, MENU_SOCK);

  QPopupMenu *m_file = new QPopupMenu;
  m_file->insertItem(TRANS("Iconify"), this,
		     SLOT(iconify_window()), CTRL + Key_Z);
  m_file->insertSeparator();
  m_file->insertItem(TRANS("E&xit"), this, SLOT(save_quit()), CTRL + Key_X);

  QPopupMenu *menu_signals = make_signal_menu();

  m_signal = new QPopupMenu;
  m_signal->insertItem(TRANS("Terminate"), MENU_SIGTERM);
  m_signal->connectItem(MENU_SIGTERM, this, SLOT(sig_term()));
  m_signal->setAccel(CTRL + Key_T, MENU_SIGTERM);
  m_signal->insertItem(TRANS("Hangup"), MENU_SIGHUP);
  m_signal->connectItem(MENU_SIGHUP, this, SLOT(sig_hup()));
  m_signal->setAccel(CTRL + Key_H, MENU_SIGHUP);
  m_signal->insertItem(TRANS("Interrupt"), MENU_SIGINT);
  m_signal->connectItem(MENU_SIGINT, this, SLOT(sig_int()));
  m_signal->insertSeparator();
  m_signal->insertItem(TRANS("Kill"), MENU_SIGKILL);
  m_signal->connectItem(MENU_SIGKILL, this, SLOT(sig_kill()));
  m_signal->setAccel(CTRL + Key_K, MENU_SIGKILL);
  m_signal->insertItem(TRANS("Other"), menu_signals, MENU_OTHERS);
  connect(menu_signals, SIGNAL(activated(int)), SLOT(other_menu(int)));

  QPopupMenu *popup_signals = make_signal_menu();

  m_popup = new QPopupMenu;
  m_popup->insertItem(TRANS("Renice..."), this, SLOT(menu_renice()));
  id = m_popup->insertItem(TRANS("Scheduling..."), this, SLOT(menu_sched()));  
  m_popup->setItemEnabled(id, geteuid()==0);
  m_popup->insertSeparator();
  m_popup->insertItem(TRANS("Socket Info"), this, SLOT(menu_info()));
  m_popup->insertSeparator();
  m_popup->insertItem(TRANS("Terminate"), this, SLOT(sig_term()));
  m_popup->insertItem(TRANS("Hangup"), this, SLOT(sig_hup()));
  m_popup->insertItem(TRANS("Interrupt"), this, SLOT(sig_int()));
  m_popup->insertSeparator();
  m_popup->insertItem(TRANS("Kill"), this, SLOT(sig_kill()));
  m_popup->insertItem(TRANS("Other signals"), popup_signals);
  connect(popup_signals, SIGNAL(activated(int)), SLOT(other_menu(int)));

  m_fields = new QPopupMenu;
  // m_fields will be filled with all non-displayed fields when used
  connect(m_fields, SIGNAL(activated(int)), SLOT(add_fields_menu(int)));

  m_headpopup = new QPopupMenu;
  m_headpopup->insertItem(TRANS("Remove field"), this, 
			  SLOT(menu_remove_field()));
  m_headpopup->insertItem(TRANS("Add field..."), m_fields);

  // KDE help on fields
  m_headpopup->insertSeparator();
  m_headpopup->insertItem(TRANS("Help on field..."), this, 
			  SLOT(menu_help_field()));

  m_view = new QPopupMenu;
  m_view->setCheckable(TRUE);
  m_view->insertItem(TRANS("Refresh process list"), this, 
		     SLOT(forced_update()), CTRL + Key_U);
  m_view->insertSeparator();
  m_view->insertItem(TRANS("All Processes"), Procview::ALL);
  m_view->insertItem(TRANS("Your Processes"), Procview::OWNED);
  m_view->insertItem(TRANS("Non-Root Processes"), Procview::NROOT );
  m_view->insertItem(TRANS("Running Processes"), Procview::RUNNING);
  m_view->insertSeparator();
  m_view->insertItem(TRANS("User Fields"), Procview::USER);
  m_view->insertItem(TRANS("Jobs Fields"), Procview::JOBS);
  m_view->insertItem(TRANS("Memory Fields"), Procview::MEM);
  m_view->insertSeparator();
  m_view->insertItem(TRANS("Select Fields..."), MENU_CUSTOM);
  m_view->connectItem(MENU_CUSTOM, this, SLOT(menu_custom()));
  connect(m_view, SIGNAL(activated(int)), SLOT(view_menu(int)));

  m_options = new QPopupMenu;
  m_options->setCheckable(TRUE);
  m_options->insertItem(TRANS("Update Period..."), this, SLOT(menu_update()));
  m_options->insertSeparator();

  // self-changing menu items are a Bad Thing(tm). Avoid that
  // in KDE
  m_options->insertItem(TRANS("Show command path"), MENU_PATH);
  m_options->connectItem(MENU_PATH, this, SLOT(menu_toggle_path()));
  m_options->insertItem(TRANS("Show ressource info"), MENU_INFOBAR);
  m_options->connectItem(MENU_INFOBAR, this, SLOT(menu_toggle_infobar()));
  m_options->insertItem(TRANS("Show toolbar"), MENU_TOOLBAR);
  m_options->connectItem(MENU_TOOLBAR, this, SLOT(menu_toggle_toolbar()));

  m_options->insertSeparator();
  m_options->insertItem(TRANS("Preferences..."), MENU_PREFS);
  m_options->connectItem(MENU_PREFS, this, SLOT(menu_prefs()));
  m_options->insertSeparator();
  m_options->insertItem(TRANS("Save Settings Now"), this, 
			SLOT(menu_savenow()));

  QString s;
  s.sprintf("kpm %sk%s", QPS_VERSION, KPM_VERSION);
  s += TRANS(" - A Visual Process Manager (KDE Version)\n\n"
	     "© 1997 Mattias Engdegård (f91-men@nada.kth.se)\n"
	     "© 1998 Mario Weilguni (mweilguni@kde.org)\n\n"
	     "kpm is the KDE version of qps, written by Matthias\n"
	     "Engdegård. The port to KDE was done by Mario\n"
	     "Weilguni. If you are interested on the original\n"
	     "qps, you can find it at the following location:\n"
	     "    ftp://sunsite.unc.edu/pub/linux/system/status\n\n"
	     "This program is published under the GNU General\n"
	     "Public License (GNU GPL). See the file COPYING\n"
	     "for details about this.");

  QPopupMenu *help = kapp->getHelpMenu(TRUE, s.data());

  kdemenu = new KMenuBar(tl);
  kdemenu->insertItem(TRANS("&Window"), m_file);
  kdemenu->insertItem(TRANS("&Process"), m_process);
  kdemenu->insertItem(TRANS("&Signal"), m_signal);
  kdemenu->insertItem(TRANS("&View"), m_view);
  kdemenu->insertItem(TRANS("&Options"), m_options);
  kdemenu->insertSeparator();
  kdemenu->insertItem(TRANS("&Help"), help);

  proc = new Proc();
  procview = new Procview(proc);
  procview->refresh();

  default_icon = 0;
  default_icon_set = FALSE;

  infobar = new Infobar(this, "infobar");
  infobar->setGeometry(0, 0, width(), 56);

  pstable = new Pstable(this, procview);
  int ly = infobar->y() + infobar->height();
  pstable->setGeometry(0, ly, width(), height() - ly);
  pstable->recompute_table_widths();

  connect(pstable, SIGNAL(selection_changed()),
	  SLOT(update_menu_selection_status()));
  connect(pstable, SIGNAL(doubleClicked(int)),
	  SLOT(open_details(int)));
  connect(pstable, SIGNAL(rightClickedRow(QPoint)),
	  this, SLOT(context_row_menu(QPoint)));
  connect(pstable, SIGNAL(rightClickedHeading(QPoint, int)),
	  this, SLOT(context_heading_menu(QPoint, int)));

  field_win = 0;
  prefs_win = 0;

  selection_items_enabled = TRUE;
  details.setAutoDelete(TRUE);

  QAccel *accel = new QAccel(this);
  accel->connectItem(accel->insertItem(Key_Space),
		    this,
		    SLOT(forced_update()));


  resize(700, 300);		// default size, can be overridden
  set_update_period(5000);	// also default
  update_load_time = 0;
  read_settings();
  infobar->configure();	// make settings take effect in status bar
  infobar->refresh(TRUE);
  update_menu_status();
  if(show_infobar)
    infobar_visibility();

  startTimer(update_period);
}

// destructor isn't actually needed, but prevents some gcc bugs
Qps::~Qps()
{
}

void Qps::dock(bool enable) {
  if(enable) {
    if(docking || !KWM::isKWMInitialized())
      return;

    docking = new DockWidget();
    connect(docking, SIGNAL(clicked()),
	    this, SLOT(dockingClicked()));
    docking->resize(24, 24);
    KWM::setDockWindow(docking->winId());
    dockicon = new QPixmap(24, 24);
    dockicon->fill(docking->backgroundColor());
    set_load_icon();
    docking->show();
  } else {
    if(!docking)
      return;
      docking->close();
      delete docking;
      delete dockicon;
      docking = 0;
      dockicon = 0;
  }
}

void Qps::dockingClicked() {
  if(topLevelWidget()->isVisible())
    topLevelWidget()->iconify();
  else
    topLevelWidget()->show();
}

// build signal menu (used in two places)
QPopupMenu *Qps::make_signal_menu()
{
  QPopupMenu *m = new QPopupMenu;
  m->insertItem(TRANS("SIGQUIT (quit)"), MENU_SIGQUIT);
  m->insertItem(TRANS("SIGILL (illegal instruction)"), MENU_SIGILL);
  m->insertItem(TRANS("SIGABRT (abort)"), MENU_SIGABRT);
  m->insertItem(TRANS("SIGFPE (floating point exception)"), MENU_SIGFPE);
  m->insertItem(TRANS("SIGSEGV (segmentation violation)"), MENU_SIGSEGV);
  m->insertItem(TRANS("SIGPIPE (broken pipe)"), MENU_SIGPIPE);
  m->insertItem(TRANS("SIGALRM (timer signal)"), MENU_SIGALRM);
  m->insertItem(TRANS("SIGUSR1 (user-defined 1)"), MENU_SIGUSR1);
  m->insertItem(TRANS("SIGUSR2 (user-defined 2)"), MENU_SIGUSR2);
  m->insertItem(TRANS("SIGCHLD (child death)"), MENU_SIGCHLD);
  m->insertItem(TRANS("SIGCONT (continue)"), MENU_SIGCONT);
  m->insertItem(TRANS("SIGSTOP (stop)"), MENU_SIGSTOP);
  m->insertItem(TRANS("SIGTSTP (stop from tty)"), MENU_SIGTSTP);
  m->insertItem(TRANS("SIGTTIN (tty input)"), MENU_SIGTTIN);
  m->insertItem(TRANS("SIGTTOU (tty output)"), MENU_SIGTTOU);
  return m;
}


void Qps::resizeEvent(QResizeEvent *)
{
  infobar->resize(width(), infobar->height());
  pstable->resize(width(), height() - pstable->y());
}

void Qps::timerEvent(QTimerEvent *)
{
  killTimers();		// avoid accumulation of timer events if slow
  timer_refresh();
  startTimer(update_period);
}

void Qps::closeEvent(QCloseEvent *)
{
  save_quit();
}

void Qps::save_quit()
{
  if(auto_save_options)
    write_settings();
  kapp->quit();
}

void Qps::timer_refresh()
{
  // don't update load more often than necessary
  bool update_load = FALSE;
  update_load_time -= update_period;
  if(update_load_time <= 0) {
    update_load = TRUE;
    update_load_time = load_update_period;
    Procinfo::read_loadavg();
    if(load_in_icon)
      set_load_icon();
    else
      set_default_icon();
  }

  if(isVisible()) {
    procview->refresh();
    if(show_infobar)
      infobar->refresh(update_load);
    pstable->recompute_table_widths();
    pstable->transfer_selection();
    pstable->repaint_changed();
    update_menu_selection_status();
  } else {
    if(update_load)
      infobar->update_load();
  }
  refresh_details();
}

// update table due to a configuration change
// update_col is leftmost column that has changed
void Qps::update_table(int update_col)
{
  pstable->recompute_table_widths();
  pstable->transfer_selection();
  pstable->set_sortcol();
  pstable->repaint_statically_changed(update_col);
}

void Qps::set_default_icon()
{
  // TODO
//   if(!default_icon_set) {
//     if(!default_icon)
//       default_icon = new QPixmap((const char**)kpm_xpm);
//     setIcon(*default_icon);
//     default_icon_set = TRUE;
//   }
}

// avoid a fvwm2/Qt 1.30 problem and create a filled mask for the icon
void Qps::make_filled_mask(QPixmap *pm)
{
  QBitmap bm(pm->size());
  bm.fill(color1);
  pm->setMask(bm);
}

void Qps::set_load_icon()  
{
  QPixmap *pm = infobar->load_icon(icon_width, icon_height);
  make_filled_mask(pm);
  setIcon(*pm);
  default_icon_set = FALSE;

  if(docking) {    
    infobar->draw_icon(dockicon);
    docking->setPixmap(*dockicon);
  }
}

void Qps::refresh_details()
{	
  details.first();
  Details *d = 0;
  Procinfo::invalidate_sockets();
  while((d = details.current()) != 0) {
    if(d->isVisible())
      d->refresh();
    details.next();
  }
}

void Qps::forced_update()
{
  killTimers();
  timer_refresh();
  startTimer(update_period);
}

// update the menu status
void Qps::update_menu_status()
{
  update_menu_selection_status();
  for(int i = Procview::ALL; i <= Procview::RUNNING; i++)
    m_view->setItemChecked(i, i == procview->viewproc);
  for(int i = Procview::USER; i <= Procview::MEM; i++)
    m_view->setItemChecked(i, i == procview->viewfields);

  m_options->setItemChecked(MENU_PATH, show_cmd_path);
  m_options->setItemChecked(MENU_INFOBAR, show_infobar);
  m_options->setItemChecked(MENU_TOOLBAR, show_toolbar);
}

// update the menu according to current selection
void Qps::update_menu_selection_status()
{
  bool enabled = (pstable->numSelected() > 0);
  if(enabled != selection_items_enabled) {
    m_process->setItemEnabled(MENU_RENICE, enabled);
    m_process->setItemEnabled(MENU_SCHED, enabled && geteuid() == 0);
    m_process->setItemEnabled(MENU_SOCK, enabled);
    m_signal->setItemEnabled(MENU_SIGTERM, enabled);
    m_signal->setItemEnabled(MENU_SIGHUP, enabled);
    m_signal->setItemEnabled(MENU_SIGINT, enabled);
    m_signal->setItemEnabled(MENU_SIGKILL, enabled);
    m_signal->setItemEnabled(MENU_OTHERS, enabled);
    selection_items_enabled = enabled;
  }
}

void Qps::sig_term()
{
  send_to_selected(SIGTERM);
}

void Qps::sig_hup()
{
  send_to_selected(SIGHUP);
}

void Qps::sig_int()
{
  send_to_selected(SIGINT);
}

void Qps::sig_kill()
{
  send_to_selected(SIGKILL);
}

void Qps::other_menu(int id)
{
  switch(id) {
  case MENU_SIGQUIT:	send_to_selected(SIGQUIT); break;
  case MENU_SIGILL:	send_to_selected(SIGILL); break;
  case MENU_SIGABRT:	send_to_selected(SIGABRT); break;
  case MENU_SIGFPE:	send_to_selected(SIGFPE); break;
  case MENU_SIGSEGV:	send_to_selected(SIGSEGV); break;
  case MENU_SIGPIPE:	send_to_selected(SIGPIPE); break;
  case MENU_SIGALRM:	send_to_selected(SIGALRM); break;
  case MENU_SIGUSR1:	send_to_selected(SIGUSR1); break;
  case MENU_SIGUSR2:	send_to_selected(SIGUSR2); break;
  case MENU_SIGCHLD:	send_to_selected(SIGCHLD); break;
  case MENU_SIGCONT:	send_to_selected(SIGCONT); break;
  case MENU_SIGSTOP:	send_to_selected(SIGSTOP); break;
  case MENU_SIGTSTP:	send_to_selected(SIGTSTP); break;
  case MENU_SIGTTIN:	send_to_selected(SIGTTIN); break;
  case MENU_SIGTTOU:	send_to_selected(SIGTTOU); break;
  }
}

void Qps::view_menu(int id)
{
  if(id >= Procview::ALL && id <= Procview::RUNNING) {
    Procview::procstates state = (Procview::procstates)id;
    if(procview->viewproc != state) {
      procview->viewproc = state;
      procview->rebuild();
      pstable->recompute_table_widths();
      pstable->transfer_selection();
      pstable->topAndRepaint();
      update_menu_status();
    }
  } else if(id >= Procview::USER && id <= Procview::MEM) {
    Procview::fieldstates state = (Procview::fieldstates)id;
    if(procview->viewfields != state) {
      procview->viewfields = state;
      procview->set_fields();
      pstable->set_sortcol();
      pstable->recompute_table_widths();
      pstable->transfer_selection();
      pstable->topAndRepaint();
      update_menu_status();
      if(field_win)
	field_win->update_boxes();
    }
  }
}

void Qps::menu_custom()
{
  if(field_win) {
    field_win->show();
    field_win->raise();
  } else {
    QPixmap pm = kapp->getMiniIcon();

    field_win = new FieldSelect(procview, proc);
    field_win->show();
    KWM::setMiniIcon(field_win->winId(), pm);
    connect(field_win, SIGNAL(added_field(int)),
	    this, SLOT(field_added(int)));
    connect(field_win, SIGNAL(removed_field(int)),
	    this, SLOT(field_removed(int)));
  }
}

void Qps::field_added(int index)
{
  Category *newcat = proc->cats[index];
  procview->add_cat(newcat);
  int newcol = -1;
  for(int i = 0; i < procview->cats.size(); i++)
    if(procview->cats[i] == newcat) {
      newcol = i; break;
    }

  update_table(newcol);
  procview->viewfields = Procview::CUSTOM;
  update_menu_status();
}

void Qps::field_removed(int index)
{
  // don't remove last field
  if(procview->cats.size() == 1) {
    field_win->update_boxes();
    return;		// don't remove
  }
  int delcol = -1;
  for(int i = 0; i < procview->cats.size(); i++) {
    Category *c = procview->cats[i];
    if(c->index == index) {
      procview->remove_cat(i);
      delcol = i;
      break;
    }
  }
  update_table(delcol);
  procview->viewfields = Procview::CUSTOM;
  update_menu_status();
}

void Qps::menu_info()
{
  for(int i = 0; i < procview->procs.size(); i++) {
    Procinfo *p = procview->procs[i];
    if(p->selected)
      open_details(i);
  }
}

void Qps::open_details(int row)
{
  Procinfo *p = procview->procs[row];
  if(p->details)
    p->details->raise();
  else {
    Procinfo::read_sockets();
    if(p->read_socket_inodes()) {
      Details *d = new Details(0, p, this);
      details.append(d);
      d->show();
      connect(d, SIGNAL(closed(Details *)),
	      this, SLOT(details_closed(Details *)));
    } else {
      if(errno == EACCES) {
	QString s(100);
	s.sprintf(TRANS("You do not have permission to view the\n"
		  "open sockets of process %d (%s).\n"
		  "Only the owner of the process and the\n"
		  "super-user may do that."),
		  p->pid, (const char*)p->comm);
	MessageDialog::message(TRANS("Permission denied"), s, "Cancel",
			       MessageDialog::warningIcon());
      }	// otherwise, the process is gone --- just do nothing
    }
  }
}

void Qps::details_closed(Details *d)
{
  details.removeRef(d);
}

// change the update period, recomputing the averaging factor
void Qps::set_update_period(int milliseconds)
{
  update_period = milliseconds;
  Procview::avg_factor =
    exp(-(float)update_period / Procview::cpu_avg_time);
}

// called when right button is clicked in table
void Qps::context_row_menu(QPoint p)
{  
  m_popup->popup(p);
}

// called when right button is clicked in heading
void Qps::context_heading_menu(QPoint p, int col)
{
  // rebuild the submenu: only include non-displayed fields
  m_fields->clear();
  // we use the fact that the fields are always in the same order
  for(int i = 0, j = 0; i < proc->cats.size(); i++) {
    if(j >= procview->cats.size() || proc->cats[i] != procview->cats[j])
      m_fields->insertItem(proc->cats[i]->name, i);
    else
      j++;
  }
  m_headpopup->setItemEnabled(1, procview->cats.size() < proc->cats.size());
  context_col = col;
  m_headpopup->popup(p);
}

// called when field is added from heading context menu
void Qps::add_fields_menu(int id)
{
  field_added(id);
  if(field_win)
    field_win->update_boxes();
}

void Qps::menu_remove_field()
{
  if(procview->cats.size() >= 2) {
    field_removed(procview->cats[context_col]->index);
    if(field_win)
      field_win->update_boxes();
  }
}

void Qps::menu_help_field() {
  kapp->invokeHTMLHelp("kpm/kpm-3.html", 
		       procview->cats[context_col]->name);
}

void Qps::menu_update()
{
  QString txt(10);
  for(;;) {
    if(update_period % 1000 == 0)
      txt.sprintf("%d s", update_period / 1000);
    else
      txt.sprintf("%d ms", update_period);
    ValueDialog vd(TRANS("qps: change update period"), 
		   TRANS("New update period:"),
		   (const char*)txt);
    if(vd.exec()) {
      QString s = vd.ed_result;
      int i = 0;
      while(s[i] >= '0' && s[i] <= '9' || s[i] == '.') i++;
      float period = (i > 0) ? s.left(i).toFloat() : -1;
      s = s.mid(i, 3).stripWhiteSpace();
      if(s.length() == 0 || s == "s")
	period *= 1000;
      else if(s == "min")
	period *= 60000;
      else if(s != "ms")
	period = -1;
      if(period < 0) {
	MessageDialog::message(TRANS("Invalid input"),
			       TRANS("The time between updates should be a"
			       " number, optionally followed\n"
			       "by a unit (ms, s or min). If no unit"
			       " is given, seconds is assumed."),
			       "Cancel", MessageDialog::warningIcon());
	continue;
      } else {
	set_update_period((int)period);
	killTimers();
	startTimer(update_period);
      }
    }
    return;
  }
}

void Qps::menu_toggle_path()
{	
  show_cmd_path = !show_cmd_path;
  update_menu_status();
  int col = procview->findCol(F_CMDLINE);
  if(col != -1)
    update_table(col);
}

void Qps::menu_toggle_toolbar() {
  show_toolbar = !show_toolbar;

  KTopLevelWidget *tl = (KTopLevelWidget *)parentWidget();  
  tl->enableToolBar();
  update_menu_status();
}

void Qps::menu_prefs()
{
  if(prefs_win) {
    prefs_win->show();
    prefs_win->raise();
  } else {
    prefs_win = new Preferences();
    prefs_win->show();

    QPixmap pm = kapp->getMiniIcon();
    KWM::setMiniIcon(prefs_win->winId(), pm);

    connect(prefs_win, SIGNAL(prefs_change()),
	    this, SLOT(config_change()));
    connect(infobar, SIGNAL(config_change()),
	    prefs_win, SLOT(update_boxes()));
  }
}

void Qps::config_change()
{
  dock(panel_docking);
  infobar->configure();
  pstable->setAppearance(old_style_tables ?
			 HeadedTable::macOS7
			 : HeadedTable::macOS8);
  details.first();
  Details *d = 0;
  while((d = details.current()) != 0) {
    d->config_change();
    details.next();
  }
}

void Qps::menu_savenow()
{
  write_settings();
  kapp->getConfig()->sync();
}

// update the visibility of the infobar according to show_infobar
void Qps::infobar_visibility()
{
  update_menu_status();
  if(show_infobar) {
    int ly = infobar->y() + infobar->height();
    pstable->setGeometry(0, ly, width(), height() - ly);
    infobar->show();
  } else {
    infobar->hide();
    pstable->setGeometry(0, 0, width(), height());
  }
}

void Qps::menu_toggle_infobar()
{	
  show_infobar = !show_infobar;
  infobar_visibility();
}

void Qps::menu_renice()
{
  if(pstable->numSelected() == 0)
    return;
  int defnice = -1000;

  // use nice of first selected process as default, and check permission
  bool possible = TRUE;
  int euid = geteuid();
  Procinfo *p = 0;
  for(int i = 0; i < procview->procs.size(); i++) {
    p = procview->procs[i];
    if(p->selected) {
      if(defnice == -1000)
	defnice = p->nice;
      if(euid != 0 && euid != p->uid && euid != p->euid)
	possible = FALSE;
    }
  }
  if(!possible) {
    QString s;
    s.sprintf(TRANS("You do not have permission to renice the\n"
	      "selected process%s.\n"
	      "Only the process owner and the super-user\n"
	      "are allowed to do that."),
	      (pstable->numSelected() == 1) ? "" : "es");
    MessageDialog::message(TRANS("Permission denied"), 
			   s, 
			   "Cancel",
			   MessageDialog::warningIcon());
    return;
  }

  int new_nice;
  for(;;) {
    SliderDialog vd(TRANS("qps: renice process"), 
		    TRANS("New nice value:"),
		    defnice, -20, 20);
    if(!vd.exec())
      return;
    bool ok;
    new_nice = vd.ed_result.toInt(&ok);
    if(ok && new_nice >= -20 && new_nice <= 20)
      break;
    else {
      MessageDialog::message(TRANS("Invalid input"),
			     TRANS("The nice value should be\n"
			     "in the range -20 to 20."), "OK",
			     MessageDialog::warningIcon());
    }
  }
  int nicecol = procview->findCol(F_NICE);
  int statcol = procview->findCol(F_STAT);

  // do the actual renicing
  for(int i = 0; i < procview->procs.size(); i++) {
    Procinfo *p = procview->procs[i];
    if(p->selected) {
      if(setpriority(PRIO_PROCESS, p->pid, new_nice) < 0) {
	QString s(100);
	switch(errno) {
	case EPERM:
	  // this shouldn't happen, but (e)uid could be changed...
	  s.sprintf(TRANS("You do not have permission to renice"
		    " process %d (%s).\n"
		    "Only the process owner and the super-user are"
		    " allowed to do that."),
		    p->pid, (const char*)p->comm);
	  MessageDialog::message(TRANS("Permission denied"), 
				 s, 
				 "Cancel",
				 MessageDialog::warningIcon());
	  break;
	case EACCES:
	  MessageDialog::message(TRANS("Permission denied"),
				 TRANS("Only the super-user may lower"
				 " the nice value of a process."),
				 "Cancel",
				 MessageDialog::warningIcon());
	  return;
	}
      } else {
	p->nice = new_nice; // don't wait for update
	if(nicecol != -1)
	  pstable->updateCell(i, nicecol);
	if(statcol != -1)
	  pstable->updateCell(i, statcol);
      }
    }
  }
}

void Qps::menu_sched()
{
  if(pstable->numSelected() == 0)
    return;
  if(geteuid() != 0) {
    MessageDialog::message(TRANS("Permission denied"),
			   TRANS("Only the super-user may change the\n"
			   "scheduling policy and static priority."),
			   "Cancel", MessageDialog::warningIcon());
    return;
  }

  // provide reasonable defaults (first selected process)
  Procinfo *p = 0;
  for(int i = 0; i < procview->procs.size(); i++) {
    p = procview->procs[i];
    if(p->selected)
      break;
  }
  int pol = p->get_policy();
  int pri = p->get_rtprio();
  SchedDialog sd(pol, pri);
  if(!sd.exec())
    return;
  if(sd.out_policy == SCHED_OTHER)
    sd.out_prio = 0;	// only allowed value
  int plcycol = procview->findCol(F_PLCY);
  int rpricol = procview->findCol(F_RPRI);
  for(int i = 0; i < procview->procs.size(); i++) {
    Procinfo *p = procview->procs[i];
    if(p->selected) {
      struct sched_param sp;
      sp.sched_priority = sd.out_prio;
      if(sched_setscheduler(p->pid, sd.out_policy, &sp) < 0) {
	QString s(100);
	if(errno == EPERM) {
	  s.sprintf(TRANS("You do not have permission to change the\n"
		    "scheduling and/or priority of"
		    " process %d (%s).\n"
		    "Only the super-user may do that."),
		    p->pid, (const char*)p->comm);
	  MessageDialog::message(TRANS("Permission denied"), 
				 s, 
				 "Cancel",
				 MessageDialog::warningIcon());
	  break;
	}
      } else {
	p->policy = sd.out_policy; // don't wait for update
	p->rtprio = sd.out_prio;
	if(plcycol != -1)
	  pstable->updateCell(i, plcycol);
	if(rpricol != -1)
	  pstable->updateCell(i, rpricol);
      }
    }
  }
}

void Qps::iconify_window()
{
  topLevelWidget()->iconify();
}

void Qps::send_to_selected(int sig)
{
  for(int i = 0; i < procview->procs.size(); i++) {
    Procinfo *p = procview->procs[i];
    if(p->selected)
      sendsig(p, sig);
  }
  earlier_refresh();		// in case we killed one
}

void Qps::sendsig(Procinfo *p, int sig)
{
  if(kill(p->pid, sig) < 0) {
    // if the process is gone, do nothing - no need to alert the user
    if(errno == EPERM) {
      QString str(100);
      str.sprintf(TRANS("You do not have permission to send a signal to"
		  " process %d (%s).\n"
		  "Only the super-user and the owner of the process"
		  " may send signals to it."),
		  p->pid, (const char*)p->comm);
      MessageDialog::message(TRANS("Permission denied"), 
			     str, 
			     "Cancel",
			     MessageDialog::warningIcon());
    }
  }
}

// make next timer_refresh happen a little earlier to remove processes that
// might have died after a signal_
void Qps::earlier_refresh()
{
  const int delay = 500;	// wait no more than this (ms)
  if(update_period > delay) {
    killTimers();
    startTimer(delay);
  }
}

void Qps::read_settings()
{
  KConfig *conf;

  conf = kapp->getConfig();

  if(!conf->hasKey("Saved"))
    return;  

  conf->setGroup("General");
  int procsel = conf->readNumEntry("Viewproc");
  int fieldsel = conf->readNumEntry("Viewfields");
  procview->viewproc = (Procview::procstates)procsel;
  procview->viewfields = (Procview::fieldstates)fieldsel;

  conf->setGroup("Catalogs");
  int num = conf->readNumEntry("Number");
  procview->cats.clear();
  for(int i = 0; i < num; i++) {
    QString s;
    s.sprintf("Cat_%d", i);
    int index = conf->readNumEntry(s.data());
    procview->add_cat(proc->cats[index]);
  }

  conf->setGroup("General");
  int sortindex, rev, showpath, showinfo, interval, autosave, oldstyle;
  int membar, swapbar, cpubar, loadgr, phosphor, icon, lookup;
  sortindex = conf->readNumEntry("SortIndex");
  rev = conf->readNumEntry("ReverseSort", 0);
  showpath = conf->readNumEntry("ShowCommandPath", 1);
  showinfo = conf->readNumEntry("ShowInfobar", 1);
  autosave = conf->readNumEntry("AutosaveOptions", 1);
  membar = conf->readNumEntry("ShowMembar", 1);
  swapbar = conf->readNumEntry("ShowSwapbar", 1);
  cpubar = conf->readNumEntry("ShowCPUbar", 1);
  loadgr = conf->readNumEntry("ShowLoadGraph", 1);
  phosphor = conf->readNumEntry("PhosphorGraph", 1);
  solid_graph = conf->readBoolEntry("SolidGraph", TRUE);
  panel_docking = conf->readBoolEntry("PanelDocking", FALSE);
  show_toolbar = conf->readNumEntry("ShowToolbar", TRUE);

  // if KWM is not running disable docking
  if(!KWM::isKWMInitialized())
    solid_graph = FALSE;

  icon = conf->readNumEntry("LoadInIcon", 0);
  oldstyle = conf->readNumEntry("OldStyleTables", 0);
  lookup = conf->readNumEntry("HostnameLookup", 0);
  interval = conf->readNumEntry("UpdatePeriod", 5000);
  

  set_update_period(interval);
  procview->sortcat = proc->cats[sortindex];
  procview->reversed = rev;
  show_cmd_path = showpath;
  show_infobar = showinfo;
  auto_save_options = autosave;
  show_mem_bar = membar;
  show_swap_bar = swapbar;
  show_cpu_bar = cpubar;
  show_load_graph = loadgr;
  phosphor_graph = phosphor;
  load_in_icon = icon;
  old_style_tables = oldstyle;
  hostname_lookup = lookup;
  
  procview->rebuild();
  pstable->recompute_table_widths();
  update_menu_status();
  pstable->set_sortcol();
  config_change();
}

void Qps::write_settings()
{  
    KConfig *conf;
    conf = kapp->getConfig();

    conf->writeEntry("Saved", 1);

    conf->setGroup("Geometry");
    conf->writeEntry("X", topLevelWidget()->pos().x());
    conf->writeEntry("Y", topLevelWidget()->pos().y());
    conf->writeEntry("Width", topLevelWidget()->width());
    conf->writeEntry("Height", topLevelWidget()->height());
    
    conf->setGroup("General");
    conf->writeEntry("Viewproc", (int)procview->viewproc);
    conf->writeEntry("Viewfields", (int)procview->viewfields);

    conf->setGroup("Catalogs");
    conf->writeEntry("Number", procview->cats.size());
    for(int i = 0; i < procview->cats.size(); i++) {
	QString s;
	s.sprintf("Cat_%d", i);
	conf->writeEntry(s.data(), procview->cats[i]->index);
    }

    conf->setGroup("General");
    conf->writeEntry("SortIndex", procview->sortcat->index);
    conf->writeEntry("ReverseSort", procview->reversed);
    conf->writeEntry("ShowCommandPath", show_cmd_path);
    conf->writeEntry("ShowInfobar", show_infobar);
    conf->writeEntry("AutosaveOptions", auto_save_options);
    conf->writeEntry("ShowMembar", show_mem_bar);
    conf->writeEntry("ShowSwapbar", show_swap_bar);
    conf->writeEntry("ShowCPUbar", show_cpu_bar);
    conf->writeEntry("ShowLoadGraph", show_load_graph);
    conf->writeEntry("PhosphorGraph", phosphor_graph);
    conf->writeEntry("SolidGraph", solid_graph);
    conf->writeEntry("LoadInIcon", load_in_icon);
    conf->writeEntry("OldStyleTables", old_style_tables);
    conf->writeEntry("HostnameLookup", hostname_lookup);
    conf->writeEntry("UpdatePeriod", update_period);
    conf->writeEntry("ShowToolbar", show_toolbar);

    if(KWM::isKWMInitialized())
      conf->writeEntry("PanelDocking", panel_docking);
}

void Qps::saveYourself() {
  write_settings();
  kapp->getConfig()->sync();
}

// return host name with domain stripped
QString short_hostname()
{
  struct utsname u;
  uname(&u);
  char *p = strchr(u.nodename, '.');
  if(p) *p = '\0';
  QString s(u.nodename);
  return s;
}

int main(int argc, char **argv)
{
  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "--version") == 0
       || strcmp(argv[i], "-version") == 0) {
      fprintf(stderr, "kpm version " QPS_VERSION
	      ", using Qt library %s\n",
	      qVersion());
      exit(0);
    }
  }

  KApplication app(argc, argv, "kpm");

  tl = new KTopLevelWidget;

  Qps *q = new Qps(tl);
  QString cap;
  cap.sprintf("kpm@%s%s", (const char *)short_hostname(),
	      (geteuid() == 0) ? " (root)" : "");

  tl->setCaption(cap);
  tl->setMinimumWidth(630);
  tl->setView(q);

  KToolBar *tb = new KToolBar(tl);
  KIconLoader *il = kapp->getIconLoader();
  tb->insertButton(il->loadIcon("exit.xpm"), 
		   1,
		   SIGNAL(clicked()),
		   q,
		   SLOT(save_quit()),
		   TRUE,
		   "Quit");
  tb->insertButton(il->loadIcon("reload.xpm"), 
		   2,
		   SIGNAL(clicked()),
		   q,
		   SLOT(forced_update()),
		   TRUE,
		   "Refresh process list");

  tl->setMenu(kdemenu);
  tl->addToolBar(tb);

  q->show();
  kdemenu->show();
  tb->show();
  
  if(Qps::show_toolbar == FALSE) {
    // fake visibility
    Qps::show_toolbar = TRUE;
    q->menu_toggle_toolbar();
  }

  // restore geometry
  KConfig *conf = kapp->getConfig();
  if(conf->hasKey("Saved")) {
    conf->setGroup("Geometry");
    int x = conf->readNumEntry("X");
    int y = conf->readNumEntry("Y");
    int w = conf->readNumEntry("Width");
    int h = conf->readNumEntry("Height");
    
    tl->setGeometry(x, y, w, h);
  }

  // icon stuff
  if(KWM::isKWMInitialized()) {
    QPixmap pm = il->loadIcon("kpm.xpm");
    KWM::setIcon(tl->winId(), pm);
    
    pm = il->loadMiniIcon("kpm.xpm");
    KWM::setMiniIcon(tl->winId(), pm);
  }

  tl->show();

  app.setMainWidget(tl);
  return app.exec();
}

