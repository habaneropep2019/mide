// -*- c++ -*-

#ifndef COLORBAR_H
#define COLORBAR_H

//#include <qwidget.h>
#include <qlist.h>
#include <qframe.h>
#include <qbutton.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>

class QButton;
class QButtonGroup;
class QHBoxLayout;

typedef QList<QRgb> RgbList;
typedef QList<QButton> ButtonList;


class ColorButton : public QToolButton
{
Q_OBJECT
public:
ColorButton(QWidget *parent=0, const char *name=0);

protected:
   virtual void drawButtonLabel(QPainter *);

private:


};



/**
 * Draws a horizontal bar with color rectangulars.
 * Has a "previous" and a "next" pointer at each end of the bar to
 * look at more colors
 * left buton emits a selection signal
 * right button eventually a menu
 */
class ColorBar : public QFrame
{
Q_OBJECT
public:
  ColorBar(QWidget *parent=0, int fixheight=-1, const char *name=0);
  ~ColorBar();

  void addColor(const QColor &color);
  void addColor(const QRgb rgb);

signals:
   void selected(QRgb rgb);

protected:
  //  virtual void paintEvent(QPaintEvent *);
  //  virtual void drawContents(QPainter *);

private slots:
  void slotClicked(int id);

private:
  //  RgbList rgbList;
  ButtonList buttonList;
  QButtonGroup *buttonGroup;
  //  QHBoxLayout *layout;
  int lastid, bcount;
};


#endif // COLORBAR_H
