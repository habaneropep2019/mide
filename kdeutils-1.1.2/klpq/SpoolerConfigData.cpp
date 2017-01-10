/**********************************************************************

	--- Qt Architect generated file ---

	File: SpoolerConfigData.cpp
	Last generated: Sat Feb 7 11:59:44 1998

 *********************************************************************/

#include "SpoolerConfigData.h"
#include "SpoolerConfigData.moc"

#define Inherited QDialog

#include <qlabel.h>
#include <qframe.h>

#include <kapp.h>

SpoolerConfigData::SpoolerConfigData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE, 0 )
{
	bg_spooler = new QButtonGroup( this, "ButtonGroup_1" );
	bg_spooler->setGeometry( 16, 56, 80, 128 );
	bg_spooler->setMinimumSize( 10, 10 );
	bg_spooler->setMaximumSize( 32767, 32767 );
	bg_spooler->setTitle( "" );
	bg_spooler->setAlignment( 1 );
	bg_spooler->setFrameStyle( QFrame::NoFrame );

	rb_bsd = new QRadioButton( this, "RadioButton_1" );
	rb_bsd->setGeometry( 24, 72, 64, 24 );
	rb_bsd->setMinimumSize( 10, 10 );
	rb_bsd->setMaximumSize( 32767, 32767 );
	rb_bsd->setText( "BSD" );
	rb_bsd->setAutoRepeat( FALSE );
	rb_bsd->setAutoResize( FALSE );

	rb_ppr = new QRadioButton( this, "RadioButton_3" );
	rb_ppr->setGeometry( 24, 104, 60, 24 );
	rb_ppr->setMinimumSize( 10, 10 );
	rb_ppr->setMaximumSize( 32767, 32767 );
	rb_ppr->setText( "PPR" );
	rb_ppr->setAutoRepeat( FALSE );
	rb_ppr->setAutoResize( FALSE );

	rb_lprng = new QRadioButton( this, "RadioButton_2" );
	rb_lprng->setGeometry( 24, 136, 70, 24 );
	rb_lprng->setMinimumSize( 10, 10 );
	rb_lprng->setMaximumSize( 32767, 32767 );
	rb_lprng->setText( "LPRNG" );
	rb_lprng->setAutoRepeat( FALSE );
	rb_lprng->setAutoResize( FALSE );

	QFrame* dlgedit_Frame_1;
	dlgedit_Frame_1 = new QFrame( this, "Frame_1" );
	dlgedit_Frame_1->setGeometry( 96, 56, 24, 128 );
	dlgedit_Frame_1->setMinimumSize( 10, 10 );
	dlgedit_Frame_1->setMaximumSize( 32767, 32767 );
	dlgedit_Frame_1->setFrameStyle( 53 );

	QLabel* dlgedit_Label_1;
	dlgedit_Label_1 = new QLabel( this, "Label_1" );
	dlgedit_Label_1->setGeometry( 120, 152, 32, 24 );
	dlgedit_Label_1->setMinimumSize( 10, 10 );
	dlgedit_Label_1->setMaximumSize( 32767, 32767 );
	dlgedit_Label_1->setText( "lprm:" );
	dlgedit_Label_1->setAlignment( 290 );
	dlgedit_Label_1->setMargin( -1 );

	QLabel* dlgedit_Label_2;
	dlgedit_Label_2 = new QLabel( this, "Label_2" );
	dlgedit_Label_2->setGeometry( 120, 120, 32, 24 );
	dlgedit_Label_2->setMinimumSize( 10, 10 );
	dlgedit_Label_2->setMaximumSize( 32767, 32767 );
	dlgedit_Label_2->setText( "lpc:" );
	dlgedit_Label_2->setAlignment( 290 );
	dlgedit_Label_2->setMargin( -1 );

	QLabel* dlgedit_Label_3;
	dlgedit_Label_3 = new QLabel( this, "Label_3" );
	dlgedit_Label_3->setGeometry( 120, 88, 32, 24 );
	dlgedit_Label_3->setMinimumSize( 10, 10 );
	dlgedit_Label_3->setMaximumSize( 32767, 32767 );
	dlgedit_Label_3->setText( "lpq:" );
	dlgedit_Label_3->setAlignment( 290 );
	dlgedit_Label_3->setMargin( -1 );

	QLabel* dlgedit_Label_4;
	dlgedit_Label_4 = new QLabel( this, "Label_4" );
	dlgedit_Label_4->setGeometry( 120, 56, 216, 24 );
	dlgedit_Label_4->setMinimumSize( 10, 10 );
	dlgedit_Label_4->setMaximumSize( 32767, 32767 );
	dlgedit_Label_4->setText( klocale->translate("Paths of spooling commands:") );
	dlgedit_Label_4->setAlignment( 289 );
	dlgedit_Label_4->setMargin( -1 );

	i_lpq = new QLineEdit( this, "LineEdit_1" );
	i_lpq->setGeometry( 160, 88, 176, 24 );
	i_lpq->setMinimumSize( 10, 10 );
	i_lpq->setMaximumSize( 32767, 32767 );
	i_lpq->setText( "" );
	i_lpq->setMaxLength( 32767 );
	i_lpq->setEchoMode( QLineEdit::Normal );
	i_lpq->setFrame( TRUE );

	i_lpc = new QLineEdit( this, "LineEdit_2" );
	i_lpc->setGeometry( 160, 120, 176, 24 );
	i_lpc->setMinimumSize( 10, 10 );
	i_lpc->setMaximumSize( 32767, 32767 );
	i_lpc->setText( "" );
	i_lpc->setMaxLength( 32767 );
	i_lpc->setEchoMode( QLineEdit::Normal );
	i_lpc->setFrame( TRUE );

	i_lprm = new QLineEdit( this, "LineEdit_3" );
	i_lprm->setGeometry( 160, 152, 176, 24 );
	i_lprm->setMinimumSize( 10, 10 );
	i_lprm->setMaximumSize( 32767, 32767 );
	i_lprm->setText( "" );
	i_lprm->setMaxLength( 32767 );
	i_lprm->setEchoMode( QLineEdit::Normal );
	i_lprm->setFrame( TRUE );

	b_ok = new QPushButton( this, "PushButton_1" );
	b_ok->setGeometry( 40, 216, 100, 24 );
	b_ok->setMinimumSize( 10, 10 );
	b_ok->setMaximumSize( 32767, 32767 );
	b_ok->setText( klocale->translate("OK") );
	b_ok->setAutoRepeat( FALSE );
	b_ok->setAutoResize( FALSE );

	b_cancel = new QPushButton( this, "PushButton_2" );
	b_cancel->setGeometry( 208, 216, 100, 24 );
	b_cancel->setMinimumSize( 10, 10 );
	b_cancel->setMaximumSize( 32767, 32767 );
	b_cancel->setText( klocale->translate("Cancel") );
	b_cancel->setAutoRepeat( FALSE );
	b_cancel->setAutoResize( FALSE );

	QLabel* dlgedit_Label_5;
	dlgedit_Label_5 = new QLabel( this, "Label_5" );
	dlgedit_Label_5->setGeometry( 8, 16, 328, 30 );
	dlgedit_Label_5->setMinimumSize( 10, 10 );
	dlgedit_Label_5->setMaximumSize( 32767, 32767 );
	dlgedit_Label_5->setText( klocale->translate("Select your spooling system") );
	dlgedit_Label_5->setAlignment( 289 );
	dlgedit_Label_5->setMargin( -1 );

	bg_spooler->insert( rb_bsd, 0 );
	bg_spooler->insert( rb_ppr, 1 );
	bg_spooler->insert( rb_lprng, 2 );


	resize( 352,264 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


SpoolerConfigData::~SpoolerConfigData()
{
}
