/*
* gridbagl.cpp -- Implementation of class KGridBagLayout.
* Author:	Sirtaj Singh Kang
* Version:	$Id: baglayout.cpp,v 1.3 1999/01/10 18:17:02 ssk Exp $
* Generated:	Mon May  4 15:57:51 EST 1998
*/

#include<assert.h>
#include<qframe.h>

#include"baglayout.h"

KBagLayout::KBagLayout( QWidget *parent, int x, int y )
	: QObject( parent ), 
	_sizeSet ( false ), _grid( x, y ),  _min( 0, 0 ),
	_topSpace( 2 ),	_bottomSpace( 2 ),
	_leftSpace( 2 ), _rightSpace( 2 )
{
	assert( parent );
	_parent = parent;

	_cstr = new QPtrDict<KBagConstraints>;
	_cstr->setAutoDelete( true );
	
	_widgets = new QList<QWidget>;
	_widgets->setAutoDelete( false );

	if( x != 0 || y != 0 ) {
		_sizeSet = true;
	}

	_parent->installEventFilter( this );

	updateContentRect();
}

KBagLayout::~KBagLayout()
{
	delete _cstr;
	delete _widgets;
}

KBagConstraints *KBagLayout::addWidget( QWidget *widget )
{
	KBagConstraints * c = new KBagConstraints;

	_widgets->append( widget );
	_cstr->insert( widget, c );

	QSize hint = widget->sizeHint();

	if( hint.isValid() ) {
		widget->setMinimumSize( hint );
	}

	updateMinSize();

	return c;
}

KBagConstraints *KBagLayout::addWidget( QWidget *widget, int x, int y,
				     int xspan, int yspan )
{
	KBagConstraints * c = addWidget( widget );

	c->setGridX( x );
	c->setGridY( y );
	c->setXSpan( xspan );
	c->setYSpan( yspan );

	return c;
}


KBagConstraints *KBagLayout::addWidget( QWidget *widget, 
		KBagConstraints& constr )
{
	KBagConstraints * c = new KBagConstraints( constr );

	_widgets->append( widget );
	_cstr->insert( widget, c );

	return c;
}

void KBagLayout::activate()
{
	if ( _sizeSet == false ) {
		calcGridExtent();
	}

	if( !_grid.isValid() ) {
		warning( "KBagLayout: illegal layout constraints." );
		return;
	}

	// arrange each widget

	QListIterator<QWidget> list( *_widgets );

	for( ; list.current(); ++list ) {
		arrangeWidget( list.current(), 
			(KBagConstraints *)
			_cstr->find ( list.current() ) );
	}

#if 0
	QRect c = _parent->childrenRect();

	if( c.isValid() ) {
		_parent->setMinimumSize( c.width(), c.height() );
	}
#endif

}

void KBagLayout::calcGridExtent()
{
	QListIterator<QWidget> list( *_widgets );

	_grid.setWidth(  0 );
	_grid.setHeight(  0 );

	for( ; list.current(); ++list ) {
		KBagConstraints *c = constraints( list.current() );
		int cx = c->gridX() + c->xSpan();
		int cy = c->gridY() + c->ySpan();

		if( cx > _grid.width() ) {
			_grid.setWidth(  cx );
		}

		if( cy > _grid.height() ) {
			_grid.setHeight(  cy );
		}
	}
}

