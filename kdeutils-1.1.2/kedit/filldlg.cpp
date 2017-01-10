    /*

    $Id: filldlg.cpp,v 1.9.2.1 1999/03/15 14:26:57 dfaure Exp $

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

#include "filldlg.h"
#include <qlayout.h>
#include <klocale.h>
#include <kapp.h>

FillDlg::FillDlg(QWidget *parent, const char *name)
     : QDialog(parent, name, TRUE)
{
    this->setFocusPolicy(QWidget::StrongFocus);
    QVBoxLayout * mainLayout = new QVBoxLayout(this, 10);
    // The unique groupbox, around everything
    frame1 = new QGroupBox(klocale->translate("KEdit Options"), 
			   this, "frame1");
    mainLayout->addWidget(frame1);
    // The vertical layout for its items
    QVBoxLayout * frameLayout = new QVBoxLayout(frame1, 15);

    // First item : fill-column checkbox + lineedit
    QHBoxLayout * hLay = new QHBoxLayout();
    frameLayout->addLayout(hLay);
    values = new QLineEdit( frame1, "values");
    connect(values, SIGNAL(returnPressed()), this, SLOT(checkit()));
    fill_column = new QCheckBox(klocale->translate("Set Fill-Column at:"),
				frame1, "fill");
    connect(fill_column, SIGNAL(toggled(bool)),this,SLOT(synchronize(bool)));
    int fontHeight = 2*fontMetrics().height();
    values->setMinimumWidth(200);
    values->setFixedHeight(fontHeight);
    fill_column->setFixedSize( fill_column->sizeHint() );
    hLay->addWidget(fill_column);
    hLay->addWidget(values);

    // Second item : word wrap
    word_wrap = new QCheckBox(klocale->translate("Word Wrap"), 
			      frame1, "word");
    word_wrap->setMinimumSize( word_wrap->sizeHint() );
    frameLayout->addWidget(word_wrap);

    // Third item : back copies
    backup_copies = new QCheckBox(klocale->translate("Backup Copies"), 
			      frame1, "backup");
    backup_copies->setMinimumSize( backup_copies->sizeHint() );
    frameLayout->addWidget(backup_copies);

    // Fourth item : mail command
    hLay = new QHBoxLayout();
    frameLayout->addLayout(hLay);
    mailcmd = new QLineEdit(frame1,"mailcmd");
    mailcmd->setMinimumWidth(200);
    mailcmd->setFixedHeight(fontHeight);
    mailcmdlabel = new QLabel(frame1,"mailcmdlable");
    mailcmdlabel->setText(klocale->translate("Mail Command:"));
    mailcmdlabel->setFixedSize( mailcmdlabel->sizeHint() );
    hLay->addWidget(mailcmdlabel);
    hLay->addWidget(mailcmd);
    frameLayout->addStretch(10); // so that frame doesn't grow
    mainLayout->addStretch(10);

    hLay = new QHBoxLayout();
    mainLayout->addLayout(hLay);
    ok = new QPushButton(klocale->translate("OK"), this, "OK");
    ok->setFixedSize(ok->sizeHint()); 
    hLay->addStretch();
    hLay->addWidget(ok);
    hLay->addStretch();
    cancel = new QPushButton(klocale->translate("Cancel"), this, "cancel");
    cancel->setFixedSize(cancel->sizeHint()); 
    hLay->addWidget(cancel);
    hLay->addStretch();
    cancel->setFocus();
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ok, SIGNAL(clicked()), this, SLOT(checkit()));
    mainLayout->activate();
    resize(minimumSize());
}

struct fill_struct  FillDlg::getFillCol() { 

  QString string;

  fillstr.word_wrap_is_set = word_wrap->isChecked();
  fillstr.fill_column_is_set = fill_column->isChecked();
  fillstr.backup_copies_is_set = backup_copies->isChecked();
  fillstr.mailcmd = mailcmd->text();
  fillstr.mailcmd.detach();
  string = values->text();  
  fillstr.fill_column_value = string.toInt();

  return fillstr;

}


void FillDlg::synchronize(bool on){

  // if the fill-column button is not checked we can't do word wrap
  // so we have to turn the word wrap button off.

  if(!on)
    word_wrap->setChecked(FALSE);
  
}

bool FillDlg::fill(){

  if (fill_column->isChecked())
    return true;
  else
    return false;


};

void FillDlg::setWidgets(struct fill_struct fill){

  QString string;

  fill_column->setChecked(fill.fill_column_is_set);
  word_wrap->setChecked(fill.word_wrap_is_set);
  backup_copies->setChecked(fill.backup_copies_is_set);
  values->setText(string.setNum(fill.fill_column_value));
  mailcmd->setText(fill.mailcmd.data());

}

bool FillDlg::wordwrap(){

  if (word_wrap->isChecked())
    return true;
  else
    return false;

};

void FillDlg::checkit(){

  bool ok;
  QString string;

  string = values->text();
  string.toInt(&ok);

  if (ok){
    accept();
  }
  else{
  QMessageBox::message(klocale->translate("Sorry"),
		       klocale->translate("You must enter an integer."),
		       klocale->translate("OK"));
  }

}


#include "filldlg.moc"
