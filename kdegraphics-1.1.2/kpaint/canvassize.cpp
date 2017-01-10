// $Id: canvassize.cpp,v 1.10 1998/10/18 19:59:39 habenich Exp $

#include <kdebug.h>
#include <stdlib.h>
#include <qlayout.h>
#include <klocale.h>
#include <kapp.h>
#include "canvassize.h"

/**
 * A modal dialog to ask for the size of the canvas.
 */
canvasSizeDialog::canvasSizeDialog(QWidget *parent, const char *name)
 : QDialog (parent, name, TRUE)
{
  setCaption(i18n("Canvas Size"));

  // Create top level layout
  QVBoxLayout *topLevelLayout= new QVBoxLayout(this, 4);

  // Create the grid for the entry widgets
  QGridLayout *mainLayout= new QGridLayout(2, 2);
  topLevelLayout->addLayout(mainLayout, 0);

  widthLabel= new QLabel(klocale->translate("Width:"), this);
  heightLabel= new QLabel(klocale->translate("Height:"), this);
  widthEdit= new QLineEdit(this);
  widthEdit->setFocus();
  heightEdit= new QLineEdit(this);
  connect(widthEdit, SIGNAL(returnPressed()), SLOT(accept()) );
  connect(heightEdit, SIGNAL(returnPressed()), SLOT(accept()) );

  mainLayout->addWidget(widthLabel, 0, 0, AlignCenter);
  widthEdit->setFixedHeight(widthLabel->sizeHint().height()+8);
  mainLayout->addWidget(widthEdit, 0, 1);
  mainLayout->addWidget(heightLabel, 1, 0, AlignCenter);
  heightEdit->setFixedHeight(heightLabel->sizeHint().height()+8);
  mainLayout->addWidget(heightEdit, 1, 1);
  mainLayout->setColStretch(0, 1);
  mainLayout->setColStretch(1, 3);

  // Now a box layout for the buttons
  QHBoxLayout *buttonLayout= new QHBoxLayout();
  topLevelLayout->addLayout(buttonLayout);

  okButton= new QPushButton(klocale->translate("OK"), this);
  cancelButton= new QPushButton(klocale->translate("Cancel"), this);
  okButton->setFixedHeight(okButton->sizeHint().height());
  okButton->setMinimumWidth(okButton->sizeHint().width());
  cancelButton->setFixedHeight(cancelButton->sizeHint().height());
  cancelButton->setMinimumWidth(okButton->sizeHint().width());

  buttonLayout->addStretch();
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);
 
  connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()) );

  resize(200, 100);

  // Now start things off
  topLevelLayout->activate();
}

int canvasSizeDialog::getHeight()
{
  return atoi(heightEdit->text());
}
   
int canvasSizeDialog::getWidth()
{
  return atoi(widthEdit->text());
}



