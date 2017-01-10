/****************************************************************************
**
** Implementation of QwSpriteField and associated classes
**
** Author  : Warwick Allison (warwick@troll.no)
** Created : 19960330
**
** Copyright (C) 1996-1997 by Warwick Allison.
**
*****************************************************************************/


#include "QwSpriteField.h"
#include "QwPublicList.cpp"
#include "QwCluster.h"
#include <qpainter.h>
#include <qimage.h>
#include <qwidget.h>
#include <qfile.h>

typedef QwSpriteFieldGraphic* GraphicPtr;
typedef QwPublicList< GraphicPtr > GraphicList;
//template class QwPublicList<GraphicPtr>;
//template void sort(QwPublicList<QwSpriteFieldGraphic *> *&, int (*)(QwSpriteFieldGraphic *const &, QwSpriteFieldGraphic *const &), bool);
//template QwPublicList<QwSpriteFieldGraphic *> *remove(QwSpriteFieldGraphic *, QwPublicList<QwSpriteFieldGraphic *> *&);

static int compareZ(void* ap, void* bp)
{
    GraphicPtr& a = *(GraphicPtr*)ap;
    GraphicPtr& b = *(GraphicPtr*)bp;
    // Order same-z objects by identity.
    if (a->z()==b->z())
	return (long)a - (long)b;

    return a->z() - b->z();
}


static int compareGraphics(const void* a, const void* b)
{
    const QwSpriteFieldGraphic* aa=*(QwSpriteFieldGraphic**)a;
    const QwSpriteFieldGraphic* bb=*(QwSpriteFieldGraphic**)b;

    // Order same-z objects by identity.
    if (aa->z()==bb->z())
	return (long)aa > (long)bb;

    return aa->z() > bb->z();
}


class QwChunkRec {
public:
    QwChunkRec() : head(0), changed(TRUE) { }
    // Other code assumes lists are not deleted.  Assignment is also
    // done on ChunkRecs.  So don't add that sort of thing here.

    QwPublicList<GraphicPtr>* head;
    bool changed;
};




/*!
\class QwSpriteField QwSpriteField.h
\brief A QwSpriteField is a 2D graphic area upon which QwSpriteFieldGraphic objects are drawn.

The QwSpriteField and related classes (primarily QwSpriteFieldView and
QwSprite, but also the other sprite abstractions)
provide for multiple concurrent views of a 2D area
containing moving graphical objects.  

The QwSpriteField is also an indexing mechanism to the sprites it contains,
providing 2D-area-based iteration and pixelwise collision detection.

Most of the methods in QwSpriteField are mainly used by the existing
QwSpriteFieldGraphic classes, so will only be interesting if you intend
to add a new type of QwSpriteFieldGraphic class.  The methods of primary
interest to the typical user are, in approximate
order of decreasing usefulness:

<ul>
      <li> \link QwSpriteField QwSpriteField \endlink (int w, int h, int chunksize, int maxclusters) 
      <li> void \link update update \endlink () 
      <li> int \link width width \endlink () const 
      <li> int \link height height \endlink () const 
      <li> Pix \link topAt topAt \endlink (int x, int y) 
      <li> Pix \link lookIn lookIn \endlink (int x, int y, int w, int h) 
      <li> QwSpriteFieldGraphic* \link at at \endlink (Pix) const 
      <li> bool \link exact exact \endlink (Pix) const 
      <li> void \link next next \endlink (Pix&) 
      <li> void \link end end \endlink (Pix&) 
      <li> void \link protectFromChange protectFromChange \endlink (Pix) 
      <li> virtual void \link drawBackground drawBackground \endlink (QPainter&, const QRect& area) 
      <li> virtual void \link drawForeground drawForeground \endlink (QPainter&, const QRect& area) 
</ul>

This class provides for very high redraw efficiency.  The properties
of the mechanism are:

<ul>
    <li>There is no size or number limitation on sprites (except memory).
    <li>QwSprites are fairly light-weight (small in memory size).
    <li>QwVirtualSprites are very light-weight (small in memory size).
    <li>Collision detection is efficient.
    <li>Finding sprites in an area is efficient.
    <li>Only moving sprites are redrawn.
    <li>The number of unmoving sprites on the field has little effect.
    <li>Redraws can be batched (see \link update update\endlink)
    <li>Only visible areas (those in the visible part of a QwSpriteFieldView) are redrawn.
</ul>

For example, it is quite feasible to have \e thousands of sprites used to
create background scenery, and even animate pieces of that scenery occasionally.
It is less feasible to have views where the entire scene is in continous
motion, such as a continously scrolling background.  You will be suprised
how little animation is required to make a scene look `alive'.

The various sprite abstractions are:

<ul>
    <dt><b>QwSprite</b>
	<dd>A sprite with a position given by int coordinates.
    <dt><b>QwRealSprite</b>
	<dd>A sprite with a position given by double coordinates.
    <dt><b>QwPositionedSprite&lt;COORD&gt;</b>
	<dd>A sprite with a position given by COORD coordinates.  This
	    is a template class.
    <dt><b>QwVirtualSprite</b>
	<dd>A sprite with a position determined dynamically.
</ul>

QwVirtualSprite is the base class, from which is derived QwPositionedSprite,
and from that is derived the QwSprite and QwRealSprite classes.  This hierarchy
provides you with multiple points at which to adapt to your requirements.

For the purpose of simplified description, we use the term sprite to refer
to all these and derived classes.
*/

/*!
Returns the chunk at a chunk position.
*/
QwChunkRec& QwSpriteField::chunk(int i, int j) const
{ return chunks[i+chwidth*j]; }

/*!
Returns the chunk at a pixel position.
*/
QwChunkRec& QwSpriteField::chunkContaining(int x, int y) const
{ return chunk(x/chunksize,y/chunksize); }

/*!
Returns the sorted list of chunks at the given chunk.  You should
not need this method - it is used internally for collision detection.
\sa QwSpriteField::topAt(int,int)
*/
void* QwSpriteField::listAtChunkTopFirst(int i, int j) const
{
    if (i>=0 && i<chwidth && j>=0 && j<chheight) {
	::sort(chunk(i,j).head,compareZ,TRUE);
	return (void*)chunk(i,j).head;
    } else {
	return 0;
    }
}

/*!
Returns the sorted list of all graphics.  You should
not need this method - it is used internally for iteration.
\sa QwSpriteField::all()
*/
void* QwSpriteField::allList()
{
    GraphicList* list=0;
    for (QwSpriteFieldGraphic* graphic=graphicList.first(); graphic != 0; graphic=graphicList.next()) {
	list=new GraphicList(graphic,list);
    }
    return (void*)list;
}

/*!
\fn void QwSpriteField::setPositionPrecision(int downshifts)

The precision of QwSpriteFieldGraphic positions (and collisions - see sprite) can
be set.  A positive precision means world co-ordinates will be
bit-shifted that many places higher.  So, if the precision is 1,
world co-ordinate (50,100) is the pixel co-ordinate (25,50); for
precision 3, (50,100) is pixel (6,12).  Negative positions are
also allowed, and they imply that world-coordinates are \e lower
resolution than pixel co-ordinates. IF ANYONE FINDS A USE FOR NEGATIVES,
LET ME KNOW, OR I MIGHT REMOVE IT.

By default, pixel and world coordinates are the same.

*/

/*!
\fn int QwSpriteField::positionPrecision()
Returns the position precision for all coordinates in the QwSpriteField.
\sa setPositionPrecision(int)
*/

/*!
\fn int QwSpriteField::world_to_x(int i)
Convert a coordinate from world to pixel coordinates.
\sa setPositionPrecision(int)
*/

/*!
\fn int QwSpriteField::x_to_world(int i)
Convert a coordinate from pixel to world coordinates.
\sa setPositionPrecision(int)
*/

/*!
Constructs a QwSpriteField with size \c w wide and \c h high, measured
in world coordinates (by default, world coordinates equal pixel coordinates).

The values \c maxclusters and \c chunksize effect the grouping heuristics
used when redrawing the QwSpriteField.

   \c chunksize is the size of square chunk used to break up the
     QwSpriteField into area to be considered for redrawing.  It
     should be about the average size of graphics in the QwSpriteField.
     Chunks too small increase the amount of calculation required
     when drawing.  Chunks too large increase the amount of drawing
     that is needed.

   \c maxclusters is the number of rectangular groups of chunks that
     will be separately drawn.  If the QwSpriteField has a large number
     of small, dispersed graphics, this should be about that number.
     The more clusters the slower the redraw, but also the bigger
     clusters are the slower the redraw, so a balance is needed.
     Testing reveals that a large number of clusters is almost
     always best.
*/
QwSpriteField::QwSpriteField(int w, int h, int chunksze, int maxclust) :
    awidth(w),aheight(h),
    chunksize(chunksze),
    maxclusters(maxclust),
    chwidth((w+chunksize-1)/chunksize),
    chheight((h+chunksize-1)/chunksize),
    chunks(new QwChunkRec[chwidth*chheight])
{
    QwSpriteFieldGraphic::setCurrentSpriteField(this);
}

/*!
Create a QwSpriteField with no size, and default chunksize/maxclusters.
You will want to call resize(int,int) at some time after creation.
*/
QwSpriteField::QwSpriteField() :
    awidth(0),aheight(0),
    chunksize(16),
    maxclusters(100),
    chwidth(0),
    chheight(0),
    chunks(new QwChunkRec[1])
{
    QwSpriteFieldGraphic::setCurrentSpriteField(this);
}

/*!
Destruct the field.  Does nothing.  Does \e not destroy QwSpriteFieldGraphic objects in
the field.
*/
QwSpriteField::~QwSpriteField()
{
}

/*!
Change the size of the QwSpriteField. This is a slow operation.
*/
void QwSpriteField::resize(int w, int h)
{
    if (awidth==w && aheight==h)
	return;

    QwSpriteFieldGraphic* graphic;
    QList<QwSpriteFieldGraphic> hidden;
    for (graphic=graphicList.first(); graphic != 0; graphic=graphicList.next()) {
	if (graphic->visible()) {
	    graphic->hide();
	    hidden.append(graphic);
	}
    }

    int nchwidth=(w+chunksize-1)/chunksize;
    int nchheight=(h+chunksize-1)/chunksize;

    QwChunkRec* newchunks = new QwChunkRec[nchwidth*nchheight];

    // Commit the new values.
    //
    awidth=w;
    aheight=h;
    chwidth=nchwidth;
    chheight=nchheight;
    delete chunks;
    chunks=newchunks;

    for (graphic=hidden.first(); graphic != 0; graphic=hidden.next()) {
	graphic->show();
    }

    for (QwAbsSpriteFieldView* view=viewList.first(); view != 0; view=viewList.next()) {
	view->updateGeometries();
    }
}

/*!
Change the efficiency tuning parameters.  This is a slow operation.
*/
void QwSpriteField::retune(int chunksze, int mxclusters)
{
    QList<QwSpriteFieldGraphic> hidden;
    QwSpriteFieldGraphic* graphic;
    for (graphic=graphicList.first(); graphic != 0; graphic=graphicList.next()) {
	if (graphic->visible()) {
	    graphic->hide();
	    hidden.append(graphic);
	}
    }

    chunksize=chunksze;
    maxclusters=mxclusters;

    int nchwidth=(awidth+chunksize-1)/chunksize;
    int nchheight=(aheight+chunksize-1)/chunksize;

    QwChunkRec* newchunks = new QwChunkRec[nchwidth*nchheight];

    // Commit the new values.
    //
    chwidth=nchwidth;
    chheight=nchheight;
    delete chunks;
    chunks=newchunks;

    for (graphic=hidden.first(); graphic != 0; graphic=hidden.next()) {
	graphic->show();
    }
}

/*!
\fn int QwSpriteField::width() const
Returns the width of the sprite field, in world coordinates.
*/

/*!
\fn int QwSpriteField::height() const
Returns the height of the sprite field, in world coordinates.
*/

/*!
\fn int QwSpriteField::chunkSize() const
Returns the chunk size of the sprite field as set at construction.
\sa QwSpriteField::QwSpriteField(...)
*/

