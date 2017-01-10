/****************************************************************************
**
** Definition of GenericTetris, a generic class for implementing Tetris.
**
** Author  : Eirik Eng
** Created : 940126
** Modified : 961129 by Nicolas Hadacek
**
** Copyright (C) 1994 by Eirik Eng. All rights reserved.
**
**---------------------------------------------------------------------------
**
** The GenericTetris class is an abstract class that can be used to implement
** the well known game of Tetris. It is totally independent of any hardware
** platform or user interface mechanism. It has no notion of time, so you
** have to supply the timer ticks (or heartbeat) of the game. Apart from
** that this is a complete Tetris implementation.
**
** In the following it is assumed that the reader is familiar with the game
** of Tetris.
**
** The class operates on a grid of squares (referred to as the "board" below),
** where each of the different types of pieces in Tetris covers 4 squares. The
** width and height of the board can be specified in the constructor (default
** is 10x22). The coordinate (0,0) is at the TOP LEFT corner. The class
** assumes that it has total control over the board and uses this to optimize
** drawing of the board. If you need to update parts of the board
** (e.g. if you are using a window system), use the updateBoard() function.
**
** An implementation of the game must subclass from TetrisBoard and must
** implement these abstract functions:
**
**     virtual void drawSquare(int x,int y,int value)
**
**        This function is called when a square needs to be drawn
**        on the Tetris board. A value of 0 means that the square
**        should be erased. Values of 1 to 7 indicate the different
**        types of pieces. (Thus a minimal implementation can
**        draw 0 in one way and 1-7 in a second way). The x and y
**        values are coordinates on the Tetris board (see above).
**
**    virtual void gameOver()
**
**        This function is called when it is impossible to put a new
**        piece at the top of the board.
**
** To get a working minimal implementation of Tetris the following functions
** must be called from the user interface:
**
**     void startGame()
**
**         Clears the board and starts a new game.
**
**    void moveLeft()
**    void moveRight()
**    void rotateLeft()
**    void rotateRight()
**
**        The standard Tetris controls for moving and rotating the
**        falling pieces.
**
**    void dropDown();
**
**        Another Tetris control, drops the falling piece and calls the
**        virtual function pieceDropped(), whose default implementation is
**        to create a new block which appears at the top of the board.
**
**    void oneLineDown();
**
**        This is where you supply the timer ticks, or heartbeat, of the
**        game. This function moves the current falling piece one line down
**        on the board, or, if that cannot be done, calls the virtual
**        function pieceDropped() (see dropDown() above). The time between
**        each call to this function directly affects the difficulty of the
**        game. If you want to pause the game, simply block calls to the
**        user control functions above and stop calling this function (you
**        might want to call hideBoard() also).
**
** And that's it! There are several other public functions you can call
** and virtual functions you can overload to modify or extend the game.
**
** Do whatever you want with this code (i.e. the files gtetris.h, 
** gtetris.cpp, tpiece.h and tpiece.cpp). It is basically a weekend hack
** and it would bring joy to my heart if anyone in any way would find
** it useful.
**
** Nostalgia, comments and/or praise can be sent to: Eirik.Eng@troll.no
**
****************************************************************************/

#ifndef GTETRIS_H
#define GTETRIS_H

#include "tpiece.h"

class GenericTetris
{
 public:
    GenericTetris();

    virtual void clearBoard();
    void revealNextPiece(int revealIt);
    void updateBoard(int x1,int y1,int x2,int y2);
    void updateNext() { if (showNext) showNextPiece(); }
    void hideBoard();
    void showBoard();

    void moveLeft(int steps = 1);
    void moveRight(int steps = 1);
    void rotateLeft();
    void rotateRight();
    void dropDown();
    void oneLineDown();
    void newPiece();
    void removePiece();

    int  getScore()                           { return score; }
    int  getLevel()                           { return level; }
	int  noOfClearLines()                     { return nClearLines; }
	  
    int  boardHeight()                        { return Height; }
    int  boardWidth()                         { return Width; }
	

    virtual void drawSquare(int, int, int) = 0;
    virtual void gameOver() = 0;
	virtual void checkOpponentGift() = 0;

    virtual void startGame();
    virtual void drawNextSquare(int x,int y,int value);
    virtual void pieceDropped(int dropHeight);
    virtual void updateRemoved(int noOfLines);
    virtual void updateScore(int newScore);
    virtual void updateLevel(int newLevel);

 protected:
	void  draw(int x, int y, int value)
		{ drawSquare(x,Height - y,value); }
	int   canMoveTo(int xPosition, int line); /* Returns a boolean value. */
	void  showPiece();
	virtual void opponentGift(int nb_cases) = 0;
	virtual int  gameType() { return TETRIS_GAME; };
	
	TetrisPiece currentPiece;
	TetrisPiece nextPiece;
	static int const   Width=10;
	static int const   Height=22;
	int         board[Width][Height];
	int         nClearLines;
	int         currentLine;
	int         currentPos;
	
 private:
    void  erasePiece();
    void  showNextPiece(int erase = 0);
    void  eraseNextPiece(){showNextPiece(1);};
    int   canPosition(TetrisPiece &piece);    /* Returns a boolean value. */
	void  moveTo(int xPosition,int line);

    void  position(TetrisPiece &piece);
    void  optimizedMove(int newPos, int newLine,TetrisPiece &newPiece);
    
    int   showNext;                    /* Boolean variable. */
	int   score;
	int   level;
};

#endif