void KBagLayout::arrangeWidget( QWidget *w, KBagConstraints *c )
{
	assert ( w );
	assert ( c );

	if( _grid.width() == 0 || _grid.height() == 0 ) {
		warning( "KBagLayout::arrangeWidget called with "
				"invalid grid size (%d,%d)",
				_grid.width(), _grid.height() );
	}

	int x=0, y=0, width=0, height=0;

	int cellx = _contents.width()/_grid.width();
	int celly = _contents.height()/_grid.height();

	_min.setWidth ( 0 );
	_min.setHeight( 0 );

	//
	// X layout
	//
	
	if( c->resizePolicy() & KBagConstraints::Horizontal ) {
		// offset from left by X
		x = c->xSpace();
		width = (cellx*c->xSpan()) - (2 * c->xSpace()) + 1;
	}
	else {
		// centre in X
		if ( c->align() & KBagConstraints::HCenter ) {
			x = (cellx - w->width())/ 2;

			_min.rwidth() += w->width() + 2 * c->xSpace();
		}
		// align left
		else if ( c->align() & KBagConstraints::Left ) {
			x = c->xSpace()/2;
		}
		// align right
		else if ( c->align() & KBagConstraints::Right ) {
			x = cellx - w->width() - (c->xSpace()/2) - 1;
		}

		width = w->width();

		_min.rwidth() += w->width() + 2 * c->xSpace() + 1;
	}

	//
	// Y layout
	//

	if( c->resizePolicy() & KBagConstraints::Vertical ) {
		// offset from top by Y
		y = c->ySpace();
		height = (celly*c->ySpan()) - (2 * c->ySpace());
	}
	else {
		// centre in Y
		if ( c->align() & KBagConstraints::VCenter ) {
			y = (celly - w->height()) /2;
		}
		// align top
		else if ( c->align() & KBagConstraints::Top ) {
			y = c->ySpace()/2;
		}
		// align bottom
		else if ( c->align() & KBagConstraints::Bottom ) {
			y = celly - w->height() - (c->ySpace()/2) - 1;
		}

		height = w->height();

		_min.rheight() += w->height() + 2 * c->ySpace() + 1;
	}

	// move/resize widget.
	w->setGeometry( _contents.left()	
			+ cellx * c->gridX() + x,
			_contents.top()	
			+ celly * c->gridY() + y,
			width,
			height );

}

bool KBagLayout::eventFilter( QObject *receiver, QEvent *event ) 
{
	if( receiver == _parent && event->type() == Event_Resize ) {
		updateContentRect();
		activate();
	}

	return false;
}

void KBagLayout::setSpaceContents( QFrame *frame )
{
	assert( frame );

	QRect r = frame->contentsRect();

	_leftSpace = r.left() + 5;
	_topSpace =  r.top() + 20; // to skip title
	_rightSpace = frame->width() - (r.width() + r.left()) + 5;
	_bottomSpace = frame->height() - (r.height() + r.top() ) + 5;

	updateContentRect();
}

void KBagLayout::updateContentRect()
{
	_contents.setCoords( _leftSpace, _topSpace, 
			_parent->rect().right() - _leftSpace,
			_parent->rect().bottom() - _bottomSpace );

//	updateMinSize();
}

void KBagLayout::updateMinSize()
{
	int width = 0;
	int height = 0;
	int *rowwidth = new int [ _grid.height() ];
	int *colheight = new int [ _grid.width() ];

	for( width = 0; width < _grid.height(); width++ ) {
		rowwidth[ width ] = 0;
	}

	for( width = 0; width < _grid.width(); width++ ) {
		colheight[ width ] = 0;
	}

	width = 0;

	QListIterator<QWidget> list( *_widgets );

	for( ; list.current(); ++list ) {
		QWidget *w = list.current();
		QSize s = w->minimumSize();
		KBagConstraints *c = _cstr->find( w );

		// increment size of every covered row/col

		for( int x = 0; x < c->xSpan(); x++ ) {
			colheight[ x + c->gridX() ] += s.height() 
				+ 2 * c->ySpace();
		}
		
		for( int y = 0; y < c->ySpan(); y++ ) {
			rowwidth[ y + c->gridY() ] += s.width()
				+ 2 * c->xSpace();
		}
	}

	for( int x = 0; x < _grid.width(); x++ ) {
		if( colheight[x] > height ) 
			height = colheight[ x ];
	}

	for( int y = 0; y < _grid.height(); y++ ) {
		if( rowwidth[y] > width ) 
			width = rowwidth[ y ];
	}
//	debug( "min width: %d height: %d", width, height );

	_parent->setMinimumSize( width + _leftSpace + _rightSpace, 
		height + _topSpace + _bottomSpace );

	delete [] rowwidth;
	delete [] colheight;
}
