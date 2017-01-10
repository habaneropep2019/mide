/*
* filter.cpp -- Implementation of class KImageFilter.
* Author:	Sirtaj Singh Kang
* Version:	$Id: filter.cpp,v 1.2 1998/10/08 14:42:41 kulow Exp $
* Generated:	Tue Apr  7 18:21:41 EST 1998
*/

#include"filter.h"

void KImageFilter::setMaxProgress( int max )
{
	_max = max;
}

void KImageFilter::setProgress( int val )
{
	if( _max == 0 )
		return;

	long pct = ((long)val * 100) / (long)_max;

	emit progress( (int)pct );
}
