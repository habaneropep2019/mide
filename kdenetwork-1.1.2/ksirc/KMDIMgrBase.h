/***************************************************************************
             KMDIMgrBase.h  -  Base MDI classes for the KDE project           
                         -------------------            
 
   version              :                                   
    begin                : Sat Oct 17 16:05:25 CDT 1998
                                           
    copyright            : (C) 1998 by Timothy Whitfield and Glover George                         
    email                : timothy@ametro.net and dime@ametro.net                                     
 ***************************************************************************/
/***************************************************************************
 Originally based on Eko Bono Suprijadi's keirc MDI classes.
 ***************************************************************************/
/***************************************************************************
 Keirc MDI classes are based from mdi.h , mdi.cpp by Aaron Granickko.
 ***************************************************************************/
/***************************************************************************
 Code used from the KDE projects kwm window manager.
 ***************************************************************************/

#ifndef _KMDIMGRBASE_H
#define _KMDIMGRBASE_H

#include <qlayout.h>
#include <qwidget.h>
#include <qevent.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qmenubar.h>
#include <qpoint.h>
#include <qtimer.h>
#include <qimage.h>
#include <qlist.h>

#include "options.h"

#include <kapp.h>
#include <kpixmap.h>
#include <ktopwidget.h> 

#define TYPE_BGMDI       1

#define MDI_SHOW         0
#define MDI_DETACHED     2
#define MDI_CLOSED       4
#define MDI_ICONIFY      1

class KMDIMgrBase;
class KMDIWindow;

QPixmap loadIcon(const char* name);

class KMDITitleLabel : public QFrame
{
  Q_OBJECT
  friend class KMDIWindow;
  friend class KMDIMgrBase;
public:
  KMDITitleLabel(QWidget* parent=0, const char* name=0);
  virtual ~KMDITitleLabel();

  void setText (const char *txt);
  const char *getText ();
signals:
  void moving ();
  void popupMenu(QPoint);
protected:

  void readConfiguration();
  void paintState(bool only_label = FALSE, bool colors_have_changed = FALSE, bool animate = FALSE);
  void kwm_gradientFill(KPixmap &pm, QColor ca, QColor cb, bool upDown);

  void paintEvent( QPaintEvent * );
  void mousePressEvent ( QMouseEvent *);

  QColor activeTitleBlend,inactiveTitleBlend;
  kwmOptions options;
  KPixmap aShadepm, iaShadepm;
  int titlestring_offset,titlestring_offset_delta;
  bool is_active;
  QString label;
};

class KMDITitle : public QWidget
{
  Q_OBJECT
  friend class KMDIWindow;
  friend class KMDIMgrBase;

public:
  KMDITitle(const char* icon=0L, QWidget* p=0L, const char* n=0L);   
  virtual ~KMDITitle(){};
  void setEnabled(bool);

  inline QButton *getIconbtn() {
    return iconBtn;
  }
  inline QButton *getMinbtn() {
    return minBtn;
  }
  inline QButton *getMaxbtn() {
    return maxBtn;
  }
  inline QButton *getClosebtn() {
    return closeBtn;
  }

protected:

  QButton* iconBtn;
  QButton* minBtn;
  QButton* maxBtn;
  QButton* closeBtn;
  KMDITitleLabel*   caption;
//  QPixmap  detachPic;
//  QPixmap  attachPic;
};

// KMDIWindow is a basic window that can appear in an KMDIMgrBase
class KMDIWindow : public QWidget
{
    Q_OBJECT
    friend class KMDIMgrBase;
protected:
    QFrame*   frame;
    KMDITitle* titleBar;
    QWidget*  view;

    QPoint anchorPoint;
    QRect  anchorRect;
    QPoint popupPoint;
    int restoreX;
    int restoreY;
    int restoreWidth;
    int restoreHeight;

    bool eraseResizeRect;
    bool moveMode;
    int  resizeMode;
    int  corner;
    bool isMinimized;
    bool isMaximized;
    bool isSelected;
    bool isAttached;
    QWidget* oldParent;
    WFlags   oldWFlags;
    int      flag;
public:

