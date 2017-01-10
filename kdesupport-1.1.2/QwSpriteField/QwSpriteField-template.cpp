#include "QwSpriteField.h"

/*!
\class QwPositionedSprite QwSpriteField.h
\brief A QwVirtualSprite with a stored position and frame number.

This is a template class which allows any numerical class to be used as
the stored coordinate type of a QwVirtualSprite.  Two derived classes,
QwSprite and QwRealSprite provide \t integer and \t double instantiations
of this class.
*/

/*!
\fn QwPositionedSprite::QwPositionedSprite(QwSpritePixmapSequence* seq)

Create a QwPositionedSprite<COORD> which uses images from the given sequence.

The sprite in initially at (0,0) on the current spritefield
(see constructor for QwSpriteField), using the 0th sequence frame.
*/
template<class COORD>
QwPositionedSprite<COORD>::QwPositionedSprite(QwSpritePixmapSequence* seq) :
    myx(0),
    myy(0),
    frm(0),
    alt(0),
    images(seq)
{
    show();
    addToChunks();
}

/*!
\fn QwPositionedSprite::QwPositionedSprite()

Create a QwPositionedSprite<COORD> without defining its image sequence.

The sprite in initially at (0,0) on the current spritefield
(see constructor for QwSpriteField), using the 0th sequence frame.

Note that you must call setSequence(QwSpritePixmapSequence*) before
doing anything else with the sprite.
*/
template<class COORD>
QwPositionedSprite<COORD>::QwPositionedSprite() :
    myx(0),
    myy(0),
    frm(0),
    alt(0),
    images(0)
{
}

/*!
\fn void QwPositionedSprite::setSequence(QwSpritePixmapSequence* seq)

Set the sequence of images used for displaying the sprite.  Note that
the sequence should have enough images for the sprites current frame()
to be valid.
*/
template<class COORD>
void QwPositionedSprite<COORD>::setSequence(QwSpritePixmapSequence* seq)
{
    bool vis=visible();
    if (vis && images) hide();
    images=seq;
    if (vis) show();
}

/*!
\fn QwPositionedSprite::changeChunks()

\internal

Marks any chunks the sprite touches as changed.
*/
template<class COORD>
void QwPositionedSprite<COORD>::changeChunks()
{
    if (visible() && spritefield) {
        int chunksize=spritefield->chunkSize();
        for (int j=absY()/chunksize; j<=absY2()/chunksize; j++) {
            for (int i=absX()/chunksize; i<=absX2()/chunksize; i++) {
                spritefield->setChangedChunk(i,j);
            }
        }
    }
}

/*!
\fn QwPositionedSprite::~QwPositionedSprite()

Destruct the sprite.
It is removed from its QwSpriteField in this process.
*/
template<class COORD>
QwPositionedSprite<COORD>::~QwPositionedSprite()
{
    removeFromChunks();
}

/*!
\fn int QwPositionedSprite::x() const

Returns the stored horizontal position of the sprite.
*/

/*!
\fn int QwPositionedSprite::y() const

Returns the stored vertical position of the sprite.
*/

/*!
\fn int QwPositionedSprite::z() const

Returns the stored z of the sprite.
*/

/*!
\fn void QwPositionedSprite::z(int a)

Sets the stored z of the sprite.
*/

/*!
\fn void QwPositionedSprite::frame(int f)

Set the animation frame used for displaying the sprite to
the given index into the QwPositionedSprite<COORD>'s QwSpritePixmapSequence.

\sa moveTo(COORD,COORD,int)
*/
template<class COORD>
void QwPositionedSprite<COORD>::frame(int f)
{
    moveTo(myx,myy,f);
}

/*!
\fn int QwPositionedSprite::frame() const
Returns the index into the QwPositionedSprite<COORD>'s QwSpritePixmapSequence
of the current animation frame.

\sa moveTo(COORD,COORD,int)
*/

/*!
\fn int QwPositionedSprite::frameCount() const
Returns the number of frames in the QwPositionedSprite<COORD>'s QwSpritePixmapSequence.
*/

