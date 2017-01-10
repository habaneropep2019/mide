// -*- c++ -*-

#ifndef VIEW_H
#define VIEW_H

#include <qwidget.h>

class QwViewport;
class SideBar;

/**
 * @author Richard Moore rich@kde.org
 */
class View : public QWidget {
  Q_OBJECT
public:
  View(QWidget *parent, const char *name);
  ~View();

  void resizeEvent(QResizeEvent *);
  void updateRects();

  QwViewport *c;
  SideBar *s;
};

#endif // VIEW_H
