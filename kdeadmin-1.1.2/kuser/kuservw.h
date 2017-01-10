#ifndef _KU_KUSERVW_H
#define _KU_KUSERVW_H

#include <qwidget.h>

#include "kheader.h"
#include "kusertbl.h"

class KUserView : public QWidget
{
    Q_OBJECT

public:
  KUserView( QWidget* parent = NULL, const char* name = NULL );

  virtual ~KUserView();

  void clear();
  void insertItem(KUser *aku);
  int currentItem();
  KUser *getCurrentUser();
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
  KUserTable *m_Users;
  int current;
};

#endif // _KU_USERVW_H
