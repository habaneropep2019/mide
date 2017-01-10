/*
* filtmenu.cpp -- Implementation of class KFiltMenuFactory.
* Author:	Sirtaj Singh Kang
* Version:	$Id: filtmenu.cpp,v 1.2 1998/10/08 14:42:45 kulow Exp $
* Generated:	Fri Sep 25 05:13:29 EST 1998
*/

#include"filtmenu.h"
#include"filtlist.h"
#include"filter.h"

#include<qstrlist.h>

#include<assert.h>

static int tokenize( QStrList& tokens, const QString& str, 
	const char *separators );

KFiltMenuFactory::KFiltMenuFactory( KFilterList *filters,
		QObject *parent )
	: QObject( parent ),
	_filters ( filters ),
	_menu ( 0 ),
	_lastMenuID( 1000 ),
	_menuList( new QList<QPopupMenu> ),
	_menuDict( new QDict<QPopupMenu> )
{
	assert( _menuList );
	_menuList->setAutoDelete( true );
	assert( _menuDict );
	_menuDict->setAutoDelete( false );
}

KFiltMenuFactory::~KFiltMenuFactory()
{
	delete _menu;
	delete _menuList;
	delete _menuDict;
}

QPopupMenu *KFiltMenuFactory::filterMenu()
{
	if( _menu == 0 ) {
		updateMenu();
	}

	return _menu;
}

void KFiltMenuFactory::updateMenu()
{
	// remove old menus
	_menuDict->clear();
	_menuList->clear();

	// create new ones

	if( _menu == 0 ) {
		_menu = new QPopupMenu( 0, "Filters" );
		connect( _menu, SIGNAL(activated(int)),
			this, SLOT(raiseFilter(int)) );
	}
	else {
		_menu->clear();
	}

	// find or create path to dest menu, starting at _menu
	for( int i = 0; i < _filters->count(); i++ ) {
		KImageFilter *filter = _filters->filter( i );

		QString token, name;

		QPopupMenu *menu = getItemMenu( name, _menu,
			filter->name() );
	
		if( menu == 0 ) {
			warning( "couldn't get menu for %s",
				filter->name() );
			continue;
		}

		menu->insertItem( name, i );

	}
}

void KFiltMenuFactory::raiseFilter( int index )
{
	KImageFilter *filter = _filters->filter( index );

	if( filter == 0 ) {
		warning( "KFiltMenuFactory:: raiseFilter: "
			"invalid index %d", index );
		return;
	}

	emit selected( filter );
}

static int tokenize( QStrList& tokens, const QString& str, 
	const char *separators )
{
	tokens.clear();
	
	if( str.length() == 0 ) {
		return 0;
	}

	unsigned start = 0, i = 0;

	for( ; i < str.length(); i++ ) {
		if ( strchr( separators, str[ i ] ) != 0 ) {
			// found separator
			tokens.append( str.mid( start, i - start ) );
			start = i + 1;
		}
	}

	if ( start < i ) {
		tokens.append( str.mid( start, i - start) );
	}

	return tokens.count();
}

QPopupMenu *KFiltMenuFactory::getItemMenu( QString& name, QPopupMenu *parent, 
	const QString& fullname )
{
	QPopupMenu *menu = 0;
	QStrList tokens;

	int count = tokenize( tokens, fullname, ":" );
	
	if( count <= 1 ) {
		name = count ? fullname.data() : "";
		return parent;
	}

	QString accum;
	int i = 0;

	for( ; i < count - 1; i++ ) {
		accum += tokens.at( i );
		accum += ':';
		menu = _menuDict->find( accum );

		if( menu == 0 ) {
			menu = new QPopupMenu;
			_menuList->append( menu );
			_menuDict->insert( accum, menu );

			connect( menu, SIGNAL(activated(int)),
					this, SLOT(raiseFilter(int)) );

			parent->insertItem( tokens.current(), menu,
				_lastMenuID++ );

			parent = menu;
		}
	}

	name = tokens.at( i );

	return menu;
}
