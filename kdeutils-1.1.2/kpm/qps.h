// qps.h				emacs, this is written in -*-c++-*-
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 1997

#ifndef QPS_H
#define QPS_H

#ifdef KDE
#include <qslider.h>
#include <kapp.h>
#include <ktopwidget.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <kiconloader.h>
#endif // KDE


#include <qwidget.h>
#include <qmenubar.h>
#include <qlist.h>
#include <qlabel.h>

#include "pstable.h"
#include "proc.h"
#include "infobar.h"
#include "fieldsel.h"
#include "details.h"
#include "prefs.h"

#if QT_VERSION < 130
#error Qt library version 1.30 or higher is needed for this version of qps
#endif

class Qps : public QWidget 
{
    Q_OBJECT
public:
    Qps(QWidget *parent = 0, const char *name = 0);
    ~Qps();

    void set_update_period(int milliseconds);

    void dock(bool enable);

    // settings
    static bool show_cmd_path;	       	// CMDLINE shows command path
    static bool show_infobar;		// information bar is shown
    static bool show_toolbar;
    static bool show_mem_bar;		// memory is shown as a bar
    static bool show_swap_bar;		// swap is shown as a bar
    static bool show_cpu_bar;		// cpu states are shown as a bar
    static bool show_load_graph;	// load is shown as a graph
    static bool phosphor_graph;		// load graph is green/black
    static bool solid_graph;            // solid filled graphs
    static bool load_in_icon;		// load graph in icon
    static bool auto_save_options;	// settings saved on exit
    static bool old_style_tables;	// macOS7 style htables are used
    static bool hostname_lookup;	// do a name lookup on socket addresses
    static bool panel_docking;          // KDE compliant docking

public slots:
    // menu:
    void sig_term();
    void sig_hup();
    void sig_int();
    void sig_kill();
    void other_menu(int id);
    void menu_update();
    void menu_toggle_path();
    void menu_toggle_infobar();
    void menu_toggle_toolbar();
    void menu_prefs();
    void menu_savenow();
    void menu_renice();
    void menu_sched();
    void menu_info();
    void menu_custom();
    void view_menu(int id);
    void save_quit();
    void add_fields_menu(int id);
    void menu_remove_field();
    void menu_help_field();

    void context_row_menu(QPoint p);
    void context_heading_menu(QPoint p, int col);

    void update_menu_selection_status();
    void forced_update();
    void field_added(int index);
    void field_removed(int index);
    void details_closed(Details *d);
    void open_details(int row);
    void iconify_window();
    void config_change();

    void saveYourself();

    void dockingClicked();

protected:
    // reimplementation of QWidget methods
    void resizeEvent(QResizeEvent *);
    void timerEvent(QTimerEvent *);
    void closeEvent(QCloseEvent *);

    void timer_refresh();
    void refresh_details();
    void update_menu_status();
    void transfer_selection();
    void update_table(int update_col);

    QPopupMenu *make_signal_menu();
    void send_to_selected(int sig);
    void sendsig(Procinfo *p, int sig);
    void earlier_refresh();
    void read_settings();
    void write_settings();
    void infobar_visibility();
    void set_default_icon();
    void set_load_icon();
    void make_filled_mask(QPixmap *pm);
    void setAppearance(HeadedTable::Appearance newapp);

private:
    QMenuBar *menu;
    QPopupMenu *m_process;
    QPopupMenu *m_signal;
    QPopupMenu *m_view;
    QPopupMenu *m_options;
    QPopupMenu *m_popup;
    QPopupMenu *m_headpopup;
    QPopupMenu *m_fields;

    bool selection_items_enabled; // state of certain menu items

    Pstable *pstable;
    Proc *proc;
    Procview *procview;
    Infobar *infobar;
    FieldSelect *field_win;
    Preferences *prefs_win;

    QList<Details> details;

    int update_period;				// in ms
    // the Linux kernel updates its load average counters every 5 seconds
    // (kernel 2.0.29), so there is no point in reading it too often
    static const int load_update_period;	// in ms
    int update_load_time;			// time left to update load

    // size of our icon
    static const int icon_width;
    static const int icon_height;
    QPixmap *default_icon;
    bool default_icon_set;	// true if default icon is current icon

    int context_col;		// where heading context menu was clicked

    enum menuid {
	MENU_SIGQUIT, MENU_SIGILL, MENU_SIGABRT, MENU_SIGFPE, MENU_SIGSEGV,
	MENU_SIGPIPE, MENU_SIGALRM, MENU_SIGUSR1, MENU_SIGUSR2, MENU_SIGCHLD,
	MENU_SIGCONT, MENU_SIGSTOP, MENU_SIGTSTP, MENU_SIGTTIN, MENU_SIGTTOU,
	MENU_RENICE, MENU_SCHED, MENU_SOCK,
	MENU_SIGTERM, MENU_SIGHUP, MENU_SIGINT, MENU_SIGKILL, MENU_OTHERS,
	MENU_PROCS, MENU_FIELDS, MENU_CUSTOM,
        MENU_PATH, MENU_INFOBAR, MENU_PREFS, MENU_TOOLBAR
    };
};

class DockWidget : public QLabel {
  Q_OBJECT
public:
  DockWidget();
  
  virtual void mousePressEvent(QMouseEvent *);

signals:
  void clicked();
};

#endif // QPS_H
