#ifndef _KU_GROUPVW_H
#define _KU_GROUPVW_H

#include <qwidget.h>

#include "kheader.h"
#include "kgrouptbl.h"

class KGroupView : public QWidget
{
    Q_OBJECT

public:
  KGroupView( QWidget* parent = NULL, const char* name = NULL );

  virtual ~KGroupView();

  void clear();
  void insertItem(KGroup *aku);
  int currentItem();
  KGroup *getCurrentGroup();
  void setCurrentItem( int item );
  void setAutoUpdate(bool state);
  void sortBy(int num);
  void repaint();

protected:
  virtual void resizeEvent( QResizeEvent *rev );

signals:
  void selected(int item);
  void highlighted(int item);
  void headerClicked(int num);

private slots:
  void onSelect(int row, int);
  void onHighlight(int row, int);
  void onHeaderClicked(int num);

private:
  void init();
  KHeader *m_Header;
  KGroupTable *m_Groups;
  int current;
};

#endif // _KGROUPVW_H
