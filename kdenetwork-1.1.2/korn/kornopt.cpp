/**********************************************************************

	--- Dlgedit generated file ---

	File: kornopt.cpp
	Last generated: Mon Jun 2 20:20:22 1997

 *********************************************************************/

#include<stdlib.h>
#include<kapp.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include "kornopt.h"

#define Inherited KornOptData

KornOptions::KornOptions ( QWidget* parent, const char* name)
	: Inherited( parent, name )
{
	setCaption( i18n("korn options") );

	_list = 0;
	_highlighted = false;

	if( boxList->count() != 0) {
		boxList->setCurrentItem( 0 );
		boxList->setFocus();
		boxHighlight( 0 );
	}
}


KornOptions::~KornOptions()
{
}

void KornOptions::cancelClicked()
{
	hide();
	emit finished( 0 );
}

void KornOptions::boxHighlight(int item)
{
	KornBox *box;

	updateList();

	// load data from new box into dialog
	box = _list->at( item );

	if( box != 0 ) {
		QString str(5);

		boxName->setText( box->name );
		boxCaption->setText( box->caption);
		boxPath->setText( box->path );
		notifyCmd->setText( box->notify );
		clickCmd->setText( box->click );
		pollTime->setText( str.setNum( box->poll ) );

		_highlighted = true;
	}
}

void KornOptions::boxSelect(int)
{
}

void KornOptions::newBox()
{
	int item;
	KornBox *newBox = new KornBox;

	newBox->name= i18n("Untitled");
	newBox->poll= 240; // default 4min poll

	updateList();

	item = boxList->currentItem();
	boxList->insertItem( i18n("Untitled"), item );

	_list->insert( item, newBox );
	_list->at( item );

	_highlighted = false;
	boxList->setCurrentItem( item );
	boxHighlight( item );
}

void KornOptions::okClicked()
{
	updateList();
	hide();

	emit finished( 1 );
}

void KornOptions::deleteBox()
{
	if( boxList->currentItem() == -1 )
		return;

	_list->setAutoDelete( TRUE );
	_list->remove( boxList->currentItem() );

	boxList->removeItem( boxList->currentItem() );

	boxHighlight( boxList->currentItem() );
}

void KornOptions::setList( QList<KornBox> *list ) {
	_list = list;

	boxList->clear();
	_list->first();
	
	while( _list->at() != -1 ) {
		boxList->insertItem( _list->current()->name );
		_list->next();
	}

	_list->at( 0 );

	boxHighlight( 0 );
}

void KornOptions::dataChanged()
{
	QLineEdit *focus;

	if( _list->current() !=  0 ) {

		if ( qApp->focusWidget()->isA("QLineEdit" ) ){
			focus = (QLineEdit *)qApp->focusWidget();

			focus->selectAll();
		}
	}
}

void KornOptions::updateList()
{
	if ( !_highlighted )
		return;

	KornBox *box = _list->current();

	if( box != 0 ) {
		// copy back now
		boxList->changeItem( boxName->text(), _list->at() );
		box->name	= boxName->text();
		box->caption	= boxCaption->text();
		box->path	= boxPath->text();
		box->notify	= notifyCmd->text();
		box->click	= clickCmd->text();
		box->poll	= atoi( pollTime->text() );
	}
}