/*!
\fn bool QwSpriteField::sameChunk(int x1, int y1, int x2, int y2) const
Tells if the points (x1,y1) and (x2,y2) are within the same chunk.
*/

/*!
This method is called for all updates of the QwSpriteField.  It renders
any background graphics.  The default is to simply clear it in the
background colour to the default painter background (white).  You may
also override this method to initialize the QPainter which will be passed
to the draw method of all QwSpriteFieldGraphic objects.

Note that you should not put \e dynamic graphics on this background.
If the graphics for a region change, call forceRedraw(const QRect&).
Such a change should be done only very occasionally, as the redraw
is inefficient.  See the main notes for this class for a discussion
of redraw efficiency.

\sa forceRedraw(QRect&)
*/
void QwSpriteField::drawBackground(QPainter& painter, const QRect& area)
{
    painter.eraseRect(area);
}

/*!
This method is called for all updates of the QwSpriteField.  It renders
any foreground graphics.  In general, it is more efficient to use additional
QwSpriteFieldGraphic objects on the QwSpriteField rather than adding rendering
at this point, as this method is <em>always</em> called, whereas
QwSpriteFieldGraphic objects are only redrawn if they are in the area of change.

The same warnings regarding change apply to this method
as given in drawBackground(QPainter&, const QRect&).

The default is to do nothing.

\sa forceRedraw(QRect&)
*/
void QwSpriteField::drawForeground(QPainter&, const QRect&)
{
}

/*!
Forces the given area to be redrawn.  This is useful when the foreground
or background are changed.
*/
void QwSpriteField::forceRedraw(const QRect& area)
{
    drawArea(area, FALSE, 0);
}

/*!
(internal)
This method adds an element to the list of QwSpriteFieldGraphic objects
in this QwSpriteField.  The QwSpriteFieldGraphic class calls this, so you
should not need it.
*/
void QwSpriteField::addGraphic(QwSpriteFieldGraphic* graphic)
{
    graphicList.append(graphic);
}

/*!
(internal)
This method removes an element from the list of QwSpriteFieldGraphic objects
in this QwSpriteField.  The QwSpriteFieldGraphic class calls this, so you
should not need it.
*/
void QwSpriteField::removeGraphic(QwSpriteFieldGraphic* graphic)
{
    // ### O(N)
    graphicList.removeRef(graphic);
}

/*!
\internal
This method adds an element to the list of QwAbsSpriteFieldView objects
viewing this QwSpriteField.  The QwAbsSpriteFieldView class calls this, so you
should not need it.
*/
void QwSpriteField::addView(QwAbsSpriteFieldView* view)
{
    viewList.append(view);
}

/*!
\internal
This method removes an element from the list of QwAbsSpriteFieldView objects
viewing this QwSpriteField.  The QwAbsSpriteFieldView class calls this, so you
should not need it.
*/
void QwSpriteField::removeView(QwAbsSpriteFieldView* view)
{
    viewList.removeRef(view);
}

/*!
This method causes all QwAbsSpriteFieldView objects currently viewing this
QwSpriteField to be refreshed in all areas of the QwSpriteField which have
changed since the last call to this method.

In a continuously animated QwSpriteField, this method should be called at
a regular rate.  The best way to accomplish this is to use the QTimer
class of the Qt toolkit, or the startTimer method of QObject.  It would
not be useful to call this method excessively (eg. whenever a sprite
is moved) as the efficiency of the system is derived from a clustering
and bulk-update mechanism.

Depending on how you use QwSpriteField and on the hardware, you may get
improved smoothness by calling \link QApplication::syncX qApp->syncX()\endlink
after this method.
*/
void QwSpriteField::update()
{
    QwClusterizer clusterizer(viewList.count());

    for (QwAbsSpriteFieldView* view=viewList.first(); view != 0; view=viewList.next()) {
	QRect area=view->viewArea();
	if (area.width()>0 && area.height()>0) {
	    clusterizer.add(area);
	}
    }

    for (int i=0; i<clusterizer.clusters(); i++) {
	drawArea(clusterizer[i],TRUE,0);
    }
}

/*!
\internal
This method causes a specific area of a QwSpriteField to be completely
redrawn in a given QwAbsSpriteFieldView.  This is called by QwAbsSpriteFieldView
to effect repaint events.
*/
void QwSpriteField::updateInView(QwAbsSpriteFieldView* view, const QRect& area)
{
    drawArea(area,FALSE,view);
}


/*!
\internal
Redraw a given area of the QwSpriteField.

If only_changes then only changes to the area are redrawn.

If one_view then only one view is updated, otherwise all are.
*/
void QwSpriteField::drawArea(const QRect& inarea, bool only_changes, QwAbsSpriteFieldView* one_view)
{
    QRect area=inarea.intersect(QRect(0,0,width(),height()));

    if (only_changes) {
	QwClusterizer clusters(maxclusters);

	int mx=(area.x()+area.width()+chunksize)/chunksize;
	int my=(area.y()+area.height()+chunksize)/chunksize;
	if (mx>chwidth) mx=chwidth;
	if (my>chheight) my=chheight;

	for (int x=area.x()/chunksize; x<mx; x++) {
	    for (int y=area.y()/chunksize; y<my; y++) {
		QwChunkRec& ch=chunk(x,y);
		if (ch.changed) {
		    clusters.add(x,y);
		    ch.changed=FALSE;
		}
	    }
	}

	for (int i=0; i<clusters.clusters(); i++) {
	    QRect elarea=clusters[i];
	    elarea.setRect(
		elarea.left()*chunksize,
		elarea.top()*chunksize,
		elarea.width()*chunksize,
		elarea.height()*chunksize
	    );
	    drawArea(elarea,FALSE,one_view);
	}
    } else {
	if (!viewList.first() && !one_view) return; // Nothing to do yet.

	// XXX
	// NON-REENTRANT.  Make this a member of QwSpriteField, or always reallocate
	// it in order to make this function reentrant.
	// XXX
	static int max=16;
	static QwSpriteFieldGraphic* *allvisible=new QwSpriteFieldGraphic*[max];

	int count=0;
	int lx=area.x()/chunksize;
	int ly=area.y()/chunksize;
	int mx=(area.x()+area.width()+chunksize)/chunksize;
	int my=(area.y()+area.height()+chunksize)/chunksize;
	if (mx>chwidth) mx=chwidth;
	if (my>chheight) my=chheight;

	for (int x=lx; x<mx; x++) {
	    for (int y=ly; y<my; y++) {
		GraphicList* cursor;

		// Only reset change if all views updating, and
		// wholy within area. (conservative:  ignore entire boundary)
		//
		// Disable this to help debugging.
		//
		if (!one_view) {
		    if (x>lx && x<mx-1 && y>ly && y<my-1)
			chunk(x,y).changed=FALSE;
		}

		for (cursor=chunk(x,y).head; cursor; cursor=cursor->next) {
		    if (count == max) {
			max*=2;
			QwSpriteFieldGraphic* *newallvisible=new QwSpriteFieldGraphic*[max];
			for (int i=0; i<count; i++) {
			    newallvisible[i]=allvisible[i];
			}
			allvisible=newallvisible;
		    }
		    allvisible[count++]=cursor->element;
		}
	    }
	}
	qsort(allvisible, count, sizeof(*allvisible), compareGraphics);

	QPainter painter;

	// Does any view need the offscreen pixmap?
	bool double_buffer=FALSE;

	QwAbsSpriteFieldView* view=0;

	for (view=one_view ? one_view : viewList.first();
	    view != 0; view=(one_view || double_buffer) ? 0 : viewList.next())
	{
	    double_buffer=double_buffer || view->preferDoubleBuffering();
	}

	view=0; // Explicitly.

	bool first_double_buffer=double_buffer;

	while (1)
	{
	    if (first_double_buffer) {
		// First run through with double-buffer - build buffer
		first_double_buffer=FALSE;
		int osw = world_to_x(area.width());
		int osh = world_to_x(area.height());
		if ( osw > offscr.width() || osh > offscr.height() )
		    offscr.resize(QMAX(osw,offscr.width()),QMAX(osh,offscr.height()));
		painter.begin(&offscr);
		//XXX painter.setViewport(-area.x(),-area.y(),area.width(),area.height());
		painter.translate(-area.x(),-area.y());
		view=0;
	    } else {
		if (one_view) {
		    if (view==one_view) break;
		    view=one_view;
		} else {
		    if (view==0) view=viewList.first();
		    else view=viewList.next();
		    if (!view) break;
		}
	    }

	    if (view && double_buffer) {
		// Double-buffer available - use the offscreen pixmap.
		view->beginPainter(painter);
		painter.setClipping(FALSE);
		painter.drawPixmap(area.topLeft(), offscr, QRect(QPoint(0,0),area.size()));
		view->flush(area);
		painter.end();
	    } else {
		// A non-double-buffered view - draw on painter setup by it.
		if (view) {
		    // (if not, it is the double-buffer and painter is set up)
		    view->beginPainter(painter);
		    // Need to xform clip rect
		    QPoint p=painter.xForm(area.topLeft());
		    painter.setClipRect(p.x(),p.y(),area.width(),area.height());
		}

		drawBackground(painter,area);

		QwSpriteFieldGraphic* prev=0;
		int drawn=0;
		for (int i=0; i<count; i++) {
		    QwSpriteFieldGraphic *g=allvisible[i];
		    if (g!=prev) {
			g->draw(painter);
			prev=g;
			drawn++;
		    }
		}

		drawForeground(painter,area);

		if (view) {
		    view->flush(area);
		    painter.end();
		} else {
		    painter.end();
		}
	    }
	}
    }
}

/*!
This method to informs the QwSpriteField that a given chunk is
`dirty' and needs to be redrawn in the next Update.

(x,y) is a chunk location.

The sprite classes call this.  Any new derived class
of QwSpriteFieldGraphic must do so too.  SetChangedChunkContaining can be used
instead.
*/
void QwSpriteField::setChangedChunk(int x, int y)
{
    if (x>=0 && x<chwidth && y>=0 && y<chheight) {
	QwChunkRec& ch=chunk(x,y);
	ch.changed=TRUE;
    }
}

/*!
This method to informs the QwSpriteField that the chunk containing
a given pixel is `dirty' and needs to be redrawn in the next Update.

(x,y) is a pixel location.

The sprite classes call this.  Any new derived class
of QwSpriteFieldGraphic must do so too. SetChangedChunk can be used instead.
*/
void QwSpriteField::setChangedChunkContaining(int x, int y)
{
    if (x>=0 && x<width() && y>=0 && y<height()) {
	QwChunkRec& chunk=chunkContaining(x,y);
	chunk.changed=TRUE;
    }
}

/*!
This method adds a QwSpriteFieldGraphic to the list of those which need to
be drawn if the given chunk is redrawn.  Like SetChangedChunk
and SetChangedChunkContaining, this method marks the chunk as `dirty'.
*/
void QwSpriteField::addGraphicToChunk(QwSpriteFieldGraphic* g, int x, int y)
{
    if (x>=0 && x<chwidth && y>=0 && y<chheight) {
	QwChunkRec& ch=chunk(x,y);
	ch.head=new GraphicList(g,ch.head);
	ch.changed=TRUE;
    }
}

/*!
This method removes a QwSpriteFieldGraphic from the list of those which need to
be drawn if the given chunk is redrawn.  Like SetChangedChunk
and SetChangedChunkContaining, this method marks the chunk as `dirty'.
*/
void QwSpriteField::removeGraphicFromChunk(QwSpriteFieldGraphic* g, int x, int y)
{
    if (x>=0 && x<chwidth && y>=0 && y<chheight) {
	QwChunkRec& ch=chunk(x,y);
	delete remove(g,ch.head);
	ch.changed=TRUE;
    }
}


/*!
This method adds a QwSpriteFieldGraphic to the list of those which need to
be drawn if the chunk containing the given pixel is redrawn.
Like SetChangedChunk and SetChangedChunkContaining, this method
marks the chunk as `dirty'.
*/
void QwSpriteField::addGraphicToChunkContaining(QwSpriteFieldGraphic* g, int x, int y)
{
    if (x>=0 && x<width() && y>=0 && y<height()) {
	QwChunkRec& ch=chunkContaining(x,y);
	ch.head=new GraphicList(g,ch.head);
	ch.changed=TRUE;
    }
}

