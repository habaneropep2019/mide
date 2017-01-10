/*
 *        kPPP: A pppd front end for the KDE project
 *
 * $Id: debug.cpp,v 1.6.2.1 1999/03/11 15:15:05 porten Exp $
 *            Copyright (C) 1997  Bernd Wuebben
 *                 wuebben@math.cornel.edu
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

#include "debug.h"
#include "stdio.h"
#include "main.h"

extern KPPPWidget *p_kppp;

myMultiEdit::myMultiEdit(QWidget *parent, const char *name)
  : QMultiLineEdit(parent, name)
{
}

void myMultiEdit::insertChar(unsigned char c) {
  QMultiLineEdit::insertChar( c);
}


void myMultiEdit::newLine() {
  QMultiLineEdit::newLine();
}


DebugWidget::DebugWidget(QWidget *parent, const char *name)
  : QDialog(parent, name, FALSE)
{
  setCaption(i18n("Login Script Debug Window"));

  text_window = new myMultiEdit(this,"debugwindow");
  text_window->setGeometry(2,5,400, 300);
  //  text_window->setReadOnly(FALSE);

  statuslabel = new QLabel( this, "statuslabel" );

  statuslabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  statuslabel->setText( "" );
  statuslabel->setAlignment( AlignLeft|AlignVCenter );
  statuslabel->setGeometry(2, 307, 400, 20);
  //statusPageLabel->setFont( QFont("helvetica",12,QFont::Normal) );

  dismiss = new QPushButton(this);
  dismiss->setGeometry(330,340,70,30);
  dismiss->setText(i18n("Close"));
  dismiss->setFocus();
  connect(dismiss,SIGNAL(clicked()),SLOT(hideit()));


  /*  fline = new QFrame(this,"line");
  fline->setFrameStyle(QFrame::HLine |QFrame::Sunken);
  fline->setGeometry(2,332,398,5);*/
  adjustSize();
  setMinimumSize(width(),height());

}


void DebugWidget::hideit() {
  this->hide();
  if (p_kppp)
    p_kppp->con->debug->setText(i18n("Log")); // set Log/Hide button text to Log 
}


void DebugWidget::clear() {
  if(text_window)
    text_window->clear(); 
}


void DebugWidget::addChar(unsigned char c) {
  QString stuff;

  if(c == '\r' || c == '\n') {
    if(c == '\n')
      text_window->newLine();    
  } else
    text_window->insertChar(c);
}


void DebugWidget::statusLabel(const char *n) {
  statuslabel->setText(n);
}


void DebugWidget::statusLabel(QString s) {
  statusLabel(s.data());
}


void DebugWidget::resizeEvent(QResizeEvent *e){
  int w = width() ;
  int h = height();
  e = e;

  text_window->setGeometry(2,5,w - 2 ,h - 63);
  statuslabel->setGeometry(2, h - 56 , w -2 , 20);
  dismiss->setGeometry(w - 72 , h - 32, 70, 30);
  //  fline->setGeometry(2,h -70 ,w - 4,5);
}


void DebugWidget::enter() {
  char character[3] = "\r\n"; // safe
  text_window->append(character);
}


void DebugWidget::toggleVisibility() {
  if(isVisible())
    hide();
  else
    show();
}


#include "debug.moc"

