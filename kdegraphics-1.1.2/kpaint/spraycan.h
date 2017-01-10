// -*- c++ -*-

// $Id: spraycan.h,v 1.5 1998/10/18 20:00:06 habenich Exp $

#ifndef SPRAYCAN_H
#define SPRAYCAN_H

#include <qpixmap.h>
#include <qpainter.h>
#include <qtimer.h>
#include "tool.h"

class SprayCan : public Tool
{
  Q_OBJECT
public:
  SprayCan(const char *toolname);
  ~SprayCan();
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void activating();
  QPixmap *pixmap();
signals:
  void modified();
private:
  int density; // Dots per second (approx)
  QTimer *timer;
  int x, y;
  int brushsize;
  bool drawing;
  int activeButton;
private slots:
  void drawDot();
};


#endif // SPRAYCAN_H
