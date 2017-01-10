#include "spiece.h"

#include "dbutton.h"

#include "spiece.moc"

ShowNextPiece::ShowNextPiece( QWidget *parent, const char *name )
: QFrame( parent, name )
{
	paint = new QPainter(this);
	setFrameStyle( QFrame::Panel | QFrame::Sunken );
	xOffset = -1;     /* -1 until first resizeEvent. */
}

ShowNextPiece::~ShowNextPiece()
{
	delete paint;
}

void ShowNextPiece::resizeEvent( QResizeEvent *e )
{
	QSize sz = e->size();
	blockWidth  = (sz.width()  - 3)/5;
	blockHeight = (sz.height() - 3)/6;
	xOffset     = (sz.width()  - 3)/5;
	yOffset     = (sz.height() - 3)/6;
}

void ShowNextPiece::paintEvent(QPaintEvent *)
{
	drawFrame( paint );
	emit update();
}

void ShowNextPiece::drawNextSquare(int x, int y, QColor *color)
{
	drawTetrisButton( paint, xOffset+x*blockWidth, yOffset+y*blockHeight,
					 blockWidth, blockHeight, color );
}
