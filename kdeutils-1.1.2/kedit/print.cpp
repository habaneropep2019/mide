/*
    $Id: print.cpp,v 1.10.2.1 1999/03/15 14:27:02 dfaure Exp $

    Requires the Qt widget libraries, available at no cost at 
    http://www.troll.no
       
    Copyright (C) 1996 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
  
    $Log: print.cpp,v $
    Revision 1.10.2.1  1999/03/15 14:27:02  dfaure
    Layouts for kedit dialog boxes. Looks really nicer.
    Translated strings that were truncated really look as horrible bugs to users.
    Thanks to Francois-Xavier.Duranceau@loria.fr for making me do it :)

    Revision 1.10  1998/09/14 20:45:05  kulow
    I know, Ok is ok too, but OK is more OK some GUI guides say :)

    Revision 1.9  1997/10/24 19:25:19  wuebben
    Lot's of fixes and improvements. Kedit was pretty screwed up.

    Revision 1.8  1997/10/24 05:52:43  wuebben
    Bernd: Some major fix ups. Hopefully I didn't break too much. Qt 1.31
    is nice, but it also uglified kedit, so I had to fix that.

    Revision 1.7  1997/08/31 21:19:58  kdecvs
    Kalle: Even more changes for the new KLocale and the new KConfig


*/


#include "print.h"
#include "stdio.h"

#define YOFFSET  5
#define XOFFSET  5
#define LABLE_LENGTH  40
#define LABLE_HEIGHT 20
#define SIZE_X 400
#define SIZE_Y 280
#define FONTLABLE_LENGTH 60
#define COMBO_BOX_HEIGHT 28
#define COMBO_ADJUST 3
#define OKBUTTONY 260
#define BUTTONHEIGHT 25

#include "print.moc"

#include <qlayout.h>
#include <klocale.h>
#include <kapp.h>

PrintDialog::PrintDialog( QWidget *parent, const char *name,  bool modal)
    : QDialog( parent, name, modal )
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this, 10);
  int fontHeight = 2*fontMetrics().height();
  
  setCaption(klocale->translate("Print Dialog"));

  bg = new QButtonGroup(this,"bg");
  mainLayout->addWidget(bg);
  QVBoxLayout * frameLayout = new QVBoxLayout(bg, 15);
 
  rawbutton = new QRadioButton(klocale->translate("Print directly using lpr")
			       ,bg,"rawbutton");
  rawbutton->setChecked(TRUE);
  rawbutton->setFixedSize( rawbutton->sizeHint() );
  frameLayout->addWidget(rawbutton, 0, AlignLeft);

  QHBoxLayout * hLay = new QHBoxLayout();
  frameLayout->addLayout(hLay);
  commandbutton = new QRadioButton(klocale->translate("Print using Command:")
				   ,bg,"commandbutton");
  commandbutton->setFixedSize( commandbutton->sizeHint() );
  hLay->addWidget(commandbutton);

  commandbox = new QLineEdit(bg,"command");
  commandbox->setMinimumWidth(200);
  commandbox->setFixedHeight(fontHeight);
  hLay->addWidget(commandbox);


  bg1 = new QButtonGroup(this,"bg1");
  mainLayout->addWidget(bg1);
  frameLayout = new QVBoxLayout(bg1, 15);

  allbutton = new QRadioButton(klocale->translate("Print Document")
			       ,bg1,"documentbutton");
  allbutton->setFixedSize( allbutton->sizeHint() );
  frameLayout->addWidget(allbutton, 0, AlignLeft);
  allbutton->setChecked(TRUE);

  selectionbutton = new QRadioButton(klocale->translate("Print Selection"),
				     bg1,"selectionbutton");
  selectionbutton->setFixedSize( selectionbutton->sizeHint() );
  frameLayout->addWidget(selectionbutton, 0, AlignLeft);
  
  hLay = new QHBoxLayout();
  mainLayout->addLayout(hLay);

  ok_button = new QPushButton(klocale->translate( "OK"), this );
  ok_button->setFocus();
  ok_button->setFixedSize(ok_button->sizeHint()); 
  hLay->addStretch();
  hLay->addWidget(ok_button);
  hLay->addStretch();
  connect( ok_button, SIGNAL( clicked() ), SLOT( accept() ) );	
  
  cancel_button = new QPushButton(klocale->translate("Cancel"),this);

  cancel_button->setGeometry( 3*XOFFSET +100, 210, 80, BUTTONHEIGHT );
  connect( cancel_button, SIGNAL( clicked() ), SLOT( reject() ) );
  cancel_button->setFixedSize(cancel_button->sizeHint()); 
  hLay->addWidget(cancel_button);
  hLay->addStretch();

  mainLayout->activate();
  resize(minimumSize());

}


struct printinfo PrintDialog::getCommand(){

  struct printinfo pi;

  QString string;
  string = commandbox->text();

  pi.command = string;
  pi.raw = rawbutton->isChecked();
  pi.selection = selectionbutton->isChecked();

  return pi;

}

void PrintDialog::setWidgets(struct printinfo pi){

  if (pi.raw == 1){
    rawbutton->setChecked(TRUE);
    commandbutton->setChecked(FALSE);
  }
  else{
    commandbutton->setChecked(TRUE);
    rawbutton->setChecked(FALSE);
  }
  
  commandbox->setText(pi.command);
  

}
