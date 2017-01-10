#ifndef _KINPUTLINE_H_
#define _KINPUTLINE_H_


#include <qdialog.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>


class KInputLine : public QDialog
{
  Q_OBJECT
  
public:

  KInputLine(QWidget *parent=0, QString text="", QString init="");
  ~KInputLine() {};

  const char* getText() { return lineedit->text(); };
  
private:

  QLabel *label;
  QLineEdit *lineedit;
  QPushButton *ok, *cancel;          
};


#endif