/**********************************************************************

	--- Qt Architect generated file ---

	File: servercontroller.h
	Last generated: Sat Nov 29 08:50:19 1997
	
 Now Under CVS control.

 $$Id: servercontroller.h,v 1.36 1998/12/28 03:40:13 asj Exp $$

 *********************************************************************/

#ifndef servercontroller_included
#define servercontroller_included

class servercontroller;
class ServMessage;
class ProcCommand;

#include <qdict.h>
#include <qpixmap.h> 
#include <qlabel.h>

#include <ktreelist.h>
#include <ktopwidget.h>
#include <kmenubar.h>

#include "ksircprocess.h"

#include "puke/controller.h"

#include "pws-0.5/pws/pws.h"

#include "KMDIMgr.h"

#include "kpopupmenu.h"

extern KMDIMgr *MDIMgr;

class ProcCommand // ServerController message
{
 public:
  static enum {
    addTopLevel,
    deleteTopLevel,
    procClose,
    newChannel,
    changeChannel,
    nickOnline,
    nickOffline,
    turnOffAutoCreate,
    turnOnAutoCreate
  } command;
};


class ServCommand // ServerController message
{
 public:
  static enum {
    updateFilters,
    updatePrefs
  } command;
};

class scInside : QFrame
{
  Q_OBJECT
  friend class servercontroller;
 public:
  scInside ( QWidget * parent=0, const char * name=0, WFlags
           f=0, bool allowLines=TRUE );
  ~scInside();

 protected:
  virtual void resizeEvent ( QResizeEvent * );

 private:
  KTreeList *ConnectionTree;
  QLabel *ASConn;  
  
};

class dockServerController : QFrame
{
  Q_OBJECT
    friend class servercontroller;
public:
  dockServerController(servercontroller *_sc, const char *_name);
  ~dockServerController();
  
protected:
  virtual void mousePressEvent(QMouseEvent *);
  virtual void paintEvent(QPaintEvent *);

private:
  servercontroller *sc;
  QPopupMenu *pop;
};

class servercontroller : public KTopLevelWidget
{
    Q_OBJECT
    friend class dockServerController;
public:

    servercontroller
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~servercontroller();

signals:
    /**
      * Filter rules have changed, need to re-read and update.
      */
    virtual void filters_update();

    void ServMessage(QString server, int command, QString args);

public slots:
    // All slots are described in servercontroll.cpp file
    /**
      * Creates popup asking for new connection
      */
    virtual void new_connection();
    /**
      *  Args:
      *    QString: new server name or IP to connect to.
      *  Action:
      *	 Creates a new sirc process and window !default connected to the 
      *	 server.  Does nothing if a server connection already exists.
      */
    virtual void new_ksircprocess(QString);
    /**
      * Creates popup asking for new channel name
      */
    virtual void new_channel();
    /**
      *  Args:
      *    str: name of the new channel to be created
      *  Action:
      *    Sends a signal to the currently selected server in the tree
      *    list and join the requested channel.  Does nothing if nothing
      *    is selected in the tree list.
      */
    virtual void new_toplevel(QString str);
    /**
      * Action:
      *     Notify all ksircprocess' to update filters
      */
    /**
     * Toggle docking of the ServerController window
     */
    virtual void toggleDocking();
    virtual void slot_filters_update();
    virtual void ToggleAutoCreate();
    virtual void colour_prefs();
    virtual void font_prefs();
    virtual void pws_prefs();
    virtual void pws_delete(QObject *);
    /**
     * Action: Popup a general preferences window which allows various 
     * settings, etc.
     */
    virtual void general_prefs();
    virtual void font_update(const QFont&);
    virtual void filter_rule_editor();
    virtual void configChange();
    virtual void help_general();
    virtual void help_colours();
    virtual void help_filters();
    virtual void help_keys();
    virtual void about_ksirc();

    virtual void ProcMessage(QString server, int command, QString args);
    /**
     * On quit we sync the config to disk and exit
     */
    virtual void endksirc();


protected slots:
  void MDIMinimized(KMDIWindow *);
  void WindowSelected(int index);
      
    
protected:

    void saveProperties(KConfig *);
    void readProperties(KConfig *);

private:

    // La raison d'etre.  We don't run ConnectionTree outr selves, but
    // we get it from out helper class scInside.
    KTreeList *ConnectionTree;

    scInside *sci;

    // Menubar for the top.
    KMenuBar *MenuBar;

    //This handles the personal webserver
    PWS *PWSTopLevel;
    
    // Hold a list of all KSircProcess's for access latter.  Index by server 
    // name
    QDict<KSircProcess> proc_list;
    KAPopupMenu *options, *connections;
    int join_id, server_id;
    
    int open_toplevels;

    QPixmap *pic_icon;
    QPixmap *pic_channel;
    QPixmap *pic_server;
    QPixmap *pic_gf;
    QPixmap *pic_run;
    QPixmap *pic_ppl;
    QPixmap *pic_dock;

    PukeController *PukeC;

    // Holds dockable widget
    dockServerController *dockWidget;
    bool docked;

};
#endif // servercontroller_included
