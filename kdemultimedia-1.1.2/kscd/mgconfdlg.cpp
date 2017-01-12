
/*
 *
 * kscd -- A simple CD player for the KDE project
 *
 * $Id$
 * 
 * Copyright (C) 1997 Bernd Johannes Wuebben 
 * wuebben@math.cornell.edu
 *
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */



#include "mgconfdlg.h"


extern KApplication *mykapp;


MGConfigDlg::MGConfigDlg(QWidget *parent=0, 
			 struct mgconfigstruct *data = 0,const char *name=0)
  : QDialog(parent, name)
{

  mgconfigdata.width = 320;
  mgconfigdata.height = 200;
  mgconfigdata.brightness = 10;
  QString temp;

  if(data){
    mgconfigdata.width = data->width;
    mgconfigdata.height = data->height;
    mgconfigdata.brightness = data->brightness;
  }

  setCaption(klocale->translate("Magic Kscd"));

  box = new QGroupBox(this, "box");
  box->setGeometry(10,10,520,420);

  label1 = new QLabel(this);
  label1->setGeometry(20,25,155,25);
  label1->setText(klocale->translate("Width of Magic Window:"));

  width_edit = new QLineEdit(this);
  width_edit->setGeometry(200,25,100,25);
  temp.setNum(mgconfigdata.width);
  width_edit->setText(temp.data());
  connect(width_edit,SIGNAL(textChanged(const char*)),
	  this,SLOT(width_changed(const char*)));  


  label2 = new QLabel(this);
  label2->setGeometry(20,65,155,25);
  label2->setText(klocale->translate("Height of Magic Window:"));

  height_edit = new QLineEdit(this);
  height_edit->setGeometry(200,65,100,25);
  temp.setNum(mgconfigdata.height);
  height_edit->setText(temp.data());
  connect(height_edit,SIGNAL(textChanged(const char*)),
	  this,SLOT(height_changed(const char*)));  


  label3 = new QLabel(this);
  label3->setGeometry(20,110,155,25);
  label3->setText(klocale->translate("MAGIC Brightness:"));

  bspin = new KNumericSpinBox(this);
  bspin->setGeometry(200,110,50,25);
  bspin->setRange(0,100);
  
  bspin->setValue(mgconfigdata.brightness);
  connect(bspin,SIGNAL(valueIncreased()),
	  this,SLOT(brightness_changed()));  
  connect(bspin,SIGNAL(valueDecreased()),
	  this,SLOT(brightness_changed()));  

  
}




void MGConfigDlg::width_changed(const char* width) {

  mgconfigdata.width = atoi(width);
}


void MGConfigDlg::height_changed(const char* height) {

  mgconfigdata.height = atoi(height);
}


void MGConfigDlg::brightness_changed() {

  mgconfigdata.brightness = bspin->getValue();
}


void MGConfigDlg::help(){

  if(mykapp)
    mykapp->invokeHTMLHelp("kscd/kscd.html","");
}


struct mgconfigstruct * MGConfigDlg::getData(){

  return &mgconfigdata;

}

#include "mgconfdlg.moc"



