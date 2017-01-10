/**********************************************************************

	--- Qt Architect generated file ---

	File: FirstStartData.cpp
	Last generated: Sat Feb 7 11:11:14 1998

 *********************************************************************/

#include "FirstStartData.h"
#include "FirstStartData.moc"

#define Inherited QDialog

#include <qlabel.h>
#include <qbttngrp.h>

#include <kapp.h>

FirstStartData::FirstStartData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE, 0 )
{
	QButtonGroup* dlgedit_ButtonGroup_1;
	dlgedit_ButtonGroup_1 = new QButtonGroup( this, "ButtonGroup_1" );
	dlgedit_ButtonGroup_1->setGeometry( 88, 136, 128, 112 );
	dlgedit_ButtonGroup_1->setMinimumSize( 10, 10 );
	dlgedit_ButtonGroup_1->setMaximumSize( 32767, 32767 );
	dlgedit_ButtonGroup_1->setTitle( "" );
	dlgedit_ButtonGroup_1->setAlignment( 1 );
	dlgedit_ButtonGroup_1->setFrameStyle( QFrame::NoFrame );

	QLabel* dlgedit_Label_1;
	dlgedit_Label_1 = new QLabel( this, "Label_1" );
	dlgedit_Label_1->setGeometry( 24, 16, 264, 120 );
	dlgedit_Label_1->setMinimumSize( 10, 10 );
	dlgedit_Label_1->setMaximumSize( 32767, 32767 );
	dlgedit_Label_1->setText( klocale->translate("Please select the spooling system you use. Most systems use the BSD spooler. So if you don't know which spooler is installed on your system, BSD is probably a good choice.") );
	dlgedit_Label_1->setAlignment( 1313 );
	dlgedit_Label_1->setMargin( -1 );

	b_ok = new QPushButton( this, "PushButton_1" );
	b_ok->setGeometry( 104, 256, 100, 24 );
	b_ok->setMinimumSize( 10, 10 );
	b_ok->setMaximumSize( 32767, 32767 );
	b_ok->setText( klocale->translate("OK") );
	b_ok->setAutoRepeat( FALSE );
	b_ok->setAutoResize( FALSE );
	b_ok->setDefault( TRUE );
	b_ok->setAutoDefault( TRUE );

	rb_bsd = new QRadioButton( this, "RadioButton_1" );
	rb_bsd->setGeometry( 120, 144, 100, 30 );
	rb_bsd->setMinimumSize( 10, 10 );
	rb_bsd->setMaximumSize( 32767, 32767 );
	rb_bsd->setText( "BSD" );
	rb_bsd->setAutoRepeat( FALSE );
	rb_bsd->setAutoResize( FALSE );
	rb_bsd->setChecked( TRUE );

	rb_ppr = new QRadioButton( this, "RadioButton_2" );
	rb_ppr->setGeometry( 120, 176, 100, 30 );
	rb_ppr->setMinimumSize( 10, 10 );
	rb_ppr->setMaximumSize( 32767, 32767 );
	rb_ppr->setText( "PPR" );
	rb_ppr->setAutoRepeat( FALSE );
	rb_ppr->setAutoResize( FALSE );

	rb_lprng = new QRadioButton( this, "RadioButton_3" );
	rb_lprng->setGeometry( 120, 208, 100, 30 );
	rb_lprng->setMinimumSize( 10, 10 );
	rb_lprng->setMaximumSize( 32767, 32767 );
	rb_lprng->setText( "LPRNG" );
	rb_lprng->setAutoRepeat( FALSE );
	rb_lprng->setAutoResize( FALSE );

	dlgedit_ButtonGroup_1->insert( rb_bsd );
	dlgedit_ButtonGroup_1->insert( rb_ppr );
	dlgedit_ButtonGroup_1->insert( rb_lprng );

	resize( 304,300 );
	setMinimumSize( 304, 300 );
	setMaximumSize( 304, 300 );
}


FirstStartData::~FirstStartData()
{
}
