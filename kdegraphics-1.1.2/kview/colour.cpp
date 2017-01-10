/*
* colour.cpp -- Implementation of class ColourFilter.
* Author:	Sirtaj Singh Kang <taj@kde.org>
* Version:	$Id: colour.cpp,v 1.8 1998/11/20 18:28:23 ssk Exp $
* Generated:	Tue Sep 22 02:26:05 EST 1998
*/

#include<qcolor.h>

#include"numdlg.h"

#include"colour.h"
#include"kapp.h"

#include"kcproc.h"

static void modifyIntensity( QImage& image, double factor );

// Brightness Filter

void BriteFilter::invoke( QImage before )
{
	if ( before.depth() < 32 ) {
		warning( "cannot use rgb for image of depth %d", 
			before.depth() );
		return;
	}

	KNumDialog dlg;
	double pct = _pct;

	if( !dlg.getNum( pct, i18n( "Enter brightness factor (%):" ) )) {
		return;
	}

	if( !(pct > 0 ) ) {
		emit message( i18n( "Brightness factor must be positive") );
		return;
	}

	_pct = pct;
	pct /= 100;

	emit status( i18n( "Changing brightness..." ) );

	QApplication::setOverrideCursor( waitCursor );    
	QImage newimage = before;
	modifyIntensity( newimage, pct );
	QApplication::restoreOverrideCursor();

	emit changed( newimage );

	emit status( 0 );
}

const char *BriteFilter::name() const
{
	return i18n( "Intensity:Brightness" );
}

KImageFilter *BriteFilter::clone() const
{
	return new BriteFilter;
}

static void modifyIntensity( QImage& image, double delta )
{
	for( int y = 0; y < image.height(); y++ ) {
		QRgb *clr = (QRgb *) image.scanLine( y );
		for( int x = image.width(); x != 0; x--, clr++ ) {

			double red = qRed(*clr);
			double green = qGreen(*clr);
			double blue = qBlue(*clr);

			if( !KColourProc::toHSV( red, green, blue ) )
				continue;

			// blue is intensity, so modify
			blue *= delta;
			if( blue > 1 || blue < 0 ) {
				continue;
			}

			KColourProc::toRGB( red, green, blue );

			*clr = qRgb( (int)red, (int)green, (int)blue );
		}
	}
}

//
// Greyscale filter
//

void GreyFilter::invoke( QImage before )
{
	int olddepth = 0;

	emit status( i18n( "Converting to Greyscale..." ) );

	QApplication::setOverrideCursor( waitCursor );    

	if ( before.depth() < 32 ) {
		olddepth = before.depth();
		before.convertDepth( 32 );
	}

	int cells = before.height() * before.width();
	QRgb *c = (QRgb *)before.bits();

	for( int y = cells; y ; y--, c++ ) {
		int g = qGray( *c );
		*c = qRgb( g, g, g );
	}

	if ( olddepth != 0 ) {
		before.convertDepth( olddepth );
	}

	QApplication::restoreOverrideCursor();

	emit changed( before );
	emit status( 0 );
}

const char *GreyFilter::name() const
{
	return i18n( "Greyscale" );
}

KImageFilter *GreyFilter::clone() const
{
	return new GreyFilter;
}

//
// Smoothen filter
//

void SmoothFilter::invoke( QImage before )
{
	int olddepth = 0;
	int delta[ 8 ];
	int i;

	emit status( i18n( "Smoothing..." ) );
	QApplication::setOverrideCursor( waitCursor );    

	QImage after = before;

	if ( after.depth() < 32 ) {
		olddepth = after.depth();
		after.convertDepth( 32 );
	}

	// init delta table

	delta[ 0 ] = - (after.width() - 1);
	delta[ 1 ] = - after.width();
	delta[ 2 ] = - ( after.width() + 1);
	delta[ 3 ] = - 1;

	for( i = 0; i < 4; i ++ )
		delta[ i + 4 ] = - delta[ i ];


	int cells = after.height() * after.width();
	QRgb *c = (QRgb *)after.bits();

	setMaxProgress( cells );

	for( int y = 0; y < cells ; y++, c++ ) {
		int r = qRed( *c );
		int g = qGreen( *c );
		int b = qBlue( *c );
		int cnt = 1;

		for (  i = 0; i < 8; i ++ ) {
			int v = y + delta[ i ];
			if ( v >= 0 && v < cells ) {
				r += qRed( *( c + delta[ i ]) );
				g += qGreen( *( c + delta[ i ]) );
				b += qBlue( *( c + delta[ i ]) );
				cnt++;
			}
		}

		if ( cnt > 1 ) {
			*c = qRgb( r / cnt, g / cnt, b / cnt );
		}

		if ( y % 50000 == 0 ) {
			setProgress( y );
			qApp->processEvents( 100 );
		}
	}


	if ( olddepth != 0 ) {
		after.convertDepth( olddepth );
	}

	QApplication::restoreOverrideCursor();
	setProgress( 0 );

	emit changed( after );
	emit status( 0 );
}

const char *SmoothFilter::name() const
{
	return i18n( "Smooth" );
}

KImageFilter *SmoothFilter::clone() const
{
	return new SmoothFilter;
}


//
// Gamma filter
//

void GammaFilter::invoke( QImage before )
{
	// get gamma value
	KNumDialog dlg;
	KColourProc color;

	double gamma = 1.0;

	if( !dlg.getNum( gamma, i18n( "Enter gamma value ( >0 ):" ) ) 
		|| gamma < 0 ) {

		emit message( i18n( "Bad Gamma value" ) );

		return;
	}

	// new image
	emit status( i18n( "Gamma-correcting..." ) );

	QApplication::setOverrideCursor( waitCursor );    

	QImage after = before;
	int olddepth = 0;

	if ( after.depth() < 32 ) {
		olddepth = after.depth();
		after.convertDepth( 32 );
	}

	int cells = after.height() * after.width();
	QRgb *c = (QRgb *)after.bits();

	setMaxProgress( cells );

	color.setGamma( gamma );

	// calc gamma for each image
	for( int y = 0; y < cells ; y++, c++ ) {
		int r = qRed( *c );
		int g = qGreen( *c );
		int b = qBlue( *c );

//		debug( "before: ( %d, %d, %d )", r, g, b );
		color.gammaCorrect( r, g, b );
//		debug( "after: ( %d, %d, %d )", r, g, b );

		*c = qRgb( r, g, b );

		if ( y % 50000 == 0 ) {
			setProgress( y );
			qApp->processEvents( 100 );
		}
	}


	if ( olddepth != 0 ) {
		after.convertDepth( olddepth );
	}

	// reset viewer

	QApplication::restoreOverrideCursor();
	setProgress( 0 );

	emit changed( after );
	emit status( 0 );
}

const char *GammaFilter::name() const
{
	return i18n( "Intensity:Gamma Correct..." );
}

KImageFilter *GammaFilter::clone() const
{
	return new GammaFilter;
}

//
// Shell filter
//

const char *ShellFilter::name() const
{
	return i18n( "External.." );
}

KImageFilter *ShellFilter::clone() const
{
	return new ShellFilter;
}

void ShellFilter::invoke( QImage )
{
	// TODO: stub
}
