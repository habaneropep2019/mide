/****************************************************************************
**
** A dialog for the selection of the view of a document.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qlayout.h>

#include "viewcontrol.h"
#include "viewcontrol.moc"

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

ViewControl::ViewControl( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 10 );
		
	QGroupBox* vcGroupBox;
	vcGroupBox = new QGroupBox(  this );
	vcGroupBox->setFrameStyle( QFrame::NoFrame );
	//vcGroupBox->setTitle( i18n("Force changes to ...") );
	//vcGroupBox->setAlignment( 1 );
	
	topLayout->addWidget( vcGroupBox, 10 );
	
	QGridLayout *grid = new QGridLayout( vcGroupBox, 2, 2, 10 );
	
	grid->setRowStretch(0,0);
	grid->setRowStretch(1,10);
	

	grid->setColStretch(0,0);
	grid->setColStretch(1,10);
	

	magComboBox = new QComboBox( FALSE, vcGroupBox );
	magComboBox->setFixedHeight( magComboBox->sizeHint().height() );
	connect ( magComboBox, SIGNAL (activated (int)),
		  SLOT (disableApply (int)) );
	magComboBox->hide();

	//grid->addWidget( magComboBox, 0, 1 );
	
	mediaComboBox = new QComboBox( FALSE, vcGroupBox );
	mediaComboBox->setFixedHeight( magComboBox->sizeHint().height() );
	connect ( mediaComboBox, SIGNAL (activated (int)),
		  SLOT (disableApply (int)) );
	
	grid->addWidget( mediaComboBox, 0, 1 );
	
	orientComboBox = new QComboBox( FALSE, vcGroupBox );
	orientComboBox->insertItem(i18n("Portrait"));
	orientComboBox->insertItem(i18n("Landscape"));
	orientComboBox->insertItem(i18n("Upside down"));
	orientComboBox->insertItem(i18n("Seascape"));
	orientComboBox->setFixedHeight( magComboBox->sizeHint().height() );
	connect ( orientComboBox, SIGNAL (activated (int)),
		  SLOT (disableApply (int)) );
					
	grid->addWidget( orientComboBox, 1, 1 );
		
	int labelWidth = 0;

	QLabel* vcLabel;
	vcLabel = new QLabel( magComboBox, i18n("magnification"), vcGroupBox );
	vcLabel->setAlignment( AlignRight | AlignVCenter | ShowPrefix );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );

	vcLabel->hide();

	//grid->addWidget( vcLabel, 0, 0 );

	vcLabel = new QLabel( mediaComboBox, i18n("&Media"), vcGroupBox );
	vcLabel->setAlignment( AlignRight | AlignVCenter | ShowPrefix );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );
	
	grid->addWidget( vcLabel, 0, 0 );
	
	vcLabel = new QLabel( orientComboBox, i18n("&Orientation"), vcGroupBox );
	vcLabel->setAlignment( AlignRight | AlignVCenter | ShowPrefix );
	if ( vcLabel->sizeHint().width() > labelWidth )
		labelWidth = vcLabel->sizeHint().width();
	vcLabel->setMinimumWidth( labelWidth );
	
	grid->addWidget( vcLabel, 1, 0 );
	
	vcGroupBox->setMinimumHeight( 2*orientComboBox->sizeHint().height()+20 );
	vcGroupBox->setMinimumWidth(
		40 + labelWidth + orientComboBox->sizeHint().width() );
	
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	tmpQFrame->setMinimumHeight( tmpQFrame->sizeHint().height() );
	
	topLayout->addWidget( tmpQFrame );
	
	// CREATE BUTTONS
	
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );

	okButton = bbox->addButton( i18n("&OK") );
	connect( okButton, SIGNAL(clicked()), SLOT(slotOKClicked()) );

	applyButton = bbox->addButton( i18n("&Apply") );
	connect( applyButton, SIGNAL(clicked()), SLOT(slotApplyClicked()) );

	QPushButton *close = bbox->addButton( i18n("&Close") );
	connect( close, SIGNAL(clicked()), SLOT(reject()) );


	bbox->layout();
	topLayout->addWidget( bbox );

	topLayout->activate();

	applyButton->setEnabled (false);
	okButton->setEnabled (false);
	
	resize( 250 ,0);
}

void
ViewControl::slotApplyClicked()
{
  emit applyChanges();
  applyButton->setEnabled (false);
  okButton->setEnabled (false);
}


void
ViewControl::slotOKClicked()
{
  emit applyChanges();
  hide();
}

void
ViewControl::updateOrientation(int orientation)
{
  orientComboBox->setCurrentItem (orientation);
}

void
ViewControl::disableApply (int)
{
  applyButton->setEnabled (true);
  okButton->setEnabled (true);
}
