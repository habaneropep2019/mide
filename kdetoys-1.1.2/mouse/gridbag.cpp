#include <iostream.h>

#include <stdlib.h>
#include <string.h>
#include <qdict.h>
#include <qobjcoll.h>
#include <qlist.h>
#include <qwidget.h>

#define MAX_VALUE 65536

#include "gridbag.h"

Rectangle::Rectangle(int xx,int yy,int h,int w)
{
    x=xx;
    y=yy;
    height=h;
    width=w;
}

Rectangle::Rectangle()
{
    x=0;
    y=0;
    width=0;
    height=0;
}

Insets::Insets(int top, int left, int bottom, int right)
{
    Insets::top = top;
    Insets::left = left;
    Insets::bottom = bottom;
    Insets::right = right;
}


Insets::Insets()
{
    top = 0;
    left = 0;
    bottom = 0;
    right = 0;
}

Insets::Insets(const Insets& i) {
    copy(i);
}

Insets& Insets::operator=(const Insets& i)
{
    copy(i);

    return (*this);
}

void Insets::copy(const Insets& i)
{
    top=i.top;
    bottom=i.bottom;
    right=i.right;
    left=i.left;
}



GridBagConstraints::GridBagConstraints()
  : RELATIVE(-1)
{
    gridx = RELATIVE;
    gridy = RELATIVE;
    gridwidth = 1;
    gridheight = 1;
	
    weightx = 0;
    weighty = 0;
    anchor = CENTER;
    fill = NONE;
    
    ipadx = 0;
    ipady = 0;

    tempX=0;
    tempY=0;

    tempWidth=0;
    tempHeight=0;
}

GridBagConstraints::GridBagConstraints(const GridBagConstraints& c)
{
    copy(c);
}

GridBagConstraints& GridBagConstraints::operator=(const GridBagConstraints& c)
{
    copy(c);

    return (*this);
}

void GridBagConstraints::copy(const GridBagConstraints& c)
{
    gridx=c.gridx;
    gridy=c.gridy;
    gridwidth=c.gridwidth;
    gridheight=c.gridheight;
	
    weightx=c.weightx;
    weighty=c.weighty;
	
    anchor=c.anchor;
    fill=c.fill;
	
    insets=c.insets;
    ipadx=c.ipadx;
    ipady=c.ipady;
 
    tempX=c.tempX;
    tempY=c.tempY;
	
    tempWidth=c.tempWidth;
    tempHeight=c.tempHeight;
	
    minWidth=c.minWidth;
    minHeight=c.minHeight;
}

ostream& operator<<(ostream& out, GridBagConstraints& c)
{
    out << "GridBagConstraints["
	<< c.gridx << "," << c.gridy << ","
	<< c.gridwidth << "," << c.gridheight << ","
	<< c.weightx << "," << c.weighty
	<< "]" << endl;

    return out;
}


ostream& operator<<(ostream& out, GridBagLayoutInfo& in)
{
    out << "Weight: " << in.width << ", " << "Height: " << in.height << endl;

    for(int i=0;i<in.width;i++) 
	cout << i << ": " << in.minWidth[i] << ", ";

    out << endl;

    for(int i=0;i<in.height;i++) 
	out << i << ": " << in.minHeight[i] << ", ";

    out << endl;

    return out;
}

/**
 * Creates a gridbag layout.
 */
GridBagLayout::GridBagLayout(Insets i)
  : MAXGRIDSIZE(128),
    MINSIZE(1),
    PREFERREDSIZE(2)

{

    containerInsets=i;
    
    columnWidthsLength=0;
    rowHeightsLength=0;
    columnWeightsLength=0;
    rowWeightsLength=0;

    layoutInfo = NULL;
}


GridBagLayout::GridBagLayout()
{
//    comptable = new QDict<GridBagConstraints>(100,false);
//    defaultConstraints = new GridBagConstraints();

    columnWidthsLength=0;
    rowHeightsLength=0;
    columnWeightsLength=0;
    rowWeightsLength=0;

    layoutInfo = NULL;
}


