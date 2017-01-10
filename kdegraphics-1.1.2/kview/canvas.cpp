/*
* canvas.cpp -- Implementation of class KImageCanvas.
* Author:	Sirtaj Singh Kang
* Version:	$Id: canvas.cpp,v 1.10 1999/01/10 18:16:43 ssk Exp $
* Generated:	Thu Oct  9 09:03:54 EST 1997
*/

#include<stdio.h>
#include<unistd.h>
#include<assert.h>

#include<qcolor.h>
#include<qpixmap.h>
#include<qapp.h>
#include<assert.h>
#include<qlabel.h>
#include<qimage.h>
#include<qpainter.h>
#include<qpen.h>

#include"canvas.h"


static QString loadStdin();
static void maxpect( QWidget *dest, QPixmap *image );

KImageCanvas::KImageCanvas( QWidget *parent )
	: QScrollView( parent ),
	_status( OK ),
	_file( "" ),
	_client( 0 ),
	_orig( 0 )
{
	_client = new KVImageHolder( this );
	
	connect(_client, SIGNAL( contextPress(const QPoint&) ), 
		this, SIGNAL ( contextPress(const QPoint&) ));
	addChild( _client );

	_client->setAutoResize( TRUE );
	_client->hide();
}

KImageCanvas::~KImageCanvas()
{
	delete _orig;
}

KImageCanvas::Error KImageCanvas::status() const
{
	return _status;
}

int KImageCanvas::load( const char *file, const char *URL,
	bool max )
{
	if( file == 0 ) {
		setStatus( BadPath );
		return 0;
	}

	QString realfile = file;
	bool std = false;

	if( realfile == "-") {
		// stdin
		std = true;
		realfile = loadStdin();

		if( realfile.isEmpty() ) {
			setStatus( BadPath );
			return 0;
		}
	}

	static int allocContext = 0;
	if( allocContext )  {
		QColor::destroyAllocContext( allocContext );
	}

	allocContext = QColor::enterAllocContext();

	QApplication::setOverrideCursor( WaitCursor );

	QPixmap newImage;
	bool loadOK = newImage.load( realfile );

	if( !loadOK && realfile.contains( ".tga", false ) ) {
		loadOK = newImage.load( realfile, "TGA" );
	}

	QColor::leaveAllocContext();

	if( std ) {
		unlink( realfile );
	}

	if( loadOK ) {
		delete _orig; // delete old buffered image
		_orig = 0;

		_client->move( 0, 0 );
		_originalSize = size();
		if ( max ) {
			maxpect( this, &newImage );
		}
		_client->setImagePix( newImage );
		_client->show();
		
		updateScrollBars();

		_file = ( URL ? URL : realfile.data() );
		setStatus( OK );
		setCaption( _file );
		QApplication::restoreOverrideCursor();
		return -1;
	}
	else {
		warning( "Couldn't open %s", realfile.data() );
		setStatus( BadPath );
	}

	QApplication::restoreOverrideCursor();
	return 0;
}

bool KImageCanvas::save( const char *urls, const char *format )
{
	if( urls == 0 ) {
		warning( "KImageCanvas::save called with null url" );
	}

	if( format == 0 ) {
		format = "BMP";
	}

	return _client->pixmap()->save( urls, format );
}

void KImageCanvas::reset()
{
	if( _orig == 0 ) {
		return;
	}

	_client->setImagePix( *_orig );
	emit imageSizeChanged();
}

void KImageCanvas::clear()
{
	_client->setBackgroundColor( QColor(0,0,0) );
	_client->resize( 0, 0 );
}

void KImageCanvas::transformImage( const QWMatrix& mat )
{
	const QPixmap *image = transPixmap();

	if( image == 0 ) {
		return;
	}

	QApplication::setOverrideCursor( waitCursor );
	QPixmap newimage = _orig->xForm( mat );

	_client->clearSelection();
	_client->setImagePix( newimage );

	QApplication::restoreOverrideCursor();
}

void KImageCanvas::tileToDesktop() const
{

	const QPixmap *image = _client->pixmap();

	if( image == 0 ) {
		return;
	}

	qApp->desktop()->setBackgroundPixmap(
			*image );
}

void KImageCanvas::maxToDesktop() const
{
	if( _client->pixmap() == 0 )
		return;
	
	QPixmap image = *(_client->pixmap());

	int h = qApp->desktop()->height();
	int w = qApp->desktop()->width();

	image.convertFromImage(image.convertToImage().smoothScale(w,h));

	qApp->desktop()->setBackgroundPixmap( image );

}

void KImageCanvas::maxpectToDesktop() const
{
	if( _client->pixmap() == 0 )
		return;
	
	QPixmap image = *(_client->pixmap());

	double dh = (double)qApp->desktop()->height()/(double)image.height();
	double dw = (double)qApp->desktop()->width()/(double)image.width();
	double d = ( dh < dw ? dh : dw );
	
	image.convertFromImage(image.convertToImage().smoothScale(
		int(d*image.width()),
		int (d*image.height())));

	qApp->desktop()->setBackgroundPixmap( image );
}

static void maxpect( QWidget *dest, QPixmap *image )
{
	double dh = (double)dest->height()/(double)image->height();
	double dw = (double)dest->width()/(double)image->width();
	double d = ( dh < dw ? dh : dw );
	
	image->convertFromImage(image->convertToImage().smoothScale(
			int(d*image->width()),
			int (d*image->height())));
}

void KImageCanvas::resizeEvent( QResizeEvent *ev )
{
	QScrollView::resizeEvent( ev );

	emit imageSizeChanged();
}