/*!
\fn void QwPositionedSprite::x(COORD nx)
Set the horizontal position of the sprite.

\sa moveTo(COORD,COORD)
*/
template<class COORD>
void QwPositionedSprite<COORD>::x(COORD nx)
{
    moveTo(nx,myy);
}

/*!
\fn void QwPositionedSprite::y(COORD ny)
Set the vertical position of the sprite.

\sa moveTo(COORD,COORD)
*/
template<class COORD>
void QwPositionedSprite<COORD>::y(COORD ny)
{
    moveTo(myx,ny);
}

/*!
\fn void QwPositionedSprite::moveBy(COORD dx, COORD dy)
Move the sprite from its current position by the given amounts.
*/
template<class COORD>
void QwPositionedSprite<COORD>::moveBy(COORD dx, COORD dy)
{
    moveTo(myx+dx,myy+dy);
}

/*!
\fn void QwPositionedSprite::moveTo(COORD nx, COORD ny)
Move the sprite to the given absolute position.

\sa moveBy(COORD,COORD) moveTo(COORD,COORD,int)
*/
template<class COORD>
void QwPositionedSprite<COORD>::moveTo(COORD nx, COORD ny)
{
    moveTo(nx,ny,frm);
}

/*!
\fn void QwPositionedSprite::moveTo(COORD nx, COORD ny, int nf)

Set both the position and the frame of the sprite.
*/
template<class COORD>
void QwPositionedSprite<COORD>::moveTo(COORD nx, COORD ny, int nf)
{
    if (myx!=nx || myy!=ny || frm!=nf) {
        if (nf==frm
        && (!spritefield ||
            spritefield->sameChunk(absX(),absY(),absX()-(int)myx+(int)nx,absY()-(int)myy+(int)ny)
            && spritefield->sameChunk(absX2(),absY2(),absX2()-(int)myx+(int)nx,absY2()-(int)myy+(int)ny)))
        {
            myx=nx;
            myy=ny;
            changeChunks();
        } else {
            removeFromChunks();
            myx=nx;
            myy=ny;
            frm=nf;
            addToChunks();
        }
    }
}


/*!
\fn Pix QwPositionedSprite::neighbourhood(int nx, int ny) const
Same as QwVirtualSprite::neighbourhood(int x, int y).
*/

/*!
\fn Pix QwPositionedSprite::neighbourhood(int nframe) const
Similar to QwVirtualSprite::neighbourhood(QwSpritePixmap*), but
the image is specified by index rather than actual value.
*/
template<class COORD>
Pix QwPositionedSprite<COORD>::neighbourhood(int nframe) const
{ return neighbourhood(myx,myy,nframe); }

/*!
\fn Pix QwPositionedSprite::neighbourhood(COORD nx, COORD ny, int nframe) const
Similar to QwVirtualSprite::neighbourhood(int x, int y, QwSpritePixmap*), but
the image is specified by index rather than actual value.
*/
template<class COORD>
Pix QwPositionedSprite<COORD>::neighbourhood(COORD nx, COORD ny, int nframe) const
{ return QwVirtualSprite::neighbourhood((int)nx,(int)ny,image(nframe)); }

/*!
\fn bool QwPositionedSprite::wouldHit(QwSpriteFieldGraphic& other, COORD x, COORD y, int frame) const
Similar to QwVirtualSprite::wouldHit(QwSpriteFieldGraphic&, int x, int y, QwSpritePixmap*),
but the image is specified by index rather than actual value.
*/
template<class COORD>
bool QwPositionedSprite<COORD>::wouldHit(QwSpriteFieldGraphic& other, COORD x, COORD y, int frame) const
{
    return QwVirtualSprite::wouldHit(other,(int)x,(int)y,image(frame));
}

/*!
\fn COORD QwPositionedSprite::exact_x() const
Returns the X-position, in COORD units rather than the integer pixel
coordinates used by the graphical engine.
*/

/*!
\fn COORD QwPositionedSprite::exact_y() const
Returns the Y-position, in COORD units rather than the integer pixel
coordinates used by the graphical engine.
*/

/*!
\fn int QwPositionedSprite::rtti() const
Returns 2.

\sa QwSpriteFieldGraphic::rtti()
*/
template<class COORD>
int QwPositionedSprite<COORD>::rtti() const { return 2; }


