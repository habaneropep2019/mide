/*
* viewer.cpp -- Implementation of class KImageViewer.
* Author:	Sirtaj Singh Kang
* Version:	$Id: viewer.cpp,v 1.20 1999/01/19 02:12:27 ssk Exp $
* Generated:	Wed Oct 15 11:37:16 EST 1997
*/

#include<assert.h>
#include<stdio.h>

#include<qpopmenu.h>
#include<qmsgbox.h>
#include<qprintdialog.h>
#include<qprinter.h>
#include<qstrlist.h>
#include<qaccel.h>
#include<qtimer.h>
#include<qevent.h>

#include<ktopwidget.h>
#include<kmenubar.h>
#include<kapp.h>
#include<drag.h>
#include<kfiledialog.h>
#include<kstring.h>
#include<kfm.h>
#include<kaccel.h>
#include<kaccelmenuwatch.h>

#include"viewer.h"
#include"canvas.h"
#include"version.h"
#include"numdlg.h"
#include"prefdlg.h"

#include"filter.h"
#include"filtmenu.h"
#include"filtlist.h"

#include"ilistdlg.h"

enum {
	StatProgress,
	StatStatus
};

const int MessageDelay = 3000;

KImageViewer::KImageViewer()
	: KTopLevelWidget(),
	_imageLoaded( false ),


	_kaccel( new KAccel( this ) ),
	_paccel( new QAccel( this ) ),
	_watcher( new KAccelMenuWatch( _kaccel, this ) ),

	_file( 0 ),
	_edit( 0 ),
	_zoom( 0 ),
	_transform( 0 ),
	_desktop( 0 ),
	_aggreg( 0 ),
	_help( 0 ),

	_barFilterID( 0 ),
	_popFilterID( 0 ),

	_kfm( 0 ),
	_transSrc( 0 ),
	_transDest( 0 ),
	_menuFact( 0 ),
	_pctBuffer( new QString ),
	_lastPct( - 1 ),
	_msgTimer( 0 ),
	_imageList( new ImgListDlg ),
	_zoomFactor( 100 ),

	_loadMode   ( ResizeWindow )
{

	// Image canvas
	_canvas = new KImageCanvas( this );
	
	connect(_canvas, SIGNAL( contextPress(const QPoint&) ), 
		this, SLOT ( contextPress(const QPoint&) ));

	assert( _canvas );
	setView( _canvas, FALSE );
	
	// Caption
	QString cap;
	cap = kapp->appName().data();
	cap += " - ";
	cap += i18n( "no image loaded" );
	setCaption( cap );

	// status bar
	_statusbar = new KStatusBar( this );
	setStatusBar( _statusbar );

	_statusbar->insertItem( "                 ", StatProgress );
	_statusbar->insertItem( i18n( "Ready" ), StatStatus );

	// list dialog
	connect( _imageList, SIGNAL(selected(const char *)),
		this, SLOT(loadURL(const char *)) );

	// accelerators
	makeAccel();

	// Drop events forwarded to image list object

	KDNDDropZone *dropObserver = new KDNDDropZone( this, DndURL );

	QObject::connect( dropObserver, SIGNAL(dropAction(KDNDDropZone *)),
			_imageList, SLOT(dropEvent(KDNDDropZone *)) );

	restoreOptions( kapp->getConfig() );
}

KImageViewer::~KImageViewer()
{
	delete _canvas; _canvas = 0;

	delete _file;
	delete _edit;
	delete _zoom;
	delete _transform;
	delete _desktop;
	delete _aggreg;
	delete _help;

	delete _contextMenu;

	delete _pctBuffer;
	delete _imageList;


	delete _kfm; _kfm = 0;

	delete _transSrc;
	_transSrc = 0;

	delete _transDest;
	_transDest = 0;
}

void KImageViewer::load()
{
	KFileDialog dlg( "", 0, 0, 0, true, true );
	QString URL = dlg.getOpenFileURL();

	if( URL.length() <= 0 ) {
		return;
	}

	_imageList->addURL( URL );
}