QImage KImageCanvas::getImage() const
{
	return _client->pixmap()->convertToImage();
}

void KImageCanvas::setImage( const QImage& image )
{
	transPixmap();

	QPixmap pixmap;
	pixmap.convertFromImage( image );

	_client->setImagePix( pixmap );

	emit imageSizeChanged();
}

QPixmap *KImageCanvas::transPixmap()
{
	QPixmap *client = _client->pixmap();

	if( client == 0 ) {
		return 0;
	}

	if( _orig == 0 ) {
		_orig = new QPixmap( *client );
	}

	return client;
}

void KImageCanvas::cropImage()
{
	QRect select = _client->selected();

	if ( select.isNull() ) {
		return;
	}

	transPixmap();

	QPixmap *oldpix = _client->pixmap();

	QPixmap newpix( select.width(), select.height() );
	
	bitBlt( &newpix, 0, 0, oldpix, select.left(),
		select.top(), select.width(), select.height(), CopyROP );

	_client->setImagePix( newpix );
	emit imageSizeChanged();
}


#define BUF_SIZE 1024
/**
* Read standard input into a file.
* @return filename
*/
static QString loadStdin()
{
	if( feof( stdin ) ) {
		return "";
	}

	QString name( 100 );
	if( tmpnam( name.data() ) == 0  ) {
		return "";
	}

	char buffer[ BUF_SIZE ];
	FILE *o;

	o = fopen( name, "w" );
	if( o == 0 ) {
		return "";
	}

	while( !feof( stdin ) ) {
		size_t bytes = fread( buffer, sizeof(char), BUF_SIZE,
			stdin );

		fwrite( buffer, sizeof( char ), bytes, o );
	}
	fclose( o );

	return name;
}


void KImageCanvas::maxToWin()
{
	transPixmap();

	if( _client->pixmap() == 0 )
		return;
	
	QPixmap image = *(_client->pixmap());

	image.convertFromImage(image.convertToImage().smoothScale(
			width(),height()));
	_client->setImagePix( image );
	
	emit imageSizeChanged();
}

void KImageCanvas::maxpectToWin()
{
	transPixmap();

	if( _client->pixmap() == 0 )
		return;
	
	QPixmap image = *(_client->pixmap());


	double dh = (double)height()/(double)image.height();
	double dw = (double)width()/(double)image.width();

	double d = ( dh < dw ? dh : dw );
	
	image.convertFromImage(image.convertToImage().smoothScale(int(d*image.width()),
								   int (d*image.height())));

	_client->setImagePix( image );
	emit imageSizeChanged();
}

void KImageCanvas::copyImage( QPaintDevice *dest ) const
{
	QPainter painter( dest );
	painter.drawPixmap( 0, 0, *(_client->pixmap()) );
}

void KImageCanvas::mouseReleaseEvent( QMouseEvent *ev )
{
	// wheel support
	// maybe one day this will work...

#if 0
	switch ( ev->button() ) {
		case 4: lineUp();	break;
		case 5: lineDown();	break;
		default:	
			QScrollView::mouseReleaseEvent( ev );
	}
#else
	QScrollView::mouseReleaseEvent( ev );
#endif

}

/*
******************************
* KVImageHolder implementation
*/

KVImageHolder::KVImageHolder( QWidget *parent )
		: QLabel( parent ), _selected( false ),
		_painter( new QPainter ),
		_pen( new QPen( QColor( 255, 255, 255 ), 0, DashLine ) )
{
	assert( _painter != 0 );
}

KVImageHolder::~KVImageHolder()
{
	delete _painter;
	delete _pen;
}

void KVImageHolder::mousePressEvent( QMouseEvent *ev )
{
    if ( ev->button() == RightButton ) {
	    emit contextPress( mapToGlobal( ev->pos() ) );
	    return;
    }

    if( pixmap() == 0 ) {
	    return;
    }

    if( _selected ) {
	    // remove old rubberband
	    eraseSelect();
	    _selected = false;
    }

    _selection.setLeft( ev->x() );
    _selection.setTop( ev->y() );
}

void KVImageHolder::mouseMoveEvent( QMouseEvent *ev )
{
	// currently called only on drag,
	// so assume a selection has been started
	bool erase = _selected;

	if( !_selected ) {
		_selected = true;
	}

	int r = (ev->x() < width()) ? ev->x() : width() - 1;
	int b = (ev->y() < height()) ? ev->y() : height() - 1;

	if( r != _selection.right() || b != _selection.bottom() ) {
		if ( erase )
			eraseSelect();
		
		_selection.setRight( r );
		_selection.setBottom( b );

		drawSelect();
	}
}

void KVImageHolder::drawSelect()
{
	_painter->begin( this );
	_painter->setRasterOp( XorROP );
	_painter->setPen( *_pen );
	_painter->drawRect( _selection );
	_painter->end();
}

void KVImageHolder::eraseSelect()
{
	QRect r = _selection.normalize();

	bitBlt( this, r.left(), r.top(), pixmap(), r.left(), r.top(),
			r.width(), r.height(), CopyROP );
}

void KVImageHolder::paintEvent( QPaintEvent *ev )
{
	QLabel::paintEvent( ev );

	if( _selected ) {
		drawSelect();
	}
}

QRect KVImageHolder::selected() const
{
	if( !_selected ) {
		return QRect();
	}

	return _selection.normalize();
}

void KVImageHolder::setImagePix( const QPixmap& pix )
{
	clearSelection();
	setPixmap( pix );
	resize( pix.size() );
}


