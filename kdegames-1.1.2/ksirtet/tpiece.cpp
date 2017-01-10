#include "tpiece.h"
#include "defines.h"

#include <stdlib.h>

static int TTYPES[NB_TYPES][NB_CASES][2] =
{
		  { { 0,-1},
		    { 0, 0},
			{-1, 0},
			{-1, 1} },
				
		  { { 0,-1},
			{ 0, 0},
			{ 1, 0},
			{ 1, 1} },

		  { { 0,-1},
			{ 0, 0},
			{ 0, 1},
			{ 0, 2} },
				
		  { {-1, 0},
			{ 0, 0},
			{ 1, 0},
			{ 0, 1} },

		  { { 0, 0},
			{ 1, 0},
			{ 0, 1},
			{ 1, 1} },

		  { {-1,-1},
			{ 0,-1},
			{ 0, 0},
			{ 0, 1} },

		  { { 1,-1},
			{ 0,-1},
			{ 0, 0},
			{ 0, 1} }
};

void TetrisPiece::rotateLeft()
{
    int tmp;
    for (int i=0; i<NB_CASES ; i++) {
        tmp = getXCoord(i);
        setXCoord(i, getYCoord(i));
        setYCoord(i, -tmp);
    }
}

void TetrisPiece::rotateRight()
{
    int tmp;
    for (int i=0; i<NB_CASES; i++) {
        tmp = getXCoord(i);
        setXCoord(i, -getYCoord(i));
        setYCoord(i, tmp);
    }
}

int TetrisPiece::getMinX()
{
    int tmp = getXCoord(0);
    for(int i=1; i<NB_CASES; i++)
        if ( tmp>getXCoord(i) )
		    tmp = getXCoord(i);
    return tmp;
}

int TetrisPiece::getMaxX()
{
    int tmp = getXCoord(0);
    for(int i=1; i<NB_CASES; i++)
        if ( tmp<getXCoord(i) )
            tmp = getXCoord(i);
    return tmp;
}

int TetrisPiece::getMinY()
{
    int tmp = getYCoord(0);
    for(int i=1; i<NB_CASES; i++)
        if ( tmp>getYCoord(i) )
            tmp = getYCoord(i);
    return tmp;
}

int TetrisPiece::getMaxY()
{
    int tmp = getYCoord(0);
    for(int i=1; i<NB_CASES; i++)
        if ( tmp<getYCoord(i) )
            tmp = getYCoord(i);
    return tmp;
}

void TetrisPiece::initialize(int ptype, int gameType)
{
    if ( ptype<1 || ptype>NB_TYPES )
        ptype = 1;
	
    type = ptype;
	
	if ( gameType==TETRIS_GAME )
		for(int i=0; i<NB_CASES; i++) {
			setCoords(i, TTYPES[type-1][i][0], TTYPES[type-1][i][1]);
			setColor(i, type);
		}
	else if ( gameType==PUYOPUYO_GAME )
		for(int i=0; i<NB_CASES; i++) {
			setCoords(i, TTYPES[type-1][i][0], TTYPES[type-1][i][1]);
			setColor(i, (random() % 5)+1);
		}
}

int TetrisPiece::randomValue(int maxPlusOne)
{
  //return (int) ((double) maxPlusOne*rand()/(RAND_MAX+1.0));
  // modified by David Faure <faure@kde.org> : don't depend on rand and RAND_MAX
  return random() % maxPlusOne;
}
