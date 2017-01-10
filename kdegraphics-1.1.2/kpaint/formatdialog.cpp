// $Id: formatdialog.cpp,v 1.8 1998/10/18 19:59:49 habenich Exp $

#include <kdebug.h>
#include <qpushbt.h>
#include <qlayout.h>
#include <klocale.h>
#include <kapp.h>
#include "formatdialog.h"
#include "formats.h"

extern FormatManager *formatMngr;

formatDialog::formatDialog(const char *format, QWidget* parent, const char* name)
  : QDialog(parent, name, TRUE)
{
  QPushButton *okButton;
  QPushButton *cancelButton;
  QLabel *tmpLabel;

  // Create top level layout
  QVBoxLayout *topLevelLayout= new QVBoxLayout(this, 4);
  QHBoxLayout *selectionLayout= new QHBoxLayout();
  QHBoxLayout *buttonLayout= new QHBoxLayout();
  topLevelLayout->addLayout(selectionLayout);
  topLevelLayout->addLayout(buttonLayout);

  tmpLabel = new QLabel( this, "Label_7" );
  tmpLabel->setGeometry( 10, 10, 80, 30 );
  tmpLabel->setText( klocale->translate("File Format:") );

  fileformat = new QComboBox( FALSE, this, "ComboBox_1" );
  fileformat->setSizeLimit( 10 );
  fileformat->setAutoResize( FALSE );
  fileformat->insertStrList(formatMngr->formats());
  fileformat->setCurrentItem(formatMngr->formats()->find(format));
  fileformat->setFixedHeight(fileformat->sizeHint().height());

  selectionLayout->addWidget(tmpLabel);
  selectionLayout->addWidget(fileformat);

  okButton= new QPushButton(klocale->translate("OK"), this);
  cancelButton= new QPushButton(klocale->translate("Cancel"), this);
  okButton->setFixedHeight(okButton->sizeHint().height());
  okButton->setMinimumWidth(okButton->sizeHint().width());
  cancelButton->setFixedHeight(cancelButton->sizeHint().height());
  cancelButton->setMinimumWidth(okButton->sizeHint().width());

  buttonLayout->addStretch();
  buttonLayout->addWidget(okButton);
  buttonLayout->addWidget(cancelButton);
 
  resize( 240, 80 );

  connect(okButton, SIGNAL(clicked()), SLOT(accept()) );
  connect(cancelButton, SIGNAL(clicked()), SLOT(reject()) );

  // Now start things off
  topLevelLayout->activate();
}

#include "formatdialog.moc"
