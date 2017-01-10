#include <kdebug.h>
#include <qpen.h>
#include <qbrush.h>
#include <kcolordlg.h>
#include <klocale.h>
#include <kapp.h>
#include <stdio.h>
#include "palette.h"

paletteWidget::paletteWidget(QPixmap *p, QWidget *parent, const char *name)
  : QTableView(parent, name)
{
  img= NULL;
  setPixmap(p);

  setTableFlags( Tbl_autoScrollBars | Tbl_cutCells | Tbl_snapToGrid );
  setCellWidth(20);
  setCellHeight(15);
}

paletteWidget::paletteWidget(QWidget *parent, const char *name)
  : QTableView(parent, name)
{
  setTableFlags( Tbl_autoScrollBars | Tbl_cutCells | Tbl_snapToGrid );
  image= NULL;
  img= NULL;
  numColours= 0;
}

void paletteWidget::setPixmap(QPixmap *p)
{
  int d;

  image= p;

  if (img != NULL)
    delete img;

  img= new QImage();

  *img= image->convertToImage();
  numColours= img->numColors();
  
  d= image->depth();

  if (d != 8) {
KDEBUG(KDEBUG_INFO, 3000, "paletteWidget: Invalid pixmap depth\n");
    numColours= 0;
    image= NULL;
    return;
  }

  setNumRows(16);
  setNumCols(16);
}

QPixmap *paletteWidget::pixmap()
{
  if (!image)
    return NULL;

  return image;
}

void paletteWidget::paintCell(QPainter *p, int row, int col)
{
  int i;

  i= row*numCols()+col;

  p->setPen(QPen(black));
  p->drawRect(0,0, cellWidth()-1, cellHeight()-1);

  if (i < numColours) {
    p->setPen(QPen(img->color(i)));
    p->setBrush(QBrush(img->color(i)));
    p->drawRect(1,1, cellWidth()-2, cellHeight()-2);
  }
  else {
    p->setPen(QPen(darkGray));
    p->setBrush(QBrush(darkGray));
    p->drawRect(1,1, cellWidth()-2, cellHeight()-2);
  }
}

void paletteWidget::mousePressEvent(QMouseEvent *e)
{
  int i;

  // Convert coordinates to colour index
  i= findRow(e->y())*numCols()+findCol(e->x());

KDEBUG1(KDEBUG_INFO, 3000, "paletteWidget:: Colour Selected (%d)\n", i);

  emit colourSelected(i);
}

void paletteWidget::editColour(int index)
{
  KColorDialog dlg(this, klocale->translate("Edit Colour"), TRUE);

  dlg.setColor(img->color(index));
  dlg.exec();
  img->setColor(index, dlg.color().rgb());
  image->convertFromImage(*img);
  repaint(0);
}


#include "palette.moc"