void KImageViewer::saveAs()
{
	KFileDialog dlg( "", 0, 0, 0, true, false );
	QString urls = dlg.getSaveFileURL();


	if ( urls.length() <= 0 ) {
		return;
	}

	KURL url( urls );
	bool stat = false;

	if( url.isMalformed() ) {
		stat = _canvas->save( urls );
	}
	else if( url.isLocalFile() ) {
		stat = _canvas->save( url.path() );
	}
	else {
		message( i18n("Net saving not yet implemented") );
	}

	if( stat == false ) {
		return;
	}

	QString msg = urls;
	ksprintf( &msg, i18n( "%s: written" ), urls.data() );
	message( msg );
}

void KImageViewer::zoomIn10()
{
	_mat.scale( 1.1, 1.1 );

	_canvas->transformImage( _mat );
}

void KImageViewer::zoomOut10()
{
	_mat.scale( 0.9, 0.9 );

	_canvas->transformImage( _mat );
}

void KImageViewer::zoomIn200()
{
	_mat.scale( 2.0, 2.0 );

	_canvas->transformImage( _mat );
}

void KImageViewer::zoomOut50()
{
	_mat.scale( 0.5, 0.5 );

	_canvas->transformImage( _mat );
}

void KImageViewer::zoomCustom()
{
	KNumDialog num;
	_zoomFactor = (int)(_mat.m11() * 100);

	if( !num.getNum( _zoomFactor, i18n(
			"Enter Zoom factor (100 = 1x):" ) ) )
		return;

	if( _zoomFactor <= 0 ) {
		message( i18n( "Illegal zoom factor" ) );
		return;
	}
		
	double val = (double)_zoomFactor/100;
	_mat.reset();
	_mat.scale( val, val );
	_canvas->transformImage( _mat );
}

void KImageViewer::rotateClock()
{
	_mat.rotate( 90 );

	_canvas->transformImage( _mat );
}

void KImageViewer::rotateAntiClock()
{
	_mat.rotate( -90 );

	_canvas->transformImage( _mat );
}

void KImageViewer::flipVertical()
{
	QWMatrix m( 1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F );

	_mat *= m;

	_canvas->transformImage( _mat );
}

void KImageViewer::flipHorizontal()
{
	QWMatrix m( -1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F );

	_mat *= m;

	_canvas->transformImage( _mat );
}

void KImageViewer::help()
{
	kapp->invokeHTMLHelp( "kview/kview.html", "" );	
}

void KImageViewer::about()
{
	QString aboutText;
	
	aboutText.sprintf( i18n( "KView -- Graphics viewer. %s\n"
		"\nSirtaj S. Kang (taj@kde.org)\n" ), KVIEW_VERSION );

	QMessageBox::about( this, i18n("About KView"), aboutText );
}

void KImageViewer::makeRootMenu(QPopupMenu *menu)
{
	/*
	   menu->insertItem( i18n( "&File" ),	_file );
	   menu->insertItem( i18n( "&Edit" ),	_edit );
	   menu->insertItem( i18n( "&Zoom" ),	_zoom );
	   menu->insertItem( i18n( "&Transform" ),	_transform );
	   menu->insertItem( i18n( "To &Desktop"),	_desktop );

	   _popFilterID = menu->insertItem( i18n( "Fi&lter"),	
	   _menuFact->filterMenu() );
	   menu->setItemEnabled( _popFilterID, false );

	   menu->insertItem( i18n( "&Options" ), _aggreg );

	   menu->insertSeparator();
	   menu->insertItem( i18n( "&Help" ), _help );

	 */	


	_watcher->setMenu( menu );
	conn( i18n("&List..."), "ImageList", this, SLOT(toggleImageList()) );
	menu->insertSeparator();
	conn( i18n("&Previous"), "Prev", _imageList, SLOT(prev()),
			 Key_Backspace );
	conn( i18n("&Next"), "Next", _imageList, SLOT(next()), Key_Tab );
	menu->insertSeparator();
	conn( i18n("&Slideshow On/Off"), "Slideshow", _imageList,
			SLOT(toggleSlideShow()), Key_S );
}

