#ifndef KU_DATECTL_H
#define KU_DATECTL_H

//#include "includes.h"
#include <qobject.h>
#include <qlined.h>
#include <qlabel.h>
#include <qchkbox.h>
#include <qfont.h>

#include <kcombo.h>

class KDateCtl: public QObject
{
  Q_OBJECT
public:
  KDateCtl(QWidget *parent, const char *name, const char *checkTitle,
	   const char *title, long adate, long abase, int ax, int ay);
  ~KDateCtl();
  void setDate(long adate, long abase);
  long getDate();	
  long getBase();	
  void setFont(const QFont &f);

protected:
  int x;
  int y;

  int iday;
  int imonth;
  int iyear;
  long base;
  
  QLineEdit *day;
  KCombo *month;
  QLineEdit *year;
  QCheckBox *isempty;
  QLabel *label;
  
  void updateControls();

protected slots:
  void isEmptyToggled(bool);
  void dayChanged(const char *text);
  void monthChanged(int);
  void yearChanged(const char *);

signals:
  void textChanged();
};

#endif // K_DATECTL_H
