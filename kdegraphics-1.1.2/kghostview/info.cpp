/****************************************************************************
**
** A dialog for the display of information about a PostScript document.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "info.h"
#include "info.moc"

#include <qaccel.h>
#include <qlayout.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

InfoDialog::InfoDialog( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 10 );
	
	//topLayout->addStretch( 10 );
	
	QGridLayout *grid = new QGridLayout( 5, 3, 5 );
	topLayout->addLayout( grid );
	
	grid->addRowSpacing(0,0);
	grid->addRowSpacing(4,0);
	
	grid->setColStretch(2,10);
	
	QLabel *tmpQLabel;
	tmpQLabel = new QLabel( this );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("File name :"));
	tmpQLabel->setMinimumHeight( tmpQLabel->sizeHint().height() );
	tmpQLabel->setMinimumWidth( tmpQLabel->sizeHint().width() + 15 );
	
	grid->addWidget( tmpQLabel, 1, 0 );
	
	tmpQLabel = new QLabel( this );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("Document title :"));
	tmpQLabel->setMinimumHeight( tmpQLabel->sizeHint().height() );
	tmpQLabel->setMinimumWidth( tmpQLabel->sizeHint().width() + 15 );
	
	grid->addWidget( tmpQLabel, 2, 0 );
	
	tmpQLabel = new QLabel( this );
	tmpQLabel->setAlignment( AlignRight|AlignVCenter );
	tmpQLabel->setText(i18n("Publication date :"));
	tmpQLabel->setMinimumHeight( tmpQLabel->sizeHint().height() );
	tmpQLabel->setMinimumWidth( tmpQLabel->sizeHint().width() + 15 );
		
	grid->addWidget( tmpQLabel, 3, 0 );
	
	fileLabel = new QLabel( this );
	fileLabel->setAlignment( AlignLeft|AlignVCenter );
	
	grid->addWidget( fileLabel, 1, 1 );
	
	titleLabel = new QLabel( this );
	titleLabel->setAlignment( AlignLeft|AlignVCenter );
	
	grid->addWidget( titleLabel, 2, 1 );
	
	dateLabel = new QLabel( this );
	dateLabel->setAlignment( AlignLeft|AlignVCenter );
	
	grid->addWidget( dateLabel, 3, 1 );
	
	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( this );
	tmpQFrame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
	tmpQFrame->setMinimumHeight( tmpQFrame->sizeHint().height() );
	
	topLayout->addWidget( tmpQFrame );
	
	// CREATE BUTTONS
	
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );

	ok = bbox->addButton( i18n("OK") );
	ok->setDefault( TRUE );
	connect( ok, SIGNAL(clicked()), SLOT(reject()) );
	
	bbox->layout();
	topLayout->addWidget( bbox );
	
	topLayout->activate();
	
	resize( 250, 0 );
}