void KImageViewer::makeRootMenu(KMenuBar *menu)
{
	menu->insertItem( i18n( "&File" ),	_file );
	menu->insertItem( i18n( "&Edit" ),	_edit );
	menu->insertItem( i18n( "&Zoom" ),	_zoom );
	menu->insertItem( i18n( "&Transform" ),	_transform );
	menu->insertItem( i18n( "To &Desktop"),	_desktop );

	_barFilterID = menu->insertItem( i18n( "F&ilter"),	
		_menuFact->filterMenu() );
	menu->setItemEnabled( _barFilterID, false );
	
	menu->insertItem( i18n( "&Images" ), _aggreg );

	menu->insertSeparator();
	menu->insertItem( i18n( "&Help" ),  _help );
}

void KImageViewer::makePopupMenus()
{
	_file		= new QPopupMenu;
	_edit		= new QPopupMenu;
	_zoom		= new QPopupMenu;
	_transform	= new QPopupMenu;
	_desktop	= new QPopupMenu;
	_aggreg	= new QPopupMenu;
	_help		= new QPopupMenu;

	// file pulldown

	_watcher->setMenu( _file );
	conn( i18n( "&Open..." ), KAccel::Open, this, SLOT(load()) );
	conn( i18n( "&Save As..." ), KAccel::Save, this, SLOT(saveAs()) );
	conn( i18n( "&Print..." ), KAccel::Print, this, SLOT(printImage()));
	_file->insertSeparator();
	conn( i18n( "&New Window" ), KAccel::New, this, SLOT(newViewer()));
	conn( i18n( "&Close Window" ),KAccel::Close, this, SLOT(closeViewer()));
	_file->insertSeparator();
	conn( i18n( "E&xit" ), KAccel::Quit, this, SLOT(quitApp()) );

	// edit pulldown
	_watcher->setMenu( _edit );
	conn( i18n( "&Full Screen" ), "FullScreen", this, SLOT(fullScreen()),
			Key_F );
	_edit->insertSeparator();
	conn( i18n( "&Crop" ), "Crop", _canvas, SLOT(cropImage()), Key_C );
	_edit->insertSeparator();
	conn( i18n( "&Reset"), "Reset", this, SLOT(reset()), CTRL + Key_R );
	_edit->insertSeparator();
	conn( i18n( "&Preferences..."), "Prefs", this, SLOT(prefs()), 
		CTRL + Key_E );

	// zoom pulldown

	_watcher->setMenu( _zoom );
	conn( i18n( "&Zoom..." ), "Zoom", this, SLOT(zoomCustom()), Key_Z );
	conn( i18n( "Zoom &in 10%" ), "ZoomIn10", this, SLOT(zoomIn10()),
			Key_Period );
	conn( i18n( "Zoom &out 10%"), "ZoomOut10", this,SLOT(zoomOut10()),
			Key_Backspace );
	conn( i18n( "&Double size" ), "ZoomIn200", this, SLOT(zoomIn200()),
			Key_BracketRight );
	conn( i18n( "&Half size"), "ZoomOut50", this, SLOT(zoomOut50()), 
			Key_BracketLeft );
	conn( i18n( "&Max size"), "Max", _canvas, SLOT(maxToWin()),
			Key_M );
	conn( i18n( "Max/&aspect"), "Maxpect", _canvas, SLOT(maxpectToWin()),
			Key_A );

	// transform pulldown

	_watcher->setMenu( _transform );
	conn( i18n( "Rotate &clockwise" ), "RotateC", this, 
			SLOT(rotateClock()), Key_Semicolon );
	conn( i18n( "Rotate &anti-clockwise" ), "RotateAC", this, 
			SLOT(rotateAntiClock()), Key_Comma );
	conn( i18n( "Flip &vertical" ), "FlipV", this, SLOT(flipVertical()),
		Key_V );
	conn( i18n( "Flip &horizontal" ),"FlipH",this,SLOT(flipHorizontal()),
		Key_H );

	// desktop pulldown

	_watcher->setMenu( _desktop );
	conn( i18n( "Desktop &Tile" ), "TileToDesktop", this, SLOT(tile()) );
	conn( i18n( "Desktop &Max" ),  "MaxpectToDesktop", 
		this, SLOT(max()) );
	conn( i18n("Desktop Max&pect"), "MaxpToDesktop", 
		this, SLOT(maxpect()) );

	_watcher->setMenu( _aggreg );

	int id = conn( i18n("&List..."), "ImageList", this, 
			SLOT(toggleImageList()), Key_I );
	_aggreg->setItemChecked( id, false );
	_aggreg->insertSeparator();
	conn( i18n("&Previous"), "Prev", _imageList, SLOT(prev()),
			Key_Backspace );
	conn( i18n("&Next"), "Next", _imageList, SLOT(next()),
			Key_Tab );
	_aggreg->insertSeparator();
	_aggreg->insertItem( i18n("&Slideshow On/Off"), "Slideshow", 
		_imageList, SLOT(toggleSlideShow()), Key_S );

	_watcher->setMenu( _help );
	conn( i18n( "&Contents" ), KAccel::Help, this, SLOT(help()));
	conn( i18n( "&How do I..." ), "HelpHow", this, SLOT(helpHow()));
	conn( i18n( "&What is a..." ), "HelpWhat", this, SLOT(helpWhat()));
	_help->insertSeparator();
	conn( i18n( "&About KView..." ), "About", this, SLOT(about()));
}

