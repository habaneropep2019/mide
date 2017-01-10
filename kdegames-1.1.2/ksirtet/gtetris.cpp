#include "gtetris.h"
#include "defines.h"

GenericTetris::GenericTetris()
{
    for(int i = 0 ; i < Height ; i++)
        for(int j = 0 ; j < Width ; j++)
            board[j][i] = 0;

    currentLine       = -1;           // -1 if no falling piece.
    currentPos        = 0;
    showNext          = 0;            // FALSE
    score             = 0;
    level	          = 1;
	nClearLines       = Height;

	nextPiece.setRandomType(gameType());
}

void GenericTetris::clearBoard()
{
    int i,j;

    erasePiece();
    for(i = Height - nClearLines - 1 ; i >= 0 ; i--) {
        for(j = 0 ; j < Width ; j++)
            if (board[j][i] != 0) {
                draw(j,i,0);
                board[j][i] = 0;
            }
    }
	
    nClearLines = Height;
}

void GenericTetris::showBoard()
{
    int i,j;

    showPiece();
    for(i = Height - nClearLines - 1 ; i >= 0 ; i--)
        for(j = 0 ; j < Width ; j++)
		    if (board[j][i] != 0)
		        draw(j,i,board[j][i]);
}

void GenericTetris::hideBoard()
{
    int i,j;

    erasePiece();
    for(i = Height - nClearLines - 1 ; i >= 0 ; i--)
        for(j = 0 ; j < Width ; j++)
            if (board[j][i] != 0)
                draw(j,i,0);
}

void GenericTetris::startGame()
{
    clearBoard();
    updateScore(0);
    updateLevel(1);
    newPiece();
}

void GenericTetris::revealNextPiece(int revealIt)
{
    if (showNext == revealIt)
        return;
    showNext = revealIt;
    if (!showNext)
        eraseNextPiece();
    else
        showNextPiece();
}

void GenericTetris::updateBoard(int x1,int y1,int x2, int y2)
{
    int i,j;
    int tmp;

    if (x1 > x2) {
        tmp = x2;
        x2  = x1;
        x1  = tmp;
    }
    if (y1 > y2) {
        tmp = y2;
        y2  = y1;
        y1  = tmp;
    }
    for(i = y1 ; i <= y2 ; i++)
        for(j = x1 ; j <=  x2 ; j++)
           draw(j,Height - i - 1,board[j][Height - i - 1]);
    showPiece();        // Remember to update piece correctly!!!!
}

void GenericTetris::moveLeft(int steps)
{
    while(steps) {
        if (!canMoveTo(currentPos - 1,currentLine))
            return;
        moveTo(currentPos - 1,currentLine);
        steps--;
    }
}

void GenericTetris::moveRight(int steps)
{
    while(steps) {
        if (!canMoveTo(currentPos + 1,currentLine))
            return;
        moveTo(currentPos + 1,currentLine);
        steps--;
    }
}

void GenericTetris::rotateLeft()
{
    TetrisPiece tmp(currentPiece);

    tmp.rotateLeft();
    if (!canPosition(tmp))
        return;
    position(tmp);
    currentPiece = tmp;
}

void GenericTetris::rotateRight()
{
    TetrisPiece tmp(currentPiece);

    tmp.rotateRight();
    if (!canPosition(tmp))
        return;
    position(tmp);
    currentPiece = tmp;
}

void GenericTetris::dropDown()
{
    if (currentLine == -1)
        return;

    int dropHeight = 0;
    int newLine    = currentLine;
    while(newLine) {
        if (!canMoveTo(currentPos,newLine - 1))
            break;
        newLine--;
        dropHeight++;
    }
	
    if (dropHeight != 0)
        moveTo(currentPos,newLine);
    pieceDropped(dropHeight);
}

void GenericTetris::oneLineDown()
{
    if (currentLine == -1)
        return;
    if (canMoveTo(currentPos,currentLine - 1)) {
        moveTo(currentPos,currentLine - 1);
    } else
		pieceDropped(0);
}

void GenericTetris::newPiece()
{
    currentPiece = nextPiece;
    if (showNext)
        eraseNextPiece();
    nextPiece.setRandomType(gameType());
    if (showNext)
        showNextPiece();
    currentLine = Height - 1 + currentPiece.getMinY();
    currentPos  = Width/2 + 1;
    if (!canMoveTo(currentPos,currentLine)) {
	currentLine = -1;
        gameOver();
    } else {
        showPiece();
    }
}

void GenericTetris::removePiece()
{
    erasePiece();
    currentLine = -1;
}

void GenericTetris::drawNextSquare(int,int,int)
{

}

void GenericTetris::pieceDropped(int)
{
    newPiece();
}

