/*
* filtlist.cpp -- Implementation of class KFilterList.
* Author:	Sirtaj Singh Kang
* Version:	$Id: filtlist.cpp,v 1.2 1998/10/08 14:42:43 kulow Exp $
* Generated:	Mon Apr 13 06:10:04 EST 1998
*/

#include<assert.h>
#include"filtlist.h"
#include"filter.h"

KFilterList::KFilterList()
	: QObject(),
	_filters( new QList<FilterContainer> )
{
	_filters->setAutoDelete( true );
}

KFilterList::~KFilterList()
{

	// delete all filters marked AutoDelete
	QListIterator<FilterContainer> iter( *_filters );

	for( ; iter.current(); ++iter ) {
		if ( iter.current()->del == AutoDelete ) {
			delete iter.current()->filter;
		}
	}

	delete _filters;
}

int KFilterList::registerFilter( KImageFilter *filter, 
			DeletePolicy policy )
{
	FilterContainer *cont = new FilterContainer;
	assert( cont );

	cont->filter = filter;
	cont->del = ( policy == AutoDelete) ? true : false;

	_filters->append( cont );

	return _filters->at();
}

KImageFilter *KFilterList::filter( int index )
{
	return _filters->at( index )->filter;
}

int KFilterList::count() const
{
	return _filters->count();
}