GridBagLayout::~GridBagLayout()
{
    GridIterator it(comptable);
    GridPair* p;
    
    while(it.current()) {

	p=it.current();
	delete p->second;
	delete p;
    }
}

/*
 * Change the insets used to pad the entire panel
 */
void GridBagLayout::setInsets(Insets i)
{
    containerInsets=i;
}


void GridBagLayout::addLayoutComponent(char* , QWidget*)
{
}

void GridBagLayout::removeLayoutComponent(QWidget* )
{
}
    
/**
     * Sets the constraints for the specified component.
     * @param comp the component to be modified
     * @param constraints the constraints to be applied
     */
void GridBagLayout::setConstraints(QWidget* comp, GridBagConstraints& constraints)
{
    comptable.insert(comp->name(),new GridPair(comp,new GridBagConstraints(constraints)));
}

/**
 * Retrieves the constraints for the specified component.  A copy of
 * the constraints is returned.
 * @param comp the component to be queried
 */
GridBagConstraints GridBagLayout::getConstraints(QWidget* comp)
{
    GridPair* p = comptable.find(comp->name());

    GridBagConstraints* constraints=p->second;
    
    if (constraints == NULL) {
	
	setConstraints(comp, defaultConstraints);
	return getConstraints(comp);
    }
	
    return GridBagConstraints(*constraints);
}

/**
 * Retrieves the constraints for the specified component.  The return
 * value is not a copy, but is the actual constraints class used by the
 * layout mechanism.
 * @param comp the component to be queried
 */
GridBagConstraints* GridBagLayout::lookupConstraints(QWidget* comp)
{
    GridPair* p = comptable.find(comp->name());

    GridBagConstraints* constraints=p->second;
    
    if (constraints == NULL)
    {
	setConstraints(comp, defaultConstraints);
	return lookupConstraints(comp);
    }
    return constraints;
}

QPoint* GridBagLayout::getLayoutOrigin ()
{
    QPoint* origin = new QPoint(0,0);
	
    if (layoutInfo != NULL) {
	origin->setX(layoutInfo->startx);
	origin->setY(layoutInfo->starty);
    }
    return origin;
}

int** GridBagLayout::getLayoutDimensions ()
{
    if (layoutInfo == NULL)
	return NULL;

    int** dim;

    dim=(int**)malloc(2*sizeof(int*));
	
    dim[0] = new int[layoutInfo->width];
    dim[1] = new int[layoutInfo->height];

    memcpy(&layoutInfo->minWidth[0],&dim[0][0],layoutInfo->width);
    memcpy(&layoutInfo->minHeight[0],&dim[1][0],layoutInfo->height);

    return dim;
}

float** GridBagLayout::getLayoutWeights ()
{
    if (layoutInfo == NULL)
	return NULL;

    float** weights = (float**)malloc(2*sizeof(float));
    weights[0] = new float[layoutInfo->width];
    weights[1] = new float[layoutInfo->height];

    memcpy(&layoutInfo->weightX[0],&weights[0][0],layoutInfo->width);
    memcpy(&layoutInfo->weightY[0],&weights[1][0],layoutInfo->height);

    return weights;
}

void GridBagLayout::location(int x, int y,QPoint* loc)
{
//    QPoint* loc = new QPoint(0,0);
    int i, d;

    if (layoutInfo == NULL)
	return;

    d = layoutInfo->startx;
    for (i=0; i<layoutInfo->width; i++) {
	d += layoutInfo->minWidth[i];
	if (d > x)
	    break;
    }
    loc->setX(i);

    d = layoutInfo->starty;
    for (i=0; i<layoutInfo->height; i++) {
	d += layoutInfo->minHeight[i];
	if (d > y)
	    break;
    }
    loc->setY(i);

    //  return loc;
}


