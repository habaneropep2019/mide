#ifndef PIXSERVER_H
#define PIXSERVER_H

#include <qpixmap.h>
#include <qcolor.h>

#include "board.h"

enum SnakePix {TailUp, TailDown, TailRight, TailLeft,
	       HeadUp, HeadDown, HeadRight, HeadLeft,
	       AngleSw, AngleSe, AngleNw, AngleNe,
	       BodyHz, BodyVt,
	       HtailUp, HtailDown, HtailRight, HtailLeft };

enum PixMap { SamyPix, CompuSnakePix, ApplePix, BallPix };

enum Image {Snake, ComputerSnake};

class PixServer
{
public:
    PixServer (Board *, QWidget *parent=0);
    QPixmap levelPix() { return roomPix; }

    void initRoomPixmap();
    void initBrickPixmap();
    void initPixmaps();
    void initbackPixmaps();

    void draw(int pos, PixMap pix, int i = 0);
    void erase(int pos);
    void restore(int pos);
private:
    QWidget *w;
    Board *board;
    QString pixDir;

    void drawBrick(QPainter *, int);

    QPixmap samyPix[18];
    QPixmap compuSnakePix[18];
    QPixmap ballPix[4];
    QPixmap applePix[2];

    QPixmap roomPix;
    QPixmap offPix;
    QPixmap backPix;

    bool plainColor;
    QColor backgroundColor;

};

#endif // PIXSERVER_H
