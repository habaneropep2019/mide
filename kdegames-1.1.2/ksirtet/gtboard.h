#ifndef KTETRIS_GT_H
#define KTETRIS_GT_H 

#include "board.h"

#define NB_CASES_RANDOM_LINE 8

/* this class has the gestion for removing full lines :
 * it is used for "normal tetris" */

class GTBoard : public Board
{
 public:
	GTBoard( KAccel * parentkacc, QWidget *parent=0, const char *name=0);
	
	int  getLinesRemoved() 
		{ return nLinesRemoved; }
	
	void clearBoard();
	void startGame();
	void checkOpponentGift();
	
 private:
	int  nLinesRemoved;
	int  filled[Height]; /* how many columns are filled? */
	
	void gluePiece();
	void removeFullLines();
	void opponentGift(int nb_cases);
	void lightFullLines(bool on);
	void pieceDropped(int dropHeight);
	bool fullLines();
};

#endif
