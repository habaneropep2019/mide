#include <kdebug.h>
#include <qpushbt.h>
#include <klocale.h>
#include <kapp.h>
#include "palettedialog.h"

paletteDialog::paletteDialog(QPixmap *p, QWidget *parent, const char *name)
  : QDialog(parent, name, TRUE)
{
  QPushButton *dismiss;
  pal= new paletteWidget(p, this);

  resize(340, 280);
  dismiss = new QPushButton( klocale->translate("Dismiss"), this );
  dismiss->setAutoResize(TRUE);
  pal->move(10, 10);
  pal->resize(320, 240);
  dismiss->move(140, 250);
  connect( dismiss, SIGNAL(clicked()), SLOT(accept()) );
  connect( pal, SIGNAL(colourSelected(int)), pal, SLOT(editColour(int)) );
}

QPixmap *paletteDialog::pixmap()
{
  return pal->pixmap();
}

#include "palettedialog.moc"