void KImageViewer::quitApp()
{
	KApplication::getKApplication()->quit();
}

void KImageViewer::tile()
{
	_canvas->tileToDesktop();
}

void KImageViewer::max()
{
	_canvas->maxToDesktop();
}

void KImageViewer::maxpect()
{
	_canvas->maxpectToDesktop();
}

void KImageViewer::loadURL( const char *url )
{
	if( url == 0 ) {
		warning( "loadURL: called with null url" );
		return;
	}

	KURL kurl( url );

	if( kurl.isMalformed() ) {
		loadFile( url );
	}
	else if( kurl.isLocalFile() ) {
		// local file - open with loadFile
		loadFile( kurl.path(), url );
	}
	else {
		// connect to kfm
		if( _kfm != 0 ) {
			message( i18n( "Existing transfer not yet complete."));
			return;
		}

		_kfm = new KFM;

		if( !_kfm || !_kfm->isKFMRunning() || !_kfm->isOK() ) {
			message( i18n( "Existing transfer not yet complete."));
			return;
		}

		// save transfer files
		if( _transSrc == 0 )
			_transSrc = new QString;
		if( _transDest == 0 )
			_transDest = new QString;

		_transSrc->setStr( url );
		_transDest->setStr( tempnam( 0, "kview_") );

		if( _transSrc->isEmpty() || _transDest->isEmpty() ) {
			message( 
			i18n( "Couldn't get temporary transfer name." ) );
			return;
		}

		// start transfer
		connect( _kfm, SIGNAL( finished() ),
			this, SLOT( urlFetchDone() ) );
		connect( _kfm, SIGNAL( error(int, const char *) ),
			this, SLOT( urlFetchError(int, const char *) ) );

		_kfm->copy( _transSrc->data(), _transDest->data() );
		_transDir = Get;
	}
}

void KImageViewer::urlFetchDone()
{
	if( _kfm == 0 ) {
		warning( "KImageViewer: urlFetchDone() called but no transfer in progress." );
		return;
	}

	switch ( _transDir ) {
		case Get:	loadFile( _transDest->data(),
				_transSrc->data() );
				unlink( _transDest->data() );
				break;
		case Put:	break;
	}

	delete _kfm;
	_kfm = 0;
}

void KImageViewer::urlFetchError( int,  const char *text )
{
	delete _kfm;
	_kfm = 0;

	QString msg( i18n("Transfer error: ") );
	msg += text;

	message( msg );
}

