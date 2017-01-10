/////////////////////////////////////////////////////////////////////////////
//
// functions generating layout-aware widgets
//
/////////////////////////////////////////////////////////////////////////////

#include "newwidget.h"
#include "macros.h"

QLabel *newLabel(const char *text, QWidget *parent) {
  QLabel *l = new QLabel(text, parent);
  l->setMinimumSize(l->sizeHint());
  return l;
}

QRadioButton *newRadioButton(const char *text, QWidget *parent) {
  QRadioButton *l = new QRadioButton(text, parent);
  l->setMinimumSize(l->sizeHint());
  return l;
}

QCheckBox *newCheckBox(const char *text, QWidget *parent) {
  QCheckBox *l = new QCheckBox(text, parent);
  l->setMinimumSize(l->sizeHint());
  return l;
}

QLineEdit *newLineEdit(int visiblewidth, QWidget *parent) {
  QLineEdit *l = new QLineEdit(parent);
  l->setFixedHeight(l->sizeHint().height());
  if(visiblewidth == 0)
    l->setMinimumWidth(l->sizeHint().width());
  else
    l->setFixedWidth(l->fontMetrics().width('H') * visiblewidth);
  
  return l;
}
