// 	$Id: catdlg.h,v 1.5 1999/01/22 19:52:02 pbrown Exp $

#ifndef _CATDLG_H
#define _CATDLG_H

#include <qdialog.h>

class QStrList;
class KButtonBox;
class QListBox;
class QPushButton;
class QLineEdit;

class CategoryDialog : public QDialog
{
  Q_OBJECT

public:

  CategoryDialog(QWidget* parent = 0L,const char* name = 0L);
  virtual ~CategoryDialog();

public slots:
  void setSelected(QStrList selList);

protected slots:
  void accept();
  void addCat();
  void removeCat();

signals:
  void okClicked(QString);

protected:
  QListBox* catListBox, *selCatListBox;
  QLineEdit *catEdit;
  QStrList catList;

  KButtonBox *mainButtonBox, *midButtonBox;
  QPushButton *addButton, *removeButton;
  QPushButton *okButton, *cancelButton;
  
};

#endif