void KImageViewer::appendURL( const char *url, bool show )
{
	_imageList->addURL( url, show );
}

void KImageViewer::invokeFilter( KImageFilter *f )
{
	assert( f != 0 );

	QObject::disconnect( f, SIGNAL(changed(const QImage&)), 0, 0 );

	connect( f, SIGNAL(changed(const QImage&)),
			_canvas, SLOT(setImage(const QImage&)) );

	f->invoke( _canvas->getImage() );
}

void KImageViewer::setFilterMenu( KFiltMenuFactory *filtmenu )
{
	connect( filtmenu, SIGNAL(selected(KImageFilter *)),
		this, SLOT(invokeFilter(KImageFilter *)) );

	// connect all filters
	KFilterList *filters = filtmenu->filterList();
	
	for( int i = 0; i < filters->count(); i++ ) {
		KImageFilter *filter = filters->filter( i );

		connect( filter, SIGNAL(changed(const QImage&)),
				_canvas, SLOT(setImage(const QImage&)) );
		connect( filter, SIGNAL(progress(int)),
			this, SLOT(setProgress(int)) );
		connect( filter, SIGNAL(status(const char *)),
			this, SLOT(setStatus(const char *)) );
		connect( filter, SIGNAL(message(const char *)),
			this, SLOT(message(const char *)) );
	}

	_menuFact = filtmenu;


	makePopupMenus();
	_menubar = new KMenuBar(this);
	makeRootMenu( _menubar );

	makePopupMenus();
	_contextMenu = new QPopupMenu;
	makeRootMenu( _contextMenu );

	setMenu( _menubar );
	_menubar->show();
	_canvas->show();

	_kaccel->readSettings();
	_kaccel->setEnabled( true );
	_watcher->updateMenus();
}

void KImageViewer::setStatus( const char *status )
{
	if ( status == 0 ) {
		status = i18n( "Ready" );
	}

	_statusbar->changeItem( status, StatStatus );
}

void KImageViewer::loadFile( const char *file, const char *url )
{
	if( url == 0 ) {
		url = file;
	}

	// We need to do this, in case the image takes too long to load.
	bool slide = _imageList->slideShowRunning();

	if( slide ) {
		_imageList->pauseSlideShow();
	}

	setStatus( i18n( "Loading..." ) );

	// load the image
	_canvas->load( file, 0, _loadMode == ResizeImage );



	// update state

	setStatus( 0 );

	if( _canvas->status() != KImageCanvas::OK ) {

		QString msg;
		msg.sprintf(i18n("Couldn't load %s"), url);
		message( msg );
	}
	else {
		// resize window to screen
		if( _loadMode == ResizeWindow && _statusbar->isVisible() ) {
			rzWinToImg();	
		}

		// set caption
		QString cap = url;
		cap += " (";
		cap += kapp->appName().data();
		cap += ")";
		setCaption( cap );

		// enable filters
		if( !_imageLoaded ) {
			_menubar->setItemEnabled( _barFilterID, true );
			_contextMenu->setItemEnabled( _popFilterID, true );
			_imageLoaded = true;
		}
		// reset matrix
		_mat.reset();
	}

	if ( slide ) {
		_imageList->continueSlideShow();
	}
}

void KImageViewer::setSize()
{
		// size
		QWidget *desk = kapp->desktop();

		if( _canvas->height() < desk->height()
			&& _canvas->width() < desk->width()
			&& _canvas->height() ) {

			resize( _canvas->size() );
		}
}

void KImageViewer::setProgress( int pct )
{
	if( pct == _lastPct )
		return;

	const char *buf = "";

	if( pct > 0 ) {
		_pctBuffer->setNum( pct );
		*_pctBuffer += '%';
		buf = _pctBuffer->data();
	}

	_lastPct = pct;

	_statusbar->changeItem( buf, StatProgress );
}

void KImageViewer::message( const char *message )
{
	_statusbar->message( message, MessageDelay );

	if( _msgTimer == 0 ) {
		_msgTimer = new QTimer( this );
		connect( _msgTimer, SIGNAL(timeout()),
			_statusbar, SLOT(clear()) );
	}

	_msgTimer->start( MessageDelay, true );
}