/*!
This method removes a QwSpriteFieldGraphic from the list of those which need to
be drawn if the chunk containing the given pixel is redrawn.
Like SetChangedChunk and SetChangedChunkContaining, this method
marks the chunk as `dirty'.
*/
void QwSpriteField::removeGraphicFromChunkContaining(QwSpriteFieldGraphic* g, int x, int y)
{
    if (x>=0 && x<width() && y>=0 && y<height()) {
	QwChunkRec& ch=chunkContaining(x,y);
	delete remove(g,ch.head);
	ch.changed=TRUE;
    }
}

/*
(internal)
A QwSpriteFieldIterator is used for the actual implementation of
the Pix iterator.  It stores all the information needed to
iterate over the various queries.

\sa QwSpriteField::topAt(int,int)
*/
class QwSpriteFieldIterator {
private:
    QwSpriteFieldIterator(int ax, int ay, int aw, int ah, QwSpriteField* spritefield, QImage* coll_mask) :
	x(ax),y(ay),
	w(aw),h(ah),
	collision_mask(coll_mask),
	i(x/spritefield->chunkSize()),
	j(y/spritefield->chunkSize()),
	x1(i), y1(j),
	x2((x+w-1)/spritefield->chunkSize()),
	y2((y+h-1)/spritefield->chunkSize()),
	list((GraphicList*)spritefield->listAtChunkTopFirst(i,j)),
	is_copy(FALSE)
    {
    }

    // Iterates using graphics list
    QwSpriteFieldIterator(QwSpriteField* spritefield) :
	x(0),y(0),
	w(0),h(0),
	i(1),j(1),
	x1(0),y1(0),
	x2(-1),y2(-1),
	list((GraphicList*)spritefield->allList()),
	is_copy(TRUE)
    {
    }

public:
    static QwSpriteFieldIterator* make(int ax, int ay, int aw, int ah, QwSpriteField* spritefield, QImage* coll_mask)
    {
	QwSpriteFieldIterator* result=new QwSpriteFieldIterator(ax,ay,aw,ah,
	    spritefield, coll_mask);

	if (result->empty()) {
	    result=result->next(spritefield);
	}

	return result;
    }

    static QwSpriteFieldIterator* make(int sx, int sy, QwSpritePixmap* im, QwSpriteField* spritefield)
    {
	return make(
	    QwVirtualSprite::col_to_world(QwVirtualSprite::world_to_col(sx)-im->colhotx),
	    QwVirtualSprite::col_to_world(QwVirtualSprite::world_to_col(sy)-im->colhoty),
	    QwVirtualSprite::col_to_world(im->colw),
		QwVirtualSprite::col_to_world(im->colh),
		spritefield, im->collision_mask);
	}

	static QwSpriteFieldIterator* make(QwSpriteField* spritefield)
	{
	    return new QwSpriteFieldIterator(spritefield);
	}

	~QwSpriteFieldIterator()
	{
	    if (is_copy) delete list;
	}

	QwSpriteFieldIterator* next(const QwSpriteField* spritefield)
	{
	    if (!empty()) {
		nextElement();
		if (!empty()) {
		    return this;
		}
	    }

	    while (empty()) {
		i++;

		if (i <= x2) {
		    newList((GraphicList*)spritefield->listAtChunkTopFirst(i,j));
		} else {
		    i=x1; j++;
		    if (j <= y2) {
			newList((GraphicList*)spritefield->listAtChunkTopFirst(i,j));
		    } else {
			delete this;
			return 0;
		    }
		}
	    }

	    return this;
	}

	QwSpriteFieldGraphic* element()
	{
	    return list->element;
	}

	void protectFromChange()
	{
	    if (!is_copy) {
		is_copy=TRUE;
		list=list->copy();
	    }
	}

	const int x, y, w, h;
	QImage* collision_mask;

    private:
	void newList(GraphicList* nl)
	{
	    if (is_copy) {
		if (list) delete list;
		list=nl->copy();
	    } else {
		list=nl;
	    }
	}

	void nextElement()
	{
	    if (is_copy) {
		GraphicList* n=list->next;
		list->next=0;
		delete list;
		list=n;
	    } else {
		list=list->next;
	    }
	}

	bool empty()
	{
	    return !list;
	}

	int i, j;
	const int x1, y1, x2, y2;

	GraphicList* list;
	bool is_copy; // indicates that list is a copy, not spritefield list.
    };

    /*!
    QwSpriteFieldGraphic objects at a <em>point</em> can be traversed from top
    to bottom using:

    \code
	for (Pix p=topAt(x,y); p; next(p)) {
	QwSpriteFieldGraphic* the_graphic = at(p);
	if (you_are_interested_in_collisions_with(the_graphic) && exact(p)) {
		// Collision!
		...
	    }
	}
    \endcode

    This traverses <em>at least</em> all the graphics at pixel position (x,y).

    The QwSpriteFieldGraphic returned by at(Pix) is very approximate, but is found
    very efficiently.  It should be used as a first-cut to ignore
    QwSpriteFieldGraphic objects you are not interested in.

    exact(Pix) should be used to further
    examine the QwSpriteFieldGraphic, as depicted above.

    During the traversal, the QwSpriteField must not be modified unless
    protectFromChange(Pix) is first called on the iterator.  This is for
    efficiency reasons (protecting the list of hits requires extra work).

    \warning Currently, the collision model is not quite correct at this
	level of abstraction, because it cannot check collision exactness
	beyond the rectangular level.  This will change in futures versions.
	For now, you should perhaps use the QwVirtualSprite::neighbourhood(...)
    methods instead.

\sa QwSpriteFieldGraphic::rtti()
*/
Pix QwSpriteField::topAt(int x, int y)
{
    QwSpriteFieldIterator* iterator=QwSpriteFieldIterator::make(x,y,1,1, this, 0);
    return (Pix)iterator;
}

/*!
Provides for traversal of all QwSpriteFieldGraphic objects in the field,
regardless of whether they are visible, on the field, or anything else.
No particular ordering it given.  You should iterate to the end of the
list, or use end(Pix&).
*/
Pix QwSpriteField::all()
{
    QwSpriteFieldIterator* iterator=QwSpriteFieldIterator::make(this);
    return (Pix)iterator;
}

/*!
QwSpriteFieldGraphic objects in an <em>area</em> can be traversed using:

\code
   for (Pix p=lookIn(x,y,w,h); p; next(p)) {
    QwSpriteFieldGraphic* the_graphic = at(p);
    ...
    }
\endcode

This traverses \e at \e least all the graphics in the given rectangle
\e at \e least once.

\sa topAt(int,int) at(Pix)
*/
Pix QwSpriteField::lookIn(int x, int y, int w, int h)
{
    QwSpriteFieldIterator* iterator=QwSpriteFieldIterator::make(x,y,w,h, this, 0);
    return (Pix)iterator;
}

/*!
Look to the next QwSpriteFieldGraphic in the traversal.

\sa topAt(int,int) lookIn(int,int,int,int)
*/
void QwSpriteField::next(Pix& p) const
{
    if (p) {
	QwSpriteFieldIterator* iterator=(QwSpriteFieldIterator*)p;

	iterator=iterator->next(this);

	p=(Pix)iterator;
    }
}

/*!
This should be called if you want to terminate a traversal without
looking at all results.  It frees memory used for the iteration.
*/
void QwSpriteField::end(Pix& p) const
{
    if (p) {
	delete (QwSpriteFieldIterator*)p;
	p=0;
    }
}

/*!
Returns the QwSpriteFieldGraphic for a given point in a traversal.

\sa topAt(int,int) lookIn(int,int,int,int)
*/
QwSpriteFieldGraphic* QwSpriteField::at(Pix p) const
{
    if (p) {
	QwSpriteFieldIterator* iterator=(QwSpriteFieldIterator*)p;
	return iterator->element();
    } else {
	return 0;
    }
}

/*!
Test to see if the QwSpriteFieldGraphic which at(Pix) indicates is
in a traversal truly \e is in that traversal.

The QwSpriteFieldGraphic returned by at(Pix) is very approximate, but is found
very efficiently.  It should be used as a first-cut to ignore
QwSpriteFieldGraphic objects you are not interested in.
*/
bool QwSpriteField::exact(Pix p) const
{
    QwSpriteFieldIterator* iterator=(QwSpriteFieldIterator*)p;
    QRect area(iterator->x,iterator->y,iterator->w,iterator->h);
    return iterator->element()->at(area);
}

/*!
During the traversal, the QwSpriteField must not be modified.
If you wish to modify it, call this method on the
Pix.  This will allow the QwSpriteField to be subsequently
modified without damaging the list of hits.
*/
void QwSpriteField::protectFromChange(Pix p)
{
    if (p) {
	QwSpriteFieldIterator* iterator=(QwSpriteFieldIterator*)p;
	iterator->protectFromChange();
    }
}


unsigned int QwSpriteField::posprec=0;

/*!
\class QwImageSpriteField QwSpriteField.h
\brief A QwSpriteField with a background image.

Although it is not very useful to provide a complex background drawing
method for a QwSpriteField (since redraw efficiency is achieved by sprites,
not static background graphics), if your application requires only a
simple repeating background image, this class will suffice.

This class is also intended as documentation-by-example for the
QwSpriteField::drawBackground method.
*/

/*!
Construct an QwImageSpriteField which uses the given image file
as the background image.  Any Qt-supported image format may be used.
*/
QwImageSpriteField::QwImageSpriteField(const char* filename, int w, int h, int chunksize, int maxclusters) :
    QwSpriteField(w, h, chunksize, maxclusters)
{
    if (!image.load(filename)) {
	fprintf(stderr,"QwImageSpriteField::QwImageSpriteField - Failed to read %s\n",filename);
	exit(1);
    }
}

// Idea courtesy of Mark Tigges
/*!
Construct an QwImageSpriteField which uses the given image
as the background image.
*/
QwImageSpriteField::QwImageSpriteField(QPixmap pm, int w, int h, int chunksize, int maxclusters) :
    QwSpriteField(w, h, chunksize, maxclusters)
{
    image = pm;
}

/*!
Does nothing.
*/
QwImageSpriteField::~QwImageSpriteField()
{
}

/*!
Reimplements QwSpriteField::drawBackground to draw the image
as the background.
*/
void QwImageSpriteField::drawBackground(QPainter& painter, const QRect& area)
{
    for (int x=area.x()/image.width();
	x<(area.x()+area.width()+image.width()-1)/image.width(); x++)
    {
	for (int y=area.y()/image.height();
	    y<(area.y()+area.height()+image.height()-1)/image.height(); y++)
	{
	    painter.drawPixmap(x*image.width(), y*image.height(),image);
	}
    }
}


/*!
\class QwSpriteFieldGraphic QwSpriteField.h
\brief An abstract graphic object on a QwSpriteField.

This class will primarily be of interest to those wanting to
add graphic objects other than the sprite object already defined.

In most graphic rendering systems, graphic objects are considered to
have a bounding \e rectangle, and redraw optimization is based on this
simplification.  This simplistic view is not used by QwSpriteField.  A
QwSpriteField considers itself to contain numerous graphic objects, each
of which covers certain `chunks' in the QwSpriteField.  For graphic
objects with a rectangular bias, this has only a minor effect on
redraw efficiency (although still a major effect on collision
detection and other area indexing).  But for other shapes, such as
lines, the tighter area-bound made possible by chunks can provide
improvements.

Whenever a QwSpriteFieldGraphic moves, it must add and remove itself
from the chunks of the QwSpriteField upon which it moves.  If the
QwSpriteFieldGraphic is much smaller than the chunk size of the
QwSpriteField, it will usually be in one, sometimes 2, and occasionally
3 or 4 chunks.  If the QwSpriteFieldGraphic is larger than the
chunk size of the QwSpriteField, it will span a number of chunks.
Clearly there is a trade-off between tight bounds and excessive
numbers of chunks a QwSpriteFieldGraphic will have to add and remove
itself from.

Note that a QwSpriteFieldGraphic may be `on' a QwSpriteField even if it's
coordinates place it far off the edge of the area of the QwSpriteField.
*/ 

