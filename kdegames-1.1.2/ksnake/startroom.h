#ifndef STARTROOM_H
#define STARTROOM_H

#include <qpushbt.h>
#include <qframe.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>

#include "lcdrange.h"

class StartRoom : public QDialog
{
    Q_OBJECT
public:
    StartRoom( int init, int *newRoom, QWidget *parent=0, const char *name=0 );
private slots:
  void ok();
  void loadPixmap(int);
private:
  QLabel       *picture;
  LCDRange     *roomRange;
  int *nr;
};

#endif // STARTROOM_H
