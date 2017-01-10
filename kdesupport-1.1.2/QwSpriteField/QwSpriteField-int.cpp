#include "QwSpriteField.h"
#include "QwSpriteField-template.cpp"

/*!
\class QwSprite QwSpriteField.h
This class is simply an instantiation of the QwPositionedSprite class,
using integers as the coordinate type.  For documentation of the methods,
see the documentation for QwPositionedSprite.
*/
template class QwPositionedSprite<int>;

/*!
\fn QwSprite::QwSprite(QwSpritePixmapSequence* s) 

Construct a QwSprite.  See QwPositionedSprite::QwPositionedSprite(QwSpritePixmapSequence*).
*/

/*!
\fn QwSprite::QwSprite ()

Construct a QwSprite.  See QwPositionedSprite::QwPositionedSprite().
*/
