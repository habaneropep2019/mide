/*
    $Id: options.cpp,v 1.4.2.1 1999/04/11 12:06:43 dfaure Exp $

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
  


*/

#include <qlayout.h>

#include "kfax.h"
#include "options.h"


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

#include "options.moc"

extern MyApp *mykapp;

OptionsDialog::OptionsDialog( QWidget *parent, const char *name)
    : QDialog( parent, name,TRUE)
{

  setCaption(i18n("KFax Options Dialog"));

  this->setFocusPolicy(QWidget::StrongFocus);

  int mw;

  QVBoxLayout *mainLayout = new QVBoxLayout(this, 10);

  bg = new QGroupBox(this,"bg");
  mainLayout->addWidget( bg );

  QVBoxLayout *vbl = new QVBoxLayout(bg, 10);

  QHBoxLayout *hbl1 = new QHBoxLayout();
  vbl->addSpacing( 15 );

  vbl->addLayout( hbl1 );

  displaylabel = new QLabel(i18n("Display Options:"),this,"displaylabel");
  displaylabel->setFixedSize( displaylabel->sizeHint() );
  hbl1->addSpacing( 10 );
  hbl1->addWidget( displaylabel );
  int labelw = displaylabel->sizeHint().width();

  landscape = new QCheckBox(i18n("Landscape"),this,"Landscape");
  landscape->setFixedSize( landscape->sizeHint() );
  hbl1->addSpacing( 10 );
  hbl1->addWidget( landscape );

  flip = new QCheckBox(i18n("Upside Down"),this,"upsidedown");
  flip->setFixedSize( flip->sizeHint() );
  hbl1->addSpacing( 10 );
  hbl1->addWidget( flip );

  invert = new QCheckBox(i18n("Invert"),this,"invert");
  invert->setFixedSize( invert->sizeHint() );
  hbl1->addSpacing( 10 );
  hbl1->addWidget( invert );
  mw = displaylabel->sizeHint().width() + landscape->sizeHint().width() +
  	flip->sizeHint().width() + invert->sizeHint().width() + 50;

  vbl->addSpacing( 15 );
  	
  QHBoxLayout *hbl8 = new QHBoxLayout();
  vbl->addLayout( hbl8 );
  hbl8->addSpacing( 10 );
  	
  resgroup = new QButtonGroup(this,"resgroup");
  resgroup->setFrameStyle(QFrame::NoFrame);
  hbl8->addWidget( resgroup );

  QHBoxLayout *hbl2 = new QHBoxLayout(resgroup);

  reslabel = new QLabel(i18n("Raw Fax Resolution:"),resgroup,"relabel");
  reslabel->setFixedSize( reslabel->sizeHint() );
  hbl2->addWidget( reslabel );
  if( reslabel->sizeHint().width() > labelw ) labelw = reslabel->sizeHint().width();

  resauto = new QRadioButton(i18n("Auto"),resgroup,"resauto");
  resauto->setFixedSize( resauto->sizeHint() );
  hbl2->addSpacing( 20 );
  hbl2->addWidget( resauto );

  fine = new QRadioButton(i18n("Fine"),resgroup,"fine");
  fine->setFixedSize( fine->sizeHint() );
  hbl2->addSpacing( 30 );
  hbl2->addWidget( fine );

  normal = new QRadioButton(i18n("Normal"),resgroup,"normal");
  normal->setFixedSize( normal->sizeHint() );
  hbl2->addSpacing( 30 );
  hbl2->addWidget( normal );

  int w2 = reslabel->width() + resauto->width() + fine->width()
	  	+ normal->width() + 80;
  if( w2 > mw ) mw = w2;

  /*  displaygroup = new QButtonGroup(this,"displaygroup");
  displaygroup->setGeometry(20,60,370,30);
  displaygroup->setFrameStyle(QFrame::NoFrame);*/

  vbl->addSpacing( 20 );

  QHBoxLayout *hbl3 = new QHBoxLayout();
  vbl->addLayout( hbl3 );

//  lsbgroup = new QButtonGroup(this,"lsbgroup");

  lsblabel = new QLabel(i18n("Raw Fax Data are:"),this,"lsblabel");
  lsblabel->setFixedSize( lsblabel->sizeHint() );
  hbl3->addSpacing( 10 );
  hbl3->addWidget( lsblabel );
  if( lsblabel->sizeHint().width() > labelw ) labelw = lsblabel->sizeHint().width();

  lsb = new QCheckBox(i18n("LS-Bit first"),this,"lsbitfirst");
  lsb->setFixedSize( lsb->sizeHint() );
  hbl3->addSpacing( 10 );
  hbl3->addWidget( lsb );

  vbl->addSpacing( 15 );

  QHBoxLayout *hbl9 = new QHBoxLayout();
  vbl->addLayout( hbl9 );
  hbl9->addSpacing( 10 );

  rawgroup = new QButtonGroup(this,"rawgroup");
  hbl9->addWidget( rawgroup );

  QHBoxLayout *hbl4 = new QHBoxLayout( rawgroup );

  rawgroup->setFrameStyle(QFrame::NoFrame);

  rawlabel = new QLabel(i18n("Raw Facsimili are:"),rawgroup,"rawlabel");
  rawlabel->setFixedSize( rawlabel->sizeHint() );
  hbl4->addWidget( rawlabel );
  if( rawlabel->sizeHint().width() > labelw ) labelw = rawlabel->sizeHint().width();

  g3 = new QRadioButton("g3",rawgroup,"g3");
  connect(g3,SIGNAL(clicked()),this,SLOT(g3toggled()));
  g3->setFixedSize( g3->sizeHint() );
  hbl4->addSpacing( 20 );
  hbl4->addWidget( g3 );


  g32d = new QRadioButton("g32d",rawgroup,"g32d");
  connect(g32d,SIGNAL(clicked()),this,SLOT(g32toggled()));
  g32d->setFixedSize( g32d->sizeHint() );
  hbl4->addSpacing( 30 );
  hbl4->addWidget( g32d );


  g4 = new QRadioButton("g4",rawgroup,"g4");
  connect(g4,SIGNAL(clicked()),this,SLOT(g4toggled()));
  g4->setFixedSize( g4->sizeHint() );
  hbl4->addSpacing( 30 );
  hbl4->addWidget( g4 );

  w2 = rawlabel->width() + g3->width() + g32d->width()
	  	+ g4->width() + 80;
  if( w2 > mw ) mw = w2;

  vbl->addSpacing( 20 );

  QHBoxLayout *hbl5 = new QHBoxLayout();
  vbl->addLayout( hbl5 );

  widthlabel = new QLabel(i18n("Raw Fax width:"),this,"widthlabel");
  widthlabel->setFixedSize( widthlabel->sizeHint() );
  hbl5->addSpacing( 10 );
  hbl5->addWidget( widthlabel );

  widthedit = new KIntLineEdit(this,"widthedit");
  widthedit->setFixedHeight( widthedit->sizeHint().height() );
  widthedit->setMinimumWidth( widthedit->sizeHint().width() );
  hbl5->addWidget( widthedit );

  heightlabel = new QLabel(i18n("height:"),this,"heightlabel");
  heightlabel->setFixedSize( heightlabel->sizeHint() );
  hbl5->addSpacing( 10 );
  hbl5->addWidget( heightlabel );

  heightedit = new KIntLineEdit(this,"heightedit");
  heightedit->setFixedHeight( heightedit->sizeHint().height() );
  heightedit->setMinimumWidth( heightedit->sizeHint().width() );
  hbl5->addWidget( heightedit );

  geomauto = new QCheckBox(i18n("Auto"),this,"geomauto");
  connect(geomauto,SIGNAL(clicked()),this,SLOT(geomtoggled()));
  geomauto->setFixedSize( geomauto->sizeHint() );
  hbl5->addSpacing( 10 );
  hbl5->addWidget( geomauto );

  w2 = widthlabel->width() + widthedit->width() + heightlabel->width()
	  	+ heightedit->width() + geomauto->width() + 50;
  if( w2 > mw ) mw = w2;

  bg->setMinimumHeight( displaylabel->sizeHint().height() +
  			reslabel->sizeHint().height() +
  			lsblabel->sizeHint().height() +
  			rawlabel->sizeHint().height() +
  			widthedit->sizeHint().height() + 160 );
  bg->setMinimumWidth( mw + 30 );

  displaylabel->setFixedWidth( labelw );
  reslabel->setFixedWidth( labelw );
  rawlabel->setFixedWidth( labelw );
  lsblabel->setFixedWidth( labelw );

  QHBoxLayout *hbl6 = new QHBoxLayout();
//  mainLayout->addStretch( 1 );
  mainLayout->addLayout( hbl6 );

  cancel_button = new QPushButton(i18n("Cancel"),this);
  cancel_button->setFixedSize( cancel_button->sizeHint());
  connect( cancel_button, SIGNAL( clicked() ), SLOT( reject() ) );

  ok_button = new QPushButton( i18n("OK"), this );
  ok_button->setFixedSize( ok_button->sizeHint());
  connect( ok_button, SIGNAL( clicked() ), SLOT( ready() ) );	

  helpbutton = new QPushButton(i18n( "Help"), this );
  helpbutton->setFixedSize( helpbutton->sizeHint());
  connect( helpbutton, SIGNAL( clicked() ), SLOT( help() ) );	

  hbl6->addWidget( ok_button );
  hbl6->addWidget( cancel_button );
  hbl6->addStretch( 1 );
  hbl6->addWidget( helpbutton );

  mainLayout->activate();
  resize(minimumSize());
}


