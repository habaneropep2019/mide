#include "QwSpriteField.h"
#include "QwSpriteField-template.cpp"

/*!
\class QwRealSprite QwSpriteField.h
This class is simply an instantiation of the QwPositionedSprite class,
using doubles as the coordinate type.  For documentation of the methods,
see the documentation for QwPositionedSprite.
*/
template class QwPositionedSprite<double>;

/*!
\fn QwRealSprite::QwRealSprite(QwSpritePixmapSequence* s) 

Construct a QwRealSprite.  See QwPositionedSprite::QwPositionedSprite(QwSpritePixmapSequence*).
*/

/*!
\fn QwRealSprite::QwRealSprite ()

Construct a QwRealSprite.  See QwPositionedSprite::QwPositionedSprite().
*/