/** 
 * Returns the preferred dimensions for this layout given the components
 * in the specified panel.
 * @param parent the component which needs to be laid out 
 * @see #minimumLayoutSize
 */
void GridBagLayout::preferredLayoutSize(QWidget* parent,QSize* dim)
{
    GridBagLayoutInfo* info = GetLayoutInfo(parent, PREFERREDSIZE);
    QSize* d=GetMinSize(parent, info);
    
    (*dim)=(*d);
    
    delete info;
}

/**
 * Returns the minimum dimensions needed to layout the components 
 * contained in the specified panel.
 * @param parent the component which needs to be laid out 
 * @see #preferredLayoutSize
 */
void  GridBagLayout::minimumLayoutSize(QWidget* parent,QSize* dim)
{
    GridBagLayoutInfo* info = GetLayoutInfo(parent, MINSIZE);
    QSize* d=GetMinSize(parent, info);

    (*dim)=(*d);

    delete info;
}

/** 
     * Lays out the container in the specified panel.  
     * @param parent the specified component being laid out
     * @see Container
     */
void GridBagLayout::layoutContainer(QWidget* parent)
{
    ArrangeGrid(parent);
}

    
/*
 * Fill in an instance of the above structure for the current set
 * of managed children.  This requires three passes through the
 * set of children:
 *
 * 1) Figure out the dimensions of the layout grid
 * 2) Determine which cells the components occupy
 * 3) Distribute the weights and min sizes amoung the rows/columns.
 *
 * This also caches the minsizes for all the children when they are
 * first encountered (so subsequent loops don't need to ask again).
 */
