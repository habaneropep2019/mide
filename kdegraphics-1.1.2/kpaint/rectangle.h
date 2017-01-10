// -*- c++ -*-

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <qpixmap.h>
#include "tool.h"

class Rectangle : public Tool
{
  Q_OBJECT
public:
  Rectangle(const char *toolname);
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

#endif // RECTANGLE_H

