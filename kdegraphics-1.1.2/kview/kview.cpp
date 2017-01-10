/*
* kview.cpp -- Implementation of class KView.
* Author:	Sirtaj Singh Kang
* Version:	$Id: kview.cpp,v 1.6 1999/01/10 18:16:49 ssk Exp $
* Generated:	Wed Oct 15 01:26:27 EST 1997
*/

#ifdef CORBA
#include"kmicoapp.h"
#else
#include<kapp.h>
#endif

#include<assert.h>

#include"kview.h"
#include"viewer.h"
#include<qimage.h>
#include<kimgio.h>

#include"filter.h"
#include"filtlist.h"
#include"filtmenu.h"
#include"colour.h"
#include"khelpidx.h"

KView::KView(int argc, char **argv)
	: QObject( 0 ),
	_app( argc, argv, "kview" ),
	_filters( new KFilterList ),
	_helper( new KHelpIndex( "kview/kview.index" ) ),
	_viewers( new QList<KImageViewer> ),
	_filtMenus( new QPtrDict<KFiltMenuFactory> ),
	_cutBuffer( 0 )
{
	assert( _filters );

	_viewers->setAutoDelete( true );
	_filtMenus->setAutoDelete( true );

	kimgioRegister();

	registerBuiltinFilters();
}

KView::~KView()
{
	delete _viewers; _viewers = 0;
	delete _helper; _helper = 0;
	delete _filters; _filters = 0;
	delete _filtMenus; _filtMenus = 0;
}

int KView::exec()
{
	if( _app.isRestored() ) {
		// restore saved viewers
		for ( int i = 1; KTopLevelWidget::canBeRestored( i ); i++ ) {
			makeViewer()->restore( i );
		}
	}
	else {
		KImageViewer *viewer = makeViewer();
		viewer->show();

		// process arguments only if not restored
		for( int i = 1; i <= _app.argc(); i++ ) {
			viewer->appendURL( _app.argv()[ i ],
				(i == 1)  );
		}
	}

	return _app.exec();
}

void KView::registerBuiltinFilters()
{
	_filters->registerFilter( new BriteFilter, 
		KFilterList::AutoDelete );
	_filters->registerFilter( new GreyFilter, 
		KFilterList::AutoDelete );
	_filters->registerFilter( new SmoothFilter, 
		KFilterList::AutoDelete );
	_filters->registerFilter( new GammaFilter, 
		KFilterList::AutoDelete );
}


void KView::help( const char *tag )
{
	assert( _helper );
	_helper->invoke( tag );
}

void KView::newViewer()
{
	KImageViewer *viewer = makeViewer();
	
	viewer->show();
}

KImageViewer *KView::makeViewer()
{
	KImageViewer *viewer = new KImageViewer;

	if( viewer == 0 )
		return 0;

	KFiltMenuFactory *menu = new KFiltMenuFactory( _filters );
	viewer->setFilterMenu( menu );

	_viewers->append( viewer );
	_filtMenus->insert( viewer, menu );

	connect( viewer, SIGNAL(wantHelp( const char *)),
		this, SLOT(help(const char *)) );
	connect( viewer, SIGNAL(wantNewViewer()),
		this, SLOT(newViewer()) );
	connect( viewer, SIGNAL(wantToDie(KImageViewer *)),
		this, SLOT(closeViewer(KImageViewer *)) );
	connect( viewer, SIGNAL(accelChanged()),
		this, SLOT( updateAllAccels()) );

	return viewer;
}

void KView::closeViewer( KImageViewer *viewer )
{
	assert( viewer != 0 );

	_filtMenus->remove( viewer );
	_viewers->remove( viewer );

	if( _viewers->count() == 0 ) {
		kapp->quit();
	}
}

void KView::setCutBuffer( QPixmap *image )
{
	if( _cutBuffer ) {
		delete _cutBuffer; _cutBuffer = 0;
	}
	
}

void KView::updateAllAccels()
{
	QListIterator<KImageViewer> iter( *_viewers );

	for( ; iter.current(); ++iter ) {
		iter.current()->updateAccel();
	}
}
