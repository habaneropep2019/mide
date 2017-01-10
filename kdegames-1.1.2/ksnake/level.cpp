#include <stdlib.h>

#include <qbitmap.h>
#include <qimage.h>
#include <qstring.h>

#include <kapp.h>

#include "level.h"
#include "board.h"
#include "bitmaps.h"
#include "levels.h"

const uchar *numbers[10] = { zero_bits, one_bits, two_bits,
			     three_bits, four_bits, five_bits,
			     six_bits, seven_bits, eight_bits, nine_bits
};

Level::Level(Board *b)
{
    board = b;
    create(Intro);
}

void Level::up()
{
    if (lev < leV->max())
	lev++;
}

void Level::create(Img img)
{
    switch(img){
    case Banner:
	createBanner();
	break;
    case Room:
	createRoom();
	break;
    case Intro:
	makeImageFromData(intro_bits);
	break;
    case GameOver:
	makeImageFromData(gameover_bits);
	break;
    }
}

void Level::createRoom()
{
    QImage image = leV->getImage(lev);
    initBoard(image);
}

void Level::makeImageFromData(const uchar *buf)
{
    QBitmap bitmap(35, 35, buf, TRUE);
    QImage image = bitmap.convertToImage();
    initBoard (image);
}

void Level::initBoard(QImage image)
{
    int index = 0;
    uchar *b;

    for ( int y = 0;y < image.height();y++ ) {
	b = image.scanLine(y);
	for ( int x = 0;x < image.width();x++ ) {

	    if ( image.bitOrder() == QImage::BigEndian ) {
		if (((*b >> (7 - (x & 7))) & 1) == 1)
		    board->set(index, brick);
		else board->set(index, empty);
	    } else {
		if (((*b >> (x & 7)) & 1) == 1)
		    board->set(index, brick);
		else board->set(index, empty);
	    }

	    if ( (x & 7) == 7 )
		b++;
	    index++;
	}
    }
}

void Level::createBanner()
{
    makeImageFromData(level_bits);

    QString num;
    num.sprintf("%d", lev);
    if(lev < 10) num.insert(0,'0');

    QString left = num.left(1);
    QString right = num.right(1);

    doNumber ( 606, numbers[left.toInt()] );
    doNumber ( 614, numbers[right.toInt()] );
}

void Level::doNumber(int beginAt, const uchar *buf)
{
    QBitmap bitmap(7, 9, buf, TRUE);
    QImage image = bitmap.convertToImage();

    int index = beginAt;
    uchar *b;

    for ( int y = 0;y < image.height();y++ ) {
	b = image.scanLine(y);
	for ( int x = 0;x < image.width();x++ ) {

	    if ( image.bitOrder() == QImage::BigEndian ) {
		if (((*b >> (7 - (x & 7))) & 1) == 1)
		    board->set(index, brick);
	    } else {
		if (((*b >> (x & 7)) & 1) == 1)
		    board->set(index, brick);
	    }

	    if ( (x & 7) == 7 )
		b++;
	    index++;
	}

	index += 28;
    }
}
