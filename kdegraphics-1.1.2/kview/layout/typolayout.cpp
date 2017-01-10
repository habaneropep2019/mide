/*
* typolayout.cpp -- Implementation of class KTypoLayout.
* Author:	Sirtaj Singh Kang
* Version:	$Id: typolayout.cpp,v 1.4 1999/01/19 02:12:31 ssk Exp $
* Generated:	Tue May  5 01:12:37 EST 1998
*/

#include"baglayout.h"
#include"typolayout.h"

#include<assert.h>

#include<qpushbt.h>
#include<qchkbox.h>
#include<qlabel.h>
#include<qlined.h>
#include<qmlined.h>
#include<qlistbox.h>
#include<qradiobt.h>
#include<qgroupbox.h>
#include<qbuttongroup.h>

KTypoLayout::KTypoLayout( QWidget *parent )
	: KBagLayout( parent )
{
	assert( parent );

	_parent = parent;
	_group = 0;
}

QLabel *KTypoLayout::newLabel( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QLabel *w = new QLabel( text, _parent );
	
	addWidget( w, x, y, xspan, yspan );

	return w;

}

QPushButton *KTypoLayout::newButton( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QPushButton *w = new QPushButton( text, _parent );
	addWidget( w, x, y, xspan, yspan );

	if( _group ) {
		_group->insert( w );
	}

	return w;
}

QRadioButton *KTypoLayout::newRadioButton( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QRadioButton *w = new QRadioButton ( text, _parent );
	addWidget( w, x, y, xspan, yspan );

	if( _group ) {
		_group->insert( w );
	}

	return w;
}

QLineEdit *KTypoLayout::newLineEdit( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QLineEdit * w = new QLineEdit( _parent );
	w->setText( text );

	KBagConstraints *c = addWidget( w, x, y, xspan, yspan );

	// line edits should only be resize horizontally by default

	c->setResizePolicy( KBagConstraints::Horizontal );

	return w;
}


QMultiLineEdit *KTypoLayout::newMultiLineEdit( const char *text, 
		int x, int y, int xspan, int yspan )
{
	QMultiLineEdit * w = new QMultiLineEdit( _parent );
	w->setText( text );

	addWidget( w, x, y, xspan, yspan );

	return w;
}

QCheckBox *KTypoLayout::newCheckBox( const char *text, int x, int y, 
			int xspan, int yspan )
{
	QCheckBox *w = new QCheckBox ( text, _parent );
	addWidget( w, x, y, xspan, yspan );

	if( _group ) {
		_group->insert( w );
	}

	return w;
}

QListBox *KTypoLayout::newListBox( int x, int y, int xspan, int yspan)
{
	QListBox *w = new QListBox( _parent );

	addWidget( w, x, y, xspan, yspan );

	return w;
}

KTypoLayout *KTypoLayout::newSubLayout( int x, int y, int xspan, int yspan )
{
	QWidget *holder = new QWidget( _parent );

	addWidget( holder, x, y, xspan, yspan );

	KTypoLayout *sublayout = new KTypoLayout( holder );

	return sublayout;
}


KTypoLayout *KTypoLayout::newSubGroup( const char *title, int x, int y, 
	int xspan, int yspan )
{
	QGroupBox *holder = new QGroupBox( title, _parent );
	holder->setLineWidth( 1 );
	holder->setFrameStyle( QFrame::Box | QFrame::Raised );

	addWidget( holder, x, y, xspan, yspan );

	KTypoLayout *sublayout = new KTypoLayout( holder );
	sublayout->setSpaceContents( holder );

	return sublayout;
}

QButtonGroup *KTypoLayout::startGroup()
{
	_group = new QButtonGroup( _parent );
	_group->hide();

	return _group;
}

void KTypoLayout::endGroup()
{
	_group = 0;
}
