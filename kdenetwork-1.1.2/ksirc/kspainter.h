#ifndef KSPAINTER_H
#define KSPAINTER_H

#include <qpainter.h>
#include <qobject.h>
#include <qlist.h>

class PainterState {
public:
  PainterState(QPainter *p);
  ~PainterState();

  inline void resetCol();
  inline void resetAll();
  
  inline void setFGCol(const QColor&);
  inline void setBGCol(const QColor&);

  // Reverse video selectors
  inline void toggleRev();
  inline void toggleSel();

  // Font modifiers
  inline void toggleItalics();
  inline void toggleUnderline();
  inline void toggleBold();
  
private:
  QPainter *painter;
  QColor  qcDefBGCol;
  QColor  qcDefFGCol;

  QFont qfDefFont, qfCurFont;

  bool bSelect, bReverse;
  
};

class KSPainter 
{
 public:
  static void colourDrawText(QPainter *p, int startx, int starty, char *str, int length = -1);
  static const int maxcolour;
  static const QColor num2colour[16];
  static int colour2num(const QColor &colour);
  static QString stripColourCodes(QString col, QList<int> *xlate = 0x0);
  
  static const QColor brown;
  static const QColor orange;
  static const QColor lightCyan;
  static const QColor lightBlue;
  static const QColor pink;
};

#endif
