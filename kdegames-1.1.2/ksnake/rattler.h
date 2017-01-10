#ifndef RATTLER_H
#define RATTLER_H

#include <qwidget.h>
#include <qlist.h>
#include <qbitarry.h>

#include "board.h"
#include "level.h"
#include "basket.h"
#include "ball.h"
#include "pixServer.h"
#include "snake.h"

enum { Init, Playing, Demo, Paused, Over };


class Rattler : public QWidget
{
    Q_OBJECT
public:
    Rattler ( QWidget *parent=0, const char *name=0 );

    void setBalls(int);
    void setCompuSnakes(int);
    void setSkill(int);
    void setRoom(int);

public slots:
    void closeGate(int);
    void openGate();

    void scoring(bool, int);

    void restart();
    void newTry();
    void levelUp();

    void pause();
    void restartTimer();

    void speedUp();

    void run();
    void demo();

    void killedComputerSnake();

    void reloadRoomPixmap();

    void initKeys();

private slots:
    void start();
    void stop();
    void showRoom();
    void restartDemo();

signals:
    void setPoints(int);
    void setTrys(int);

    void setScore(int);
    void togglePaused();

    // progress
    void rewind();
    void advance();

protected:
    void  timerEvent( QTimerEvent * );
    void  paintEvent( QPaintEvent * );
    void  keyPressEvent( QKeyEvent * );
    void  focusOutEvent( QFocusEvent * ) { ; }
    void  focusInEvent( QFocusEvent * )  { ; }
private:

    //    QBitArray *gameState;

    int  timerCount;
    bool leaving;

    int check;
    int points;
    int trys;

    int direction;

    Board     *board;
    PixServer *pix;
    Level     *level;
    Basket    *basket;
    SamySnake *samy;

    QList<Ball> *balls;
    void restartBalls(bool);
    int numBalls;

    QList<CompuSnake> *computerSnakes;
    void restartComputerSnakes(bool);
    int numSnakes;

    int room;
    int skill;

    int  gameTimer;
    bool timerHasRunOut;
    void start(int);
    void resetFlags();
    void init(bool);

    void score(int);
    void cleanLabel();

    uint UpKey;
    uint DownKey;
    uint RightKey;
    uint LeftKey;

};


#endif // RATTLER_H
