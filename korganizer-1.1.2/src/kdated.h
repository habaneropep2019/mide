// 	$Id: kdated.h,v 1.16 1999/01/23 05:19:50 pbrown Exp $	

#ifndef _KDATED_H
#define _KDATED_H

#include <qlined.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <kdatepik.h>

#include "dateedit.h"

class KODatePicker: public KDatePicker
{
public:
  KODatePicker(QWidget *parent = 0, QDate qd=QDate::currentDate()) { 
    KDatePicker(parent,qd);
    setWFlags(WStyle_Customize|WStyle_NoBorder);
  }
protected:
  virtual void focusOutEvent(QFocusEvent *)
    { hide(); }
};
	      
class KDateEdit : public QFrame
{
  Q_OBJECT
    
public:
  
  KDateEdit(QWidget *parent=0, const char *name=0);
  virtual ~KDateEdit();
  virtual QSize sizeHint() const;
signals:
  void dateChanged(QDate);

public slots:
  void setDate(QDate date);
  QDate getDate() const;
  void setEnabled(bool on);
  void updateConfig();

protected slots:
  void toggleDatePicker();
 
protected:
  int numFromMonthName(QString name) const;
  QPushButton *dateButton;
  //  QLineEdit *kfEdit;
  DateEdit *kfEdit;
  KODatePicker *datePicker;
  DateValidator::DateFormat fmt;
};

#endif
