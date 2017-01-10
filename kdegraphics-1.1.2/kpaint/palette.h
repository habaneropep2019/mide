#ifndef PALETTE_H
#define PALETTE_H

/*
** This file contains a widget which takes a qpixmap and displays the
** palette of the widget in a table. This not possible for images that
** use true colour so a message informing the user off his mistake is
** displayed instead.


Convert the pixmap to an image
Check the colour depth and report the error if the image is true colour or a
different error if the image is a bitmap.
Draw a grid of squares of colour

NOTES
-----

Use a table view
Have a signal that says which colour index has been selected (so you can use
it as selector for the colours).
*/


#include <qtablevw.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qevent.h>

class paletteWidget : public QTableView
{
  Q_OBJECT

 public:
   paletteWidget(QPixmap *, QWidget *parent= 0, const char *name= 0);
   paletteWidget(QWidget *parent= 0, const char *name= 0);

   void paintCell(QPainter *p, int row, int col);

   void setPixmap(QPixmap *);
   QPixmap *pixmap();
   QImage *img;
   void mousePressEvent(QMouseEvent *e);

   // setColour(int index, QColor c)

 public slots:
   void editColour(int);

signals:
   void colourSelected(int);
   
 private:
   QPixmap *image;
   int numColours;
};

#endif // PALETTE_H

