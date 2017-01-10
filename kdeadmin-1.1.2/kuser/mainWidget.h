#ifndef _KU_MAINWIDGET_H
#define _KU_MAINWIDGET_H

#include <ktmainwindow.h>
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <kmenubar.h>
#include <qevent.h>
#include <qpixmap.h>

class mainWidget : public KTMainWindow {
Q_OBJECT
public:
  mainWidget(const char *name = 0);
  ~mainWidget();

protected:
  virtual void resizeEvent (QResizeEvent *event);
  virtual void closeEvent (QCloseEvent *event);

private:
  KMenuBar   *menubar;
  KToolBar   *toolbar;
  KStatusBar *sbar;
};

#endif // _KU_MAINWIDGET_H
