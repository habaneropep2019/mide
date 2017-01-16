/*
    KDE Draw - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#include "debug.h"
#include <qkeycode.h>
#include "kresize.h"

KResize::KResize(QWidget *parent, const char *name, bool m, const QSize s )
 : QDialog( parent, name, m )
{
  setFocusPolicy(QWidget::NoFocus);
  grp = new QGroupBox(i18n("Size:"), this);
  QLabel *label = new QLabel("X", grp);
  x_line = new KIntegerLine(grp);
  connect(x_line, SIGNAL(valueChanged(int)), SLOT(checkValue(int)));
  x_line->setFixedSize(100, label->sizeHint().height()+10);
  y_line = new KIntegerLine(grp);
  connect(y_line, SIGNAL(valueChanged(int)), SLOT(checkValue(int)));
  y_line->setFixedSize(100, label->sizeHint().height()+10);
  label->setFixedSize(label->sizeHint());

  ml = new QVBoxLayout(this, 10);
  ml->addWidget(grp, 10, AlignLeft);
  l0 = new QHBoxLayout(grp, 20);
  l0->addWidget(x_line);
  l0->addWidget(label);
  l0->addWidget(y_line);
  l0->activate();
  if(testWFlags(WType_Modal))
  {
    connect(x_line, SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
    connect(y_line, SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
    ok = new QPushButton(i18n("&OK"), this);
    connect(ok, SIGNAL(clicked()), SLOT(accept()));
    ok->setFixedSize(ok->sizeHint());
    ok->setEnabled(false);
    ok->setDefault(true);
    cancel = new QPushButton(i18n("&Cancel"), this);
    connect(cancel, SIGNAL(clicked()), SLOT(reject()));
    cancel->setFixedSize(cancel->sizeHint());
    QBoxLayout *l1 = new QHBoxLayout();
    ml->addLayout(l1);
    l1->addWidget(ok);
    l1->addWidget(cancel);
    l1->addStretch(1);
    ml->freeze();
    setCaption(i18n("Select size:"));
  }
  else
    ml->activate();

  x_line->setValue(s.width());
  y_line->setValue(s.height());
  checkValue(0);
}

KResize::~KResize()
{
  debug("KResize - destructor");
  debug("KResize - destructor: done");
}

bool KResize::eventFilter(QObject *obj, QEvent *e)
{
  if ( e->type() == Event_KeyPress && obj == this && !testWFlags(WType_Modal))
  {
    QKeyEvent *k = (QKeyEvent*)e;
    if(k->key() == Key_Escape || k->key() == Key_Return || k->key() == Key_Enter)
    {
      debug("KResize: Received keyevent");
      return true;
    }
  }
  return false;
}

const QSize KResize::getSize()
{
  return (QSize(x_line->value(), y_line->value()));
}

void KResize::checkValue(int v)
{
  if(x_line->value() > 0 && y_line->value() > 0)
  {
    //debug("Setting ok");
    if(testWFlags(WType_Modal))
      ok->setEnabled(true);
    emit validsize(true);
  }
  else
  {
    //debug("Setting not ok");
    if(testWFlags(WType_Modal))
      ok->setEnabled(false);
    emit validsize(false);
  }
}

void KResize::slotReturnPressed()
{
  if(x_line->value() > 0 && y_line->value() > 0)
    accept();
}


