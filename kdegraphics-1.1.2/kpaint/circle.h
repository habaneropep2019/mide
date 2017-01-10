// -*- c++ -*-
// $Id: circle.h,v 1.5 1998/10/18 19:59:42 habenich Exp $

#ifndef CIRCLE_H
#define CIRCLE_H

#include "tool.h"

class Circle : public Tool
{
  Q_OBJECT
public:
  Circle();
  Circle(const char *toolname);
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

#endif // CIRCLE_H

