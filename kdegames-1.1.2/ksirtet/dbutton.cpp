#include <qpainter.h>


void drawTetrisButton( QPainter *p, int x, int y, int w, int h,
					   const QColor *color )
{
	QColor fc;
	if ( color ) {
		QPointArray a;
		a.setPoints( 3,  x,y+h-1, x,y, x+w-1,y );
		p->setPen( color->light() );
		p->drawPolyline( a );
		a.setPoints( 3, x+1,y+h-1, x+w-1,y+h-1, x+w-1,y+1 );
		p->setPen( color->dark() );
		p->drawPolyline( a );
		x++; y++;
		w -= 2; h -= 2;
		fc = *color;
	} else
		fc = p->backgroundColor();
	
	p->fillRect( x, y, w, h, fc );
}
