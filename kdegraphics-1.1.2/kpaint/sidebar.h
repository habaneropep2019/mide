// -*- c++ -*-

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <qwidget.h>
#include <qframe.h>
#include "navwidget.h"

class Canvas;

/**
 * @author Richard Moore rich@kde.org
 */
class SideBar : public QFrame {
  Q_OBJECT
public:
  SideBar(Canvas *c, QWidget *parent, const char *name);
  ~SideBar();

signals:
  void lmbColourChanged(const QColor &);
  void rmbColourChanged(const QColor &);

public slots:
  void pixmapChanged();
private:
  NavWidget *nav;
};

#endif // SIDEBAR_H