void KImageViewer::toggleImageList()
{
	if ( _imageList->isVisible() ) {
		_imageList->hide();
	}
	else {
		_imageList->show();
	}
}


void KImageViewer::contextPress(const QPoint& p)
{
    _contextMenu->popup(p);

}

void KImageViewer::fullScreen()
{
	if( _statusbar->isVisible() ) {
		// change to full

		_menubar->hide();
		_statusbar->hide();

		_posSave = pos();
		_sizeSave = size();

		move( frameGeometry().x() - geometry().x(),
			frameGeometry().y() - geometry().y() );

		resize( qApp->desktop()->size() );

		_canvas->resize( size() );
		_canvas->move( 0, 0 );

		// Grab WM focus. We need to do this for some
		// reason, atleast with KWM. Otherwise the
		// window loses focus and keyboard accelerators
		// don't work unless the mouse is clicked once.
		setActiveWindow();
	}
	else {
		// change to normal
		move( _posSave );
		resize( _sizeSave );
		_menubar->show();
		_statusbar->show();
		updateRects();
	}
}

void KImageViewer::reset()
{
	_canvas->reset();
	_mat.reset();
}

void KImageViewer::saveProperties( KConfig *cfg ) const
{
	if( _statusbar->isVisible() ) {
		cfg->writeEntry( "ViewerFullScreen", false );
		cfg->writeEntry( "ViewerPos", _posSave );
		cfg->writeEntry( "ViewerSize", _sizeSave );
	}
	else {
		cfg->writeEntry( "ViewerFullScreen", true );
		cfg->writeEntry( "ViewerPos", pos() );
		cfg->writeEntry( "ViewerPos", size() );
	}
	
	_imageList->saveProperties( cfg );

}

void KImageViewer::restoreProperties( KConfig *cfg )
{

	bool full = cfg->readBoolEntry( "ViewerFullScreen" );
	
	if( full ) {
		fullScreen();
		_posSave = cfg->readPointEntry( "ViewerPos" );
		_sizeSave = cfg->readSizeEntry( "ViewerSize" );
	}
	else {
		move( cfg->readPointEntry( "ViewerPos" ) );
		resize( cfg->readSizeEntry( "ViewerSize" ) );
	}

	_imageList->restoreProperties( cfg );
}

void KImageViewer::saveOptions( KConfig *cfg ) const
{
	KConfigGroupSaver save( cfg, "kview" );
	QString lmode;

	switch ( _loadMode ) {
		case ResizeNone:	lmode = "ResizeNone";	break;
		case ResizeImage:	lmode = "ResizeImage";	break;
		case ResizeWindow:	
		default:		lmode = "ResizeWindow"; break;
	}
	cfg->writeEntry( "LoadMode", lmode );

	_imageList->saveOptions( cfg );
	_kaccel->writeSettings( cfg );
}

void KImageViewer::restoreOptions( KConfig *cfg )
{
	KConfigGroupSaver save( cfg, "kview" );

	QString lmode = cfg->readEntry( "LoadMode", "ResizeWindow" );

	if ( !stricmp( lmode, "ResizeWindow" ) ) {
		_loadMode = ResizeWindow;
	}
	else if ( !stricmp( lmode, "ResizeImage" ) ) {
		_loadMode = ResizeImage;
	}
	else if ( !stricmp( lmode, "ResizeNone" ) ) {
		_loadMode = ResizeNone;
	}
	else {
		warning( "kview: warning: Unknown resize mode '%s'", 
		(const char *)lmode );
		_loadMode = ResizeWindow;
	}

	_imageList->restoreOptions( cfg );
	_kaccel->readSettings( cfg );
}

void KImageViewer::printImage()
{
	QPrinter printer;

	// get settings
	if( QPrintDialog::getPrinterSetup( &printer ) == false )
		return;

	// print

	setStatus( i18n( "Printing..." ) );
	QApplication::setOverrideCursor( WaitCursor );
	_canvas->copyImage( &printer );
	printer.newPage();
	QApplication::restoreOverrideCursor();
	setStatus( 0 );
}