GridBagLayoutInfo* GridBagLayout::GetLayoutInfo(QWidget* , int)
{
    GridBagLayoutInfo* r = new GridBagLayoutInfo();
    QWidget* comp=NULL;;
    GridBagConstraints* constraints=NULL;;
    QSize* d=NULL;

    int i=0, k=0, px=0, py=0, pixels_diff=0, nextSize=0;
    int curX=0, curY=0, curWidth=0, curHeight=0, curRow=0, curCol=0;
    float weight_diff=0, weight=0;

    /*
     * Pass #1
     *
     * Figure out the dimensions of the layout grid (use a value of 1 for
     * zero or negative widths and heights).
     */
    
    r->width = r->height = 0;
    curRow = curCol = -1;
    int* xMax = new int[MAXGRIDSIZE];
    int* yMax = new int[MAXGRIDSIZE];

//    QObjectList  *list = parent->queryList( "QWidget" );
//    QObjectListIt it( *list );          // iterate over the buttons

    GridIterator it(comptable);

    it.toFirst();
    
    while(it.current())
    {
	GridPair* p=it.current();
	
	comp = p->first;
	constraints = p->second;
      
	curX = constraints->gridx;
	curY = constraints->gridy;
	curWidth = constraints->gridwidth;
	if (curWidth <= 0)
	    curWidth = 1;
	curHeight = constraints->gridheight;
	if (curHeight <= 0)
	    curHeight = 1;
      
	/* If x or y is negative, then use relative positioning: */
	if (curX < 0 && curY < 0) {
	    if (curRow >= 0)
		curY = curRow;
	    else if (curCol >= 0)
		curX = curCol;
	    else
		curY = 0;
	}
	if (curX < 0) {
	    px = 0;
	    for (i = curY; i < (curY + curHeight); i++)
		px = QMAX(px, xMax[i]);
	
	    curX = px - curX - 1;
	    if(curX < 0)
		curX = 0;
	}
	else if (curY < 0) {
	    py = 0;
	    for (i = curX; i < (curX + curWidth); i++)
		py = QMAX(py, yMax[i]);
	
	    curY = py - curY - 1;
	    if(curY < 0)
		curY = 0;
	}
      
	/* Adjust the grid width and height */
	for (px = curX + curWidth; r->width < px; r->width++);
	for (py = curY + curHeight; r->height < py; r->height++);
      
	/* Adjust the xMax and yMax arrays */
	for (i = curX; i < (curX + curWidth); i++) { yMax[i] = py; }
	for (i = curY; i < (curY + curHeight); i++) { xMax[i] = px; }
      
	/* Cache the current slave's size. */
//	if (sizeflag == PREFERREDSIZE)
//	    d = &comp->maximumSize();
//	else

	d = &comp->sizeHint();
//	cout << "preferred For widget: " << comp->name() << ", "
//	     << d->width() <<"," << d->height() << endl;
	
	    
	constraints->minWidth = d->width();
	constraints->minHeight = d->height();
      
	/* Zero width and height must mean that this is the last item (or
	 * else something is wrong). */
	if (constraints->gridheight == 0 && constraints->gridwidth == 0)
	    curRow = curCol = -1;
      
	/* Zero width starts a new row */
	if (constraints->gridheight == 0 && curRow < 0)
	    curCol = curX + curWidth;
      
	/* Zero height starts a new column */
	else if (constraints->gridwidth == 0 && curCol < 0)
	    curRow = curY + curHeight;

	++it;
    }
    
    /*
     * Apply minimum row/column dimensions
     */
    if (columnWidths != NULL && r->width < columnWidthsLength)
	r->width = columnWidthsLength;
    if (rowHeights != NULL && r->height < rowHeightsLength)
	r->height = rowHeightsLength;

    /*
     * Pass #2
     *
     * Negative values for gridX are filled in with the current x value.
     * Negative values for gridY are filled in with the current y value.
     * Negative or zero values for gridWidth and gridHeight end the current
     *  row or column, respectively.
     */
    
    curRow = curCol = -1;
    xMax = new int[MAXGRIDSIZE];
    yMax = new int[MAXGRIDSIZE];


    it.toFirst();
    
    while(it.current())
    {
	GridPair* p=it.current();
	
	comp = p->first;
	constraints = p->second;
      
	curX = constraints->gridx;
	curY = constraints->gridy;
	curWidth = constraints->gridwidth;
	curHeight = constraints->gridheight;
      
	/* If x or y is negative, then use relative positioning: */
	if (curX < 0 && curY < 0) {
	    if(curRow >= 0)
		curY = curRow;
	    else if(curCol >= 0)
		curX = curCol;
	    else
		curY = 0;
	}
      
	if (curX < 0) {
	    if (curHeight <= 0) {
		curHeight += r->height - curY;
		if (curHeight < 1)
		    curHeight = 1;
	    }
	
	    px = 0;
	    for (i = curY; i < (curY + curHeight); i++)
		px = QMAX(px, xMax[i]);
	
	    curX = px - curX - 1;
	    if(curX < 0)
		curX = 0;
	}
	else if (curY < 0) {
	    if (curWidth <= 0) {
		curWidth += r->width - curX;
		if (curWidth < 1)
		    curWidth = 1;
	    }
	
	    py = 0;
	    for (i = curX; i < (curX + curWidth); i++)
		py = QMAX(py, yMax[i]);
	
	    curY = py - curY - 1;
	    if(curY < 0)
		curY = 0;
	}
      
	if (curWidth <= 0) {
	    curWidth += r->width - curX;
	    if (curWidth < 1)
		curWidth = 1;
	}
      
	if (curHeight <= 0) {
	    curHeight += r->height - curY;
	    if (curHeight < 1)
		curHeight = 1;
	}
      
	px = curX + curWidth;
	py = curY + curHeight;
      
	for (i = curX; i < (curX + curWidth); i++) { yMax[i] = py; }
	for (i = curY; i < (curY + curHeight); i++) { xMax[i] = px; }
      
	/* Make negative sizes start a new row/column */
	if (constraints->gridheight == 0 && constraints->gridwidth == 0)
	    curRow = curCol = -1;
	if (constraints->gridheight == 0 && curRow < 0)
	    curCol = curX + curWidth;
	else if (constraints->gridwidth == 0 && curCol < 0)
	    curRow = curY + curHeight;
      
	/* Assign the new values to the gridbag slave */
	constraints->tempX = curX;
	constraints->tempY = curY;
	constraints->tempWidth = curWidth;
	constraints->tempHeight = curHeight;

	++it;
    }
    
    /*
     * Apply minimum row/column dimensions and weights
     */
    if (columnWidths != NULL)
	memcpy(columnWidths, r->minWidth,  columnWidthsLength);
    if (rowHeights != NULL)
	memcpy(rowHeights,r->minHeight, rowHeightsLength);
    if (columnWeights != NULL)
	memcpy(columnWeights, r->weightX, columnWeightsLength);
    if (rowWeights != NULL)
	memcpy(rowWeights,  r->weightY,  rowWeightsLength);

    /*
     * Pass #3
     *
     * Distribute the minimun widths and weights:
     */
    
    nextSize = MAX_VALUE;
    
    for (i = 1;
	 i != MAX_VALUE;
	 i = nextSize, nextSize = MAX_VALUE) {
	    
	GridIterator it3(comptable);
	    
	while(it3.current())
	{
	    comp = it3.current()->first;
		
	    constraints = it3.current()->second;
      
	    if (constraints->tempWidth == i)
	    {
		/* right column */
		px = constraints->tempX + constraints->tempWidth;
	  
		/* Figure out if we should use this slave\'s
		 * weight.  If the weight is less than the total
		 * weight spanned by the width of the cell, then
		 * discard the weight.  Otherwise split the
		 * difference according to the existing weights.  */
	  
		weight_diff = constraints->weightx;
		    
		for (k = constraints->tempX; k < px; k++)
		    weight_diff -= r->weightX[k];
		    
		if (weight_diff > 0.0)
		{
		    weight = 0.0;
		    for (k = constraints->tempX; k < px; k++)
			weight += r->weightX[k];
			
		    for (k = constraints->tempX; weight > 0.0 && k < px; k++) {
			float wt = r->weightX[k];
			float dx = (wt * weight_diff) / weight;
			r->weightX[k] += dx;
			weight_diff -= dx;
			weight -= wt;
		    }
		    /* Assign the remainder to the rightmost cell */
		    r->weightX[px-1] += weight_diff;
		}
	  
		/*
		 * Calculate the minWidth array values.
		 * First, figure out how wide the current slave needs to be.
		 * Then, see if it will fit within the current minWidth values.
		 * If it will not fit, add the difference according to the
		 * weightX array.
		 */
	  
		pixels_diff =
		    constraints->minWidth + constraints->ipadx +
		    constraints->insets.left + constraints->insets.right;

		for (k = constraints->tempX; k < px; k++)
		    pixels_diff -= r->minWidth[k];
		if (pixels_diff > 0) {
		    weight = 0.0;
		    for (k = constraints->tempX; k < px; k++)
			weight += r->weightX[k];
		    for (k = constraints->tempX; weight > 0.0 && k < px; k++) {
			float wt = r->weightX[k];
			int dx = (int)((wt * ((float)pixels_diff)) / weight);
			r->minWidth[k] += dx;
			pixels_diff -= dx;
			weight -= wt;
		    }
		    /* Any leftovers go into the rightmost cell */
		    r->minWidth[px-1] += pixels_diff;
		}
	    }
	    else if (constraints->tempWidth > i && constraints->tempWidth < nextSize)
		nextSize = constraints->tempWidth;
	
	
	    if (constraints->tempHeight == i) {
		py = constraints->tempY + constraints->tempHeight;
		/* bottom row */
	  
		/* Figure out if we should use this slave\'s
		 * weight.  If the weight is less than the total
		 * weight spanned by the height of the cell, then
		 * discard the weight.  Otherwise split it the
		 * difference according to the existing weights.  */
	  
		weight_diff = constraints->weighty;
		for (k = constraints->tempY; k < py; k++)
		    weight_diff -= r->weightY[k];
		if (weight_diff > 0.0) {
		    weight = 0.0;
		    for (k = constraints->tempY; k < py; k++)
			weight += r->weightY[k];
		    for (k = constraints->tempY; weight > 0.0 && k < py; k++) {
			float wt = r->weightY[k];
			float dy = (wt * weight_diff) / weight;
			r->weightY[k] += dy;
			weight_diff -= dy;
			weight -= wt;
		    }
		    /* Assign the remainder to the bottom cell */
		    r->weightY[py-1] += weight_diff;
		}
	  
		/*
		 * Calculate the minHeight array values.
		 * First, figure out how tall the current slave needs to be.
		 * Then, see if it will fit within the current minHeight values.
		 * If it will not fit, add the difference according to the
		 * weightY array.
		 */
	  
		pixels_diff =
		    constraints->minHeight + constraints->ipady +
		    constraints->insets.top + constraints->insets.bottom;
		for (k = constraints->tempY; k < py; k++)
		    pixels_diff -= r->minHeight[k];
		if (pixels_diff > 0) {
		    weight = 0.0;
		    for (k = constraints->tempY; k < py; k++)
			weight += r->weightY[k];
		    for (k = constraints->tempY; weight > 0.0 && k < py; k++) {
			float wt = r->weightY[k];
			int dy = (int)((wt * ((float)pixels_diff)) / weight);
			r->minHeight[k] += dy;
			pixels_diff -= dy;
			weight -= wt;
		    }
		    /* Any leftovers go into the bottom cell */
		    r->minHeight[py-1] += pixels_diff;
		}
	    }
	    else if (constraints->tempHeight > i &&
		     constraints->tempHeight < nextSize)
		nextSize = constraints->tempHeight;

	    ++it3;
	}
	    
    }

    delete yMax;
    delete xMax;
    
    return r;
}
  