    KMDIWindow ( QWidget *parent=0, const char *name=0, 
		int flag=0, const char* icon=0L);
    virtual ~KMDIWindow(){}
    inline bool IsMinimized () { 
      return isMinimized; 
    }
    inline bool IsMaximized () { 
      return isMaximized; 
    }
    inline bool IsSelected () { 
      return isSelected; 
    }
    void drawRect ( QRect &rt, bool fancy=false );
    int getFlag(){
       return flag;
    }
    inline QFrame *getFrame() {
      return frame;
    }
    inline KMDITitle *getTitlebar() {
      return titleBar;
    }
    inline QWidget *getView() {
      return view;
    }

public slots:
    void slotMinimize(bool emitted);
    void slotMaximize(bool emitted);
    void slotMinimize(){
       slotMinimize(true);
    }
    void slotMaximize(){
       slotMaximize(true);
    }
    void slotRestore ();
    void slotClose   ();
    void slotSelect(bool restoreIfMinimized = true);//, bool emitted=true);
    void slotDetach();
    void slotAttach(); 
    void slotMoving();
    void setView    (QWidget *);
    void setCaption (const char * );
    void changeColorSelect(bool);
signals: 
    void minimized (KMDIWindow *);
    void maximized (KMDIWindow *);
    void restored  (KMDIWindow *);
    void closed    (KMDIWindow *);
    void selected  (KMDIWindow *); 
    void moved     (int x, int y);
    void moved     (KMDIWindow *, int x, int y);
    void captionChanged(KMDIWindow *, const char *);
protected:

    virtual void focusInEvent(QFocusEvent*);
    virtual void focusOutEvent(QFocusEvent*);
    virtual bool eventFilter ( QObject *, QEvent *);
    virtual void resizeEvent ( QResizeEvent *);
    virtual void mousePressEvent ( QMouseEvent *);
    virtual void mouseMoveEvent ( QMouseEvent *);
    virtual void mouseReleaseEvent ( QMouseEvent *);
    void linkChildren(QWidget *);
    void drawResizeAnimation(QRect &, QRect &);
    void doResizing();
    void doMove(QMouseEvent*);
    int isInResizeArea ( int x, int y);
    QPoint getCursorPos();
};


// KMDIMgrBase manages a collection of KMDIWindows, and provides for minimizing, maximizing,
// closing, resizing and moving these windows
class KMDIMgrBase : public QFrame
{
    Q_OBJECT
    friend class KMDITitle;
    friend class KMDIWindow;
protected:
    KMDIWindow* selectedWnd;
    int numWindows;
    int defaultWindowWidth;
    int defaultWindowHeight;
    int defaultWindowPosX;
    int defaultWindowPosY;
    bool smartPlacement;
    QList <KMDIWindow>* windowList;
    static QString minimizePic;
    static QString maximizePic;
    static QString closePic;
    QPixmap bgPixmap;
public:
    KMDIMgrBase (QWidget* p=0L, const char *name = 0);
    virtual ~KMDIMgrBase();
    inline KMDIWindow *SelectedWindow() { 
      return selectedWnd; 
    }
    inline QList<KMDIWindow> *WindowList() { 
      return windowList; 
    }
signals:
    void windowRemoved(KMDIWindow *);
    void windowAdded  (KMDIWindow *);
public slots:
    KMDIWindow *getWindowByName (const char *name, bool caseSensitive = false);
    KMDIWindow *getWindowByIndex(int index );
    inline int numberOfWindows () { 
      return numWindows; 
    }
    KMDIWindow* addWindow (QWidget *,   int flag , const char* icon=0L); 
    void addWindow       (KMDIWindow *, int flag=0);
    void removeWindow    (KMDIWindow * );
    void removeWindow    (const char* name);
    void tile (); // tiles the windows
    void cascade (); // cascades the windows from the upper left corner
    void setDefaultWindowSize( int w, int h);
    void setDefaultWindowPos ( int x, int y);
    void setSmartPlacement(bool);
    void nextWindow(); // switches focus to the next window
    void prevWindow(); // switches focus to the previous window
    void slotSetBgImage(const char*);
    void slotSetColor  (const QColor&, int);
protected:
    bool isPointTaken    (int x, int y); // true if point is not already occupied
    virtual void resizeEvent ( QResizeEvent *);
protected slots:
     void handleWindowSelect ( KMDIWindow *);
};



#endif

