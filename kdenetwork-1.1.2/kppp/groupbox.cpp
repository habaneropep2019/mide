/////////////////////////////////////////////////////////////////////////////
//
// A QGroupBox like box with title, optional a checkbox and a peer window 
// inside
//
/////////////////////////////////////////////////////////////////////////////

#include "groupbox.h"
#include <qlabel.h>

#define BORDER_X 6
#define BORDER_Y 6


KGroupBoxBase::KGroupBoxBase(QWidget *parent, const char *name) :
  QFrame(parent, name),
  _title(0)
{
  setFrameStyle(Box | Sunken);
  _peer = new QWidget(this);
}


QSize KGroupBoxBase::minimumSize() const {
  return QSize(0, 0);
}


QSize KGroupBoxBase::maximumSize() const {
  return QSize(1 << 15, 1 << 15);
}


QSize KGroupBoxBase::sizeHint() const {
  return minimumSize();
}


void KGroupBoxBase::resizeEvent(QResizeEvent *) {
  initBox();
}


void KGroupBoxBase::initBox() {
  QRect peergeo;

  frect = QRect(BORDER_X, BORDER_Y, 
		width() - 2*BORDER_X, height() - 2*BORDER_Y);
  peergeo = QRect(frect.left() + 6,
		  frect.top() + 6,
		  frect.width() - 12 - 1,
		  frect.height() - 12 - 1);

  // adjust frame to fit title
  if(_title != 0) {
    frect.setTop(frect.top() + _title->minimumSize().height()/2);
    peergeo.setTop(peergeo.top() + _title->minimumSize().height());
    _title->resize(_title->minimumSize().width() + 8,
		   _title->minimumSize().height());
    _title->move(peergeo.left() + 6,
		 frect.top() - _title->minimumSize().height()/2);
  }

  setFrameRect(frect);
  _peer->setGeometry(peergeo);
}


void KGroupBoxBase::setTitleWidget(QWidget *title) {
  if(_title)
    delete _title;
  
  _title = title;
  initBox();  
}


KGroupBox::KGroupBox(const char *title, QWidget *parent, const char *name) :
  KGroupBoxBase(parent, name)
{
  setTitle(title);
}


void KGroupBox::setTitle(const char *name) {
  QLabel *l = new QLabel(name, this);
  l->setMinimumSize(l->sizeHint());
  l->setAlignment(AlignCenter);
  setTitleWidget(l);
}


KCheckGroupBox::KCheckGroupBox(const char *title, QWidget *parent, const char *name) :
  KGroupBoxBase(parent, name)
{
  setTitle(title);
}


void KCheckGroupBox::setTitle(const char *name) {
  cbox = new QCheckBox(name, this);
  cbox->setMinimumSize(cbox->sizeHint());
  connect(cbox, SIGNAL(toggled(bool)),
	  this, SLOT(slot_toggled(bool)));
  setTitleWidget(cbox);  
}


void KCheckGroupBox::slot_toggled(bool b) {
  emit toggled(b);
}


void KCheckGroupBox::setChecked(bool b) {
  cbox->setChecked(b);
}


bool KCheckGroupBox::isChecked() {
  return cbox->isChecked();
}


KRadioGroupBox::KRadioGroupBox(const char *title, QWidget *parent, const char *name) :
  KGroupBoxBase(parent, name)
{
  setTitle(title);
}


void KRadioGroupBox::setTitle(const char *name) {
  rb = new QRadioButton(name, this);
  rb->setMinimumSize(rb->sizeHint());
  connect(rb, SIGNAL(toggled(bool)),
	  this, SLOT(slot_toggled(bool)));
  setTitleWidget(rb);
}


void KRadioGroupBox::slot_toggled(bool b) {
  emit toggled(b);
}


bool KRadioGroupBox::isChecked() {
  return rb->isChecked();
}


#include "groupbox.moc"
