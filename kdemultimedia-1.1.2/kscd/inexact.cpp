

/*   
   Kscd - A simple cd player for the KDE Project

   $Id$
 
   Copyright (c) 1997 Bernd Johannes Wuebben math.cornell.edu

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


 */

#include "inexact.h"
#include "stdio.h"
#include <klocale.h>
#include <kapp.h>


InexactDialog::InexactDialog(QWidget *parent, const char *name,bool _listbox)
  : QDialog(parent, name, TRUE)
{

  setCaption("Kscd");

  listbox = _listbox;
  if(listbox){
    list_box = new QListBox(this,"debugwindow");
    list_box->setGeometry(2,5,400, 300);
    connect(list_box,SIGNAL(highlighted(int)),SLOT(setStatusBar(int)));
  }
  else{
    edit = new QMultiLineEdit(this,"debugwindow");
    edit->setGeometry(2,5,400, 300);
  }


  text = new QLabel(this,"textlabel");
  text->setAlignment(WordBreak|AlignCenter);

  text->setText(klocale->translate("No exact match could be found. Please select the appropriate"\
		" CD from the list of choices presented below."));

  errorstring = klocale->translate("Please select a Disk Title or press Cancel");

  statuslabel = new QLabel( this, "statuslabel" );
  
  statuslabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  statuslabel->setText( "" );
  statuslabel->setAlignment( AlignCenter );
  statuslabel->setGeometry(2, 307, 400, 20);
  //statusPageLabel->setFont( QFont("helvetica",12,QFont::Normal) );

  cancel_button = new QPushButton(this,"cancel_button");
  cancel_button->setGeometry(330,340,70,30);
  cancel_button->setText(klocale->translate("Cancel"));
  cancel_button->setFocus();

  ok_button = new QPushButton(this,"ok_button");
  ok_button->setGeometry(250,340,70,30);
  ok_button->setText(klocale->translate("OK"));
  //  ok_button->setFocus();


  connect(ok_button,SIGNAL(clicked()),SLOT(checkit()));
  connect(cancel_button,SIGNAL(clicked()),SLOT(reject()));

  returnstring = "";

  adjustSize();
  setMinimumSize(width(),height());
  
}


InexactDialog::~InexactDialog() {

}

void InexactDialog::setTitle(char* t){

  titlestring = t;
  text->setText(t);

}

void InexactDialog::setErrorString(char* t){

  errorstring = t;
}

void InexactDialog::checkit(){

  if(listbox){
    if(list_box->currentItem() == -1){
      QMessageBox::information(this,
			       "Kscd",
			       errorstring.data()
			       );
      return;
    }
    returnstring = list_box->text(list_box->currentItem());
  }
  else{

    returnstring = edit->text();
  }

  accept();
}

void InexactDialog::getSelection(QString& string){

  string = returnstring.copy();
}


void InexactDialog::insertList(QStrList& strlist){

  if(listbox){
    list_box->setAutoUpdate(FALSE);
    list_box->insertStrList(&strlist,-1);
    list_box->setAutoUpdate(TRUE);
  }

}

void InexactDialog::insertText(char* str){

  if(!listbox){
    edit->setAutoUpdate(FALSE);
    edit->setText(str);
    edit->setAutoUpdate(TRUE);
  }

}

void InexactDialog::setStatusBar(int i){

  returnstring = list_box->text(i);
  statuslabel->setText(returnstring);

}
void InexactDialog::resizeEvent(QResizeEvent *){

  int w = width() ;
  int h = height();
  text->setGeometry(10,5,w-10,45);
  if(listbox)
    list_box->setGeometry(2,45 + 5,w - 2 ,h - 60 -45 );
  else
    edit->setGeometry(2,45 + 5,w - 2 ,h - 60 -45 );

  statuslabel->setGeometry(2, h - 53 , w -2 , 20);
  ok_button->setGeometry(w - 72 - 80 , h - 28, 70, 25);
  cancel_button->setGeometry(w - 72 , h - 28, 70, 25);
  
}


#include "inexact.moc"
