#include "ggboard.h"


GGBoard::GGBoard( QWidget *p, const char *name )
: Board(p, name)
{
	    fragmentation = 0;
}


void GGBoard::internalPieceDropped(int dropHeight)
{
	gluePiece();
	defragmentBoard();
	
	removeColoredAreas();
	
	updateScore(score);
	pieceDropped(dropHeight);
}


void GGBoard::gluePiece()
{
	int x, y, min;

	if (currentLine == -1)
		return;
	for(int i = 0 ; i < 4 ; i++) {
		currentPiece.getCoords(i,x,y);
		board[currentPos + x][currentLine - y] = currentPiece.getColor(i);
	}
	
	min = currentPiece.getMinY();
	if (currentLine - min >= Height - nClearLines)
		nClearLines = Height - currentLine + min - 1;
}


void GGBoard::defragmentBoard()
{
	int upper_empty_lines, i;
	bool empty_line;
	
	fragmentation = 0;
	
	upper_empty_lines = 0;
	for (int j=1; j<Height-nClearLines ; j++) {
		empty_line = 1;
		for (i=0; i<Width; i++) {
			if (board[i][j]!=0) {
				if (board[i][j-1]!=0) {
					/* this case cannot fall : there is one solid under */
					empty_line = 0;
				} else {
					if ( (j>=2) && (board[i][j-2]==0))
						fragmentation = 1;
					board[i][j-1] = board[i][j];
					draw(i, j-1, board[i][j-1]);
					board[i][j] = 0;
					draw(i, j, 0);
				}
			}
			if (empty_line)
				upper_empty_lines++;
			else
				upper_empty_lines = 0;
		}
	}
	
	nClearLines += upper_empty_lines;
}


void GGBoard::internalOneLineDown()
{
	if (fragmentation)
		defragmentBoard();
	else
		GenericTetris::oneLineDown();
}


void GGBoard::removeColoredAreas()
{
}
 
