#ifndef SAMY_H
#define SAMY_H

#include <config.h>
#include <qobject.h>
#include <qwidget.h>

#include <qlist.h>
#include <qpixmap.h>


#include "board.h"
#include "pixServer.h"

class Snake : public QObject
{
    Q_OBJECT
public:
    Snake(Board *b, PixServer *p, Gate g, PixMap x);
    Snake::~Snake() {}
    void repaint( bool );
    void zero();
signals:
    void score(bool, int);
    void killed();
    void closeGate(int);
    void restartTimer();
    void goingOut();
protected:
    Board   *board;
    PixServer *pixServer;
    Gate  gate;
    PixMap pixmap;

    struct Samy {
	int direction;
	SnakePix pixmap;
	int index;
    };

    QList<Samy> list;

    void reset(int index, int border);
    void appendSamy();
    void updateSamy();
    int  tail()        { return (list.count() -1 ); }
    bool growing()     { return (grow > 0 ? TRUE : FALSE); }

    int   hold;
    int   grow;
};

class CompuSnake : public Snake
{
public:
    CompuSnake(Board *b, PixServer *p);
    void nextMove();
private:
    bool init();
    void removeSamy();
    bool findEmpty(int i, int it);
    bool permission();
    void out();
};

enum samyState { ok, ko, out };

class SamySnake : public Snake
{
public:
    SamySnake(Board *, PixServer *);
    samyState nextMove(int direction);
    void init();
};

#endif // SAMY_H
