// -*- c++ -*-

#ifndef MAINVIEW_H
#define MAINVIEW_H


#include <qdialog.h>

class View;
class QwViewport;
class Canvas;
class QBoxLayout;
class SideBar;
class ColorBar;

/**
 * view window for kpaint
 * just created for layout.
 */
class MainView : public QDialog
{
Q_OBJECT
public:
MainView(QWidget *parent=0, const char *name=0);

  inline QwViewport *getViewport() const;
  inline Canvas *getCanvas() const;
  inline SideBar * getSideBar() const;
  inline ColorBar *getDefaultColorBar() const;

private:

  // Child widgets
  View *view;
  QwViewport *v;
  Canvas *c;
  SideBar *side;
  ColorBar *dcb;

  // Layoutmanager
  QBoxLayout *lm;

};

QwViewport *
MainView::getViewport() const
{
  return v;
}

Canvas *
MainView::getCanvas() const
{
  return c;
}

SideBar *
MainView::getSideBar() const
{
  return side;
}

ColorBar *
MainView::getDefaultColorBar() const
{
  return dcb;
}

#endif // MAINVIEW_H