/*
     * Adjusts the x, y, width, and height fields to the correct
     * values depending on the constraint geometry and pads.
     */
void GridBagLayout::AdjustForGravity(GridBagConstraints* constraints,
		      Rectangle& r)
{
    int diffx, diffy;

    r.x += constraints->insets.left;
    r.width -=(constraints->insets.left + constraints->insets.right);
	
    r.y += constraints->insets.top;
    r.height -= (constraints->insets.top + constraints->insets.bottom);
    
    diffx = 0;
    if ((constraints->fill != GridBagConstraints::HORIZONTAL &&
	 constraints->fill != GridBagConstraints::BOTH)
	&& (r.width > (constraints->minWidth + constraints->ipadx))) {
	diffx = r.width - (constraints->minWidth + constraints->ipadx);
	r.width = constraints->minWidth + constraints->ipadx;
    }
    
    diffy = 0;
    if ((constraints->fill != GridBagConstraints::VERTICAL &&
	 constraints->fill != GridBagConstraints::BOTH)
	&& (r.height > (constraints->minHeight + constraints->ipady))) {
	diffy = r.height - (constraints->minHeight + constraints->ipady);
	r.height = constraints->minHeight + constraints->ipady;
    }
    
    switch (constraints->anchor) {
    case GridBagConstraints::CENTER:
	r.x += diffx/2;
	r.y += diffy/2;
	break;
    case GridBagConstraints::NORTH:
	r.x += diffx/2;
	break;
    case GridBagConstraints::NORTHEAST:
	r.x += diffx;
	break;
    case GridBagConstraints::EAST:
	r.x += diffx;
	r.y += diffy/2;
	break;
    case GridBagConstraints::SOUTHEAST:
	r.x += diffx;
	r.y += diffy;
	break;
    case GridBagConstraints::SOUTH:
	r.x += diffx/2;
	r.y += diffy;
	break;
    case GridBagConstraints::SOUTHWEST:
	r.y += diffy;
	break;
    case GridBagConstraints::WEST:
	r.y += diffy/2;
	break;
    case GridBagConstraints::NORTHWEST:
	break;
    default:
	break;
    }
}

