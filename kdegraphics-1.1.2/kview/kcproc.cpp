/*
* kcproc.cpp -- Implementation of class KColourProc.
* Author:	Sirtaj Singh Kang <taj@kde.org>
* Version:	$Id: kcproc.cpp,v 1.2 1998/10/26 06:22:38 ssk Exp $
* Generated:	Thu Oct 15 13:51:46 EST 1998
*/

#include<math.h>

#include"kcproc.h"
#include<qglobal.h>

#define MAX_GAMMA 100

static inline int max( int a, int b ) { return a > b ? a : b; }
static inline int max( int a, int b, int c ) { return max( a, max( b, c ) ); }

static inline int min( int a, int b ) { return a < b ? a : b; }
static inline int min( int a, int b, int c ) { return min( a, min( b, c ) ); }

static inline double max( double a, double b ) { return a > b ? a : b; }
static inline double max( double a, double b, double c ) 
	{ return max( a, max( b, c ) ); }

static inline double min( double a, double b ) { return a < b ? a : b; }
static inline double min( double a, double b, double c ) 
	{ return min( a, min( b, c ) ); }

KColourProc::KColourProc()
	: _gammav( -1 ),
	_gammat( 0 )
{
}

KColourProc::~KColourProc()
{
	// delete gamma table
	if ( _gammat != 0 ) {
		delete [] _gammat;
		_gammat = 0;
	}
}

//
// From Harrington 2nd Ed Chap 10
//

bool KColourProc::toHSV( double& r, double& g, double &b )
{
	double v = max( r, g, b );
	double x = min( r, g, b );

	if( v == x ) {
		return false;
	}

	r /= 255; 
	g /= 255;
	b /= 255;

	v = max( r, g, b );
	x = min( r, g, b );

	double vmx = ( v - x );
	double s = vmx / v;
	
	double r1 = ( v - r ) / vmx;
	double b1 = ( v - b ) / vmx;
	double g1 = ( v - g ) / vmx;

	if( r == v ) {
		if ( g == x ) 
			r = 5.0 + b1;
		else 
			r = 1.0 - g1;
	}
	else if ( g == v ) {
		if( b == x ) 
			r = r1 + 1.0;
		else
			r = 3.0 - b1;
	}
	else {
		if ( r == x )
			r = 3.0 + g1;
		else
			r = 5.0 - r1;
	}

	r *= 60;
	g = s;
	b = v;
	
	return true;
}

//
// From Harrington 2nd Ed Chap 10
//
bool KColourProc::toRGB( double& h, double& s, double &v )
{
	// convert from degrees to hexagon section
	double h1 = h / 60.0;

	// find the dominant colour
	int i = (int)h1;
	double f = h1 - (double)i;

	double a[ 7 ];
	a[ 1 ] = a[ 2 ] = v;
	a[ 3 ] = v * ( 1.0 - (s * f) );
	a[ 4 ] = a[ 5 ] = v * ( 1.0 - s );
	a[ 6 ] = v * ( 1.0 - ( s * (1.0 - f) ) );

	// map strengths to rgb
	i += ( i > 4 ) ? -4 : + 2;
	h = a[ i ] * 255.0;
	i += ( i > 4 ) ? -4 : + 2;
	v = a[ i ] * 255.0;
	i += ( i > 4 ) ? -4 : + 2;
	s = a[ i ] * 255.0;

	return true;
}


void KColourProc::setGamma( double gamma )
{
	if( _gammat == 0 ) {
		// create gamma table
		_gammat = new double[ MAX_GAMMA + 1];
	}

	_gammav = gamma;
	gamma = 1 / gamma;

	// init gamma table

	for( int i = 0; i <= MAX_GAMMA; i++ ) {
		_gammat [ i ] = pow( (double)i / 100.0, gamma );
	}
}

void KColourProc::gammaCorrect( int& r, int& g, int& b ) const
{
	double dr = (double)r;
	double dg = (double)g;
	double db = (double)b;

	if( toHSV( dr, dg, db ) == false ) 
		return;
	db = _gammat [ (int) (db * MAX_GAMMA) ];
	toRGB( dr, dg, db );

	r = (int)dr;
	g = (int)dg;
	b = (int)db;
}

void KColourProc::gammaCorrect(  double& v ) const
{
	v = _gammat[ (int) v * MAX_GAMMA ];
}
