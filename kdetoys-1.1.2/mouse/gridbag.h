#ifndef __Q_GRIDBAG__
#define __Q_GRIDBAG__

#include <stdlib.h>
#include <string.h>
#include <qdict.h>
#include <qobjcoll.h>
#include <qlist.h>
#include <qwidget.h>

#include <iostream>

#define MAX_VALUE 65536

#include "stl.hxx"

class LayoutManager
{
public:
    virtual void addLayoutComponent(char* , QWidget*)=0;

    virtual void removeLayoutComponent(QWidget* )=0;
    virtual void  preferredLayoutSize(QWidget*, QSize* )=0;

    virtual void  minimumLayoutSize(QWidget*,QSize* )=0;
    virtual void layoutContainer(QWidget* )=0;
};

class Rectangle
{
public:
    
    int x;
    int y;
    int width;
    int height;

    Rectangle(int xx,int yy,int h,int w);
    Rectangle();
};

class Insets
{

public:
 
    /// The inset from the top.
    int top;

    /// left inset
    int left;

    /// The inset from the bottom.
     int bottom;

    /// The inset from the right.
     int right;

    /**
     * Constructs and initializes a new Inset with the specified top,
     * left, bottom, and right insets.
     * @param top the inset from the top
     * @param left the inset from the left
     * @param bottom the inset from the bottom
     * @param right the inset from the right
     */
    Insets(int top, int left, int bottom, int right);

    Insets(const Insets& i);

    Insets();

    Insets& operator=(const Insets& i);

    void copy(const Insets& i);

    void set(int t,int l,int b,int r) {
	top=t;
	left=l;
	bottom=b;
	right=r;
    }
};



class GridBagConstraints
{
public:
 
    const int RELATIVE;

    enum { NONE = 0, BOTH, HORIZONTAL, VERTICAL };
 	   
    enum { CENTER = 10, NORTH, NORTHEAST, EAST, SOUTHEAST,
	   SOUTH, SOUTHWEST, WEST, NORTHWEST };
 
    int gridx, gridy, gridwidth, gridheight;
    float weightx, weighty;
    int anchor, fill;
    Insets insets;
    int ipadx, ipady;
 
    int tempX, tempY;
    int tempWidth, tempHeight;
    int minWidth, minHeight;

    GridBagConstraints();
 
    GridBagConstraints(const GridBagConstraints& c);

    void copy(const GridBagConstraints& c);

    GridBagConstraints& operator=(const GridBagConstraints& c);
};


class GridBagLayoutInfo
{
public:
 
    int width, height;		/* number of cells horizontally, vertically */
    int startx, starty;		/* starting point for layout */
    int* minWidth;		/* largest minWidth in each column */
    int* minHeight;		/* largest minHeight in each row */
    float* weightX;		/* largest weight in each column */
    float* weightY;		/* largest weight in each row */
 
    GridBagLayoutInfo()
    {
	int MAXGRIDSIZE=128;

	width=0;
	height=0;
	startx=0;
	starty=0;
	
	
	minWidth = new int[MAXGRIDSIZE];
	memset(minWidth,0,sizeof(int)*MAXGRIDSIZE);
	
	minHeight = new int[MAXGRIDSIZE];
	memset(minHeight,0,sizeof(int)*MAXGRIDSIZE);
	
	weightX = new float[MAXGRIDSIZE];
	memset(weightX,0,sizeof(int)*MAXGRIDSIZE);
	
	weightY = new float[MAXGRIDSIZE];
	memset(weightY,0,sizeof(int)*MAXGRIDSIZE);
    }


};

std::ostream& operator<<(std::ostream& out, GridBagLayoutInfo& i);



typedef pair<QWidget*,GridBagConstraints*> GridPair;
typedef QDictIterator<GridPair> GridIterator;

class GridBagLayout : QObject
{
    Q_OBJECT
public:
    
    /**
     * Creates a gridbag layout.
     */
    GridBagLayout();

    ~GridBagLayout();

    /**
     * Create a gridbag, and specify insets to surround the whole
     * layout area with (To specify the insets of the rectangle
     * surrounding all the widgets, from the edge of the container)
     * @param i The insets you want to use
     */
    GridBagLayout(Insets i);

    void setInsets(Insets i);
    
    void addLayoutComponent(char* , QWidget*);

    void removeLayoutComponent(QWidget* );
    
    /**
     * Sets the constraints for the specified component.
     * @param comp the component to be modified
     * @param constraints the constraints to be applied
     */
    void setConstraints(QWidget* comp, GridBagConstraints& constraints);

    /**
     * Retrieves the constraints for the specified component.  A copy of
     * the constraints is returned.
     * @param comp the component to be queried
     */
    GridBagConstraints getConstraints(QWidget* comp);

    
    /** 
     * Returns the preferred dimensions for this layout given the components
     * in the specified panel.
     * @param parent the component which needs to be laid out 
     * @see #minimumLayoutSize
     */
     void preferredLayoutSize(QWidget* parent,QSize* dim);
    
    /**
     * Returns the minimum dimensions needed to layout the components 
     * contained in the specified panel.
     * @param parent the component which needs to be laid out 
     * @see #preferredLayoutSize
     */
    void minimumLayoutSize(QWidget* parent,QSize* dim);
    
    /** 
     * Lays out the container in the specified panel.  
     * @param parent the specified component being laid out
     * @see Container
     */
    void layoutContainer(QWidget* parent);
    
//protected:
 
    const int MAXGRIDSIZE;
    const int MINSIZE;
    const int PREFERREDSIZE;

    QDict<GridPair > comptable;
 
    GridBagConstraints defaultConstraints;
    GridBagLayoutInfo* layoutInfo;

    Insets containerInsets;

    int* columnWidths;
    int columnWidthsLength;
    int* rowHeights;
    int rowHeightsLength;
    float* columnWeights;
    float* rowWeights;
    int columnWeightsLength;
    int rowWeightsLength;

    /**
     * Retrieves the constraints for the specified component.  The return
     * value is not a copy, but is the actual constraints class used by the
     * layout mechanism.
     * @param comp the component to be queried
     */
    GridBagConstraints* lookupConstraints(QWidget* comp);

    QPoint* getLayoutOrigin ();

    int** getLayoutDimensions ();

    float** getLayoutWeights ();

    void location(int x, int y,QPoint*);

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

    GridBagLayoutInfo* GetLayoutInfo(QWidget* parent, int sizeflag);
  
    /*
     * Adjusts the x, y, width, and height fields to the correct
     * values depending on the constraint geometry and pads.
     */
    void AdjustForGravity(GridBagConstraints* constraints,
			  Rectangle& r);

    /*
     * Figure out the minimum size of the
     * master based on the information from GetLayoutInfo()
     */
    QSize* GetMinSize(QWidget*, GridBagLayoutInfo* info);
    /*
     * Lay out the grid
     */
    void ArrangeGrid(QWidget* parent);

    GridBagLayoutInfo* getGridInfo(QWidget* parent);
};


#endif
