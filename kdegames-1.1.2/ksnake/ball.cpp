#include "ball.h"
#include "board.h"
#include "pixServer.h"

int bounce[8][8]={
    { NE, NW, SE, SW, N, E, S, W },
    { SE, SW, NE, NW, S, E, N, W },
    { NW, NE, SW, SE, N, W, S, E },
    { SW, SE, NW, NE, S, W, N, E },
    { NE, NW, SE, SW, N, E, S, W },
    { SE, SW, NE, NW, S, E, N, W },
    { NW, NE, SW, SE, N, W, S, E },
    { SW, SE, NW, NE, S, W, N, E }
};

Ball::Ball(Board *b, PixServer *p)
{
    board = b;
    pixServer = p;

    int i = BoardWidth+1;
    while( !board->isEmpty(i) ) i++;
    hold = index = i;
    board->set(index, Balle);
    next = SE;
}

void Ball::zero()
{
    board->set(index, empty);
    pixServer->erase(index);
}

void Ball::nextMove()
{
    hold = index;
    board->set(hold, empty);

    for ( int x = 0; x < 8 ; x++) {
	int d = bounce[next][x];
	int nextSq = board->getNext(d, index);

	if (board->isHead(nextSq) || board->isEmpty(nextSq)) {
	    next = d;
	    index = nextSq;
	    board->set(index, Balle);
	    break;
	}
    }
}

void Ball::repaint()
{
    static int i = 0;
    static bool rotate = TRUE;

    pixServer->erase(hold);
    pixServer->draw(index, BallPix, i);

    if (rotate)
	if (++i > 3) i=0;

	rotate = !rotate;
}
