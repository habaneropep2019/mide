#ifndef _KGROUPTBL_H
#define _KGROUPTBL_H


#include <qpainter.h>
#include <qpixmap.h>
#include "krowtable.h"
#include "kgroup.h"

class KGroupRow : public KRow {
public:
  KGroupRow(KGroup *kg, QPixmap *pGroup);
  KGroup *getData();

protected:
  KGroup *kg;
  virtual void paint(QPainter *painter, int col, int width);
  QPixmap *pmGroup;
};

class KGroupTable : public KRowTable {
  Q_OBJECT
public:
  KGroupTable(QWidget *parent = NULL, const char *name = NULL);
  ~KGroupTable();

  void setAutoUpdate(bool state);
  void clear();
  void insertItem(KGroup *kg);
  int currentItem();
  void setCurrentItem(int item);
  void sortBy(int num);

private:
  QPixmap *pmGroup;
  int fontpos;
  int current;
  int sort;
};

#endif // _KGROUPTBL_H