void GenericTetris::updateRemoved(int) 
{
}

void GenericTetris::updateScore(int sc)
{
	score = sc;
}

void GenericTetris::updateLevel(int lev)
{
	level = lev;
}

void GenericTetris::showPiece()
{
    int x,y;

    if (currentLine == -1)
        return;

    for(int i = 0 ; i < 4 ; i++) {
        currentPiece.getCoords(i,x,y);
        draw(currentPos + x,currentLine - y,currentPiece.getColor(i));
    }
}

void GenericTetris::erasePiece()
{
    int x,y;

    if (currentLine == -1)
        return;

    for(int i = 0 ; i < 4 ; i++) {
        currentPiece.getCoords(i,x,y);
        draw(currentPos + x,currentLine - y,0);
    }
}

void GenericTetris::showNextPiece(int erase)
{
    int x,y;
    int minX = nextPiece.getMinX();
    int minY = nextPiece.getMinY();
    int maxX = nextPiece.getMaxX();
    int maxY = nextPiece.getMaxY();

    int xOffset = (3 - (maxX - minX))/2;
    int yOffset = (3 - (maxY - minY))/2;

    for(int i = 0 ; i < 4 ; i++) {
        nextPiece.getCoords(i,x,y);
	if (erase)
            drawNextSquare(x + xOffset - minX,
	                   y + yOffset - minY,0);
	else
            drawNextSquare(x + xOffset - minX,
	                   y + yOffset - minY,nextPiece.getColor(i));
    }
}

int GenericTetris::canPosition(TetrisPiece &piece)
{
    if (currentLine == -1)
        return 0;

    int x,y;

    for(int i = 0 ; i < 4 ; i++) {
        piece.getCoords(i,x,y);
        x = currentPos + x;
        y = currentLine - y; // Board and pieces have inverted y-coord. systems.
        if (x < 0 || x >= Width || y < 0 || y >= Height)
            return 0;     // Outside board, cannot put piece here.
        if (board[x][y] != 0)
            return 0;     // Over a non-zero square, cannot put piece here.
    }
    return 1;             // Inside board and no non-zero squares underneath.

}

int GenericTetris::canMoveTo(int xPosition,int line)
{
    if (currentLine == -1)
        return 0;

    int x,y;

    for(int i = 0 ; i < 4 ; i++) {
        currentPiece.getCoords(i,x,y);
        x = xPosition + x;
        y = line - y;     // Board and pieces have inverted y-coord. systems.
        if (x < 0 || x >= Width || y < 0 || y >= Height)
            return 0;     // Outside board, cannot put piece here.
        if (board[x][y] != 0)
            return 0;     // Over a non-zero square, cannot put piece here.
    }
    return 1;             // Inside board and no non-zero squares underneath.
}

void GenericTetris::moveTo(int xPosition,int line)
{
    if (currentLine == -1)
        return;
    optimizedMove(xPosition,line,currentPiece);
    currentPos  = xPosition;
    currentLine = line;
}

void GenericTetris::position(TetrisPiece &piece)
{
    if (currentLine == -1)
        return;

    optimizedMove(currentPos,currentLine,piece);
}

void GenericTetris::optimizedMove(int newPos, int newLine,
                                  TetrisPiece &newPiece)
{
    int updates [8][3];
    int nUpdates;
    int x, y, col;
    int i, j;

    for(i = 0 ; i < 4 ; i++) { /* Put the erasing coords into updates */
        currentPiece.getCoords(i,x,y);
		updates[i][0] = currentPos  + x;
		updates[i][1] = currentLine - y;
		updates[i][2] = currentPiece.getColor(i);
    }
	
	nUpdates = 4;
	
    for(i = 0 ; i < 4 ; i++) {
        newPiece.getCoords(i,x,y);
		x = newPos  + x;
		y = newLine - y;
		col = newPiece.getColor(i);
		
		for (j = 0 ; j < 4 ; j++)
			if ( updates[j][0] == x && updates[j][1] == y 
				 && updates[j][2] == col ) {
				/* Same coords & same color : don't have to erase */
				if (currentPiece.getType() == newPiece.getType())
					updates[j][2] = -1; /* Correct on screen, no update! */
				else
					updates[j][2] = newPiece.getColor(i);
				break;
			} else
			    updates[j][2] = 0;
		
		/* This coord does not overlap an erasing one */
		updates[nUpdates][0] = x;
		updates[nUpdates][1] = y;
		updates[nUpdates][2] = newPiece.getColor(i);
		nUpdates++;
    }
	
	/* do the updating */
	for (i = 0 ; i < nUpdates ; i++)
		if (updates[i][2] != -1) {
			draw(updates[i][0], updates[i][1], updates[i][2]);
		}
}