/*!
Construct a QwSpriteFieldGraphic on the current spritefield.

\sa setCurrentSpriteField(QwSpriteField*) setSpriteField(QwSpriteField*)
*/
QwSpriteFieldGraphic::QwSpriteFieldGraphic() :
    spritefield(current_spritefield),
    vis(TRUE)
{
    if (spritefield) spritefield->addGraphic(this);
}

/*!
Destruct a QwSpriteFieldGraphic.  It is removed from its spritefield.
*/
QwSpriteFieldGraphic::~QwSpriteFieldGraphic()
{
    if (spritefield) spritefield->removeGraphic(this);
}

/*!
\fn int QwSpriteFieldGraphic::z() const

This abstract method should return the z of the graphic,
which is used for visual order:  higher z sprites obscure
lower-z ones.
*/

/*!
\fn void QwSpriteFieldGraphic::draw(QPainter&)

This abstract method should draw the the graphic on the given QPainter.
*/

/*!
QwSpriteFieldGraphics may be created on a `current' QwSpriteField, which must be
set prior to any graphic creation.  QwSpriteField objects set this,
so the most recently created QwSpriteField will get new QwSpriteFieldGraphics.

This notion of `currency' makes the most common case easy to use - that
of a single instance of QwSpriteField in the application - rather than
having to pass around a pointer to a QwSpriteField.
*/
void QwSpriteFieldGraphic::setCurrentSpriteField(QwSpriteField* pf)
{
    current_spritefield=pf;
}

/*!
Set the QwSpriteField upon which the QwSpriteFieldGraphic is to be drawn.
Initially this will be the current spritefield.

\sa setCurrentSpriteField(QwSpriteField*)
*/
void QwSpriteFieldGraphic::setSpriteField(QwSpriteField* pf)
{
    bool v=visible();
    visible(FALSE);
    if (spritefield) spritefield->removeGraphic(this);
    spritefield=pf;
    if (spritefield) spritefield->addGraphic(this);
    visible(v);
}

/*!
Alias for visible(TRUE).
*/
void QwSpriteFieldGraphic::show()
{
    visible(TRUE);
}

/*!
Alias for visible(FALSE).
*/
void QwSpriteFieldGraphic::hide()
{
    visible(FALSE);
}

/*!
Calls makeVisible(bool) if necessary to set the
visibility of the QwSpriteFieldGraphic to the given state.
*/
void QwSpriteFieldGraphic::visible(bool yes)
{
    if (vis!=yes) {
	if (yes) {
	    vis=yes;
	    makeVisible(yes);
	} else {
	    makeVisible(yes);
	    vis=yes;
	}
    }
}

/*!
Returns TRUE if the QwSpriteFieldGraphic is visible.  This does <em>not</em>
mean the QwSpriteFieldGraphic is currently in a view, merely that if a view
was showing the area where the QwSpriteFieldGraphic is, and the graphic
was not obscured by graphics at a higher z, it would be visible.
*/
bool QwSpriteFieldGraphic::visible() const
{
    return vis;
}

QwSpriteField* QwSpriteFieldGraphic::current_spritefield=0;

/*!
\fn bool QwSpriteFieldGraphic::at(int x, int y) const
Should return TRUE if the graphic includes the given pixel position.
*/

/*!
\fn bool QwSpriteFieldGraphic::at(const QRect& rect) const
TRUE if the graphic intersects with the given area.  This need not be
perfectly accurate.
*/

/*!
\fn int QwSpriteFieldGraphic::world_to_x(int i)

Same as QwSpriteField::world_to_x(int i)
*/
/*!
\fn int QwSpriteFieldGraphic::x_to_world(int i)

Same as QwSpriteField::x_to_world(int i)
*/

/*!
QwSpriteFieldGraphic derivatives can give pixel-accuracy for collision
detection by overriding this method.  By default, it simply returns
at(const QRect&) on the same area.
*/
bool QwSpriteFieldGraphic::at(const QImage*, const QRect& yourarea) const
{
    return at(yourarea);
}

/*!
The default implementation of makeVisible(bool) does nothing.

Note: derived classes should not be in any chunks if they are not visible,
so during this method, they should remove themselves if they are becoming
invisible, and add themselves if they are becoming visible.
*/
void QwSpriteFieldGraphic::makeVisible(bool)
{
}

/*!
\class QwSpritePixmap QwSpriteField.h
\brief A QwSpritePixmap is a sprite frame image

Note that QwSpritePixmap should be considered an internal class at
the current time.  This will change (as will the interface) once
alpha and description support for QImage is clarified.

\sa QwSpritePixmapSequence QwVirtualSprite QwSprite
*/

/*!
Construct a QwSpritePixmap from two image files.

The QwSpritePixmap is a masked QPixmap used internally by
sprite classes.

The dataname file must be a PPM file of the form:

\code
P6
# HOTSPOT x y
...
\endcode

That is, it must have an additional comment which gives the (x,y)
coordinate of the `hotspot' of the image.

The `hotspot' position defines the origin pixel in the image
For example, if the hotspot is (10,5), it will be displayed
drawn 10 pixels to the left of and 5 pixels above the actual
(x,y) coordinate of the sprite.

The maskname can be any monochrome image format, such as PBM.
No special comments in the file are needed or recognized.

The maskname may also be 0, in which case the sprite has no mask (it is
a solid rectangle).  This will also be the case if the file maskname
does not exist.
*/
QwSpritePixmap::QwSpritePixmap(const char* dataname, const char* maskname) :
    hotx(0),hoty(0),
    collision_mask(0),
    colhotx(0),colhoty(0)
{
    {
	QFile file(dataname);
	if (file.open(IO_ReadOnly)) {
	    char line[128];
	    file.readLine(line,128); // Skip "P6"/"P3" line
	    file.readLine(line,128);

	    while (line[0]=='#') {
		// Comment line - see if it has additional parameters
		if (0==strncmp(line,"# HOTSPOT ",10)) {
		    sscanf(line+10,"%d %d",&hotx,&hoty);
		    colhotx=hotx;
		    colhoty=hoty;
		}
		file.readLine(line,128);
	    }
	}
    }

    if (!load(dataname)) {
	fprintf(stderr,"QwSpritePixmap::QwSpritePixmap - Failed to read %s\n",dataname);
	exit(1);
    }

    if (maskname) {
	QImageIO iio;
	iio.setFileName(maskname);
	if (iio.read()) {
	    collision_mask=new QImage(iio.image());
	    mask.convertFromImage(*collision_mask);
	    setMask(mask);
	} else {
	    collision_mask=0;
	}
    } else {
	collision_mask=0;
    }

    colw=width();
    colh=height();
}

/*!
Construct a QwSpritePixmap from a QPixmap and a
\link QwSpritePixmap::setHotSpot() hotspot.\endlink

The QwSpritePixmap is a masked QPixmap used internally by
sprite classes.
*/
QwSpritePixmap::QwSpritePixmap(const QPixmap& pm, QPoint hotspot) :
    QPixmap(pm),
    hotx(hotspot.x()),hoty(hotspot.y()),
    collision_mask(0),
    colhotx(hotspot.x()),colhoty(hotspot.y())
{
    const QBitmap *mask = pm.mask();
    if (mask) {
	collision_mask = new QImage(mask->convertToImage());
    } else {
	collision_mask=0;
    }

    colw=width();
    colh=height();
}


/*!
Deletes any collision mask.
*/
QwSpritePixmap::~QwSpritePixmap()
{
    delete collision_mask;
}

/*!
  \fn int QwSpritePixmap::hotX() const
  Returns the X-position \link QwSpritePixmap::setHotSpot() hotspot\endlink set
  for the pixmap.
*/
/*!
  \fn int QwSpritePixmap::hotY() const
  Returns the Y-position \link QwSpritePixmap::setHotSpot() hotspot\endlink set
  for the pixmap.
*/
/*!
  \fn void QwSpritePixmap::setHotSpot(int x, int y)
  Sets the hotspot to (\a x, \a y).

  The hotspot position defines the origin pixel in the image
  For example, if the hotspot is (10,5), it will be displayed
  drawn 10 pixels to the left of and 5 pixels above the actual
  (x,y) coordinate of the sprite.
*/

/*!
\class QwSpritePixmapSequence QwSpriteField.h
\brief A sequence of QwSpritePixmap
to have multiple frames for animation.

This allows sprite objects
to have multiple frames for animation.

QwSpritePixmaps for sprite objects are collected into
QwSpritePixmapSequences, which are
the set of images a sprite will use, indexed by the sprite's
frame.  This allows sprites to simply have animated forms.
*/

/*!
Construct a QwSpritePixmapSequence from files.

The framecount parameter sets the number of frames to be
loaded for this image.  The filenames should be printf-style
strings such as "foo%03d.ppm" or "animdir/%4d.pbm".  The actual
filenames are formed by sprintf-ing a string with each integer
from 0 to framecount-1, eg. foo000.ppm, foo001.ppm, foo002.ppm, etc.
*/
QwSpritePixmapSequence::QwSpritePixmapSequence(const char* datafilenamepattern,
	const char* maskfilenamepattern, int fc) :
    framecount(fc),
    img(new QwSpritePixmap*[fc])
{
    for (int i=0; i<framecount; i++) {
	char data[1024],mask[1024];
	sprintf(data,datafilenamepattern,i);
	if (maskfilenamepattern) {
	    sprintf(mask,maskfilenamepattern,i);
	    img[i]=new QwSpritePixmap(data,mask);
	} else {
	    img[i]=new QwSpritePixmap(data,0);
	}
    }
}

/*!
Construct a QwSpritePixmapSequence from QPixmaps.
*/
QwSpritePixmapSequence::QwSpritePixmapSequence(QList<QPixmap> list, QList<QPoint> hotspots) :
    framecount(list.count()),
    img(new QwSpritePixmap*[list.count()])
{
    if (list.count() != hotspots.count())
	warning("QwSpritePixmapSequence: lists have different lengths");
    list.first();
    hotspots.first();
    for (int i=0; i<framecount; i++) {
	img[i]=new QwSpritePixmap(*list.current(), *hotspots.current());
	list.next();
	hotspots.next();
    }
}

/*!
  Destructs the pixmap sequence.
*/
QwSpritePixmapSequence::~QwSpritePixmapSequence()
{
    for (int i=0; i<framecount; i++)
	delete img[i];
    delete img;
}

/*!
Constructor failure check.  Currently not implemented.
Exceptions would be a better solution.
*/
int QwSpritePixmapSequence::operator!()
{
    return 0;
}

/*!
\fn QwSpritePixmap* QwSpritePixmapSequence::image(int i) const
Returns the \a i-th pixmap in the sequence.
*/

/*!
\fn void QwSpritePixmapSequence::setImage(int i, QwSpritePixmap* p)
Deletes the \a i-th pixmap and replaces it by \a p.  Note that \a p
becomes owned by the sequence - it will be deleted later.
*/

/*!
\fn int QwSpritePixmapSequence::frameCount() const
Returns the length of the sequence.
*/

/*!
When testing sprite collision detection with QwSpriteField::exact(Pix), 
the default is to use the image mask of the sprite.  By using
readCollisionMasks(const char*), an alternate mask can be used.
Also, by using QwVirtualSprite::setPixelCollisionPrecision(int),
the resolution of the collision mask can be different to the
resolution of the pixel coordinates of the sprites.

The filename is a printf-style string, as per the constructor
for QwSpritePixmapSequence.

The image must be a PBM file, with a HOTSPOT comment as described
in QwSpritePixmap::QwSpritePixmap(const char*, const char*).

\sa QwVirtualSprite::setPixelCollisionPrecision(int).
*/
void QwSpritePixmapSequence::readCollisionMasks(const char* fname)
{
    for (int i=0; i<framecount; i++) {
	char filename[1024];
	sprintf(filename,fname,i);

	{
	    QFile file(filename);
	    if (file.open(IO_ReadOnly)) {
		char line[128];
		file.readLine(line,128); // Skip "P1"/"P4" line
		file.readLine(line,128);

		while (line[0]=='#') {
		    // Comment line - see if it has additional parameters
		    if (0==strncmp(line,"# HOTSPOT ",10)) {
			sscanf(line+10,"%d %d",&img[i]->colhotx,&img[i]->colhoty);
		    }
		    file.readLine(line,128);
		}
	    }
	}
	delete img[i]->collision_mask;
	QImageIO iio;
	iio.setFileName(filename);
	if (!iio.read()) {
	    fprintf(stderr,"QwSpritePixmapSequence::readCollisionMasks - Failed to read %s\n",filename);
	    exit(1);
	}
	img[i]->collision_mask=new QImage(iio.image());
    }
}

