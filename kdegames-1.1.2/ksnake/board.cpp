#include <qrect.h>

#include "board.h"

#define BrickSize 16

Board::Board(int s)
  :QArray<int> (s)
{
  sz = s;
}

void Board::index(int i)
{
  row = i/BoardWidth;
  col = i-(row*BoardWidth);
}

bool Board::inBounds(int i)
{
  return ( i < 0 || i > sz-1 ? FALSE : TRUE);
}

void Board::set(int i, Square sq)
{
  if (inBounds(i))
    at(i) = sq;
}

QRect Board::rect(int i)
{
  index(i);
  return (QRect(col*BrickSize, row*BrickSize, BrickSize, BrickSize));
}

bool Board::isEmpty(int i)
{
  if (inBounds(i))
    return (at(i) == empty ? TRUE : FALSE);
  return TRUE;
}

bool Board::isBrick(int i)
{
  if (inBounds(i))
    return (at(i) == brick ? TRUE : FALSE);
  return FALSE;
}

bool Board::isApple(int i)
{
  if (inBounds(i))
    return (at(i) == Apple ? TRUE : FALSE);
  return FALSE;
}

bool Board::isHead(int i)
{
  if (inBounds(i))
    return (at(i) == head ? TRUE : FALSE);
  return FALSE;
}

bool Board::isSnake(int i)
{
  if (inBounds(i))
    return (at(i) == snake ? TRUE : FALSE);
  return FALSE;
}

int Board::getNext(int n, int i)
{
  index(i);

  switch(n)
    {
    case NW:
      return( i >= BoardWidth && col > 0 ? (i-BoardWidth)-1 : OUT);
    case N:
      return( i >= BoardWidth ? i-BoardWidth : OUT );
    case NE:
      return( i >= BoardWidth && col < BoardWidth-1 ? (i-BoardWidth)+1 : OUT);
    case W:
      return(col > 0 ? i-1 : OUT );
    case E:
      return(col < BoardWidth-1 ? i+1 : OUT );
    case SW:
      return( row < sz-BoardWidth && col > 0 ? (i+BoardWidth)-1 : OUT);
    case S:
      return( row < sz-BoardWidth ? i+BoardWidth : OUT );
    case SE:
      return( row < sz-BoardWidth && col < BoardWidth-1 ? (i+BoardWidth)+1 : OUT);
    default:
      return OUT;
    }
}
