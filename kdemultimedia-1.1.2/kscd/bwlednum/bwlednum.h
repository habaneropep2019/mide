/* 
 *
 *             BW_LED_Number a very very primitive LED
 * 
 * Copyright: Bernd Johannes Wuebben, wuebben@math.cornell.edu
 * 
 *
 * $Id$
 * 
 * $Log$
 * Revision 1.1  1997/06/21 23:10:11  wuebben
 * Initial revision
 *
 * Revision 1.2  1997/03/22 22:09:37  wuebben
 * Added support for visibe non-illuminated Segments
 *
 * Revision 1.1  1997/03/22 12:45:25  wuebben
 * Initial revision
 *
 *
 * 
 *
 */


#ifndef BW_LED_NUM_H
#define BW_LED_NUM_H

#include <qframe.h>
#include <qbitarry.h>


class BW_LED_Number : public QFrame	
{
    Q_OBJECT

public:

    BW_LED_Number( QWidget *parent=0, const char *name=0 );
   ~BW_LED_Number();

    void    setSmallLED(bool ); // if you LED is small it might look better
                                // if you call setSmallLED(TRUE)
   
            // this sets the fore and  background color of the LED
            // the forground defaults to yellow, the background defaults
            // to black 
   
    void    setLEDColor( const QColor& foregroundColor, const QColor& backgroundColor );
   
   
            // this sets the color of the segments that are not iluminated
            // the default is a rather dark red. 
   
    void    setLEDoffColor(QColor color);

	    // calling showOffColon(TRUE) will show the colon if not illuminated
	    // this is rather ugly -- the default is that they are not shown.

    void    showOffColon(bool off);

public slots:

            // display one of the characters " 0 1 2 3 4 5 6 7 8 9 . : - "
    void    display( char c );
  
            // display on e of the numbers   " 0 1 2 3 4 5 6 7 8 9"
    void    display( int i );

protected:

    void    resizeEvent( QResizeEvent * );
    void    drawContents( QPainter * );

private:

    bool    seg_contained_in( char c, char* seg);
    void    drawSegment( const QPoint &, char, QPainter &, int, bool = FALSE );
    void    drawSymbol( QPainter *p,char s ,bool repaint);

    char* old_segments;
    char* current_segments;

    char    current_symbol;
    char    old_symbol;
    QColor  offcolor;
    QColor  fgcolor;
    QColor  bgcolor;
    

    bool    smallLED;
    bool    show_off_colon;

private:	// Disabled copy constructor and operator=

    BW_LED_Number( const BW_LED_Number & ) {}
    BW_LED_Number &operator=( const BW_LED_Number & ) { return *this; }

};


#endif // BW_LED_NUM_H
