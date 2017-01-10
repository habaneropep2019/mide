// -*- c++ -*-

// $Id: tool.h,v 1.10 1998/11/04 20:49:16 habenich Exp $

#ifndef TOOL_H
#define TOOL_H

#include <qevent.h>
#include <stdio.h>
#include <assert.h>
#include <qpen.h>
#include <qobject.h>
#include <qbrush.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <qpixmap.h>
#include "canvas.h"

#define DEFAULT_RASTER_OP NotROP

class QPopupMenu;

/**
 * @short Tool - Abstract base class
 * Tool is the parent of all kpaint tools, it provides virtual methods
 * for all of the events that are passed to the current tool by the
 * canvas. The default implementation of all of these methods is to
 * ignore the event. Tools should override those methods to provide
 * functionality.
 * 
 * A tool can be either active or inactive, an inactive tool should
 * receive no events. When a tool is active it can access a pointer
 * to the current canvas which it can use to find the current image
 * in order to draw on it. The activating() method is called when a
 * tool is made active, the deactivating() method is called immediately
 * before the tool becomes inactive.
 */
class Tool : public QObject
{
Q_OBJECT
public:
  /**
   * Page flags
   */
  enum {
    HasLineProperties = 1,
    HasFillProperties = 2,
    HasCustomProperties = 4
  };

  Tool();
  Tool(const char *toolname);
  /**
   * Make the tool active for canvas c
   */
  void activate(Canvas *c);

  /**
   * Make the tool inactive
   */
  void deactivate();
  bool isActive();
  void setLeftPen(QPen &p);
  void setLeftBrush(QBrush &b);
  void setRightPen(QPen &p);
  void setRightBrush(QBrush &b);
  const char *tip() { return tooltip; };
  virtual QPixmap *pixmap();

  /**
   * Used by the properties dialog
   */
  int getPages();
  QWidget *getCustomPage(QWidget *) { return 0; };

  // Event handlers (handle events on canvas)
  virtual void mousePressEvent(QMouseEvent *) {} ;
  virtual void mouseReleaseEvent(QMouseEvent *) {};
  virtual void mouseDoubleClickEvent(QMouseEvent *) {};
  virtual void mouseMoveEvent(QMouseEvent *) {};
  virtual void keyPressEvent(QKeyEvent *) {};
  virtual void keyReleaseEvent(QKeyEvent *) {};
  virtual void enterEvent(QEvent *) {};
  virtual void leaveEvent(QEvent *) {};

  virtual const QPopupMenu *getMenu() { return NULL; };

  int id;
  inline const char *getName() const;

signals:
  void modified();

private:
  bool active;

protected:
  QPixmap *pix;
  QString name;
  const char *tooltip;

  /**
   * Current canvas if active
   */
  Canvas *canvas;

  /**
   * Called by activate()
   */
  virtual void activating() {};

  /**
   * Called by deactivate()
   */
  virtual void deactivating() {};

  QPen leftpen;
  QBrush leftbrush;
  QPen rightpen;
  QBrush rightbrush;

  /**
   * Which pages are supported
   */
  unsigned int props;
};

const char *
Tool::getName() const
{
  return (const char *)name;
}

#endif // TOOL_H

