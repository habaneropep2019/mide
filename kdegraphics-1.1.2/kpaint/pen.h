// -*- c++ -*-
// $Id$

#ifndef PEN_H
#define PEN_H

#include <qpixmap.h>
#include <qpainter.h>
#include "tool.h"

class Pen : public Tool
{
  Q_OBJECT
public:
  Pen(const char *toolname);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating();
  QPixmap *pixmap();
signals:
  void modified();
private:
  int lastx;
  int lasty;
  bool drawing;
  int activeButton;
};

#endif // PEN_H

