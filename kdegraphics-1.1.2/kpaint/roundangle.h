// -*- c++ -*-

#ifndef ROUNDANGLE_H
#define ROUNDANGLE_H

#include <qpixmap.h>
#include "tool.h"

class Roundangle : public Tool
{
  Q_OBJECT
public:
  Roundangle(const char *toolname);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating();
  QPixmap *pixmap();
signals:
  void modified();
private:
  int startx, starty;
  int lastx, lasty;
  bool drawing;
  int activeButton;
};

#endif // ROUNDANGLE_H

