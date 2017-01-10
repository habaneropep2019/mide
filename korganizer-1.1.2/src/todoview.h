#ifndef TODOVIEW_H
#define TODOVIEW_H

#include <ktablistbox.h>
#include <qlined.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qlistbox.h>
#include <qpopmenu.h>

#include "calobject.h"
#include "kdpevent.h"

class TodoView : public KTabListBox
{
  Q_OBJECT

public:
  TodoView(CalObject *, QWidget *parent=0, const char *name=0);
  virtual ~TodoView();
  
public slots:
  void updateView(); 
  void updateConfig(); 
  KDPEvent *getSelected() { return aTodo; };
  void changeEventDisplay(KDPEvent *which, int action);

protected slots:
  void updateItem(int, int);
  void changeSummary(const char *newsum);
  void updateSummary();
  void changePriority(int pri);
  void lowPriority() { changePriority(3); };
  void mediumPriority() { changePriority(2); };
  void highPriority() { changePriority(1); };
  void doPopup(int, int);
  void newTodo();
  void editTodo();
  void cleanWindow(QWidget *);
  void deleteTodo();
  void purgeCompleted();
  void sortPriority();
  void sortDescription();
  void sortCompleted();
  void headerAction(int);
  void hiliteAction(int, int);
  void showDates(bool);
  inline void showDates();
  inline void hideDates();

signals:
  void editEventSignal(KDPEvent *);

protected:
  enum { PRIORITY, DESCRIPTION, COMPLETED, DUEDATE };
  QStrList makeStrList(int sortOrder);
  void insortItem(QString insStr, QStrList *insList, int sortOrder);
  bool eventFilter(QObject *o, QEvent *event);
  void doMouseEvent(QMouseEvent *);
  void adjustColumns();

  bool editingFlag;
  int prevRow, updatingRow;
  QString tmpSummary;
  QLineEdit *editor;
  QListBox *priList;
  QPopupMenu *rmbMenu1;
  QPopupMenu *rmbMenu2;
  KDPEvent *aTodo;
  CalObject *calendar;
};

#endif
