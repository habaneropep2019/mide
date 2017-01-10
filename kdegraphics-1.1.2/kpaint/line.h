// -*- c++ -*-

// Line class
// This class is a subclass of Tool and provides a rubber line drawing tool.

// $Id: line.h,v 1.5 1998/10/18 19:59:54 habenich Exp $

#ifndef LINE_H
#define LINE_H

#include <qpixmap.h>
#include "tool.h"

class Line : public Tool
{
  Q_OBJECT
public:
  Line(const char *toolname);
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

#endif // LINE_H