/*!
\class QwVirtualSprite QwSpriteField.h
\brief A QwSpriteFieldGraphic which renders as a masked image.

QwVirtualSprites are most of the implementation of sprites, except they
do not define the x(), y(), z() and image() methods.
This allows for user-defined coordinates systems and animation methods.

\sa QwSprite
*/

unsigned int QwVirtualSprite::colprec=0;

/*!
Construct a QwVirtualSprite.  Derived classes should call addToChunks()
in their constructor once x(), y(), and image() are valid.
*/
QwVirtualSprite::QwVirtualSprite()
{
}

/*!
\fn QwSpritePixmap* QwVirtualSprite::image() const
This abstract method should return the pixmap to be drawn
for the sprite.
*/

/*!
\fn int QwVirtualSprite::x() const
This abstract method should return the horizontal location, measured
from the left edge of the QwSpriteField, at
which to draw the pixmap of the sprite.  Note that the hotspot of the
pixmap will be taken into account.
*/
/*!
\fn int QwVirtualSprite::y() const
This abstract method should return the vertical location, measured
from the top edge of the QwSpriteField, at
which to draw the pixmap of the sprite.  Note that the hotspot of the
pixmap will be taken into account.
*/

/*!
\fn int QwVirtualSprite::world_to_col(int i)
Convert a coordinate from world to collision coordinates.
\sa setPixelCollisionPrecision(int) QwSpriteField::setPositionPrecision
*/
/*!
\fn int QwVirtualSprite::col_to_world(int i)
Convert a coordinate from collision to world coordinates.
\sa setPixelCollisionPrecision(int) QwSpriteField::setPositionPrecision
*/

/*!
Adds/removes the sprite from the QwSpriteField chunks it covers, as
required by QwSpriteFieldGraphic.  This, addToChunks, and the removeFromChunks
method may be moved up to QwSpriteFieldGraphic.  Either way, this or
the other two would have to be virtual.
*/
void QwVirtualSprite::makeVisible(bool yes)
{
    if (yes) {
	addToChunks();
    } else {
	removeFromChunks();
    }
}

/*!
The absolute horizontal position of the QwVirtualSprite.  This is
the pixel position of the left edge of the image, as it takes into
account the hotspot.
*/
int QwVirtualSprite::absX() const
{
    return x()-image()->hotx;
}

/*!
The absolute horizontal position of the QwVirtualSprite, if it was moved to
\a nx.
*/
int QwVirtualSprite::absX(int nx) const
{
    return nx-image()->hotx;
}

/*!
The absolute vertical position of the QwVirtualSprite.  This is
the pixel position of the top edge of the image, as it takes into
account the hotspot.
*/
int QwVirtualSprite::absY() const
{
    return y()-image()->hoty;
}

/*!
The absolute vertical position of the QwVirtualSprite, if it was
moved to \a ny.
*/
int QwVirtualSprite::absY(int ny) const
{
    return ny-image()->hoty;
}

/*!
The right edge of the sprite image.

\sa absX()
*/
int QwVirtualSprite::absX2() const
{
    return absX()+image()->width()-1;
}

/*!
The right edge of the sprite image, if it was moved to \a nx.

\sa absX()
*/
int QwVirtualSprite::absX2(int nx) const
{
    return absX(nx)+image()->width()-1;
}

/*!
The bottom edge of the sprite image.

\sa absY()
*/
int QwVirtualSprite::absY2() const
{
    return absY()+image()->height()-1;
}

/*!
The bottom edge of the sprite image, \a if it was moved to \a ny.

\sa absY()
*/
int QwVirtualSprite::absY2(int ny) const
{
    return absY(ny)+image()->height()-1;
}

/*!
The left edge, in collision-detection coordinates.

\sa QwVirtualSprite::setPixelCollisionPrecision(int).
*/
int QwVirtualSprite::absColX() const
{
    return world_to_col(x())-image()->colhotx;
}

/*!
The top edge, in collision-detection coordinates.

\sa QwVirtualSprite::setPixelCollisionPrecision(int).
*/
int QwVirtualSprite::absColY() const
{
    return world_to_col(y())-image()->colhoty;
}

/*!
The right edge, in collision-detection coordinates.

\sa QwVirtualSprite::setPixelCollisionPrecision(int).
*/
int QwVirtualSprite::absColX2() const
{
    return absColX()+image()->colw-1;
}

/*!
The bottom edge, in collision-detection coordinates.

\sa QwVirtualSprite::setPixelCollisionPrecision(int).
*/
int QwVirtualSprite::absColY2() const
{
    return absColY()+image()->colh-1;
}

/*!
Add the sprite to the chunks in its QwSpriteField which it overlaps.

This must be called as the values of x(), y(), and image() change
such that the QwVirtualSprite is removed from chunks it is in,
the values of x(), y(), and image() change, then it is added
back into the then covered chunks in the QwSpriteField.

The QwPositionedSprite and derived classes deals with this automatically,
as must other derived classes of QwVirtualSprite.
*/
void QwVirtualSprite::addToChunks()
{
    if (visible() && spritefield) {
	int chunksize=spritefield->chunkSize();
	for (int j=absY()/chunksize; j<=absY2()/chunksize; j++) {
	    for (int i=absX()/chunksize; i<=absX2()/chunksize; i++) {
		spritefield->addGraphicToChunk(this,i,j);
	    }
	}
    }
}

/*!
Remove the sprite from the chunks in its QwSpriteField which it overlaps.

\sa addToChunks()
*/
void QwVirtualSprite::removeFromChunks()
{
    if (visible() && spritefield) {
	int chunksize=spritefield->chunkSize();
	for (int j=absY()/chunksize; j<=absY2()/chunksize; j++) {
	    for (int i=absX()/chunksize; i<=absX2()/chunksize; i++) {
		spritefield->removeGraphicFromChunk(this,i,j);
	    }
	}
    }
}

/*!
Destruct the QwVirtualSprite.  Derived classes must remove the sprite
from any chunks, as this destructor cannot call the virtual methods
required to do so.
*/
QwVirtualSprite::~QwVirtualSprite()
{
}

/*!
The width of the sprite, in its current image.
*/
int QwVirtualSprite::width() const
{
    return image()->width();
}

/*!
The height of the sprite, in its current image.
*/
int QwVirtualSprite::height() const
{
    return image()->height();
}

/*!
The width of the sprite in collision-detection coordinates.
*/
int QwVirtualSprite::colWidth() const
{
    return image()->colw;
}

/*!
The height of the sprite in collision-detection coordinates.
*/
int QwVirtualSprite::colHeight() const
{
    return image()->colh;
}

/*!
(override)

Tests if the given pixel touches the sprite.  This uses pixel-accurate
detection, using the collision mask of the sprites current image (which
is by default the image mask).  This test is useful for example to test
when the user clicks a point with they mouse.  Note however, that
QwSpriteField::topAt(int x, int y) and associated methods give a more
efficient and flexible technique for this purpose.
*/
bool QwVirtualSprite::at(int px, int py) const
{
    px=world_to_col(px)-absColX();
    py=world_to_col(py)-absColY();

    if (px<0 || px>=colWidth() || py<0 || py>=colHeight())
	return FALSE;

    QImage* img=image()->collision_mask;

    if (img) {
	if (img->bitOrder() == QImage::LittleEndian) {
	    return *(img->scanLine(py) + (px >> 3)) & (1 << (px & 7));
	} else {
	    return *(img->scanLine(py) + (px >> 3)) & (1 << (7 -(px & 7)));
	}
    } else {
	return TRUE;
    }
}

/*!
\sa neighbourhood(int,int,QwSpritePixmap*) const
*/
bool QwVirtualSprite::exact(Pix p) const
{
    QwSpriteFieldIterator* iterator=(QwSpriteFieldIterator*)p;
    QRect area(iterator->x,iterator->y,iterator->w,iterator->h);
    return iterator->element()->at(area)
	&& iterator->element()->at(iterator->collision_mask, area);
}

/*!
Used by the methods associated with
QwSpriteField::topAt(int x, int y) to test for a close hit.
\sa neighbourhood(int,int,QwSpritePixmap*)
*/
bool QwVirtualSprite::at(const QRect& rect) const
{
    QRect crect(world_to_col(rect.x()),world_to_col(rect.y()),
	world_to_col(rect.width()),world_to_col(rect.height()));
    QRect myarea(absColX(),absColY(),colWidth(),colHeight());
    return crect.intersects(myarea);
}

/*!
Used by the methods associated with
QwSpriteField::topAt(int x, int y) to test for an exact hit.
\sa neighbourhood(int,int,QwSpritePixmap*)
*/
bool QwVirtualSprite::at(const QImage* yourimage, const QRect& yourarea) const
{
    QRect cyourarea(world_to_col(yourarea.x()),world_to_col(yourarea.y()),
	world_to_col(yourarea.width()),world_to_col(yourarea.height()));

    QImage* myimage=image()->collision_mask;

    QRect ourarea(absColX(),absColY(),colWidth(),colHeight()); // myarea

    ourarea=ourarea.intersect(cyourarea);

    int yx=ourarea.x()-cyourarea.x();
    int yy=ourarea.y()-cyourarea.y();
    int mx=ourarea.x()-absColX();
    int my=ourarea.y()-absColY();
    int w=ourarea.width();
    int h=ourarea.height();

    if ( !yourimage ) {
	if ( !myimage )
	    return w>0 && h>0;
	// swap everything around
	int t;
	t=mx; mx=yx; yx=t;
	t=my; mx=yy; yy=t;
	yourimage = myimage;
	myimage = 0;
    }

    // yourimage != 0

    // XXX 
    // XXX A non-linear search would typically be more
    // XXX efficient.  Optimal would be spiralling out
    // XXX from the center, but a simple vertical expansion
    // XXX from the centreline would suffice.
    // XXX
    // XXX My sister just had a baby 40 minutes ago, so
    // XXX I'm too brain-spun to implement it correctly!
    // XXX 
    //

    // Let's make an assumption.  That sprite masks don't have
    // bit orders for different machines!
    //
    // ASSERT(myimage->bitOrder()==yourimage->bitOrder());

    if (myimage) {
	if (myimage->bitOrder() == QImage::LittleEndian) {
	    for (int j=0; j<h; j++) {
		uchar* ml = myimage->scanLine(my+j);
		uchar* yl = yourimage->scanLine(yy+j);
		for (int i=0; i<w; i++) {
		    if (*(yl + (yx+i >> 3)) & (1 << ((yx+i) & 7))
		    && *(ml + (mx+i >> 3)) & (1 << ((mx+i) & 7)))
		    {
			return TRUE;
		    }
		}
	    }
	} else {
	    for (int j=0; j<h; j++) {
		uchar* ml = myimage->scanLine(my+j);
		uchar* yl = yourimage->scanLine(yy+j);
		for (int i=0; i<w; i++) {
		    if (*(yl + (yx+i >> 3)) & (1 << (7-((yx+i) & 7)))
		    && *(ml + (mx+i >> 3)) & (1 << (7-((mx+i) & 7))))
		    {
			return TRUE;
		    }
		}
	    }
	}
    } else {
	if (yourimage->bitOrder() == QImage::LittleEndian) {
	    for (int j=0; j<h; j++) {
		uchar* yl = yourimage->scanLine(yy+j);
		for (int i=0; i<w; i++) {
		    if (*(yl + (yx+i >> 3)) & (1 << ((yx+i) & 7)))
		    {
			return TRUE;
		    }
		}
	    }
	} else {
	    for (int j=0; j<h; j++) {
		uchar* yl = yourimage->scanLine(yy+j);
		for (int i=0; i<w; i++) {
		    if (*(yl + (yx+i >> 3)) & (1 << (7-((yx+i) & 7))))
		    {
			return TRUE;
		    }
		}
	    }
	}
    }

    return FALSE;
}

