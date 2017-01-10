// $Id: palettedialog.h,v 1.3 1998/05/07 13:34:50 rich Exp $

#ifndef PALETTEDIALOG_H
#define PALETTEDIALOG_H

#include <qdialog.h>
#include "palette.h"

class paletteDialog : public QDialog
{
  Q_OBJECT

public:
  paletteDialog(QPixmap *, QWidget *parent= 0, const char *name= 0);
  QPixmap *pixmap();
private:
  paletteWidget *pal;
};


#endif // PALETTEDIALOG_H


