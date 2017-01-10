#ifndef BOARD_H
#define BOARD_H

#include <qarray.h>
#include <qrect.h>

enum Square {empty, brick, Apple, Balle, snake, head};

#define N  0
#define S  1
#define E  2
#define W  3
#define NE 4
#define SE 5
#define NW 6
#define SW 7

#define BoardWidth 35
#define OUT -1

//enum Gate {NORTH_GATE = BoardWidth/2,
//SOUTH_GATE =(BoardWidth*(BoardWidth-1))+(BoardWidth/2)};


typedef int Gate;
const int NORTH_GATE = BoardWidth/2;
const int SOUTH_GATE =(BoardWidth*BoardWidth)-(NORTH_GATE+1);

class Board : public QArray<int>
{
public:
  Board  (int s);
  ~Board() {};
  QRect  rect(int i);

  void   set(int i, Square sq);

  bool   isBrick(int i);
  bool   isEmpty(int i);
  bool   isApple(int i);
  bool   isHead(int i);
  bool   isSnake(int i);

  int    getNext(int, int);

private:
  void   index(int i);
  bool   inBounds(int i);
  int    row;
  int    col;
  int    sz;
};


#endif // BOARD_H
