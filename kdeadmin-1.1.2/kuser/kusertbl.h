#ifndef _KUSERTBL_H
#define _KUSERTBL_H


#include <qpainter.h>
#include <qpixmap.h>
#include "krowtable.h"
#include "kuser.h"

class KUserRow : public KRow
{
public:
  KUserRow( KUser *aku, QPixmap *pUser);
  KUser *getData();

protected:
  KUser *ku;
  virtual void paint( QPainter *painter, int col, int width );
  QPixmap *pmUser;
};

class KUserTable : public KRowTable
{
  Q_OBJECT
public:
  KUserTable( QWidget *parent = NULL, const char *name = NULL );
  ~KUserTable();

  void setAutoUpdate(bool state);
  void clear();
  void insertItem(KUser *aku);
  int currentItem();
  void setCurrentItem(int item);
  void sortBy(int num);

private:
  QPixmap *pmUser;
  int fontpos;
  int current;
  int sort;
};

#endif // _KUSERTBL_H