struct optionsinfo *  OptionsDialog::getInfo(){

  if(resauto->isChecked())
    oi.resauto = 1;

  if(fine->isChecked())
    oi.fine = 1;
  else
    oi.fine = 0;

  if(landscape->isChecked())
    oi.landscape = 1;
  else
    oi.landscape = 0;

  if(flip->isChecked())
    oi.flip = 1;
  else
    oi.flip = 0;

  if(invert->isChecked())
    oi.invert = 1;
  else
    oi.invert = 0;

  if(lsb->isChecked())
    oi.lsbfirst = 1;
  else
    oi.lsbfirst = 0;

  if(geomauto->isChecked())
    oi.geomauto = 1;


  if(g3->isChecked()){
    oi.raw = 3;
  }

  if(g32d->isChecked()){
    oi.raw = 2;
    oi.geomauto = 0;

  }

  if(g4->isChecked()){
    oi.raw = 4;
    oi.geomauto = 0;
  }

  oi.height = heightedit->getValue();
  oi.width = widthedit->getValue();



  return &oi;

}

void OptionsDialog::setWidgets(struct optionsinfo* newoi ){

  if(!newoi)
    return;

  if(newoi->resauto == 1){
    resauto->setChecked(TRUE);
    fine->setChecked(FALSE);
    normal->setChecked(FALSE);
  }
  else{
    if(newoi->fine == 1){
      resauto->setChecked(FALSE);
      fine->setChecked(TRUE);
      normal->setChecked(FALSE);
    }
    else{
      resauto->setChecked(FALSE);
      fine->setChecked(FALSE);
      normal->setChecked(TRUE);
    }
  }

  if(newoi->landscape == 1)
    landscape->setChecked(TRUE);
  else
    landscape->setChecked(FALSE);

  if(newoi->flip == 1)
    flip->setChecked(TRUE);
  else
    flip->setChecked(FALSE);

  if(newoi->invert == 1)
    invert->setChecked(TRUE);
  else
    invert->setChecked(FALSE);

  if(newoi->lsbfirst == 1)
    lsb->setChecked(TRUE);
  else
    lsb->setChecked(FALSE);

  if(newoi->raw == 3){
    geomauto->setEnabled(TRUE);
    g3->setChecked(TRUE);
  }

  if(newoi->raw == 2){
    geomauto->setEnabled(FALSE);
    g32d->setChecked(TRUE);
  }

  if(newoi->raw == 4){
    geomauto->setEnabled(FALSE);
    g4->setChecked(TRUE);
  }
  QString widthstr;
  widthstr.setNum(newoi->width);
  widthedit->setText(widthstr);

  QString heightstr;
  heightstr.setNum(newoi->height);
  heightedit->setText(heightstr);

  // auto height and width can only work with g3 faxes
  if(newoi->geomauto == 1 && newoi->raw != 4 && newoi->raw != 2){ 
    geomauto->setChecked(TRUE);
    widthedit->setEnabled(FALSE);
    heightedit->setEnabled(FALSE);
  }
  else{
    geomauto->setChecked(FALSE);
    widthedit->setEnabled(TRUE);
    heightedit->setEnabled(TRUE);

  }

}


void OptionsDialog::g32toggled(){

  geomauto->setChecked(FALSE);
  geomauto->setEnabled(FALSE);
  widthedit->setEnabled(TRUE);
  heightedit->setEnabled(TRUE);
  
}

void OptionsDialog::g4toggled(){

  geomauto->setChecked(FALSE);
  geomauto->setEnabled(FALSE);
  widthedit->setEnabled(TRUE);
  heightedit->setEnabled(TRUE);


}


void OptionsDialog::g3toggled(){

  geomauto->setEnabled(TRUE);
  geomauto->setChecked(TRUE);
  widthedit->setEnabled(FALSE);
  heightedit->setEnabled(FALSE);


}

void OptionsDialog::geomtoggled(){

  if(geomauto->isChecked()){

    widthedit->setEnabled(FALSE);
    heightedit->setEnabled(FALSE);

  }
  else{

    widthedit->setEnabled(TRUE);
    heightedit->setEnabled(TRUE);

  }

}

void OptionsDialog::ready(){

  accept();

}

void OptionsDialog::help(){

  mykapp->invokeHTMLHelp("","");

}




void OptionsDialog::focusInEvent(QFocusEvent* ){



}

