// -*- c++ -*-
// $Id: ellipse.h,v 1.5 1998/10/18 19:59:48 habenich Exp $

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <qpixmap.h>
#include <qpainter.h>
#include "tool.h"

class Ellipse : public Tool
{
  Q_OBJECT
public:
  Ellipse();
  Ellipse(const char *toolname);
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
  int activeButton;
  bool drawing;
};

#endif // ELLIPSE_H

