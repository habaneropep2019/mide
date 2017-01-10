#include <kapp.h>

#include "kinputline.h"
#include "kinputline.moc"


KInputLine::KInputLine(QWidget *parent, QString text, QString init) 
  : QDialog(parent,"",TRUE)
{
  int width = 200;

  label = new QLabel(text, this);
  label->adjustSize();
  
  if (label->width() > width)
    width = label->width();

  ok = new QPushButton(kapp->i18n("&OK"), this);
  cancel = new QPushButton(kapp->i18n("&Cancel"), this);
  ok->adjustSize();
  cancel->adjustSize();
  
  int btn_width = ok->width();
  if (cancel->width() > btn_width)
    btn_width = cancel->width();

  if (8+2*btn_width > width)
    width = 8+2*btn_width;

  lineedit = new QLineEdit(this);
  lineedit->setGeometry(8,label->height()+16, width, lineedit->height());
  lineedit->setText(init);

  label->move((width - label->width())/2, 8);    

  int btn_height = 24+label->height()+lineedit->height();
  ok->setGeometry(width/2 - btn_width - 8, btn_height+8, btn_width, ok->height());
  cancel->setGeometry(width/2 + 8, btn_height+8, btn_width, ok->height());

  ok->setDefault(true);

  connect(ok, SIGNAL(pressed()), this, SLOT(accept()));
  connect(cancel, SIGNAL(pressed()), this, SLOT(reject()));

  adjustSize();
  setFixedSize(this->width(), this->height());
}