/*!
Creates an iterator which can traverse the area which the QwVirtualSprite
would cover if it had the given position and image.  This `would cover'
concept is useful as it allows you to check for a collision <em>before</em>
moving the sprite.

*/
Pix QwVirtualSprite::neighbourhood(int nx, int ny, QwSpritePixmap* img) const
{
    if (!spritefield) return 0;

    QwSpriteFieldIterator* iterator=
	QwSpriteFieldIterator::make(nx,ny,img,spritefield);

    while (iterator && iterator->element()==(QwVirtualSprite*)this)
	iterator=iterator->next(spritefield);

    return (Pix)iterator;
}

/*!
Creates an iterator which traverses the QwSpriteFieldGraphic objects which
collide with this sprite at its current position.

\sa neighbourhood(int,int,QwSpritePixmap*)
*/
Pix QwVirtualSprite::neighbourhood() const
{ return neighbourhood(x(),y(),image()); }

/*!
Creates an iterator which traverses the QwSpriteFieldGraphic objects which
would collide with this sprite if it was moved to the given position (yet
kept its current image).

\sa neighbourhood(int,int,QwSpritePixmap*)
*/
Pix QwVirtualSprite::neighbourhood(int nx, int ny) const
{ return neighbourhood(nx,ny,image()); }

/*!
Traverse to the next QwSpriteFieldGraphic in a collision list.

\sa neighbourhood(int,int,QwSpritePixmap*)
*/
void QwVirtualSprite::next(Pix& p) const
{
    if (p) {
	QwSpriteFieldIterator* iterator=(QwSpriteFieldIterator*)p;

	iterator=iterator->next(spritefield);

	while (iterator && iterator->element()==(QwVirtualSprite*)this)
	    iterator=iterator->next(spritefield);

	p=(Pix)iterator;
    }
}

/*!
Terminate a traversal early.

\sa neighbourhood(int,int,QwSpritePixmap*) QwSpriteField::end(Pix&)
*/
void QwVirtualSprite::end(Pix& p) const
{
    if (p) {
	delete (QwSpriteFieldIterator*)p;
	p=0;
    }
}

/*!
Returns the QwSpriteFieldGraphic at the given traversal point.

\sa neighbourhood(int,int,QwSpritePixmap*)
*/
QwSpriteFieldGraphic* QwVirtualSprite::at(Pix p) const
{
    if (p) {
	QwSpriteFieldIterator* iterator=(QwSpriteFieldIterator*)p;
	return iterator->element();
    } else {
	return 0;
    }
}

/*!
Test if this sprite is touching the given QwSpriteFieldGraphic.
This is a convenient packaging of neighbourhood() and related methods.
*/
bool QwVirtualSprite::hitting(QwSpriteFieldGraphic& other) const
{
    return wouldHit(other,x(),y(),image());
}

/*!
Test if this sprite would (exactly) touch the given QwSpriteFieldGraphic.
This is a convenient packaging of neighbourhood() and related methods.
*/
bool QwVirtualSprite::wouldHit(QwSpriteFieldGraphic& other, int x, int y, QwSpritePixmap* img) const
{
    // Cast off const.  Safe, because we don't call non-const methods
    // of the QwVirtualSprite returnsed by at().
    //
    QwVirtualSprite* nconst_this=(QwVirtualSprite*)this;

    for (Pix p=nconst_this->neighbourhood(x,y,img); p; nconst_this->next(p)) {
	if (nconst_this->at(p)==&other) {
	    if (nconst_this->exact(p)) {
		nconst_this->end(p);
		return TRUE;
	    }
	}
    }
    return FALSE;
}




/*!
(override)

Draws the current image of the sprite at its current position,
as given by image() and x(), y().
*/
void QwVirtualSprite::draw(QPainter& painter)
{
    painter.drawPixmap(absX(),absY(),*image());
}

/*!
Using this method,
the resolution of the collision mask can be different to the
resolution of the pixel coordinates of the sprites.  One possible
use for this is to have differently scaled graphics being used by
different players in a multiplayer distributed game, yet for the
collision detection to be identical for each player.

\sa QwSpritePixmapSequence::readCollisionMasks(...)
*/
void QwVirtualSprite::setPixelCollisionPrecision(int downshifts)
{
    colprec=downshifts;
}


/*!
\class QwAbsSpriteFieldView QwSpriteField.h
\brief An abstraction which views a QwSpriteField

Use one of the derived classes.
*/

/*!
Construct a QwAbsSpriteFieldView which views the given QwSpriteField.
*/
QwAbsSpriteFieldView::QwAbsSpriteFieldView(QwSpriteField* v) :
    viewing(v)
{
    this->view(v);
}

/*!
Change the QwSpriteField which the QwSpriteFieldView is viewing.
*/
void QwAbsSpriteFieldView::view(QwSpriteField* v)
{
    if (viewing) {
	viewing->removeView(this);
    }
    viewing=v;
    if (viewing) {
	viewing->addView(this);
    }
}

/*!
  Called by the field whenever it changes size.
  The default implementation does nothing.
*/
void QwAbsSpriteFieldView::updateGeometries()
{
}

/*!
\fn QRect QwAbsSpriteFieldView::viewArea() const

viewArea returns the area of `viewing' which this QwAbsSpriteFieldView is
viewing.
The tighter this rectangle can
be defined, the more efficient QwSpriteField::update will be.

Return zero-width area if nothing viewed.
*/

/*!
\fn void  QwAbsSpriteFieldView::beginPainter(QPainter & )
Called to initialize the QPainter - normally calls QPainter::begin().
*/
/*!
\fn void  QwAbsSpriteFieldView::flush (const QRect & area)
Called just before the painter is ended.
*/

/*!
\fn bool QwAbsSpriteFieldView::preferDoubleBuffering () const

Return TRUE if double-buffering is best for this class of widget.
If any view viewing an area prefers double-buffering, that area will
be double buffered for all views.
*/

/*!
\class QwSpriteFieldView QwSpriteField.h
\brief A QWidget which views a QwSpriteField

This is where QwSpriteField meets a QWidget - this class displays
a QwSpriteField.
*/

/*!
Configure the QPainter - just makes it draw on this widget.
*/
void QwSpriteFieldView::beginPainter(QPainter& painter)
{
    // Nothing special
    painter.begin(this);
}

/*!
Nothing.
*/
void QwSpriteFieldView::flush(const QRect& /*area*/)
{
    // Nothing special
}

/*!
Construct a QwSpriteFieldView which views the given QwSpriteField.  The
usual QWidget parameters may also be passed.
*/
QwSpriteFieldView::QwSpriteFieldView(QwSpriteField* v, QWidget* parent, const char* name, WFlags f) :
    QWidget(parent,name,f),
    QwAbsSpriteFieldView(v)
{
    setBackgroundMode(NoBackground);
    if (v) {
	resize(v->width(),v->height());
    }
}

/*!
Deconstruct the QwSpriteFieldView
*/
QwSpriteFieldView::~QwSpriteFieldView()
{
    QwAbsSpriteFieldView::view(0);
}

/*!
Returns the area of
the widget, clipped by the area of the widget's parent (if any).
This should suffice for most cases.
*/
QRect QwSpriteFieldView::viewArea() const
{
    QWidget* parent=parentWidget();

    if (parent) {
	// translate parent rect to coord system of child (this)
	QRect clip=parent->rect();

	// clip child rect by parent rect
	clip=clip.intersect(geometry());

	// translate child rect to coord system of field
	clip.moveBy(-x(),-y());

	return clip;
    } else {
	// translate this to coord system of field
	return rect();
    }
}

/*!
Returns TRUE:  double-buffering \e is best for widget views of a QwSpriteField.
*/
bool QwSpriteFieldView::preferDoubleBuffering() const
{
    return backgroundMode() == NoBackground;
}

/*!
(override)

Repaint the appropriate area of the QwSpriteField which this
QwSpriteFieldView is viewing.
*/
void QwSpriteFieldView::paintEvent(QPaintEvent *ev)
{
    if (viewing) {
	viewing->updateInView(this,ev->rect());
    }
}

/*!
\class QwScrollingSpriteFieldView QwSpriteField.h
\brief A QWidget which views a QwSpriteField, and has scrollbars.

While QwSpriteFieldView inside a QwViewport can be used for viewing
a QwSpriteField, this may cause poor redraw effects if a lot of activity
is occurring in the QwSpriteField, because scrolling by this method
is a two part process:  copy some area, then redraw the new area.

QwScrollingSpriteFieldView avoids this problem by doing these in
a single operation.  The downside is that it uses an off-screen pixmap
for the whole visible area of the view and so is not practical if very
large areas, or large numbers of areas, are required.
*/

/*!
Configure the QPainter - makes it draw on this widget's offscreen
pixmap, translated according to the scrollbars.
*/
void QwScrollingSpriteFieldView::beginPainter(QPainter& painter)
{
    painter.begin(&offscr);
    painter.translate(-hscroll.value(),-vscroll.value());
}

/*!
Flush the off-screen changes.
*/
void QwScrollingSpriteFieldView::flush(const QRect& area)
{
    repaint(area.x()-hscroll.value(),area.y()-vscroll.value(),
	area.width(),area.height(),FALSE);
}

/*!
Construct a QwSpriteFieldView which views the given QwSpriteField.  The
usual QWidget parameters may also be passed.
*/
QwScrollingSpriteFieldView::QwScrollingSpriteFieldView(QwSpriteField* v, QWidget* parent, const char* name, WFlags f) :
    QWidget(parent,name,f),
    QwAbsSpriteFieldView(v),
    hscroll(QScrollBar::Horizontal,this),
    vscroll(QScrollBar::Vertical,this)
{
    setBackgroundMode(NoBackground);
    hscroll.resize(18,18);
    vscroll.resize(18,18);
    if (v) {
	resize(v->width()+vscroll.width(),v->height()+hscroll.height());
    }
    hscrpv=vscrpv=0;
    connect(&hscroll,SIGNAL(valueChanged(int)),this,SLOT(hScroll(int)));
    connect(&vscroll,SIGNAL(valueChanged(int)),this,SLOT(vScroll(int)));
}

/*!
Deconstruct the QwScrollingSpriteFieldView
*/
QwScrollingSpriteFieldView::~QwScrollingSpriteFieldView()
{
    QwAbsSpriteFieldView::view(0);
}

/*!
Returns the area of
the viewport.
*/
QRect QwScrollingSpriteFieldView::viewArea() const
{
    return QRect(hscroll.value(),vscroll.value(),
	width()-vscroll.width(),height()-hscroll.height());
}

/*!
Returns FALSE:  double-buffering is not needed, since this class has its
    own offscreen pixmap.
*/
bool QwScrollingSpriteFieldView::preferDoubleBuffering() const
{
    return FALSE;
}

/*!
(override)

Repaint the appropriate area of the QwScrollingSpriteFieldView which this
QwScrollingSpriteFieldView is viewing.
*/
void QwScrollingSpriteFieldView::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    if (viewing) {
	p.setClipRect(ev->rect());
	p.drawPixmap(0,0,offscr);
	p.eraseRect(width()-vscroll.width(), height()-hscroll.height(),
	    vscroll.width(), hscroll.height());
    } else {
	p.eraseRect(ev->rect());
    }
}

/*!
(override)

Deal with scrollbars.
*/
void QwScrollingSpriteFieldView::resizeEvent(QResizeEvent *)
{
    updateGeometries();
}

