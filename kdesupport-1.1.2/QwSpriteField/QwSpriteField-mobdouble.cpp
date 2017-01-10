#include "QwSpriteField.h"
#include "QwSpriteField-template.cpp"

/*!
\class QwRealMobileSprite QwSpriteField.h
This class is simply an instantiation of the QwMobilePositionedSprite class,
using doubles as the coordinate type.  For documentation of the methods,
see the documentation for QwMobilePositionedSprite.
*/
template class QwMobilePositionedSprite<double>;


/*!
\fn QwRealMobileSprite::QwRealMobileSprite(QwSpritePixmapSequence* s) 

Construct a QwRealMobileSprite.  See QwPositionedSprite::QwPositionedSprite(QwSpritePixmapSequence*).
*/

/*!
\fn QwRealMobileSprite::QwRealMobileSprite ()

Construct a QwSprite.  See QwPositionedSprite::QwPositionedSprite().
*/
