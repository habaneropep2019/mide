#ifndef _BOARDWIDGET_H_
#define _BOARDWIDGET_H_

#include "Move.h"
#include "Board.h"
#include <qwidget.h>

class BoardWidget : public QWidget
{
	Q_OBJECT
	
 public:
	BoardWidget(Board&, QWidget *parent = 0, const char *name = 0);
	~BoardWidget();

	virtual void resizeEvent(QResizeEvent *);
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent( QMouseEvent* pEvent );
	virtual void mouseReleaseEvent( QMouseEvent* pEvent );
	virtual void mouseMoveEvent( QMouseEvent* pEvent );

	void draw();
	void drawBoard();
	
	void choseMove(MoveList*);
	
	/* copy actual board position */
	void copyPosition();
	void clearPosition();

 signals:
	void moveChoosen(Move&);
	void mousePressed();
	void updateSpy(QString);
	
 private:
	int positionOf(int x, int y);
	bool isValidStart(int pos, bool);
	bool isValidEnd(int pos);

	QPixmap pm, boardPM;
	Board& board;
	int actValue;

	/* copied position */
	int field[Board::AllFields];
	int color1Count, color2Count;	
	
	/* for getting user Move */
	MoveList *pList;
	Move actMove;
	bool gettingMove, mbDown, startValid, startShown;
	int startPos, actPos, oldPos, shownDirection;
	int startField, startField2, actField, oldField, startType;
	QColor *boardColor, *redColor, *yellowColor, *redHColor, *yellowHColor;
	QCursor *arrowAll, *arrow[7];	
};

#endif /* _BOARDWIDGET_H_ */

