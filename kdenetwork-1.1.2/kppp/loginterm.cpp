/*
 *              kPPP: A pppd Front End for the KDE project
 *
 * $Id: loginterm.cpp,v 1.8.2.1 1999/03/11 15:15:07 porten Exp $
 *
 *              Copyright (C) 1997-98 Bernd Johannes Wuebben
 *                      wuebben@math.cornell.edu
 *
 * This file was contributed by Harri Porten <porten@tu-harburg.de>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "loginterm.h"
#include "main.h"
#include "modem.h"

#include <stdio.h>
#include <qlayout.h>
#include <qpushbt.h>
#include <qapp.h>

extern KPPPWidget *p_kppp;

LoginMultiLineEdit::LoginMultiLineEdit(QWidget *parent, const char *name)
  : QMultiLineEdit(parent, name)
{
}


LoginMultiLineEdit::~LoginMultiLineEdit() {
  Modem::modem->stop();
}


void LoginMultiLineEdit::insertChar(unsigned char c) {
  QMultiLineEdit::insertChar(c);
  p_kppp->debugwindow->addChar(c);
}


void LoginMultiLineEdit::myreturn() {
  QMultiLineEdit::home();
}


void LoginMultiLineEdit::mynewline() {
  QMultiLineEdit::end(FALSE);
  QMultiLineEdit::newLine();

    p_kppp->debugwindow->addChar('\n');
}


void LoginMultiLineEdit::keyPressEvent(QKeyEvent *k) {
  unsigned char c = (unsigned char) k->ascii();

  if ((int)c == 0) return;

  if((int)c == 13)
    Modem::modem->writeLine("");
  else
    Modem::modem->writeChar(c);
}


void LoginMultiLineEdit::readChar(unsigned char c) {

  if(((int)c != 13) && ((int)c != 10) && ((int)c != 8))
    insertChar(c);      

  if((int)c == 8)
    backspace();
  if((int)c == 127)
    backspace();
  if((int)c == 10)
    mynewline();
  if((int)c == 13)
    myreturn();
}


LoginTerm::LoginTerm (QWidget *parent, const char *name)
  : QDialog(parent, name, FALSE)
{
  setCaption("Login Terminal Window");
  setMinimumSize(300, 200);
  setMaximumSize(600, 400);
  resize(400, 300);

  QVBoxLayout *tl = new QVBoxLayout(this, 2);
  QGridLayout *vgr = new QGridLayout(2, 1);
  QGridLayout *hgr = new QGridLayout(1, 2, 30);

  tl->addLayout(vgr);
  vgr->addLayout(hgr, 1, 0);
  vgr->setRowStretch(0, 1);
  vgr->addRowSpacing(1, 40);

  text_window = new LoginMultiLineEdit(this, "term");
  text_window->setFocus();
  vgr->addWidget(text_window, 0, 0);

  cancel_b = new QPushButton(this, "cancel");
  cancel_b->setText(i18n("Ca&ncel"));
  cancel_b->setFixedHeight(25);
  connect(cancel_b, SIGNAL(clicked()), SLOT(cancelbutton()));

  continue_b = new QPushButton(this, "continue");
  continue_b->setText(i18n("&Continue"));
  continue_b->setFixedHeight(25);
  connect(continue_b, SIGNAL(clicked()), SLOT(continuebutton()));

  int mwidth;
  if (cancel_b->sizeHint().width() > continue_b->sizeHint().width())
    mwidth = cancel_b->sizeHint().width();
  else
    mwidth = continue_b->sizeHint().width();

  cancel_b->setFixedWidth(mwidth + 20);
  continue_b->setFixedWidth(mwidth + 20);

  hgr->addWidget(cancel_b, 0, 0, AlignCenter);
  hgr->addWidget(continue_b, 0, 1, AlignCenter);

  cont = false;

  Modem::modem->notify(text_window, SLOT(readChar(unsigned char)));
}


void LoginTerm::cancelbutton () {
  hide();
}


void LoginTerm::continuebutton() {
  cont = true;
  hide();
}


bool LoginTerm::pressedContinue() {
  return cont;
}


#include "loginterm.moc"