/*!
\class QwMobilePositionedSprite QwSpriteField.h

A QwPositionedSprite which has \e velocity as well as position, and a
bounding area in which it may move.  This class is useful for simple
applications where objects have simple motion.  More complex uses will
require use of one of the QwPositionedSprite derived classes,
QwSprite or QwRealSprite.
*/

/*!
\fn QwMobilePositionedSprite::QwMobilePositionedSprite(QwSpritePixmapSequence*)
Create a QwPositionedSprite which uses images from the given sequence.
*/
template <class COORD>
QwMobilePositionedSprite<COORD>::QwMobilePositionedSprite(QwSpritePixmapSequence* s) :
    QwPositionedSprite<COORD>(s),
    bounds_action(Ignore),
    dx(0),dy(0)
{
    adoptSpritefieldBounds();
}

/*!
\fn QwMobilePositionedSprite::QwMobilePositionedSprite()
Create a QwPositionedSprite without defining its image sequence. 
Note that you must call setSequence(QwSpritePixmapSequence*) before doing anything else with the sprite.
*/
template <class COORD>
QwMobilePositionedSprite<COORD>::QwMobilePositionedSprite() :
    QwPositionedSprite<COORD>(),
    bounds_action(Ignore),
    dx(0),dy(0)
{
    adoptSpritefieldBounds();
}

/*!
\fn void QwMobilePositionedSprite::adoptSpritefieldBounds()
Use the width and height of the sprite's spritefield as the bounds.
This is the default when the sprite is created.
*/
template <class COORD>
void QwMobilePositionedSprite<COORD>::adoptSpritefieldBounds()
{
    if (spritefield) {
        bounds(0,0,spritefield->width()-1,spritefield->height()-1);
    } else {
        // Simple default so programmer can see the problem
        bounds(0,0,50,50);
    }
}

/*!
\fn void QwMobilePositionedSprite::bounds(COORD l, COORD t, COORD r, COORD b)
Set the bounds of the sprite's motion.
The default is the bounds of the Spritefield containing the sprite.
*/
template <class COORD>
void QwMobilePositionedSprite<COORD>::bounds(COORD l, COORD t, COORD r, COORD b)
{
    b_left=l;
    b_top=t;
    b_right=r;
    b_bottom=b;
}

/*!
\fn void QwMobilePositionedSprite::setBoundsAction(int a)
Set the action to perform when the sprite hits its bounds.
It can:
<ul>
 <li>Ignore the bounds, continuing on.
 <li>Stop moving at the boundary.
 <li>Wrap from one side of the boundary to the other.
 <li>Bounce away from the boundary as it hits.
</ul>
The default is to Ignore the bounds.
*/
template <class COORD>
void QwMobilePositionedSprite<COORD>::setBoundsAction(int a)
{
    bounds_action=a;
}

/*!
\fn bool QwMobilePositionedSprite::outOfBounds()
Returns TRUE if the sprite has moved out of its bounds.  This should only
happen if the bounds action is set to Ignore.
*/
template <class COORD>
bool QwMobilePositionedSprite<COORD>::outOfBounds()
{
    return (exact_x()<b_left) || (exact_x()>b_right)
        || (exact_y()<b_top) || (exact_y()>b_bottom);
}

/*!
\fn void  QwMobilePositionedSprite::moveTo(COORD x, COORD y) 
Override to implement bounds action.
*/

template <class COORD>
COORD QwMobilePositionedSprite<COORD>::mod(COORD a,COORD m)
{
   return a-((int)(a/m))*a;
}


