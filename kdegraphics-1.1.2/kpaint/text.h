// -*- c++ -*-


#ifndef TEXT_H
#define TEXT_H

#include "tool.h"
#include <qstack.h>

typedef QStack<int> LineStack;

class Text : public Tool
{
Q_OBJECT
public:
Text();
Text(const char *toolname);

  void mousePressEvent(QMouseEvent *);
  //  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *);
  void keyPressEvent(QKeyEvent *);
  QPixmap *pixmap();

signals:
  //  void modified();

protected:
  virtual void activating();
  virtual void deactivating();
  virtual void timerEvent(QTimerEvent *);

private:
  void paintC(QPainter *p=0);
  void paintBuffer();

  int startx, starty,
    cursorH, cursorW,
    lastx, lasty;
  bool drawing, showingC;
  //  int activeButton;
  QString charBuffer;
  LineStack lineStack;
};

#endif // TEXT_H

