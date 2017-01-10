#include "QwSpriteField.h"
#include "QwSpriteField-template.cpp"

/*!
\class QwMobileSprite QwSpriteField.h
This class is simply an instantiation of the QwMobilePositionedSprite class,
using integers as the coordinate type.  For documentation of the methods,
see the documentation for QwMobilePositionedSprite.
*/
template class QwMobilePositionedSprite<int>;


/*!
\fn QwMobileSprite::QwMobileSprite(QwSpritePixmapSequence* s) 

Construct a QwMobileSprite.  See QwPositionedSprite::QwPositionedSprite(QwSpritePixmapSequence*).
*/

/*!
\fn QwMobileSprite::QwMobileSprite()

Construct a QwMobileSprite.  See QwPositionedSprite::QwPositionedSprite().
*/
