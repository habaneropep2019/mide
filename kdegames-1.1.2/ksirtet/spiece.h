#ifndef SPIECE_H
#define SPIECE_H

#include <qframe.h>
#include <qpainter.h>

class ShowNextPiece : public QFrame
{
 Q_OBJECT
 friend class QTetris;
	
 public:
    ShowNextPiece( QWidget *parent=0, const char *name=0  );
	~ShowNextPiece();
 
 public slots:
    void drawNextSquare( int x, int y,QColor *color );

 signals:
    void update();
	
 private:
    void paintEvent( QPaintEvent * );
    void resizeEvent( QResizeEvent * );
    
	QPainter *paint;
    int  blockWidth, blockHeight;
    int  xOffset, yOffset;
};

#endif // SPIECE_H
