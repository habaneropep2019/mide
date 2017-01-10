/* Implementation of class BoardWidget
 * 
 * This class handles rendering of a Board to a KDE/QT widget,
 * shows moves (with a timer) and manages input of moves
 *
 * Josef Weidendorfer, 9/97
 */

#include <kapp.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qdatetm.h>

#include "Board.h"
#include "BoardWidget.h"

/* Cursors */
#include "bitmaps/Arrow1"
#include "bitmaps/Arrow1Mask"
#include "bitmaps/Arrow2"
#include "bitmaps/Arrow2Mask"
#include "bitmaps/Arrow3"
#include "bitmaps/Arrow3Mask"
#include "bitmaps/Arrow4"
#include "bitmaps/Arrow4Mask"
#include "bitmaps/Arrow5"
#include "bitmaps/Arrow5Mask"
#include "bitmaps/Arrow6"
#include "bitmaps/Arrow6Mask"

/* for debugging purposes... */
#if 0
#define PRINT(x) debug##x
#else
#define PRINT(x) 
#endif


BoardWidget::BoardWidget(Board& b, QWidget *parent, const char *name)
  : QWidget(parent, name), board(b)
{
  pList =0;
  gettingMove = false;

  /* setup cursors */
	
#define createCursor(bitmap,name) \
  static QBitmap bitmap(bitmap##_width, bitmap##_height,               \
                        (unsigned char *) bitmap##_bits, TRUE);        \
  static QBitmap bitmap##Mask(bitmap##Mask_width, bitmap##Mask_height, \
			(unsigned char *) bitmap##Mask_bits, TRUE);    \
  name = new QCursor(bitmap, bitmap##Mask, bitmap##_x_hot, bitmap##_y_hot);
	
  createCursor(Arrow1, arrow[1]);
  createCursor(Arrow2, arrow[2]);
  createCursor(Arrow3, arrow[3]);
  createCursor(Arrow4, arrow[4]);
  createCursor(Arrow5, arrow[5]);
  createCursor(Arrow6, arrow[6]);

  setCursor(crossCursor);

  //  boardColor   = new QColor("lightblue");
  boardColor   = new QColor(backgroundColor());
  redColor     = new QColor("red2");
  yellowColor  = new QColor("yellow2");
  redHColor    = new QColor("orange");
  yellowHColor = new QColor("green");
	
  copyPosition();
}

BoardWidget::~BoardWidget()
{
  for(int i=1; i<7; i++)
    if (arrow[i] != 0)
      delete arrow[i];
}

void BoardWidget::resizeEvent(QResizeEvent *)
{
  drawBoard();
}

void BoardWidget::paintEvent(QPaintEvent *)
{
  bitBlt(this, 0, 0, &pm);
}


void drawShadedHexagon(QPainter *p, int x, int y, int r, int lineWidth,
		       const QColorGroup& g, bool sunken)
{
  int dx=r/2, dy=(r*87)/100;
  int y1=y-dy, y2=y+dy;
  int i;

  QPen oldPen = p->pen();
    
  p->setPen(sunken ? g.midlight() : g.dark());

  for(i=0; i<lineWidth; i++) {
    p->drawLine( x-i+dx, y-dy, x+2*dx-i, y);
    p->drawLine( x+2*dx-i, y, x-i+dx, y+dy);
    p->drawLine( x-i+dx, y1+i, x+i-dx, y1+i);
  }

  p->setPen(sunken ? g.dark() : g.midlight());

  for(i=0; i<lineWidth; i++) {
    p->drawLine( x+i-dx, y-dy, x+i-2*dx, y);
    p->drawLine( x+i-2*dx, y, x+i-dx, y+dy);
    p->drawLine( x-i+dx, y2-i, x+i-dx, y2-i);
  }

  p->setPen(oldPen);
}


void drawColor(QPainter *p, int x, int y, int r, QColor* c)
{
  QColor w("white");
  QPalette pal(*c);

  QPen oldPen = p->pen();
  QBrush oldBrush = p->brush();

  p->setBrush(pal.normal().dark());
  p->setPen(pal.normal().dark());
  p->drawEllipse( x-r - 10,y-r +5, 2*r,2*r);

  p->setBrush(pal.normal().mid());
  p->setPen(pal.normal().mid());
  p->drawEllipse( x-r,y-r, 2*r,2*r);

  p->setBrush(pal.normal().light());
  p->setPen(pal.normal().light());
  p->drawEllipse( x-r/3, y-r/3, 4*r/3,4*r/3);

  p->setBrush(w);
  p->setPen(w);
  p->drawEllipse( x+r/3, y+r/3, r/3,r/3);

  p->setPen(oldPen);
  p->setBrush(oldBrush);
}  


void BoardWidget::drawBoard()
{
  QColorGroup g = QPalette( *boardColor ).normal();
  QColorGroup g2 = QWidget::colorGroup();

  int boardSize = width();
  if (boardSize > height()) boardSize = height();
  
  boardPM.resize(width(), height());
  boardPM.fill(backgroundColor());
  
  QPainter p;
  p.begin(&boardPM);
  p.setBrush(g.mid());
  
  QWMatrix m;
  QPoint cp = rect().center();
  m.translate(cp.x(), cp.y());
  m.scale(boardSize/1100.0, boardSize/1000.0);

  m.rotate(0);

  p.setWorldMatrix(m);

  /* draw field */

  int i,j;

  QPointArray a;
  int dx=520 /2, dy=(520 *87)/100;
  a.setPoints(6, -dx,-dy, dx,-dy, 2*dx,0, dx,dy, -dx,dy, -2*dx,0 );
  p.drawPolygon(a);

  drawShadedHexagon(&p, 0,0, 505, 1, g, false);
  drawShadedHexagon(&p, 0,0, 512, 3, g, true);
  drawShadedHexagon(&p, 0,0, 525, 5, g2, true);

#define xpos(i,j) (495*(2*(i)-(j))/9)
#define ypos(j)   (500*19*(j)/100)
	
  for(j=-4;j<5;j++)
    for(i= ((j>0)?j-4:-4) ; i< ((j<0)?5+j:5) ;i++) {
      int x=xpos(i,j);
      int y=ypos(j);
		  
      drawShadedHexagon(&p, x,y, 50, 2, g, true);
      drawShadedHexagon(&p, x,y, 30, 1, g, false);
    }
  p.end();

  draw();
}


void BoardWidget::draw()
{
  int boardSize = width();
  if (boardSize > height()) boardSize = height();
  
  pm = boardPM;
  
  QPainter p;
  p.begin(&pm);
  p.setBrush(foregroundColor());
  
  QWMatrix m;
  QPoint cp = rect().center();
  m.translate(cp.x(), cp.y());
  m.scale(boardSize/1100.0, boardSize/1000.0);

  m.rotate(0);

  p.setWorldMatrix(m);

  /* draw fields */

  int i,j;

  for(j=-4;j<5;j++)
    for(i= ((j>0)?j-4:-4) ; i< ((j<0)?5+j:5) ;i++) {
      int x=xpos(i,j);
      int y=ypos(j);
      int w=field[60+j*11+i];
		  
      if (w==Board::color1) 
	drawColor(&p, x,y, 35, redColor );
      else if (w==Board::color1bright) 
	drawColor(&p, x,y, 35, redHColor );
      else if (w==Board::color2) 
	drawColor(&p, x,y, 35, yellowColor );
      else if (w==Board::color2bright)
	drawColor(&p, x,y, 35, yellowHColor );
    }

  if (color1Count >0) {
    /* the outer marks of color1 */
    if (color1Count <12) {
      for(i=11; i>color1Count ;i--)
	drawColor(&p, xpos(12-i,7-i)+55, ypos(7-i), 35, redColor );
    }
    for(i=14; i>11 && i>color1Count ;i--)
      drawColor(&p, xpos(-6,10-i)+55, ypos(10-i), 35, redColor );
    
    /* the outer marks of color2 */
    if (color2Count <12) {
      for(i=11; i>color2Count ;i--)
	drawColor(&p, xpos(i-12,i-7)-55, ypos(i-7), 35, yellowColor);
    }
    for(i=14; i>11 && i>color2Count ;i--)
      drawColor(&p, xpos(6,i-10)-55, ypos(i-10), 35, yellowColor);
  }
  p.end();
  bitBlt(this, 0, 0, &pm);
}

void BoardWidget::copyPosition()
{
	for(int i=0; i<Board::AllFields;i++)
	  field[i] = board[i];
	color1Count = board.getColor1Count();
	color2Count = board.getColor2Count();
}

void BoardWidget::clearPosition()
{
  for(int i=0; i<Board::AllFields;i++)
    field[i] = 0;
  color1Count = color2Count = 0;
}

void BoardWidget::choseMove(MoveList *pl)
{ 
  if (!gettingMove && pl != 0) {
    pList = pl;
    gettingMove = true;
    mbDown = false;
    actValue = - board.calcValue();
    PRINT(( "Chose Move...\n" ));
  }
}


/* returns position of point (xx,yy)
 */
int BoardWidget::positionOf(int xx, int yy)
{
  int boardSize = QMIN( width(), height() );
  int x = (1000 * (xx- (width()-boardSize)/2)) / boardSize;
  int y = (1000 * (yy- (height()-boardSize)/2)) / boardSize;
  
  PRINT(( "(%d,%d) -> (%d,%d)\n", xx,yy, x,y ));

  y = (y-2)/47;
  if (y < 2 || y > 18) return 0;
  x= ((x+25)/25+ (y-10) )/2;
  if (y <  10 && ((x < 2) || (x > 8+y) )) return 0;
  if (y >= 10 && ((x < y-8) || (x > 18) )) return 0;

  return 22*y + x;
}


bool isBetweenFields(int pos)
{
  bool res = ( ((pos%2) == 1) || ( ((pos/22)%2) == 1) );
  PRINT(( "Pos %d is %s between fields\n", pos, res ? "":"not" ));
  return res;
}

int fieldOf(int pos)
{
  int f = 11*(pos/44) + (pos%22)/2;
  PRINT(( "Field1 of pos %d is %d\n",pos, f ));
  return f;
}

int field2Of(int pos)
{
  int y = pos/22, x = pos%22;
  int f2 = 0, f1 = 11*(y/2) + (x/2);

  if ( (y%2) == 0) {
    /* exact on row */
    if ( (x%2) != 0) {
      /* horizontial between fields */
      f2 = f1+1;
    }
  }
  else {
    /* vertical between fields */
    f2 = f1 + ( ((x%2)==0) ? 11:12 );
  }

  PRINT(( "Field2 of pos %d is %d\n", pos, f2 ));
  return f2;
}


/* get direction depending on difference of positions */
int directionOfPosDiff(int d)
{
  if (d>0) {
    return ((d<21) ? Move::Right : 
	    ((d%22) == 0) ? Move::LeftDown :
	    ((d%23) == 0) ? Move::RightDown : 0);
  }
  else if (d<0) {
    return ((d>-21) ? Move::Left : 
	    ((d%23) == 0) ? Move::LeftUp :
	    ((d%22) == 0) ? Move::RightUp : 0);
  }
  return 0;
}

int directionOfFieldDiff(int d)
{
  if (d>0) {
    return ((d<10) ? Move::Right : 
	    ((d%11) == 0) ? Move::LeftDown :
	    ((d%12) == 0) ? Move::RightDown : 0);
  }
  else if (d<0) {
    return ((d>-10) ? Move::Left : 
	    ((d%12) == 0) ? Move::LeftUp :
	    ((d%11) == 0) ? Move::RightUp : 0);
  }
  return 0;
}

/* Check if <pos> is a valid start position for a allowed move
 * If yes, set 
 *   <startField>, <actMove> and <startType>
 */
bool BoardWidget::isValidStart(int pos, bool midPressed)
{
  bool res = false;
  int f1 = fieldOf(pos);

  startField = f1;

  if (isBetweenFields(pos)) {
    int f2 = field2Of(pos);

    actMove = Move(f1, directionOfFieldDiff( f2-f1 ), Move::none);
    res = pList->isElement(actMove, MoveList::start2);
    if (!res) {
      startField = f2;
      actMove = Move(f2, directionOfFieldDiff( f1-f2 ), Move::none);
      res = pList->isElement(actMove, MoveList::start2);
    }
    startType = MoveList::start2;
    return res;
  }

  if (midPressed) {
    startType = MoveList::start3;

    /* Check all 6 directions */
    for(int dir=1;dir<7;dir++) {
      actMove = Move(f1 - Board::fieldDiffOfDir(dir), dir, Move::none );
      if (pList->isElement(actMove, startType))
	return true;
    }
    /* if we don't find a side move3 fall trough to normal moves... */
  }

  startType = MoveList::start1;
  actMove = Move(f1, 0, Move::none);
  res = pList->isElement(actMove, startType);

  return res;
}


/* Check if <pos> is a valid end position for a move
 * regarding <startPos>
 * If yes, set <actMove> 
 */
bool BoardWidget::isValidEnd(int pos)
{
  int dir = directionOfPosDiff(pos - startPos);
  Move m;

  if (dir == 0) return false;

  switch(startType) {
  case MoveList::start1:
    m = Move(startField, dir, Move::none);
    if (!pList->isElement(m, startType))
      return false;
    break;

  case MoveList::start2:
    {
      int f1 = fieldOf(startPos);
      int f2 = field2Of(startPos);
      int dir2 = directionOfFieldDiff( f2-f1 );
      int dir3 = directionOfFieldDiff( f1-f2 );

      switch((dir2-dir+6)%6) {
      case 1:
	m = Move(f1, dir2, Move::left2);
	break;
      case 2:
	m = Move(f2, dir3, Move::right2);
	break;
      case 4:
	m = Move(f2, dir3, Move::left2);
	break;
      case 5:
	m = Move(f1, dir2, Move::right2);
	break;
      default:
	return false;
      }
      if (!pList->isElement(m, startType))
	return false;

      break;
    }
  case MoveList::start3:
    {
      int rightDir = (dir%6)+1;
      m = Move( startField - Board::fieldDiffOfDir(rightDir), rightDir, Move::left3 );
      if (!pList->isElement(m, startType)) {
	int leftDir = ((dir-2)%6)+1;
	m = Move( startField - Board::fieldDiffOfDir(leftDir), leftDir, Move::right3 );
	if (!pList->isElement(m, startType))
	  return false;
      }
    }
    break;
  }

  actMove = m;
  shownDirection = dir;
  return true;
}



void BoardWidget::mousePressEvent( QMouseEvent* pEvent )
{
  if (!gettingMove) {
    emit mousePressed();
    return;
  }
  mbDown = true;

  int pos = positionOf( pEvent->x(), pEvent->y() );
  startValid = isValidStart(pos, (pEvent->button() == MidButton));
  PRINT(( "Start pos %d is %svalid.\n", pos, startValid ? "":"in" ));
  //  actMove.print();

  if (!startValid) return;
  startPos = oldPos = pos;

  board.showStart(actMove,1);
  startShown = true;
  copyPosition();
  draw();

  QString tmp;
  tmp.sprintf("%s: %d", klocale->translate("Board value"), actValue);
  emit updateSpy(tmp);
}


void BoardWidget::mouseMoveEvent( QMouseEvent* pEvent )
{
  if (!gettingMove || !mbDown) return;

  int pos = positionOf( pEvent->x(), pEvent->y() );
  if (pos == oldPos) return;
  oldPos = pos;

  if (!startValid) {
    /* We haven't a valid move yet. Check if we are over a valid start */

    startValid = isValidStart(pos, (pEvent->button() == MidButton));
    PRINT(( "Start pos %d is %svalid.\n", pos, startValid ? "":"in" ));
    //    actMove.print();

    if (!startValid) return;
    startPos = oldPos = pos;

    board.showStart(actMove,1);
    startShown = true;
    copyPosition();
    draw();

    QString tmp;
    tmp.sprintf("%s: %d", klocale->translate("Board value"), actValue);
    emit updateSpy(tmp);
    return;
  }

  /* restore board */
  if (startShown)
    board.showStart(actMove,0);
  else
    board.showMove(actMove,0);
  startShown = false;

  if (isValidEnd(pos)) {
    //    actMove.print();

    board.showMove(actMove,0);
    board.playMove(actMove);
    int v = board.calcValue();
    board.takeBack();

    QString tmp;
    tmp.sprintf("%s : %+d",
		(const char*) actMove.name(), v-actValue);
    emit updateSpy(tmp);

    board.showMove(actMove,3);
    setCursor(*arrow[shownDirection]);
  }
  else {
    QString tmp;

    setCursor(crossCursor);
    if (pos == startPos) {
	board.showStart(actMove,1);
	startShown = true;
	tmp.sprintf("%s: %d", klocale->translate("Board value"), actValue);
    }	
    emit updateSpy(tmp);

  }
  copyPosition();
  draw();
}


void BoardWidget::mouseReleaseEvent( QMouseEvent* pEvent )
{
  if (!gettingMove) return;
  mbDown = false;
  if (!startValid) return;

  int pos = positionOf( pEvent->x(), pEvent->y() );
  if (isValidEnd(pos)) {
    //    actMove.print();
    board.showMove(actMove,0);
    startValid = false;
    setCursor(crossCursor);
    gettingMove = false;
    emit moveChoosen(actMove);
    return;
  }

  board.showStart(actMove,0);
  copyPosition();
  draw();
  startValid = false;
  setCursor(crossCursor);

  QString tmp;
  emit updateSpy(tmp);
}

#include "BoardWidget.moc"









