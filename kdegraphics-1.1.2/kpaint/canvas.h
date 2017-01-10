// -*- c++ -*-

// $Id: canvas.h,v 1.8 1998/10/25 16:06:43 habenich Exp $

#ifndef CANVAS_H
#define CANVAS_H

#include <qwidget.h>
#include <qevent.h>
#include <qpixmap.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qwmatrix.h>

class Tool;

class Canvas : public QWidget
{
Q_OBJECT

public:
Canvas(int width, int height,
       QWidget *parent= 0, const char *name=0);
Canvas(const char *filename,
       QWidget *parent= 0, const char *name=0);
~Canvas();

  QPixmap *pixmap();
  void setPixmap(QPixmap *);
  void setDepth(int);
  inline int getDepth();

  const QRect &selection();
  void setSelection(const QRect&);
  inline bool hasSelection() const;
  void clearSelection();
  QPixmap *selectionData();

  void cut(); // cuts from selection into clipboard
  void copy(); // copys from selection into clipboard
  void paste(); // paste from clipboard into canvas
  static inline bool hasClipboardData();
  static inline const QPixmap *clipboardData();


  QPixmap *zoomedPixmap();
  void setZoom(int);
  int zoom();
  void updateZoomed();
  void resizeImage(int, int);

  bool load(const char *filename= 0, const char *format= 0);
  bool save(const char *filename= 0, const char *format= 0);

  bool isActive();
  bool isModified();
  void clearModified();

  // This controls which tool the events go to (if any)
  void activate(Tool *tool);
  void deactivate();

  // Handle paint events
  void paintEvent(QPaintEvent *e);

  // Redirect events tools can use
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

public slots:
  void markModified();

signals:
  void sizeChanged();
  void pixmapChanged(QPixmap *);
  void modified();
  void selection(bool);
  void clipboard(bool);

protected:
  enum state {
    ACTIVE,
    INACTIVE
  };

  QWMatrix *matrix;
  Tool *currentTool;

  /**
   * Zoomed copy
   */
  QPixmap *zoomed;

  /**
   * Master copy
   */
  QPixmap *pix;

  /**
   * Clipboard Data from Cut/Copy
   */
  static QPixmap *clipboardPix;

  /**
   * % of original size
   */
  int zoomFactor;

  /**
   * Has the pixmap been modified?
   */
  bool modified_;
  state s;
  QRect selection_;
  bool haveSelection_;

  static int inst; // how many instances have we ?
};

int 
Canvas::getDepth()
{ return pix->depth(); }

bool 
Canvas::hasSelection() const
{ return haveSelection_; }

bool 
Canvas::hasClipboardData()
{ return (NULL==clipboardPix)? false : !Canvas::clipboardPix->isNull(); }

const QPixmap *
Canvas::clipboardData()
{ return Canvas::clipboardPix; }

#endif // CANVAS_H