void KImageViewer::newViewer()
{
	emit wantNewViewer();
}

void KImageViewer::closeViewer()
{
	emit wantToDie( this );
}

void KImageViewer::closeEvent( QCloseEvent * )
{
	closeViewer();
}

void KImageViewer::cut()
{
	// TODO: stub
}

void KImageViewer::copy()
{
	// TODO: stub
}

void KImageViewer::paste()
{
	// TODO: stub
}

void KImageViewer::prefs()
{
	KConfig *cfg = kapp->getConfig();
	KViewPrefDlg dlg( cfg, _kaccel );

	saveOptions( cfg );

	if( dlg.exec() ) {
		restoreOptions( cfg );

		if ( dlg.keysDirty() ) {
			emit accelChanged();
		}
	}
}

void KImageViewer::helpHow()
{
	emit wantHelp( "how" );
}

void KImageViewer::helpWhat()
{
	emit wantHelp( "what" );
}


void KImageViewer::makeAccel()
{
	// keyboard scrolling
	_paccel->connectItem( _paccel->insertItem( Key_Down ),
			_canvas, SLOT(lineDown()) );

	_paccel->connectItem( _paccel->insertItem( Key_Up ),
			_canvas, SLOT(lineUp()) );

	_paccel->connectItem( _paccel->insertItem( Key_PageDown ),
			_canvas, SLOT(pageDown()) );

	_paccel->connectItem( _paccel->insertItem( Key_PageUp ),
			_canvas, SLOT(pageUp()) );


	_paccel->connectItem( _paccel->insertItem( Key_Right ),
			_canvas, SLOT(lineRight()) );

	_paccel->connectItem( _paccel->insertItem( Key_Left ),
			_canvas, SLOT(lineLeft()) );

	// Quit on Q. Not configurable. I don't care.
	_paccel->connectItem( _paccel->insertItem( Key_Q ),	
			this, SLOT(quitApp()) );
}

void KImageViewer::updateAccel()
{
	_kaccel->readSettings();
	_watcher->updateMenus();
}

int KImageViewer::conn( const char *text, const char *action,
	QObject *receiver, const char *method, uint key )
{
	QPopupMenu *menu = _watcher->currentMenu();
	assert( menu );

	QString desc;
	for( const char *t = text; *t; t++ ) {
		if( *t == '&' )
			continue;
		desc += *t;
	}

	_kaccel->insertItem( desc, action, key );
	_kaccel->connectItem( action, receiver, method );

	int id = menu->insertItem( text, receiver, method );
	_watcher->connectAccel( id, action );

	return id;
}

int KImageViewer::conn( const char *text, KAccel::StdAccel action,
	QObject *receiver, const char *method )
{
	QPopupMenu *menu = _watcher->currentMenu();
	assert( menu );

	QString desc;
	for( const char *t = text; *t; t++ ) {
		if( *t == '&' )
			continue;
		desc += *t;
	}

	_kaccel->insertStdItem( action, desc );
	_kaccel->connectItem( action, receiver, method );
	int id = menu->insertItem( text, receiver, method );

	_watcher->connectAccel( id, action );

	return id;
}

void KImageViewer::rzWinToImg()
{
/*
plan: to make the client area of the KTW == area of canvas contents.
	pending:	image will fit on screen
				size + extra 
			move allowed (TODO)
*/
	int iw = _canvas->contentsWidth();
	int ih = _canvas->contentsHeight();
	QRect geom = frameGeometry();
	QRect desk = kapp->desktop()->geometry();

	int xextra = width() - _canvas->width();
	int yextra = height() - _canvas->height();

	int xmax = desk.width()  - ( xextra + geom.x() );
	int ymax = desk.height() - ( yextra + geom.y() );

	setGeometry( geom.x(), geom.y(), 
		QMIN( iw, xmax ) + xextra, 
		QMIN( ih, ymax ) + yextra );
}
