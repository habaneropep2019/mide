// $Id$

#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "KStatusbar.h"

class StatusBar : public KStatusBar {
  // Q_OBJECT

public:
  StatusBar(QWidget *parent= 0, const char *name= 0);

  // public slots:
  void setMessage(const char *message);


protected:
  void drawContents(QPainter *);

private:
  QString *mess; // Mesage area for menu hints etc.
  QWidget *pal;  // Small palette widget
  QString *info; // Image size/co-ords?

}

#endif // STATUSBAR_H
