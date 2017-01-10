#ifndef LEVEL_H
#define LEVEL_H

#include <qimage.h>


#include "board.h"

enum Img {Banner, Room, Intro, GameOver};


class Level {
public:
    Level  (Board *);
    void    up();
    void    set(int l) { lev = l; }
    int     get() { return lev; }
    void    create(Img);

private:
    Board  *board;
    QImage  makeImage(char *);
    void    makeImageFromData(const uchar *buf);
    void    doNumber(int beginAt, const uchar *buf);
    void    initBoard(QImage );
    void    createRoom();
    void    createBanner();

    int     lev;
};


#endif // LEVEL_H