/*
 * Figure out the minimum size of the
 * master based on the information from GetLayoutInfo()
 */
QSize* GridBagLayout::GetMinSize(QWidget*, GridBagLayoutInfo* info)
{
    QSize* d = new QSize();
    int i, t;
    

    t = 0;
    for(i = 0; i < info->width; i++)
	t += info->minWidth[i];
    d->setWidth(t + containerInsets.left + containerInsets.right);

    t = 0;
    for(i = 0; i < info->height; i++)
	t += info->minHeight[i];
    d->setHeight(t + containerInsets.top + containerInsets.bottom);

    return d;
}

/*
 * Lay out the grid.  This is where all the work happens
 */
void GridBagLayout::ArrangeGrid(QWidget* parent)
{
    QWidget* comp=NULL;
    GridBagConstraints* constraints=NULL;
    Insets insets=containerInsets;
    QSize* d=NULL;
    Rectangle r;
    int i=0, diffw=0, diffh=0;
    float weight=0;
    GridBagLayoutInfo* info;
    
    /*
     * If the parent has no slaves anymore, then don't do anything
     * at all:  just leave the parent's size as-is.
     */
    if (parent->children()->count() == 0 &&
	(columnWidths == NULL || columnWidthsLength == 0) &&
	(rowHeights == NULL || rowHeightsLength == 0)) {
	return;
    }
    
    /*
     * Pass #1: scan all the slaves to figure out the total amount
     * of space needed.
     */
    
    info = GetLayoutInfo(parent, PREFERREDSIZE);
    d = GetMinSize(parent, info);

    if (d->width() < parent->width() || d->height() < parent->height()) {
	delete info;
	info = GetLayoutInfo(parent, MINSIZE);
	d = GetMinSize(parent, info);
    }

    layoutInfo = info;
    r.width=d->width();
    r.height=d->height();

    
    /*
     * If the current dimensions of the window don't match the desired
     * dimensions, then adjust the minWidth and minHeight arrays
     * according to the weights.
     */
    
    diffw = parent->width() - r.width;
    if (diffw != 0) {
	weight = 0.0;
	for (i = 0; i < info->width; i++)
	    weight += info->weightX[i];
	if (weight > 0.0) {
	    for (i = 0; i < info->width; i++) {
		int dx = (int)(( ((float)diffw) * info->weightX[i]) / weight);
		info->minWidth[i] += dx;
		r.width += dx;
		if (info->minWidth[i] < 0) {
		    r.width -= info->minWidth[i];
		    info->minWidth[i] = 0;
		}
	    }
	}
	diffw = parent->width() - r.width;
    }
    else {
	diffw = 0;
    }
    
    diffh = parent->height() - r.height;
    if (diffh != 0) {
	weight = 0.0;
	for (i = 0; i < info->height; i++)
	    weight += info->weightY[i];
	if (weight > 0.0) {
	    for (i = 0; i < info->height; i++) {
		int dy = (int)(( ((float)diffh) * info->weightY[i]) / weight);
		info->minHeight[i] += dy;
		r.height += dy;
		if (info->minHeight[i] < 0) {
		    r.height -= info->minHeight[i];
		    info->minHeight[i] = 0;
		}
	    }
	}
	diffh = parent->height() - r.height;
    }
    else {
	diffh = 0;
    }

    /*
     * DEBUG
     *
     * System.out.println("Re-adjusted:");
     * DumpLayoutInfo(info);
     */
    
    /*
     * Now do the actual layout of the slaves using the layout information
     * that has been collected.
     */
    
    info->startx = diffw/2 + insets.left;
    info->starty = diffh/2 + insets.top;

    
    // Tranverse over the managed children
    GridIterator it2( comptable );          
	
    while(it2.current()) {
	
	comp = it2.current()->first;
	constraints = it2.current()->second;

	r.x=info->startx;
	for(i = 0; i < constraints->tempX; i++)
	   r.x += info->minWidth[i];
	
      
	r.y = info->starty;
	for(i = 0; i < constraints->tempY; i++)
	    r.y +=info->minHeight[i];

	r.width=0;
	for(i = constraints->tempX;
	    i < (constraints->tempX + constraints->tempWidth);
	    i++) {
	    r.width += info->minWidth[i];
	}

      
	r.height = 0;
	for(i = constraints->tempY;
	    i < (constraints->tempY + constraints->tempHeight);
	    i++) {
	    r.height += info->minHeight[i];
	}
      
	AdjustForGravity(constraints, r);
      
	/*
	 * If the window is too small to be interesting then
	 * unmap it.  Otherwise configure it and then make sure
	 * it's mapped.
	 */
      
	if ((r.width <= 0) || (r.height <= 0)) {
	    
	    comp->setGeometry(0, 0, 0, 0);
	}
	else {
	    
	    if (comp->x() != r.x || comp->y() != r.y ||
		comp->width() != r.width || comp->height() != r.height) {
		
		comp->setGeometry(r.x, r.y, r.width, r.height);
	    }
	}

	++it2;
    }

    delete info;
}


