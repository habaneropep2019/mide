/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

// KSVGui Preferences Dialog

#include <qlabel.h>
#include <qpushbt.h>
#include <qlayout.h>
#include <kspinbox.h>
#include <kbuttonbox.h>
#include "ksv_core.h"
#include "PropDlg.h"

// include meta-object code
#include "PropDlg.moc"

#define HALF 5
#define FULL 2*HALF
#define MINIMAL 1
#define MIN_SIZE(A) A->setMinimumSize(A->sizeHint())

KSVPropDlg::KSVPropDlg( KSVDragData* data, QWidget* parent, char* name)
  : QDialog(parent, name, TRUE)
{
  QBoxLayout* topLayout = new QVBoxLayout( this, HALF );

  QBoxLayout* content = new QVBoxLayout( HALF );
  topLayout->addLayout(content, FULL);

  // add the "content" widgets
  QLabel* name_l = new QLabel(i18n("&Name"), this);
  MIN_SIZE(name_l);
  QLabel* target_l = new QLabel(i18n("&Points to"), this);
  MIN_SIZE(target_l);
  QLabel* number_l = new QLabel(i18n("&Sorting number"), this);
  MIN_SIZE(number_l);

  name_le = new QLineEdit( this );
  MIN_SIZE(name_le);
  target_le = new QLineEdit( this );
  MIN_SIZE(target_le);
  number_il = new QSpinBox(0, 99, 1, this, "Sorting Number");
  number_il->setWrapping(true);
  name_l->setBuddy(name_le);
  target_l->setBuddy(target_le);
  number_l->setBuddy(number_il);

  content->addWidget(name_l);
  content->addWidget(name_le, MINIMAL);
  content->addWidget(target_l);
  content->addWidget(target_le, MINIMAL);
  content->addWidget(number_l);

  QBoxLayout* numberLayout = new QHBoxLayout( HALF );
  content->addLayout(numberLayout, MINIMAL);

  number_il->setRange(0, 99);
  MIN_SIZE(number_il);
  number_il->setMinimumWidth(50);

  numberLayout->addWidget(number_il, MINIMAL);
  numberLayout->addStretch(FULL);

  content->addStretch(FULL);

  // add ok and cancel buttons
  KButtonBox* buttons = new KButtonBox(this);
  buttons->addStretch(FULL);
  QPushButton* ok_bt = buttons->addButton(i18n("OK"));
  QPushButton* cancel_bt = buttons->addButton(i18n("Cancel"));
  buttons->layout();

  topLayout->addWidget(buttons);

  topLayout->activate();

  // some button/dialog stuff
  ok_bt->setAutoDefault(TRUE);
  cancel_bt->setAutoDefault(TRUE);
  ok_bt->setDefault(TRUE);

  connect(ok_bt, SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancel_bt, SIGNAL(clicked()), this, SLOT(reject()));

  // set content
  datum = data;
  name_le->setText(datum->currentName());
  target_le->setText(datum->filename());
  number_il->setValue(datum->number());

  // set caption to "Runlevel X: foobar - Properties"
  setCaption(QString().sprintf(i18n("%s - Properties"), datum->currentName().data()));

  // make fixed size... I don´t think we need the extra space
  topLayout->freeze(minimumSize().width() * 1.3f, minimumSize().height() * 1.1f);
}

// Destructor

KSVPropDlg::~KSVPropDlg() {

}

void KSVPropDlg::accept() {
  bool _changed = false;

  if (datum->currentName() != name_le->text())
    {
      datum->setName(name_le->text());
      _changed = true;
    }

  if (datum->number() != number_il->value())
    {
      datum->setNumber(number_il->value());
      _changed = true;
      
      emit numberChanged(); // resort
    }

  if (datum->filename() != target_le->text())
    {
      datum->setFilename(target_le->text());
      _changed = true;
    }

  if (_changed)
    emit changed(datum);

  QDialog::accept();
}

void KSVPropDlg::setEnabled( bool val )
{
  name_le->setEnabled(val);
  number_il->setEnabled(val);
  target_le->setEnabled(val);
}
