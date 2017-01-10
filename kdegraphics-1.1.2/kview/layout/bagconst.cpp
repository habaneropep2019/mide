/*
* bagconst.cpp -- Implementation of class KBagConstraints.
* Author:	Sirtaj Singh Kang
* Version:	$Id: bagconst.cpp,v 1.3 1999/01/10 18:17:00 ssk Exp $
* Generated:	Mon May  4 15:58:10 EST 1998
*/

#include"bagconst.h"

KBagConstraints::KBagConstraints()
        : _gridx( 0 ), _gridy( 0 ), _xSpan( 1 ), _ySpan( 1 ), 
	_resizePolicy ( Horizontal | Vertical ), _xSpace( 1 ), 
	_ySpace( 1 ), _align( Center )
{
}
