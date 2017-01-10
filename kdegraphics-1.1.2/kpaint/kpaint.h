// -*- c++ -*-

/* modified by jha (j.habenicht@usa.net)
 */

// $Id: kpaint.h,v 1.16 1998/11/04 20:49:14 habenich Exp $

#ifndef KPAINT_H
#define KPAINT_H

#include <qwidget.h>
#include "QwViewport.h"
/* obsolet (jha)
   #include <kaccel.h> */
#include <ktopwidget.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kfm.h>
#include <qrect.h>
#include "canvas.h"
#include "manager.h"
#include "commands.h"

class ColorBar;
class MainView;
//class QVBoxLayout;

class KPaint : public KTMainWindow
{
Q_OBJECT

public:
  KPaint(const char *url_= 0);
  ~KPaint();

  void setPixmap(QPixmap *);

  // File
  void fileNew();
  void fileOpen();
  void fileSave();
  void fileSaveAs();
  void fileFormat();
  void fileOpenURL();
  void fileSaveAsURL();
  void newWindow();
  void closeWindow();
  void fileExit();

  // Edit
  void editCopy();
  void editCut();
  void editPaste();
  void editPasteImage();
  void editZoomIn();
  void editZoomOut();
  void editMask();
  void editOptions();
  
  // Image
  void imageInfo();
  void imageResize();
  void imageEditPalette();
  void imageChangeDepth();

  // Tool
  void toolProperties();

  // Help
  void helpAbout();
  void helpContents();
  void helpIndex();

public slots:
  void setTool(int);
  void handleCommand(int command);

  /**
   * Update toolbars/status bars etc.
   */
  void updateControls();
  void updateCommands();

  /**
   * Read the options
   */
  void readOptions();

  /**
   * Write the options
   */
  void writeOptions();

  void enableEditCutCopy(bool);
  void enableEditPaste(bool);

protected:
  // Load/Save files
  bool loadLocal(const char *filename_, const char *url_= 0);
  bool loadRemote(const char *url_= 0);
  bool saveLocal(const char *filename_, const char *url_= 0);
  bool saveRemote(const char *url_);

  /**
   * Should we really?
   */
  int exit();

private:
  // Initialisation
  void initToolbars();
  void initMenus();
  void initStatus();
  void addDefaultColors(ColorBar &);

  /**
   * Catch close events
   */
  void closeEvent(QCloseEvent *);

  /**
   * Update status item
   */
  void canvasSizeChanged();

  // Info about the currently open image
  QString filename; // actual local filename
  QString url; // URL it came from or empty if local
  QString tempURL;
  QString format;
  int zoom;
  bool modified;

  // UI configuation
  bool showStatusBar;
  bool showCommandsToolBar;
  bool showToolsToolBar;

  // Command status
  bool allowEditPalette;

  // Tool manager
  Manager *man;

  //GUIManager
  //  QVBoxLayout *vlayout;

  // Child widgets
  QwViewport *v;
  Canvas *c;

  KToolBar *toolsToolbar;
  KToolBar *commandsToolbar;
  KStatusBar *statusbar;
  KMenuBar *menu;
  MainView *mv;
/* obsolet (jha)
   KAccel *keys; */

  QPixmap *lmbColor, *rmbColor;
  ColorBar *defaultCb;
};

#endif


