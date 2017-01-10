/*
    $Id: print.cpp,v 1.5 1998/09/14 20:43:50 kulow Exp $
    
    KFax -- a fax file viewer for KDE
       
    Copyright (C) 1997 Bernd Johannes Wuebben   
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


#include "print.h"
#include "stdio.h"

#include "kfax.h"

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

extern KApplication* mykapp;

PrintDialog::PrintDialog( QWidget *parent, const char *name,  bool )
    : QDialog( parent, name,FALSE)
{

  setCaption(i18n("KFax Print Dialog"));

  this->setFocusPolicy(QWidget::StrongFocus);
  
  bg = new QButtonGroup(this,"bg");

  lprbutton = new QRadioButton(i18n("Print as Postscript to Printer"),bg,"rawbutton");
  lprbutton->setGeometry(15,20,250,25);
  lprbutton->setChecked(TRUE);
  connect(lprbutton,SIGNAL(clicked()),this,SLOT(filebuttontoggled()));
  
  cmdlabel = new QLabel (i18n("Print Command:"),this);
  cmdlabel->setGeometry(45,60,90,BUTTONHEIGHT);
  cmdlabel->setEnabled(TRUE);

  cmdedit = new QLineEdit(bg,"lprcmd");
  cmdedit->setGeometry(140,50,200,25);
  cmdedit->setEnabled(TRUE);
  cmdedit->setText("lpr");

  filebutton = new QRadioButton(i18n("Print as Postscript to File:")
				,bg,"commandbutton");
  filebutton->setGeometry(15,90,250,25);
  connect(filebutton,SIGNAL(clicked()),this,SLOT(filebuttontoggled()));


  selectfile = new QPushButton (i18n("Browse"),this);
  selectfile->setGeometry(25,140,100,BUTTONHEIGHT);
  selectfile->setEnabled(FALSE);
  connect(selectfile,SIGNAL(clicked()),this,SLOT(choosefile()));

  filename = new QLineEdit(bg,"command");
  filename->setGeometry(125,130,215,25);
  filename->setEnabled(FALSE);  

  pagesizelabel = new QLabel (i18n("Page Size:"),bg);
  pagesizelabel->setGeometry(15,175,60,25);
  pagesizelabel->setEnabled(TRUE);

  papercombo = new QComboBox(bg,"pagesizecombo");
  papercombo->setGeometry(125,175,220,25);
  papercombo->insertItem(US_LETTER);
  papercombo->insertItem(US_LEGAL);
  papercombo->insertItem(US_EXECUTIVE);
  papercombo->insertItem(US_LEDGER);
  papercombo->insertItem(DIN_A3);
  papercombo->insertItem(DIN_A4);
  papercombo->insertItem(DIN_A5);
  papercombo->insertItem(DIN_A6);
  papercombo->insertItem(DIN_B4);
  papercombo->insertItem(JAP_LETTER);
  papercombo->insertItem(JAP_LEGAL);


  scalebutton = new QCheckBox(i18n("Scale fax to page size")
			      ,this,"scalebutton");
  scalebutton->setGeometry(25,225,220,25);
  scalebutton->setChecked(FALSE);

  marginbutton = 
    new QCheckBox(i18n("Add Printer Margins (measured in cm):")
		  ,this,"marginbutton");

  marginbutton->setGeometry(25,255,300,25);
  marginbutton->setChecked(FALSE);
  connect(marginbutton,SIGNAL(toggled(bool)),SLOT(margins_toggled(bool)));

  xmarginlabel = new QLabel(this);
  xmarginlabel->setText(i18n("Horizontal:"));
  xmarginlabel->setGeometry(25,285,105,25);
  xmarginlabel->setEnabled(FALSE);

  xmarginedit = new QLineEdit(this);
  xmarginedit->setGeometry(120,285,50,25);
  xmarginedit->setEnabled(FALSE);

  ymarginlabel = new QLabel(this);
  ymarginlabel->setText(i18n("Vertical:"));
  ymarginlabel->setGeometry(185,285,100,25);
  ymarginlabel->setEnabled(FALSE);

  ymarginedit = new QLineEdit(this);
  ymarginedit->setGeometry(295,285,50,25);
  ymarginedit->setEnabled(FALSE);

  bg->setGeometry(10,10,380,320);
  
  cancel_button = new QPushButton(i18n("Cancel"),this);

  cancel_button->setGeometry( 3*XOFFSET +100, 340, 80, BUTTONHEIGHT );
  connect( cancel_button, SIGNAL( clicked() ), SLOT( cancel() ) );

  ok_button = new QPushButton( i18n("OK"), this );
  ok_button->setGeometry( 3*XOFFSET, 340, 80, BUTTONHEIGHT );
  connect( ok_button, SIGNAL( clicked() ), SLOT( ready() ) );	

  help_button = new QPushButton(i18n( "Help"), this );
  help_button->setGeometry( 300, 340, 80, BUTTONHEIGHT );
  connect( help_button, SIGNAL( clicked() ), SLOT( help() ) );	


  this->setFixedSize(400,370);


}

void PrintDialog::margins_toggled(bool set){

    
    xmarginedit->setEnabled(set);
    xmarginlabel->setEnabled(set);
    ymarginedit->setEnabled(set);
    ymarginlabel->setEnabled(set);


}

struct printinfo *  PrintDialog::getInfo(){

  pi.file  = filename->text();
  pi.cmd = cmdedit->text();
  if(lprbutton->isChecked())
    pi.lpr = 1;
  else
    pi.lpr = 0;

  if(scalebutton->isChecked())
    pi.scale = 1;
  else
    pi.scale = 0;

  pi.pagesize = papercombo->currentText();

  bool ok;
  
  pi.margins = marginbutton->isChecked();
  QString xm = xmarginedit->text();
  pi.xmargin = xm.toDouble(&ok);
  if(!ok) pi.xmargin = 0.0;

  QString ym = ymarginedit->text();
  pi.ymargin = ym.toDouble(&ok);
  if(!ok) pi.ymargin = 0.0;
  return &pi;

}

void PrintDialog::setWidgets(struct printinfo* pi ){


  if (pi->lpr == 1){
    lprbutton->setChecked(TRUE);
    filebutton->setChecked(FALSE);
  }
  else{
    lprbutton->setChecked(TRUE);
    filebutton->setChecked(FALSE);
  }
  
  if( pi->margins == 1){
    marginbutton->setChecked(TRUE);
    margins_toggled(TRUE);
  }
  else{
    marginbutton->setChecked(FALSE);
    margins_toggled(FALSE);
  }

  QString tempstring;

  tempstring.sprintf("%.2f",pi->xmargin);
  xmarginedit->setText(tempstring.data());

  tempstring = "";
  tempstring.sprintf("%.2f",pi->ymargin);
  ymarginedit->setText(tempstring.data());

  filename->setText(pi->file);
  cmdedit->setText(pi->cmd);

  if( pi->scale ==1){
    scalebutton->setChecked(TRUE);
  }
  else{
    scalebutton ->setChecked(FALSE);
  }

  for( int i =0; i<papercombo->count();i++){
    if(strcmp(papercombo->text(i),pi->pagesize.data()) == 0){
      papercombo->setCurrentItem(i);
      break;
    }
  }
      

}

void PrintDialog::help(){

  mykapp->invokeHTMLHelp( "" , "" );

}
void PrintDialog::choosefile(){



  QFileDialog* box = new QFileDialog(this,"printfiledialog",TRUE);
  box->setCaption(i18n("Select Filename"));

  if (!box->exec()) {
    return;
  }

  if(box->selectedFile().isEmpty()) {  

      return ;
  }

  filename->setText( box->selectedFile());

  delete box;

}

void PrintDialog::ready(){

  if(filebutton->isChecked() && QString(filename->text()).isEmpty()){

    QMessageBox::warning(this,"KFax",
			 i18n("You must enter a file name if you wish to "\
			 "print to a file."),i18n("OK"),0);
    return;


  }

  if(lprbutton->isChecked() && QString(cmdedit->text()).isEmpty()){

    QMessageBox::warning(this,"KFax",
			 i18n("You must enter a print command such as \"lpr\"\n "\
			 "if you wish to "\
			 "print to a printer."),i18n("OK"),0);
    return;


  }

  this->hide();
  emit print();


}

void PrintDialog::cancel(){

  this->hide();

}


void PrintDialog::filebuttontoggled(){

  if(filebutton->isChecked()){
    selectfile->setEnabled(TRUE); 
    filename->setEnabled(TRUE);
    cmdedit->setEnabled(FALSE);
    cmdlabel->setEnabled(FALSE);
  }
  else{
    selectfile->setEnabled(FALSE); 
    filename->setEnabled(FALSE);
    cmdedit->setEnabled(TRUE);
    cmdlabel->setEnabled(TRUE);
  }

}

void PrintDialog::focusInEvent(QFocusEvent* ){

  lprbutton->setFocus();

}
