/*
* numdlg.cpp -- Implementation of class KNumDialog.
* Author:	Sirtaj Singh Kang
* Version:	$Id: numdlg.cpp,v 1.2 1998/10/16 04:07:41 ssk Exp $
* Generated:	Fri Oct  9 01:51:29 EST 1998
*/

#include<stdlib.h>

#include<qpushbt.h>
#include<qlined.h>
#include<qlabel.h>

#include"typolayout.h"

#include"numdlg.h"


KNumDialog::KNumDialog( QWidget *parent )
	: QDialog( parent, "", true )
{
	KTypoLayout *layout = new KTypoLayout( this );
	layout->setGridSize( 2, 3 );
	layout->setHSpace( 2, 2 );
	layout->setVSpace( 2, 2 );
	
	_message = layout->newLabel( "Enter a value:", 
			0, 0, 2, 1 );
	_edit = layout->newLineEdit( "", 0, 1, 2, 1 );
	connect( _edit, SIGNAL(returnPressed()), this, SLOT(accept()));
	_edit->setFocus();
	
	QPushButton *ok = layout->newButton( "OK", 0, 2, 1, 1 );
	connect( ok, SIGNAL(clicked()), SLOT(accept()) );
	KBagConstraints *c = layout->constraints( ok );
	c->setXSpace( 2 );
	c->setYSpace( 4 );

	QPushButton *canc = layout->newButton( "Cancel", 1, 2, 1, 1 );
	c = layout->constraints( canc );
	c->setXSpace( 2 );
	c->setYSpace( 4 );
	connect( canc, SIGNAL(clicked()), SLOT(reject()) );

	resize( 200, 120 );
}

KNumDialog::~KNumDialog()
{
}

bool KNumDialog::getNum( int& num, const char *message )
{
	_message->setText( message );
	QString numtxt;
	numtxt.setNum( num );
	_edit->setText( numtxt );

	if ( exec() ) {
		num = atoi( _edit->text() );
		return true;
	}

	return false;
}

bool KNumDialog::getNum( double& num, const char *message )
{
	_message->setText( message );
	QString numtxt;
	numtxt.setNum( num );
	_edit->setText( numtxt );

	if ( exec() ) {
		num = atof( _edit->text() );
		return true;
	}

	return false;
}
