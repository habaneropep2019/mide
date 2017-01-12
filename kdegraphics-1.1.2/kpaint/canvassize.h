// $Id$

#ifndef CANVASSIZE_H
#define CANVASSIZE_H

#include <qdialog.h>
#include <qlabel.h>
#include <qlined.h>
#include <qpushbt.h>

class canvasSizeDialog : public QDialog
{
public:
  canvasSizeDialog(QWidget *parent=0, const char *name= 0);
  int getWidth();
  int getHeight();
private:
  QLabel *widthLabel;
  QLabel *heightLabel;
  QLineEdit *widthEdit;
  QLineEdit *heightEdit;
   QPushButton *okButton;
   QPushButton *cancelButton;
};

#endif // CANVASSIZE_H
