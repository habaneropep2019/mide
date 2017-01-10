// dialogs.C
//
// This program is free software. See the file COPYING for details.
// Author: Mattias Engdegård, 199

#include "dialogs.h"

#include <qaccel.h>
#include <qlayout.h>
#include <kslider.h>
#include <kapp.h>
#include <kmsgbox.h>
#include <kbuttonbox.h>

const int MessageDialog::button_minwidth = 64;
const int MessageDialog::margin_top = 16;		// above text
const int MessageDialog::margin_middle = 16;	// between text and button
const int MessageDialog::margin_bottom = 16;	// below button
const int MessageDialog::margin_side = 32;		// to the left and right of text


// ValueDialog: modal dialog to input a single string value

ValueDialog::ValueDialog(const char *caption, const char *msg,
			 const char *ed_txt)
            : QDialog(kapp->mainWidget(), caption, TRUE)
{
    setCaption(caption);
    QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
    QHBoxLayout *l1 = new QHBoxLayout;
    tl->addLayout(l1, 1);
    
    label = new QLabel(msg, this);
    label->setMinimumSize(label->sizeHint());
    l1->addWidget(label);

    lined = new QLineEdit(this);
    lined->setMinimumWidth(lined->sizeHint().width()/2);
    lined->setFixedHeight(lined->sizeHint().height());
    lined->setMaxLength(7);
    lined->setText(ed_txt);
    lined->selectAll();    
    lined->setFocus();
    l1->addWidget(lined, 1);

    tl->addSpacing(10);

    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    ok = bbox->addButton(i18n("OK"));
    ok->setDefault(TRUE);
    connect(ok, SIGNAL(clicked()), SLOT(done_dialog()));

    cancel = bbox->addButton(i18n("Cancel"));
    connect(cancel, SIGNAL(clicked()), SLOT(reject()));
    bbox->layout();
    tl->addWidget(bbox);

    connect(lined, SIGNAL(returnPressed()), SLOT(done_dialog()));
    QAccel *acc = new QAccel(this);
    acc->connectItem(acc->insertItem(Key_Escape), this, SLOT(reject()));

    tl->freeze();
}

void ValueDialog::done_dialog()
{
    ed_result = lined->text();
    ed_result = ed_result.stripWhiteSpace();
    accept();
}

// MessageDialog: modal dialog that just gives a message

MessageDialog::MessageDialog(QWidget *parent)
             : QDialog(parent, 0, TRUE)
{
    label = new QLabel(this);
    label->setAlignment(AlignCenter);
    icon = 0;			// no icon by default
    button = new QPushButton(i18n("OK"), this);
    connect(button, SIGNAL(clicked()), SLOT(accept()));
}

void MessageDialog::setText(const char *text)
{
    label->setText(text);
}

void MessageDialog::setButtonText(const char *buttonText)
{
    button->setText(buttonText);
}

void MessageDialog::setIcon(QPixmap *pixmap)
{
    if(pixmap) {
	if(!icon) {
	    icon = new QLabel(this);
	    label->setAlignment(AlignLeft | AlignVCenter);
	}
	icon->setPixmap(*pixmap);
    }
}

void MessageDialog::adjustSize()
{
    button->adjustSize();
    // don't let the button be too narrow
    if(button->width() < button_minwidth)
	button->resize(button_minwidth, button->height());
    button->setDefault(TRUE);	// do this _after_ adjusting button size

    label->adjustSize();
    if(icon)
	icon->adjustSize();

    int label_x;
    // give generous margins in all directions
    if(icon) {
	icon->move(margin_side, margin_top);
	label_x = margin_side + icon->width() + margin_side;
    } else {
	label_x = margin_side;
    }
    label->move(label_x, margin_top);
    button->move(label_x + (label->width() - button->width()) / 2,
		 margin_top + label->height() + margin_middle);
    resize(label_x + label->width() + margin_side,
	   button->y() + button->height() + margin_bottom);
}

// static function: pop up a simple modal message box
void MessageDialog::message(const char *caption, const char *text,
			    const char *buttonText,
			    QPixmap *icon)
{
  (void)buttonText;

  int type = 0;
  if(icon == warningIcon())
    type = KMsgBox::STOP;

  KMsgBox::message(kapp->mainWidget(),
		   caption,
		   text,
		   type);
}

QPixmap *MessageDialog::warn_icon = 0;

// static function: return a warning icon, for use in alert boxes
QPixmap *MessageDialog::warningIcon()
{
//     if(!warn_icon)
// 	warn_icon = new QPixmap((const char**)warn_xpm);
    return warn_icon;
}

SliderDialog::SliderDialog(const char *caption, const char *msg,
			   int defaultval, int minval, int maxval)
            : QDialog(kapp->mainWidget(), caption, TRUE)
{
    setCaption(caption);
    QVBoxLayout *tl = new QVBoxLayout(this, 10, 10);
    QHBoxLayout *l1 = new QHBoxLayout;
    tl->addLayout(l1, 1);
    
    label = new QLabel(msg, this);
    label->setMinimumSize(label->sizeHint());
    l1->addWidget(label);

    lined = new QLineEdit(this);
    lined->setMinimumWidth(lined->sizeHint().width()/2);
    lined->setFixedHeight(lined->sizeHint().height());
    lined->setMaxLength(7);

    QString s;
    s.setNum(defaultval);
    lined->setText(s.data());
    lined->selectAll();    
    lined->setFocus();
    l1->addWidget(lined, 1);
    
    QVBoxLayout *l2 = new QVBoxLayout;
    tl->addLayout(l2);
    slider = new KSlider(minval, maxval, 1, defaultval, KSlider::Horizontal,
			 this);
    slider->setTickInterval(10);
    slider->setFixedHeight(slider->sizeHint().height());
    slider->setMinimumWidth(200);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slider_change(int)));
    l2->addWidget(slider);

    QHBoxLayout *l21 = new QHBoxLayout;
    l2->addLayout(l21);

    // decorate slider
    QLabel *left = new QLabel(this);
    QLabel *mid = new QLabel(this);
    QLabel *right = new QLabel(this);
    left->setNum(minval);
    mid->setNum((minval + maxval) / 2);
    right->setNum(maxval);

    left->setFixedSize(left->sizeHint());
    mid->setFixedSize(mid->sizeHint());
    right->setFixedSize(right->sizeHint());
    l21->addWidget(left);
    l21->addStretch(1);
    l21->addWidget(mid);
    l21->addStretch(1);
    l21->addWidget(right);

    tl->addSpacing(10);

    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    ok = bbox->addButton(i18n("OK"));
    ok->setDefault(TRUE);
    connect(ok, SIGNAL(clicked()), SLOT(done_dialog()));

    cancel = bbox->addButton(i18n("Cancel"));
    connect(cancel, SIGNAL(clicked()), SLOT(reject()));
    bbox->layout();
    tl->addWidget(bbox);

    connect(lined, SIGNAL(returnPressed()), SLOT(done_dialog()));
    QAccel *acc = new QAccel(this);
    acc->connectItem(acc->insertItem(Key_Escape), this, SLOT(reject()));

    tl->freeze();
}

void SliderDialog::slider_change(int val)
{
    QString s;
    s.setNum(val);
    lined->setText(s);
    lined->selectAll();
}

void SliderDialog::done_dialog()
{
    ed_result = lined->text();
    ed_result = ed_result.stripWhiteSpace();
    accept();
}
