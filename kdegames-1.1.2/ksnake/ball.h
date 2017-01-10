#ifndef BALL_H
#define BALL_H

#include <qpixmap.h>
#include <qwidget.h>

#include "board.h"
#include "pixServer.h"

class Ball {
public:
    Ball(Board *b, PixServer *p);
    void nextMove();
    void repaint();
    void zero();
private:
    Board   *board;
    PixServer *pixServer;
    int  index;
    int  hold;
    int  next;
};

#endif // BALL_H