/*!
  Ensures scrollbars and off-screen pixmap are of the correct
  size given then current widget and field sizes.  The field
  calls this automatically as needed.
*/
void QwScrollingSpriteFieldView::updateGeometries()
{
    hscroll.setGeometry(0,height()-hscroll.height(),
	width()-vscroll.width(),hscroll.height());
    vscroll.setGeometry(width()-vscroll.width(), 0,
	vscroll.width(),height()-hscroll.height());
    if (viewing) {
	int portw=width()-vscroll.width();
	int porth=height()-hscroll.height();
	hscroll.setRange(0,QMAX(0,viewing->width()-portw));
	vscroll.setRange(0,QMAX(0,viewing->height()-porth));
	hscroll.setSteps(hSteps(),portw);
	vscroll.setSteps(vSteps(),porth);
	int extraw=portw-offscr.width();
	int extrah=porth-offscr.height();
	offscr.resize(portw,porth);
	if (extrah>0) {
	    QRect area(hscroll.value(),vscroll.value()+porth-extrah,
		portw,extrah);
	    viewing->updateInView(this,area);
	}
	if (extraw>0) {
	    QRect area(hscroll.value()+portw-extraw,vscroll.value(),
		extraw,porth);
	    viewing->updateInView(this,area);
	}
    }
}

/*!
  The stepping rate for the horizontal scrollbar.  Returns 16.
*/
int QwScrollingSpriteFieldView::hSteps() const
{
    return 16;
}

/*!
  The stepping rate for the vertical scrollbar.  Returns 16.
*/
int QwScrollingSpriteFieldView::vSteps() const
{
    return 16;
}

/*!
  Scrolls the view vertically by the given amount.
*/
void QwScrollingSpriteFieldView::vScroll(int v)
{
    int portw=width()-vscroll.width();
    int porth=height()-hscroll.height();

    // Scroll area
    bitBlt(&offscr,0,vscrpv-v,&offscr,0,0,portw,porth,CopyROP,TRUE);

    // Repaint new area
    if (viewing) {
	if (vscrpv < v) {
	    QRect area(hscroll.value(),vscrpv+porth,
		portw,v-vscrpv);
	    viewing->updateInView(this,area);
	} else {
	    QRect area(hscroll.value(),vscroll.value(),portw,vscrpv-v);
	    viewing->updateInView(this,area);
	}
    }

    vscrpv=v;

    // Then repaint entire view area
    repaint(0,0,portw,porth,FALSE);
}

/*!
  Scrolls the view horizontally by the given amount.
*/
void QwScrollingSpriteFieldView::hScroll(int v)
{
    // Scroll area
    bitBlt(&offscr,hscrpv-v,0,&offscr,0,0,width(),height(),CopyROP,TRUE);

    int portw=width()-vscroll.width();
    int porth=height()-hscroll.height();

    // Repaint new area
    if (viewing) {
	if (hscrpv < v) {
	    QRect area(hscrpv+portw,vscroll.value(),
		v-hscrpv,porth);
	    viewing->updateInView(this,area);
	} else {
	    QRect area(hscroll.value(),vscroll.value(),hscrpv-v,porth);
	    viewing->updateInView(this,area);
	}
    }

    hscrpv=v;

    // Then repaint entire view area
    repaint(0,0,portw,porth,FALSE);
}

/*!
\class QwPolygonalGraphic QwSpriteField.h
\brief A QwSpriteFieldGraphic which renders itself in a polygonal area.

QwPolygonalGraphic is an abstract class that is useful for all graphics
which cover a polygonal area of chunks on the field.
Sprites, the other branch of QwSpriteFieldGraphic derivatives usually
cover
a simple rectangular area and are dealt with specially, but typical
geometric shapes such as lines and circles would be quite inefficiently
bounded by rectangular areas - a diagonal line from one corner of the
field area to the other bound be bounded by a rectangle covering
the entire area! QwPolygonalGraphic objects allow the area to be
defined by a polygon - a sequence of points indicating the chunks
bounding the area covered by the graphic.

Derived classes should try to define as small as possible an area
to maximize efficiency, but must \e definately be contained completely
within the polygonal area.  Calculating the exact requirements will
generally be difficult, and hence a certain amount of over-estimation
could be expected.

\sa QwLineGraphic?, QwCircleGraphic?
*/

/*!
Construct a QwPolygonalGraphic.
Derived classes should call addToChunks()
in their constructor once numAreaPoints() and getAreaPoints() are valid.
*/
QwPolygonalGraphic::QwPolygonalGraphic()
{
}

/*!
Destruct the QwPolygonalGraphic.  Derived classes \e must remove the area
from any chunks, as this destructor cannot call the virtual methods
required to do so.  That is, they must call hide() in their
destructor.
*/
QwPolygonalGraphic::~QwPolygonalGraphic()
{
}

/*!
\fn QPointArray QwPolygonalGraphic::areaPoints(QPoint& offset) const;
Sets \a offset to the offset of the area, and
returns the points of the polygonal area covered
by the graphic.

Call hide() before this changes, show() afterwards.
*/

/*
 * concave: scan convert nvert-sided concave non-simple polygon with vertices at
 * (point[i].x, point[i].y) for i in [0..nvert-1] within the window win by
 * calling spanproc for each visible span of pixels.
 * Polygon can be clockwise or counterclockwise.
 * Algorithm does uniform point sampling at pixel centers.
 * Inside-outside test done by Jordan's rule: a point is considered inside if
 * an emanating ray intersects the polygon an odd number of times.
 *
 *  Paul Heckbert    30 June 81, 18 Dec 89
 */

typedef struct {	/* a polygon edge */
    double x;    /* x coordinate of edge's intersection with current scanline */
    double dx;    /* change in x with respect to y */
    int i;    /* edge number: edge i goes from g_pt[i] to g_pt[i+1] */
} Edge;

static QPointArray g_pt;	/* vertices */

static void
delete_edge(Edge* active, int& nact, int i)	/* remove edge i from active list */
{
    int j;

    for (j=0; j<nact && active[j].i!=i; j++);
    if (j>=nact) return;    /* edge not in active list; happens at win->y0*/
    nact--;
    memcpy(&active[j], &active[j+1], (nact-j)*sizeof(active[0]));
}

static void
insert_edge(Edge* active, int& nact, int i, int y)	/* append edge i to end of active list */
{
    int j;
    double dx;
    QPoint p, q;

    j = i<(int)g_pt.size()-1 ? i+1 : 0;
    if (g_pt[i].y() < g_pt[j].y()) {p = g_pt[i]; q = g_pt[j];}
    else	   {p = g_pt[j]; q = g_pt[i];}
    /* initialize x position at intersection of edge with scanline y */
    active[nact].dx = dx = double(q.x()-p.x())/(q.y()-p.y());
    active[nact].x = dx*(y+.5-p.y())+p.x();
    active[nact].i = i;
    nact++;
}

/* comparison routines for qsort */
static
int compare_ind(const void *u, const void *v)
{
    return g_pt[*(int*)u].y() <= g_pt[*(int*)v].y() ? -1 : 1;
}
static
int compare_active(const void *u, const void *v)
{
    return ((Edge*)u)->x <= ((Edge*)v)->x ? -1 : 1;
}


bool QwPolygonalGraphic::scan(const QRect& win) const
{
    int k, y0, y1, y, i, j, xl, xr;
    int *ind;		/* list of vertex indices, sorted by g_pt[ind[j]].y */

    int nact;			/* number of active edges */
    Edge *active;		/* active edge list:edges crossing scanline y */

    g_pt = areaPoints();
    int n = g_pt.size();	/* number of vertices */
    if (n<=0) return FALSE;
    ind = new int[n];
    active = new Edge[n];

    /* create y-sorted array of indices ind[k] into vertex list */
    for (k=0; k<n; k++)
	ind[k] = k;
    qsort(ind, n, sizeof(ind[0]), compare_ind);	/* sort ind by g_pt[ind[k]].y */

    nact = 0;				/* start with empty active list */
    k = 0;				/* ind[k] is next vertex to process */
    y0 = QMAX(win.top(), g_pt[ind[0]].y());		/* ymin of polygon */
    y1 = QMIN(win.bottom(), g_pt[ind[n-1]].y()-1);	/* ymax of polygon */

    for (y=y0; y<=y1; y++) {		/* step through scanlines */
	/* scanline y is at y+.5 in continuous coordinates */

	/* check vertices between previous scanline and current one, if any */
	for (; k<n && g_pt[ind[k]].y()<=y; k++) {
	    /* to simplify, if pt.y=y+.5, pretend it's above */
	    /* invariant: y-.5 < g_pt[i].y <= y+.5 */
	    i = ind[k];	
	    /*
	     * insert or delete edges before and after vertex i (i-1 to i,
	     * and i to i+1) from active list if they cross scanline y
	     */
	    j = i>0 ? i-1 : n-1;	/* vertex previous to i */
	    if (g_pt[j].y() < y)	/* old edge, remove from active list */
		delete_edge(active,nact,j);
	    else if (g_pt[j].y() > y)	/* new edge, add to active list */
		insert_edge(active,nact,j, y);
	    j = i<n-1 ? i+1 : 0;	/* vertex next after i */
	    if (g_pt[j].y() < y)	/* old edge, remove from active list */
		delete_edge(active,nact,i);
	    else if (g_pt[j].y() > y)	/* new edge, add to active list */
		insert_edge(active,nact,i, y);
	}

	/* sort active edge list by active[j].x */
	qsort(active, nact, sizeof(active[0]), compare_active);

	/* draw horizontal segments for scanline y */
	for (j=0; j<nact; j+=2) {	/* draw horizontal segments */
	    /* span 'tween j & j+1 is inside, span tween j+1 & j+2 is outside */
	    xl = int(active[j].x+.5);		/* left end of span */
	    if (xl<win.left()) xl = win.left();
	    xr = int(active[j+1].x-.5);	/* right end of span */
	    if (xr>win.right()) xr = win.right();
	    if (xl<=xr)
		return TRUE;
	    active[j].x += active[j].dx;	/* increment edge coords */
	    active[j+1].x += active[j+1].dx;
	}
    }

    return FALSE;
}

bool QwPolygonalGraphic::at(int x, int y) const
{
    return scan(QRect(x,y,1,1));
}

bool QwPolygonalGraphic::at(const QRect &r) const
{
    return scan(r);
}

/*!
Adds/removes the area from the QwSpriteField chunks it covers, as
required by QwSpriteFieldGraphic. 
*/
void QwPolygonalGraphic::makeVisible(bool show)
{
    chunkify(show);
}

void QwPolygonalGraphic::chunkify(int type)
{
    QPointArray pa = areaPoints();

    if ( !pa.size() )
	return;

    // XXX Just a simple implementation for now - find the bounding
    //     rectangle and add to every chunk thus touched.
    // XXX A better algorithm would be some kine of scanline polygon
    //     render (hence the API asks for some points).

    QRect brect = pa.boundingRect();
    int chunksize=spritefield->chunkSize();
    for (int j=brect.top()/chunksize; j<=brect.bottom()/chunksize; j++) {
	int i;
	switch ( type ) {
	  case 0:
	    for (i=brect.left()/chunksize; i<=brect.right()/chunksize; i++)
		spritefield->removeGraphicFromChunk(this,i,j);
	    break;
	  case 1:
	    for (i=brect.left()/chunksize; i<=brect.right()/chunksize; i++)
		spritefield->addGraphicToChunk(this,i,j);
	    break;
	  case 2:
	    for (i=brect.left()/chunksize; i<=brect.right()/chunksize; i++)
		spritefield->setChangedChunk(i,j);
	    break;
	}
    }
}

/*!
  Adds the sprite to the appropriate chunks.
*/
void QwPolygonalGraphic::addToChunks()
{
    if (visible() && spritefield)
	chunkify(1);
}

/*!
  Removes the sprite from the appropriate chunks.
*/
void QwPolygonalGraphic::removeFromChunks()
{
    if (visible() && spritefield)
	chunkify(0);
}

/*!
  Marks the appropriate chunks as changed.
*/
void QwPolygonalGraphic::changeChunks()
{
    if (visible() && spritefield)
	chunkify(2);
}

void QwPolygonalGraphic::draw(QPainter & p)
{
    p.setPen(pen);
    p.setBrush(brush);
    drawShape(p);
}

void QwPolygonalGraphic::moveTo(QPoint p)
{
    moveBy(p-pos);
}

void QwPolygonalGraphic::moveTo(int x, int y)
{
    moveBy(QPoint(x,y)-pos);
}