/*!
\fn void QwMobilePositionedSprite::moveTo(COORD x, COORD y, int frame)
Override to implement bounds action.
*/
template <class COORD>
void QwMobilePositionedSprite<COORD>::moveTo(COORD x, COORD y, int frame)
{
   if (x<b_left) {
      switch (bounds_action) { 
         case Ignore: break;
         case Stop: 
            x=b_left; break;
         case Wrap:
            x=b_right-mod(b_left-x,b_right-b_left);
            break;
         case Bounce:
            x=b_left+mod(b_left-x,b_right-b_left);
            dx=-dx;
      }
   } else if (x>b_right) {
      switch (bounds_action) { 
         case Ignore: break;
         case Stop: 
            x=b_right; break;
         case Wrap:
            x=b_left+mod(x-b_right,b_right-b_left);
            break;
         case Bounce:
            x=b_right-mod(x-b_right,b_right-b_left);
            dx=-dx;
      }
   }

   if (y<b_top) {
      switch (bounds_action) { 
         case Ignore: break;
         case Stop: 
            y=b_top; break;
         case Wrap:
            y=b_bottom-mod(b_top-y,b_bottom-b_top);
            break;
         case Bounce: 
            y=b_top+mod(b_top-y,b_bottom-b_top);
            dy=-dy;
      }
   } else if (y>b_bottom) {
      switch (bounds_action) { 
         case Ignore: break;
         case Stop: 
            y=b_bottom; break;
         case Wrap:
            y=b_top+mod(y-b_bottom,b_bottom-b_top);
            break;
         case Bounce:
            y=b_bottom-mod(y-b_bottom,b_bottom-b_top);
            dy=-dy;
      }
   }

    QwPositionedSprite<COORD>::moveTo(x,y,frame);
}

/*!
\fn void QwMobilePositionedSprite::forward(COORD multiplier)
Move forward (or backward if multiplier is negative) by the given amount
multiplied by the current velocity of the sprite.
*/
template <class COORD>
void QwMobilePositionedSprite<COORD>::forward(COORD multiplier)
{
    QwPositionedSprite<COORD>::moveTo(exact_x()+multiplier*dx, exact_y()+multiplier*dy);
}

/*!
\fn Pix QwMobilePositionedSprite::forwardHits(COORD multiplier, int frame) const
Returns an iterator over objects that might be hit if the sprite
moves forward, and changes frame.  Note that if the movement would not change the pixel
position of the sprite, 0 is returned, even if the sprite is already
in contact with something - the assumption being that such a collision
has already been dealt with.
*/
template <class COORD>
Pix QwMobilePositionedSprite<COORD>::forwardHits(COORD multiplier, int frme) const
{
    int ix = int(exact_x()+multiplier*dx);
    int iy = int(exact_y()+multiplier*dy);
    if (ix == x() && iy == y())
	return 0;
    return QwPositionedSprite<COORD>::neighbourhood((COORD)ix, (COORD)iy, frme);
}

/*!
\fn Pix QwMobilePositionedSprite::forwardHits(COORD multiplier) const
Returns an iterator over objects that might be hit if the sprite
moves forward.  Note that if the movement would not change the pixel
position of the sprite, 0 is returned, even if the sprite is already
in contact with something - the assumption being that such a collision
has already been dealt with.
*/
template <class COORD>
Pix QwMobilePositionedSprite<COORD>::forwardHits(COORD multiplier) const
{
    int ix = int(exact_x()+multiplier*dx);
    int iy = int(exact_y()+multiplier*dy);
    if (ix == x() && iy == y())
	return 0;
    return QwPositionedSprite<COORD>::neighbourhood(ix, iy);
}

/*!
\fn void QwMobilePositionedSprite::forward(COORD multiplier, int frame)
Move forward (or backward if multiplier is negative) by the given amount
multiplied by the current velocity of the sprite, and set the frame to
the given index.
*/
template <class COORD>
void QwMobilePositionedSprite<COORD>::forward(COORD multiplier, int frme)
{
    moveTo(exact_x()+multiplier*dx, exact_y()+multiplier*dy, frme);
}

/*!
\fn void QwMobilePositionedSprite::setVelocity(COORD dX, COORD dY) 
Set the velocity of the sprite.  It will only actually move when
QwMobilePositionedSprite::forward is called.
*/

/*!
\fn COORD QwMobilePositionedSprite::dX() const
Returns the horizontal velocity of the sprite.
\sa QwMobilePositionedSprite::setVelocity
*/

/*!
\fn COORD QwMobilePositionedSprite::dY() const
Returns the vertical velocity of the sprite.
\sa QwMobilePositionedSprite::setVelocity
*/

