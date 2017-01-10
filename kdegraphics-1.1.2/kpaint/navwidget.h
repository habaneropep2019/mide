// -*- c++ -*-

#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include <qwidget.h>
#include <qpixmap.h>

/**
 * A class to provide navigation information about the current view of
 * the image.
 * @author Richard Moore rich@kde.org
 */
class NavWidget : public QWidget {
  Q_OBJECT
public:
  NavWidget(QWidget *parent, const char *name);
  ~NavWidget();
  void paintEvent(QPaintEvent *);

public slots:
  void sizeChanged();
  void setPixmap(QPixmap *);
  void pixmapChanged();

  /**
   * x, y, w, h
   */
  void viewportChanged(int, int, int, int);

signals:
  void clicked(int, int);
  void scaleChanged(int);
private:
  QPixmap *view;
};

#endif // NAVWIDGET_H
