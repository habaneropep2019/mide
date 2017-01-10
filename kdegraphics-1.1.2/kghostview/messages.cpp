/****************************************************************************
**
** A dialog for the display of error messages from Ghostscript.
**
** Copyright (C) 1997 by Mark Donohoe.
** Based on original work by Tim Theisen.
**
** This code is freely distributable under the GNU Public License.
**
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "messages.h"
#include "messages.moc"

#include <qaccel.h>
#include <qlayout.h>

#include <klocale.h>
#include <kapp.h>
#include <kbuttonbox.h>

MessagesDialog::MessagesDialog( QWidget *parent, const char *name )
	: QDialog( parent, name )
{
	setFocusPolicy(QWidget::StrongFocus);
	setCaption(i18n("Ghostscript messages"));
	
	QBoxLayout *topLayout = new QVBoxLayout( this, 0, 10 );

	frame = new QFrame ( this );
	if ( style() == WindowsStyle ) {
		frame ->setFrameStyle( QFrame::WinPanel | QFrame::Sunken);
	} else {
		frame ->setFrameStyle( QFrame::Panel | QFrame::Sunken);
   		frame ->setLineWidth(2);
	}
	
	topLayout->addWidget( frame, 10 );
	
	messageBox = new QMultiLineEdit( frame );
	messageBox->setFrameStyle( QFrame::NoFrame );
	messageBox->setFont( kapp->fixedFont );
	messageBox->setReadOnly( TRUE );
	
	QFontMetrics fm( kapp->fixedFont );
	frame->setMinimumWidth( 80*fm.width(" ") );
	
	// CREATE BUTTONS
	
	KButtonBox *bbox = new KButtonBox( this );
	bbox->addStretch( 10 );

	QPushButton* clear = bbox->addButton( i18n("&Clear") );
	connect( clear, SIGNAL(clicked()), SLOT(clear()) );

	cancel = bbox->addButton( i18n("&Dismiss") );
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );

	bbox->layout();
	
	QBoxLayout *pushLayout = new QHBoxLayout( 10 );
	topLayout->addLayout( pushLayout, 0 );
	topLayout->addSpacing( 10 );
	
	pushLayout->addWidget( bbox, 0 );
	pushLayout->addSpacing( 10 );
	
	
	topLayout->activate();
	
	resize( 250, 250 );
}

void MessagesDialog::clear() {
	messageBox->setText( "" );
}

void MessagesDialog::resizeEvent( QResizeEvent * )
{
	messageBox->setGeometry( 2, 2, frame->width()-4, frame->height()-4 );
} 


