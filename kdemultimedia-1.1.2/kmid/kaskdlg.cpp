/**************************************************************************

    kaskdlg.cpp  - A dialog that ask the user something definable  
    Copyright (C) 1998  Antonio Larrosa Jimenez

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

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include "kaskdlg.h"
#include <qpushbt.h>
#include <qlistbox.h>
#include <qlabel.h> 
#include <kapp.h>
#include <kmsgbox.h>
#include <klined.h>
#include "version.h"

KAskDialog::KAskDialog(const char *labeltext,const char *title,QWidget *parent,const char *name) : QDialog(parent,name,TRUE)
{
setCaption(title);
ok=new QPushButton(i18n("OK"),this);
ok->setGeometry(140,200,100,30);
connect(ok,SIGNAL(clicked()),SLOT(OK_pressed()) );
cancel=new QPushButton(i18n("Cancel"),this);
cancel->setGeometry(250,200,100,30);
connect(cancel,SIGNAL(clicked()),SLOT(reject()) );

label=new QLabel(labeltext,this);
label->adjustSize();
label->move(10,10);
kline= new KLined(this,"ask");
kline->setGeometry(10,label->y()+label->height()+5,width()-20,kline->height());
connect(kline,SIGNAL(returnPressed()),SLOT(OK_pressed()) );

ok->move(ok->x(),kline->y()+kline->height()+10);
cancel->move(ok->x()+ok->width()+5,ok->y());

int maxw=label->x()+label->width()+10;
setMinimumSize((maxw>200)? maxw : 200,ok->y()+ok->height()+5);
setMaximumHeight(ok->y()+ok->height()+5);
kline->setFocus();
};

void KAskDialog::resizeEvent(QResizeEvent *)
{
kline->resize(width()-20,kline->height());

cancel->move(width()-cancel->width()-5,height()-cancel->height()-5);
ok->move(cancel->x()-5-ok->width(),height()-ok->height()-5);
};

void KAskDialog::OK_pressed()
{
if (textresult!=NULL) delete textresult;
textresult=new char[strlen(kline->text())+1];
strcpy(textresult,kline->text());
accept();
};

char *KAskDialog::textresult=NULL;
char *KAskDialog::getResult(void)
{
return textresult;
};