void QwPolygonalGraphic::moveBy(QPoint p)
{
    moveBy(p.x(),p.y());
}

void QwPolygonalGraphic::moveBy(int dx, int dy)
{
    if ( dx || dy ) {
	removeFromChunks();
	movingBy(dx,dy);
	pos += QPoint(dx,dy);
	addToChunks();
    }
}

void QwPolygonalGraphic::movingBy(int dx, int dy)
{
}

void QwPolygonalGraphic::setPen(QPen p)
{
    pen = p;
    changeChunks();
}

void QwPolygonalGraphic::setBrush(QBrush b)
{
    // XXX if transparent, needn't add to inner chunks
    brush = b;
    changeChunks();
}



/*!
  \class QPolygon qspritefield.h
  \brief A polygon with a movable reference point.

  Paints a polygon in a QBrush and QPen.
*/
QwPolygon::QwPolygon()
{
}

QwPolygon::~QwPolygon()
{
    hide();
}

void QwPolygon::drawShape(QPainter & p)
{
    p.drawPolygon(poly);
}

void QwPolygon::setPoints(QPointArray pa)
{
    removeFromChunks();
    poly = pa;
    poly.translate(x(),y());
    addToChunks();
}

void QwPolygon::movingBy(int dx, int dy)
{
    poly.translate(dx,dy);
}

QPointArray QwPolygon::areaPoints() const
{
    return poly;
}

/*!
  \class QwRectangle QwSpritefield.h
  \brief A rectangle with a movable top-left point.

  Paints a rectangle in a QBrush and QPen.
*/
QwRectangle::QwRectangle() :
    w(32), h(32)
{
}

QwRectangle::~QwRectangle()
{
    hide();
}


int QwRectangle::width() const
{
    return w;
}

int QwRectangle::height() const
{
    return h;
}

void QwRectangle::setSize(int width, int height)
{
    removeFromChunks();
    w = width;
    h = height;
    addToChunks();
}

QPointArray QwRectangle::areaPoints() const
{
    QPointArray pa(4);
    pa[0] = position();
    pa[1] = position() + QPoint(w-1,0);
    pa[2] = position() + QPoint(w-1,h-1);
    pa[3] = position() + QPoint(0,h-1);
    return pa;
}

void QwRectangle::drawShape(class QPainter & p)
{
    p.drawRect(x(), y(), w, h);
}


/*!
  \class QwEllipse QwSpritefield.h
  \brief An ellipse with a movable center point.

  Paints an ellipse in a QBrush and QPen.
*/
QwEllipse::QwEllipse() :
    w(32), h(32),
    a1(0), a2(360*16)
{
}

QwEllipse::~QwEllipse()
{
    hide();
}

int QwEllipse::width() const
{
    return w;
}

int QwEllipse::height() const
{
    return h;
}

void QwEllipse::setSize(int width, int height)
{
    removeFromChunks();
    w = width;
    h = height;
    addToChunks();
}

void QwEllipse::setAngles(int start, int length)
{
    removeFromChunks();
    a1 = start;
    a2 = length;
    addToChunks();
}

QPointArray QwEllipse::areaPoints() const
{
    // XXX need something better to improve hit test now, and
    //     paint efficiency later.
    QPointArray pa(4);
    pa[0] = position() + QPoint(   -w/2,    -h/2);
    pa[1] = position() + QPoint((w+1)/2,    -h/2);
    pa[2] = position() + QPoint((w+1)/2, (w+1)/2);
    pa[3] = position() + QPoint(   -w/2, (w+1)/2);
    return pa;
}

void QwEllipse::drawShape(class QPainter & p)
{
    if ( !a1 && a2 == 360*16 ) {
	p.drawEllipse(QRect(position()+QPoint(   -w/2,    -h/2),
			position()+QPoint((w+1)/2, (w+1)/2)));
    } else {
	p.drawPie(QRect(position()+QPoint(   -w/2,    -h/2),
			position()+QPoint((w+1)/2, (w+1)/2)), a1, a2);
    }
}


/*!
\class QwTextSprite QwSpriteField.h
\brief A text object on a QwSpriteField.

  A QwTextSprite has text, a font, color, and position.
*/ 

/*!
  Construct a QwTextSprite with the text "<text>".
*/
QwTextSprite::QwTextSprite() :
    alt(0), ox(0), oy(0),
    text("<text>"), flags(0)
{
    setRect();
}

/*!
  Construct a QwTextSprite with the text \a t.

  The text should not contain newlines.
*/
QwTextSprite::QwTextSprite(const char* t) :
    alt(0), ox(0), oy(0),
    text(t), flags(0)
{
    setRect();
}

/*!
  Construct a QwTextSprite with the text \a t and font \a f.

  The text should not contain newlines.
*/
QwTextSprite::QwTextSprite(const char* t, QFont f) :
    alt(0), ox(0), oy(0),
    text(t), flags(0),
    font(f)
{
    setRect();
}

/*!
  Destruct the sprite.
*/
QwTextSprite::~QwTextSprite()
{
}

void QwTextSprite::setRect()
{
    static QWidget *w=0;
    if (!w) w = new QWidget;
    QPainter p(w);
    p.setFont(font);
    brect = p.boundingRect(QRect(ox,oy,0,0), flags, text);
}

/*!
  \fn int QwTextSprite::textFlags() const
  Returns the currently set alignment flags.
  \sa setTextFlags()
*/


/*!
  Sets the alignment flags.  These are a bitwise OR or \e some of the
  flags available to QPainter::drawText().

  <ul>
       <li> AlignLeft aligns to the left border. 
       <li> AlignRight aligns to the right border. 
       <li> AlignHCenter aligns horizontally centered. 
       <li> AlignTop aligns to the top border. 
       <li> AlignBottom aligns to the bottom border. 
       <li> AlignVCenter aligns vertically centered 
       <li> AlignCenter (= AlignHCenter | AlignVCenter) 
       <li> SingleLine ignores newline characters in the text. 
       <li> ExpandTabs expands tabulators. 
       <li> ShowPrefix displays "&x" as "x" underlined. 
       <li> GrayText grays out the text. 
  </ul>

  The DontClip and WordBreak flags are not supported.
*/
void QwTextSprite::setTextFlags(int f)
{
    removeFromChunks();
    flags = f;
    setRect();
    addToChunks();
}

/*!
  \fn int QwTextSprite::x() const
  Returns the X-position of the left edge of the text.
*/

/*!
  \fn int QwTextSprite::y() const
  Returns the Y-position of the top edge of the text.
*/

/*!
  \fn const QRect& QwTextSprite::boundingRect()
  Returns the bounding rectangle of the text.
*/

/*!
  Sets the text to be displayed.  The text may contain newlines.
*/
void QwTextSprite::setText( const char* t )
{
    removeFromChunks();
    text = t;
    setRect();
    addToChunks();
}

/*!
  Sets the font in which the text is drawn.
*/
void QwTextSprite::setFont( const QFont& f )
{
    removeFromChunks();
    font = f;
    setRect();
    addToChunks();
}

/*!
  Sets the color of the text.
*/
void QwTextSprite::setColor(const QColor& c)
{
    col=c;
    changeChunks();
}


/*!
  Sets the X-positions of the bottom-left corner of the text.
*/
void QwTextSprite::x(int c)
{
    removeFromChunks();
    brect.moveBy(c-ox,0);
    ox = c;
    addToChunks();
}

/*!
  Sets the Y-positions of the bottom-left corner of the text.
*/
void QwTextSprite::y(int c)
{
    removeFromChunks();
    brect.moveBy(0,c-oy);
    oy = c;
    addToChunks();
}

/*!
  Moves text by the given displacements.
*/
void QwTextSprite::moveBy(int dx, int dy)
{
    removeFromChunks();
    brect.moveBy(dx,dy);
    addToChunks();
}

/*!
  Sets the position of the bottom-left corner of the text.
*/
void QwTextSprite::moveTo(int x, int y)
{
    removeFromChunks();
    brect.moveBy(x-ox, y-oy);
    ox = x;
    oy = y;
    addToChunks();
}

/*!
  Sets the altitude of the text.
*/
void QwTextSprite::z(int a)
{
    alt=a;
    changeChunks();
}

/*!
  Return the altitude of the text.
*/
int QwTextSprite::z() const
{
    return alt;
}

/*!
  For collision detection.
*/
bool QwTextSprite::at(int x, int y) const
{
    return brect.contains(QPoint(x,y));
}

/*!
  For collision detection.
*/
bool QwTextSprite::at(const class QRect & r) const
{
    return r.intersects(brect);
}




/*!
  Toggles visibility.
*/
void QwTextSprite::makeVisible(bool yes)
{
    if (yes) {
	addToChunks();
    } else {
	removeFromChunks();
    }
}


/*!
  Draws the text.
*/
void QwTextSprite::draw(QPainter& painter)
{
    painter.setFont(font);
    painter.setPen(col);
    painter.drawText(brect, flags, text);
}

void QwTextSprite::changeChunks()
{
    if (visible() && spritefield) {
	int chunksize=spritefield->chunkSize();
	for (int j=brect.top()/chunksize; j<=brect.bottom()/chunksize; j++) {
	    for (int i=brect.left()/chunksize; i<=brect.right()/chunksize; i++) {
		spritefield->setChangedChunk(i,j);
	    }
	}
    }
}

/*!
  Adds the sprite to the appropriate chunks.
*/
void QwTextSprite::addToChunks()
{
    if (visible() && spritefield) {
	int chunksize=spritefield->chunkSize();
	for (int j=brect.top()/chunksize; j<=brect.bottom()/chunksize; j++) {
	    for (int i=brect.left()/chunksize; i<=brect.right()/chunksize; i++) {
		spritefield->addGraphicToChunk(this,i,j);
	    }
	}
    }
}

/*!
  Removes the sprite to the appropriate chunks.
*/
void QwTextSprite::removeFromChunks()
{
    if (visible() && spritefield) {
	int chunksize=spritefield->chunkSize();
	for (int j=brect.top()/chunksize; j<=brect.bottom()/chunksize; j++) {
	    for (int i=brect.left()/chunksize; i<=brect.right()/chunksize; i++) {
		spritefield->removeGraphicFromChunk(this,i,j);
	    }
	}
    }
}



/*!
Returns 0.

Although often frowned upon by purists, Run Time
Type Identification is very useful in this case, as it allows
a QwSpriteField to be an efficient indexed storage mechanism.

Make your derived classes return their own values for rtti(), and you
can distinguish between objects returned by QwSpriteField::at().  You should
use values greater than 1000 preferrably a large random number,
to allow for extensions to this class.

However, it is important not to overuse this facility, as
it damages extensibility.  For example, once you have identified
a base class of a QwSpriteFieldGraphic found by QwSpriteField::at(), cast it
to that type and call meaningful methods rather than acting
upon the object based on its rtti value.

For example:

\code
    if (field.at(p)->rtti() == MySprite::rtti()) {
    MySprite* s = (MySprite*)field.at(p);
    if (s->isDamagable()) s->loseHitPoints(1000);
    if (s->isHot()) myself->loseHitPoints(1000);
    ...
    }
\endcode
*/
int QwSpriteFieldGraphic::rtti() const { return 0; }

/*!
Returns 1.

\sa QwSpriteFieldGraphic::rtti()
*/
int QwVirtualSprite::rtti() const { return 1; }

/*!
Returns 2.

\sa QwSpriteFieldGraphic::rtti()
*/
int QwPolygonalGraphic::rtti() const { return 2; }

/*!
Returns 3.

\sa QwSpriteFieldGraphic::rtti()
*/
int QwTextSprite::rtti() const { return 3; }

/*!
Returns 4.

\sa QwSpriteFieldGraphic::rtti()
*/
int QwPolygon::rtti() const { return 4; }

/*!
Returns 5.

\sa QwSpriteFieldGraphic::rtti()
*/
int QwRectangle::rtti() const { return 5; }

/*!
Returns 6.

\sa QwSpriteFieldGraphic::rtti()
*/
int QwEllipse::rtti() const { return 6; }


//#include "moc_QwSpriteField.cpp"