/*
 * Lay out the grid.  This is where all the work happens
 */
GridBagLayoutInfo* GridBagLayout::getGridInfo(QWidget* parent)
{
//    QWidget* comp=NULL;
//    GridBagConstraints* constraints=NULL;
//    Insets insets=containerInsets;
    QSize* d=NULL;
    Rectangle r;
    int i=0, diffw=0, diffh=0;
    float weight=0;
    GridBagLayoutInfo* info;
    
    /*
     * If the parent has no slaves anymore, then don't do anything
     * at all:  just leave the parent's size as-is.
     */
    if (parent->children()->count() == 0 &&
	(columnWidths == NULL || columnWidthsLength == 0) &&
	(rowHeights == NULL || rowHeightsLength == 0)) {
	return NULL;
    }
    
    /*
     * Pass #1: scan all the slaves to figure out the total amount
     * of space needed.
     */
    
    info = GetLayoutInfo(parent, PREFERREDSIZE);
    d = GetMinSize(parent, info);

    if (d->width() < parent->width() || d->height() < parent->height()) {
	delete info;
	info = GetLayoutInfo(parent, MINSIZE);
	d = GetMinSize(parent, info);
    }

    layoutInfo = info;
    r.width=d->width();
    r.height=d->height();

    
    /*
     * If the current dimensions of the window don't match the desired
     * dimensions, then adjust the minWidth and minHeight arrays
     * according to the weights.
     */
    
    diffw = parent->width() - r.width;
    if (diffw != 0) {
	weight = 0.0;
	for (i = 0; i < info->width; i++)
	    weight += info->weightX[i];
	if (weight > 0.0) {
	    for (i = 0; i < info->width; i++) {
		int dx = (int)(( ((float)diffw) * info->weightX[i]) / weight);
		info->minWidth[i] += dx;
		r.width += dx;
		if (info->minWidth[i] < 0) {
		    r.width -= info->minWidth[i];
		    info->minWidth[i] = 0;
		}
	    }
	}
	diffw = parent->width() - r.width;
    }
    else {
	diffw = 0;
    }
    
    diffh = parent->height() - r.height;
    if (diffh != 0) {
	weight = 0.0;
	for (i = 0; i < info->height; i++)
	    weight += info->weightY[i];
	if (weight > 0.0) {
	    for (i = 0; i < info->height; i++) {
		int dy = (int)(( ((float)diffh) * info->weightY[i]) / weight);
		info->minHeight[i] += dy;
		r.height += dy;
		if (info->minHeight[i] < 0) {
		    r.height -= info->minHeight[i];
		    info->minHeight[i] = 0;
		}
	    }
	}
	diffh = parent->height() - r.height;
    }
    else {
	diffh = 0;
    }

    return info;
}
