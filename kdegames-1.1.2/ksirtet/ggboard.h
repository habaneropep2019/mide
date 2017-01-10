#ifndef KTETRIS_GG_H
#define KTETRIS_GG_H 

#include "board.h"

/* this class has the gestion of removing group of same-colored groups
 * it is used for the "puyo puyo" game 
 * not completely implemented */

class GGBoard : public Board
{
 public:
	GGBoard( QWidget *parent=0, const char *name=0 );
	
 protected:
	void internalOneLineDown();
	void internalPieceDropped(int dropHeight);
	int gameType() { return PUYOPYUO_GAME; };
	
 private:
	void defragmentBoard();
	void gluePiece();
	void removeColoredAreas();
	
	int fragmentation;
};

#endif
