/* Class Board
 * 
 *   with methods for
 *    - play/take back moves
 *    - generate allowed moves
 *    - calculate rating for position
 *    - search for best move 
 * 
 * Josef Weidendorfer, 28.8.97
*/

#include <qdatetm.h>
#include <qstrlist.h>
#include <kapp.h>
#include "Board.h"

// #define MYTRACE 1

#if 0
#define CHECK(b)  ASSERT(b)
#else
#define CHECK(b)
#endif


/*********************** Class MainCombination *************************/

void MainCombination::clear(int d)
{
	int i,j;
	
	for(i=0;i<maxDepth;i++)
	  for(j=0;j<maxDepth;j++) {
		  move[i][j].type = Move::none;
	  }	
	actMaxDepth = (d<maxDepth) ? d:maxDepth-1;
}

void MainCombination::update(int d, Move& m)
{
	int i;

	if (d>actMaxDepth) return;
	for(i=d+1;i<=actMaxDepth;i++) {
		move[d][i]=move[d+1][i];
		move[d+1][i].type = Move::none;
	}
	move[d][d]=m;
}



/****************************** Class Board ****************************/


/* Board for start of a game */
int Board::startBoard[]={
                       10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	             10,  1,  1,  1,  1,  1, 10, 10, 10, 10, 10,
	           10,  1,  1,  1,  1,  1,  1, 10, 10, 10, 10,
                 10,  0,  0,  1,  1,  1,  0,  0, 10, 10, 10,
	       10,  0,  0,  0,  0,  0,  0,  0,  0, 10, 10,
             10,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,
           10, 10,  0,  0,  0,  0,  0,  0,  0,  0, 10,
         10, 10, 10,  0,  0,  2,  2,  2,  0,  0, 10,
       10, 10, 10, 10,  2,  2,  2,  2,  2,  2, 10,
     10, 10, 10, 10, 10,  2,  2,  2,  2,  2, 10,
   10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10          };


/* first centrum of board, then rings around (numbers are indexes) */
int Board::order[]={
          60,
	  61,72,71,59,48,49,
	  62,73,84,83,82,70,58,47,36,37,38,50,
	  63,74,85,96,95,94,93,81,69,57,46,35,24,25,26,27,39,51,
	  64,75,86,97,108,107,106,105,104,92,80,68,56,45,34,23,12,
	  13,14,15,16,28,40,52 };

/* ratings for fields are calculated out of these values
 * (see setFieldValues)
 */
int Board::ringValue[] = { 45, 35, 25, 10, 0 };
int Board::ringDiff[]  = {  0, 10, 10,  8, 5 };

int Board::fieldValue[61];

/* Value added to board rating according to the difference of 
 * stones in game of player1 and player2 */
int Board::stoneValue[]= { 0,-800,-1800,-3000,-4400,-6000 };

int Board::direction[]= { -11,1,12,11,-1,-12,-11,1 };

/* Values for possible moves: 
 * (move2, move3, push1, push2, out) */
int Board::moveValue[]= {
  1,3,5,7,30 };

Board::Board()
{
	clear();
	breakOut = bUpdateSpy = false;
	boardOk = true;
	debug = 0;
	realMaxDepth = 1;
	setFieldValues();
}

void Board::setFieldValues()
{
  int i, j = 0, k = 57;

  fieldValue[0] = ringValue[0];
  for(i=1;i<7;i++) 
    fieldValue[i] = ringValue[1] + ((j+=k) % ringDiff[1]);
  for(i=7;i<19;i++)
    fieldValue[i] = ringValue[2] + ((j+=k) % ringDiff[2]);
  for(i=19;i<37;i++)
    fieldValue[i] = ringValue[3] + ((j+=k) % ringDiff[3]);
  for(i=37;i<61;i++)
    fieldValue[i] = ringValue[4] + ((j+=k) % ringDiff[4]);
}

void Board::readRating(KConfig *config)
{
  QStrList list;
  QString tmp;

  if (config->readListEntry("StoneValues", list)) {
    stoneValue[0] = 0;
    for(int i=1;i<6;i++)
      stoneValue[i] = (tmp = list.at(i-1)).toInt();
  }

  if (config->readListEntry("MoveValues", list)) {
    for(int i=0;i<5;i++)
      moveValue[i] = (tmp = list.at(i)).toInt();
  }

  if (config->readListEntry("RingValues", list)) {
    for(int i=0;i<5;i++)
      ringValue[i] = (tmp = list.at(i)).toInt();
  }

  if (config->readListEntry("RingDiffs", list)) {
    for(int i=0;i<5;i++)
      ringDiff[i] = (tmp = list.at(i)).toInt();
  }

  setFieldValues();
}

void Board::saveRating(KConfig *config)
{
  QString entry;

  entry.sprintf("%d,%d,%d,%d,%d", stoneValue[1], stoneValue[2],
		stoneValue[3], stoneValue[4], stoneValue[5]);
  config->writeEntry("StoneValues", entry);

  entry.sprintf("%d,%d,%d,%d,%d", moveValue[0], moveValue[1],
		moveValue[2], moveValue[3], moveValue[4]);
  config->writeEntry("MoveValues", entry);

  entry.sprintf("%d,%d,%d,%d,%d", ringValue[0], ringValue[1],
		ringValue[2], ringValue[3], ringValue[4]);
  config->writeEntry("RingValues", entry);

  entry.sprintf("%d,%d,%d,%d,%d", ringDiff[0], ringDiff[1],
		ringDiff[2], ringDiff[3], ringDiff[4]);
  config->writeEntry("RingDiffs", entry);
}



void Board::begin(int startColor)
{
	int i;
	
	for(i=0;i<AllFields;i++)
	  field[i] = startBoard[i];
	storedFirst = storedLast = 0;
	color = startColor;
	color1Count = color2Count = 14;
	boardOk = true;
}

void Board::clear()
{
	int i;
	
	for(i=0;i<AllFields;i++)
	  field[i] = (startBoard[i] == out) ? out: free;
	storedFirst = storedLast = 0;
	color1Count = color2Count = 0;
	boardOk = true;
}

/* generate moves starting at field <startField> */
inline void Board::generateFieldMoves(int startField, MoveList& list)
{
	int d, dir, c, actField, left, right;
	int opponent = (color == color1) ? color2 : color1;

	ASSERT( field[startField] == color );
	
	/* 6 directions	*/
	for(d=1;d<7;d++) {
		dir = direction[d];
		  
		/* 2nd field */
		c = field[actField = startField+dir];
		if (c == free) {			
			/* (c .) */
			list.insert(startField, d, Move::move1);
			continue;
		}		
		if (c != color) 
		  continue;
		
		/* 2nd == color */
		
		left = (field[startField+direction[d-1]] == free);
		if (left) {
			left = (field[actField+direction[d-1]] == free);
			if (left) 
			  /* 2 left */
			  list.insert(startField, d, Move::left2);
		}
		
		right = (field[startField+direction[d+1]] == free);
		if (right) {
			right = (field[actField+direction[d+1]] == free);
			if (right) 
			  /* 2 right */
			  list.insert(startField, d, Move::right2);
		}
		
		/* 3rd field */
		c = field[actField += dir];
		if (c == free) {
			/* (c c .) */
			list.insert(startField, d, Move::move2);
			continue;
		}		
		else if (c == opponent) {
			
			/* 4th field */
			c = field[actField += dir];
			if (c == free) {
				/* (c c o .) */
				list.insert(startField, d, Move::push1with2);
			}
			else if (c == out) {
				/* (c c o |) */
				list.insert(startField, d, Move::out1with2);
			}
			continue;
		}
		if (c != color)
		  continue;
		
		/* 3nd == color */

		if (left) {
			if (field[actField+direction[d-1]] == free) 
			  /* 3 left */
			  list.insert(startField, d, Move::left3);
		}

		if (right) {
			if (field[actField+direction[d+1]] == free)
			  /* 3 right */
			  list.insert(startField, d, Move::right3);
		}
		
		/* 4th field */
		c = field[actField += dir];
		if (c == free) {
			/* (c c c .) */
			list.insert(startField, d, Move::move3);
			continue;
		}
		if (c != opponent)
		  continue;

		/* 4nd == opponent */
		
		/* 5. field */
		c = field[actField += dir];
		if (c == free) {
			/* (c c c o .) */
			list.insert(startField, d, Move::push1with3);
			continue;
		}
		else if (c == out) {
			/* (c c c o |) */
			list.insert(startField, d, Move::out1with3);
			continue;
		}
		if (c != opponent)
		  continue;

		/* 5nd == opponent */

		/* 6. field */
		c = field[actField += dir];
		if (c == free) {
			/* (c c c o o .) */
			list.insert(startField, d, Move::push2);
		}
		else if (c == out) {
			/* (c c c o o |) */
			list.insert(startField, d, Move::out2);
		}
	}
}


void Board::generateMoves(MoveList& list)
{
	int actField, f;

	ASSERT( boardOk );

	for(f=0;f<RealFields;f++) {
		actField = order[f];
		if ( field[actField] == color)
		   generateFieldMoves(actField, list);
	}
}



void Board::playMove(const Move& m)
{
	int f, dir, dir2;
	int opponent = (color == color1) ? color2:color1;

	CHECK( boardOk );
	CHECK( isConsistent() );
	
	if (++storedLast == MvsStored) storedLast = 0;
	
	/* Buffer full -> delete oldest entry */
	if (storedLast == storedFirst)
	  	if (++storedFirst == MvsStored) storedFirst = 0;
	
	storedMove[storedLast] = m;

	f = m.field;
	CHECK( (m.type >= 0) && (m.type < Move::none));
	CHECK( field[f] == color );
	field[f] = free;
	dir = direction[m.direction];
	
	switch(m.type) {
	 case Move::out2:        /* (c c c o o |) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == opponent );
		CHECK( field[f + 4*dir] == opponent );
		CHECK( field[f + 5*dir] == out );
		field[f + 3*dir] = color;
		break;		
	 case Move::out1with3:   /* (c c c o |)   */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == opponent );
		CHECK( field[f + 4*dir] == out );
		field[f + 3*dir] = color;
		break;
	 case Move::move3:       /* (c c c .)     */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == free );
		field[f + 3*dir] = color;
		break;
	 case Move::out1with2:   /* (c c o |)     */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == opponent );
		CHECK( field[f + 3*dir] == out );
		field[f + 2*dir] = color;
		break;
	 case Move::move2:       /* (c c .)       */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == free );
		field[f + 2*dir] = color;
		break;
	 case Move::push2:       /* (c c c o o .) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == opponent );
		CHECK( field[f + 4*dir] == opponent );
		CHECK( field[f + 5*dir] == free );
		field[f + 3*dir] = color;
		field[f + 5*dir] = opponent;
		break;
	 case Move::left3:		
		dir2 = direction[m.direction-1];
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + dir2] == free );
		CHECK( field[f + dir+dir2] == free );
		CHECK( field[f + 2*dir+dir2] == free );
		field[f+dir2] = color;
		field[f+=dir] = free;
		field[f+dir2] = color;
		field[f+=dir] = free;
		field[f+dir2] = color;
		break;
	 case Move::right3:
		dir2 = direction[m.direction+1];
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + dir2] == free );
		CHECK( field[f + dir+dir2] == free );
		CHECK( field[f + 2*dir+dir2] == free );
		field[f+dir2] = color;
		field[f+=dir] = free;
		field[f+dir2] = color;
		field[f+=dir] = free;
		field[f+dir2] = color;
		break;
	 case Move::push1with3:   /* (c c c o .) => (. c c c o) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == opponent );
		CHECK( field[f + 4*dir] == free );
		field[f + 3*dir] = color;
		field[f + 4*dir] = opponent;
		break;
	 case Move::push1with2:   /* (c c o .) => (. c c o) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == opponent );
		CHECK( field[f + 3*dir] == free );
		field[f + 2*dir] = color;
		field[f + 3*dir] = opponent;
		break;
	 case Move::left2:
		dir2 = direction[m.direction-1];
		CHECK( field[f + dir] == color );
		CHECK( field[f + dir2] == free );
		CHECK( field[f + dir+dir2] == free );
		field[f+dir2] = color;
		field[f+=dir] = free;
		field[f+dir2] = color;
		break;
	 case Move::right2:
		dir2 = direction[m.direction+1];
		CHECK( field[f + dir] == color );
		CHECK( field[f + dir2] == free );
		CHECK( field[f + dir+dir2] == free );
		field[f+dir2] = color;
		field[f+=dir] = free;
		field[f+dir2] = color;
		break;
	 case Move::move1:       /* (c .) => (. c) */
		CHECK( field[f + dir] == free );
		field[f + dir] = color;
		break;
	}

	if (m.isOutMove()) {
		if (color == color1)
		  color2Count--;
		else
		  color1Count--;
	}       
	
	/* change actual color */
	color = opponent;
	
	CHECK( isConsistent() );
	
}

void Board::takeBack()
{
	int f, dir, dir2;
	int opponent = color;
	Move& m = storedMove[storedLast];

	CHECK( boardOk );
	CHECK( isConsistent() );
	
	if (storedFirst == storedLast) return;

	/* change actual color */
	color = (color == color1) ? color2:color1;	

	if (m.isOutMove()) {
		if (color == color1)
		  color2Count++;
		else
		  color1Count++;
	}       
	
	f = m.field;
	CHECK( field[f] == free );
	field[f] = color;
	dir = direction[m.direction];
	
	switch(m.type) {
	 case Move::out2:        /* (. c c c o |) => (c c c o o |) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == color );
		CHECK( field[f + 4*dir] == opponent );
		CHECK( field[f + 5*dir] == out );
		field[f + 3*dir] = opponent;
		break;
	 case Move::out1with3:   /* (. c c c |) => (c c c o |) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == color );
		CHECK( field[f + 4*dir] == out );
		field[f + 3*dir] = opponent;
		break;
	 case Move::move3:       /* (. c c c) => (c c c .)     */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == color );
		field[f + 3*dir] = free;
		break;
	 case Move::out1with2:   /* (. c c | ) => (c c o |)     */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == out );
		field[f + 2*dir] = opponent;
		break;
	 case Move::move2:       /* (. c c) => (c c .)       */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		field[f + 2*dir] = free;
		break;
	 case Move::push2:       /* (. c c c o o) => (c c c o o .) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == color );
		CHECK( field[f + 4*dir] == opponent );
		CHECK( field[f + 5*dir] == opponent );
		field[f + 3*dir] = opponent;
		field[f + 5*dir] = free;
		break;
	 case Move::left3:
		dir2 = direction[m.direction-1];
		CHECK( field[f + dir] == free );
		CHECK( field[f + 2*dir] == free );
		CHECK( field[f + dir2] == color );
		CHECK( field[f + dir+dir2] == color );
		CHECK( field[f + 2*dir+dir2] == color );
		field[f+dir2] = free;
		field[f+=dir] = color;
		field[f+dir2] = free;
		field[f+=dir] = color;
		field[f+dir2] = free;
		break;
	 case Move::right3:
		dir2 = direction[m.direction+1];
		CHECK( field[f + dir] == free );
		CHECK( field[f + 2*dir] == free );
		CHECK( field[f + dir2] == color );
		CHECK( field[f + dir+dir2] == color );
		CHECK( field[f + 2*dir+dir2] == color );
		field[f+dir2] = free;
		field[f+=dir] = color;
		field[f+dir2] = free;
		field[f+=dir] = color;
		field[f+dir2] = free;
		break;
	 case Move::push1with3:   /* (. c c c o) => (c c c o .) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == color );
		CHECK( field[f + 4*dir] == opponent );
		field[f + 3*dir] = opponent;
		field[f + 4*dir] = free;
		break;
	 case Move::push1with2:   /* (. c c o) => (c c o .) */
		CHECK( field[f + dir] == color );
		CHECK( field[f + 2*dir] == color );
		CHECK( field[f + 3*dir] == opponent );
		field[f + 2*dir] = opponent;
		field[f + 3*dir] = free;
		break;
	 case Move::left2:
		dir2 = direction[m.direction-1];
		CHECK( field[f + dir] == free );
		CHECK( field[f + dir2] == color );
		CHECK( field[f + dir+dir2] == color );
		field[f+dir2] = free;
		field[f+=dir] = color;
		field[f+dir2] = free;
		break;
	 case Move::right2:
		dir2 = direction[m.direction+1];
		CHECK( field[f + dir] == free );
		CHECK( field[f + dir2] == color );
		CHECK( field[f + dir+dir2] == color );
		field[f+dir2] = free;
		field[f+=dir] = color;
		field[f+dir2] = free;
		break;
	 case Move::move1:       /* (. c) => (c .) */
		CHECK( field[f + dir] == color );
		field[f + dir] = free;
		break;
	}

	if (--storedLast < 0) storedLast = MvsStored-1;
	
	CHECK( isConsistent() );
}

int Board::movesStored()
{
  int c = storedLast - storedFirst;
  if (c<0) c+= MvsStored;
  return c;
}

void Board::showMove(const Move& mm, int step)
{
	int f, dir, dir2;
	int opponentNew, colorNew;
	bool afterMove;
	static Move lastMove;
	Move m;

	if (boardOk) {
	  /* board ok means: board has the normal state before move */
	  CHECK( isConsistent() );
	  if (step == 0)
	    return;      /* nothing to be done */
	}
	boardOk = (step == 0) ? true:false;

	if (step == 0) 
	  m = lastMove;
	else {
	  m = lastMove = mm;
	}

	if (color == color1) {
	  colorNew = (step<2) ? color1 : ((step>2) ? color1bright:free );
	  opponentNew = (step<2) ? color2 : color2bright;
	}
	else {
	  colorNew = (step<2) ? color2 : ((step>2) ? color2bright:free );
	  opponentNew = (step<2) ? color1 : color1bright;
	}

	afterMove = (step == 1) || (step == 4);

	f = m.field;
	CHECK( (m.type >= 0) && (m.type < Move::none));
	dir = direction[m.direction];

	/* first field */
	field[f] = afterMove ? free : colorNew;

	switch(m.type) {
	case Move::out2:        /* (c c c o o |) */
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = colorNew;
	  field[f + 3*dir] = afterMove ? colorNew : opponentNew;
	  field[f + 4*dir] = opponentNew;
	  break;		
	case Move::out1with3:   /* (c c c o |)   */
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = colorNew;
	  field[f + 3*dir] = afterMove ? colorNew : opponentNew;
	  break;
	case Move::move3:       /* (c c c .)     */
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = colorNew;
	  field[f + 3*dir] = afterMove ? colorNew : free;
	  break;
	case Move::out1with2:   /* (c c o |)     */
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = afterMove ? colorNew : opponentNew;
	  break;
	case Move::move2:       /* (c c .)       */
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = afterMove ? colorNew : free;
	  break;
	case Move::push2:       /* (c c c o o .) */
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = colorNew;
	  field[f + 3*dir] = afterMove ? colorNew : opponentNew;
	  field[f + 4*dir] = opponentNew;
	  field[f + 5*dir] = afterMove ? opponentNew : free;
	  break;
	case Move::left3:		
	  dir2 = direction[m.direction-1];
	  field[f+dir2] = afterMove ? colorNew : free;
	  field[f+=dir] = afterMove ? free : colorNew;
	  field[f+dir2] = afterMove ? colorNew : free;
	  field[f+=dir] = afterMove ? free : colorNew;
	  field[f+dir2] = afterMove ? colorNew : free;
	  break;
	case Move::right3:
	  dir2 = direction[m.direction+1];
	  field[f+dir2] = afterMove ? colorNew : free;
	  field[f+=dir] = afterMove ? free : colorNew;
	  field[f+dir2] = afterMove ? colorNew : free;
	  field[f+=dir] = afterMove ? free : colorNew;
	  field[f+dir2] = afterMove ? colorNew : free;
	  break;
	case Move::push1with3:   /* (c c c o .) => (. c c c o) */
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = colorNew;
	  field[f + 3*dir] = afterMove ? colorNew : opponentNew;
	  field[f + 4*dir] = afterMove ? opponentNew : free;
	  break;
	case Move::push1with2:   /* (c c o .) => (. c c o) */
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = afterMove ? colorNew : opponentNew;
	  field[f + 3*dir] = afterMove ? opponentNew : free;
	  break;
	case Move::left2:
	  dir2 = direction[m.direction-1];
	  field[f+dir2] = afterMove ? colorNew : free;
	  field[f+=dir] = afterMove ? free : colorNew;
	  field[f+dir2] = afterMove ? colorNew : free;
	  break;
	case Move::right2:
	  dir2 = direction[m.direction+1];
	  field[f+dir2] = afterMove ? colorNew : free;
	  field[f+=dir] = afterMove ? free : colorNew;
	  field[f+dir2] = afterMove ? colorNew : free;
	  break;
	case Move::move1:       /* (c .) => (. c) */
	  field[f + dir] = afterMove ? colorNew : free;
	  break;
	}
}

void Board::showStart(const Move& m, int step)
{
	int f, dir;
	int colorNew;

	if (boardOk) {
	  /* board ok means: board has the normal state before move */
	  CHECK( isConsistent() );
	  if (step == 0)
	    return;      /* nothing to be done */
	}
	boardOk = (step == 0) ? true:false;

	if (color == color1)
	  colorNew = (step==0) ? color1 : color1bright;
	else 
	  colorNew = (step==0) ? color2 : color2bright;

	f = m.field;

	/* first field */
	field[f] = colorNew;

	switch(m.type) {
	case Move::left3:
	case Move::right3:
	  dir = direction[m.direction];
	  field[f + dir] = colorNew;
	  field[f + 2*dir] = colorNew;
	  break;
	case Move::left2:
	case Move::right2:
	  dir = direction[m.direction];
	  field[f + dir] = colorNew;
	}
}


/* calc value out of possible moves starting at field <startField>
 * parameter color shadows class member variable !
 */
inline int Board::calcFieldValue(int startField, int color, int value)
{
	int d, dir, c, actField;
	int opponent = (color == color1) ? color2 : color1;

	/* 6 directions	*/
	for(d=1;d<7;d++) {
		dir = direction[d];
		  
		/* 2nd field */
		if (field[actField = startField+dir] != color)
		  continue;
		
		/* 2nd == color */
		
		/* 3rd field */
		c = field[actField += dir];
		if (c == free) {
			/* (c c .) */
			value+=moveValue[0];
			continue;
		}		
		else if (c == opponent) {
			
			/* 4th field */
			c = field[actField += dir];
			if (c == free) {
			  /* (c c o .) */
			  value+=moveValue[2];
			}
			else if (c == out) {
			  /* (c c o |) */
			  value+=moveValue[4];
			}
			continue;
		}
		if (c != color)
		  continue;
		
		/* 3nd == color */

		/* 4th field */
		c = field[actField += dir];
		if (c == free) {
		  /* (c c c .) */
		  value+=moveValue[1];
		  continue;
		}
		if (c != opponent)
		  continue;

		/* 4nd == opponent */
		
		/* 5. field */
		c = field[actField += dir];
		if (c == free) {
		  /* (c c c o .) */
		  value+=moveValue[2];
		  continue;
		}
		else if (c == out) {
		  /* (c c c o |) */
		  value+=moveValue[4];
		  continue;
		}
		if (c != opponent)
		  continue;

		/* 5nd == opponent */

		/* 6. field */
		c = field[actField += dir];
		if (c == free) {
		  /* (c c c o o .) */
		  value+=moveValue[3];
		}
		else if (c == out) {
		  /* (c c c o o |) */
		  value+=moveValue[4];
		}
	}
	return value;
}



/* Calculate a rating for actual position
 * 
 * A higher value means a better position for opponent
 * NB: This means a higher value for better position of
 *     'color before last move'
 */
int Board::calcValue()
{
        int value=0,f;
	int i,j;
	
	for(i=0;i<RealFields;i++) {		  
		j=field[f=order[i]];
		if (j == 0) continue;
		if (j == color) 
		  value -= calcFieldValue(f,j,fieldValue[i]);
		else
		  value += calcFieldValue(f,j,fieldValue[i]);
	}
	if (color == color2)
	  value += stoneValue[14 - color1Count] - 
                   stoneValue[14 - color2Count];
	else
	  value += stoneValue[14 - color2Count] - 
                   stoneValue[14 - color1Count];

	return value;
}

bool Board::isConsistent()
{
        int c1 = 0, c2 = 0;
	int i,j;
	
	for(i=0;i<RealFields;i++) {		  
		j=field[order[i]];
		if (j == color1) c1++;
		if (j == color2) c2++;
	}
	return (color1Count == c1 && color2Count == c2); 
}

void Board::changeRating()
{
	int i,tmp;
	
	/* innermost ring */
	tmp=fieldValue[1];
	for(i=1;i<6;i++)
	  fieldValue[i] = fieldValue[i+1];
	fieldValue[6] = tmp;
	
	tmp=fieldValue[7];
	for(i=7;i<18;i++)
	  fieldValue[i] = fieldValue[i+1];
	fieldValue[18] = tmp;
	
	tmp=fieldValue[19];
	for(i=19;i<36;i++)
	  fieldValue[i] = fieldValue[i+1];
	fieldValue[36] = tmp;

	/* the outermost ring */
	tmp=fieldValue[37];
	for(i=37;i<61;i++)
	  fieldValue[i] = fieldValue[i+1];
	fieldValue[61] = tmp;
}

/*
void Board::showHist()
{	
	Move m1, m2;
	
	printf("After playing ");
	(m1=lastMove()).print();
	print();
	printf("TakeBack "); m1.print();
	takeBack(); print();
	printf("TakeBack ");
	(m2=lastMove()).print();
	takeBack(); print();
	printf("Play "); m2.print();
	playMove(m2); print();
	printf("Play "); m1.print();
	playMove(m1); print();
	getchar();
}
*/

void indent(int d)
{
	char tmp[]="                                  ";
	tmp[d*3] = 0;
	printf("%s",tmp);
}

/*
 * We always try the maximize the board value
 */
int Board::search(int depth, int alpha, int beta)
{
	int actValue=-16000, value;
	Move m;
	MoveList list;
	bool stop = false;

	/* We make a depth search for the following move types... */
	int maxType = (depth < maxDepth/2) ? Move::maxMoveType() :
	              (depth < maxDepth)   ? Move::maxPushType() : 
                                             Move::maxOutType();

	generateMoves(list);

#ifdef MYTRACE
	printf(">>>>>>>> Depth %d\n", depth);
#endif
	
	/* check for a old best move in main combination */
	if (inMainCombination) {
		m = mc[depth];
		if (!list.isElement(m, 0)) 
		  m.type = Move::none;
		if (m.type == Move::none)
		  inMainCombination = false;
		if (m.type > maxType)
		  m.type = Move::none;
	}
	if (m.type == Move::none)
	  stop = !list.getNext(m, maxType);

	/* depth first search */
	while(!stop) {

#ifdef MYTRACE
	  indent(depth);
	  m.print();
	  printf("\n");
#endif	
		
#ifdef SPION
	  	if (bUpdateSpy) emit update(depth, 0, m, false);
#endif
		playMove(m);
		if (!isValid())
		  value = ((depth < maxDepth) ? 15999:14999) - depth;
		else {
		  /* opponent searches for his maximum; but we won't the
		   * minimum: so change sign (for alpha/beta window too!)
		   */
		  value = - search(depth+1,-beta,-alpha);
		}
		takeBack();

		/* For GUI response */
		if (maxDepth - depth >2)
		  emit searchBreak();		

#ifdef MYTRACE	
		indent(depth);
		printf("=> (%d - %d): Value %d [ %d ] for ",
		       alpha, beta, value,actValue);
		m.print();
	  printf("\n");
#endif

#ifdef SPION
		if (bUpdateSpy) {
		  if (value > actValue)
		    emit updateBest(depth, value, m, value >= beta);
		  emit update(depth, value, m, true);
		}		  
#endif
		if (value > actValue) {
			actValue = value;
			mc.update(depth, m);

			if (bUpdateSpy && depth == 0)
			  emit updateBestMove(m, actValue);
			
			if (actValue >= beta) {
#ifdef MYTRACE
			  indent(depth);
			  printf("CUTOFF\n");
#endif
			  return actValue;
			}			
			if (actValue > alpha) alpha = actValue;
		}
		
		stop = (!list.getNext(m, maxType)) || breakOut;
	}
	
	/* other moves: calculate rating */
	while(list.getNext(m, Move::none)) {
		
		playMove(m);
		if (!isValid())
		  value = ((depth < maxDepth) ? 15999:14999) - depth;
		else
		  value = calcValue();
		takeBack();

#ifdef SPION
		if (bUpdateSpy) {
		  if (value > actValue)
		    emit updateBest(depth, value, m, value >= beta);
		  emit update(depth, value, m, true);
		}		  
#endif

#ifdef MYTRACE
		indent(depth);
		printf("> (%d - %d): Value %d [ %d ] for ",
		       alpha, beta, value, actValue);
		m.print();
		printf("\n");
#endif
		if (value > actValue) {
			actValue = value;
			mc.update(depth, m);

			if (actValue >= beta) {
#ifdef MYTRACE
			  indent(depth);
			  printf("CUTOFF\n");
#endif
			  break;
			}
			
			if (actValue > alpha) alpha = actValue;
		}
	}	
	return actValue;
}


Move& Board::bestMove()
{
	int alpha=-15000,beta=15000;
	int nalpha,nbeta, actValue;
	
	mc.clear(realMaxDepth);
	
	maxDepth=1;
	show = false;
	breakOut = false;

#ifdef MYTRACE
	printf(">>>>>>>>>>>>>>>>>> New Search\n");
#endif
	do {
#ifdef MYTRACE
		printf(">> MaxDepth: %d\n", maxDepth);
#endif
		// ShowTiefe(maxtiefe);
		do {
#ifdef MYTRACE
			  printf(">> Alpha/Beta: (%d ... %d)\n", alpha, beta);
#endif
			nalpha=alpha, nbeta=beta;
			inMainCombination = (mc[0].type != Move::none);
			actValue = search(0,alpha,beta);
			
			/* Don't break out if we haven't found a move */
			if (mc[0].type == Move::none) 
			  breakOut=false;
			if (actValue > 15000 || actValue < -15000)
			  breakOut=true;
			if (actValue <= nalpha)
			  alpha = -15000, beta=actValue+1;
			if (actValue >= nbeta)
			  alpha = actValue-1, beta=15000;
		}
		while(!breakOut && (actValue<=nalpha || actValue>=nbeta));
		//		if ( (maxDepth+((color == color2)?1:0)) %2 ==1)
		alpha=actValue-200, beta=actValue+1;
		// else
		// alpha=actValue-1, beta=actValue+200;
		// ShowHv2();
	}	
	while(++maxDepth< realMaxDepth && !breakOut);
	
	changeRating();
	
	return(mc[0]);
}	  

Move Board::randomMove()
{
	static int i = 999;
	unsigned int j,l;

	Move m;
	MoveList list;
	
	/* we prefer to use QT... */
	j = (QTime::currentTime()).msec();
	
	generateMoves(list);
	l = list.getLength();

	j = (j + i) % l +1;
	if ( (i+=7)>10000) i-=10000;
	
	while(j != 0) {
		list.getNext(m, Move::none);
		j--;
	}	
	
	return m;
}
	

void Board::print()
{
	int row,i;
	char spaces[]="      ";
	char *z[]={". ","O ","X ", "o ", "x "};

	printf("\n       -----------\n");
	for(row=0;row<4;row++) {
		printf("%s/ ",spaces+row);
		for(i=0;i<5+row;i++) printf("%s",z[field[row*11+12+i]]);
		printf("\\\n");
	}
	printf("  | ");
	for(i=0;i<9;i++) printf("%s",z[field[56+i]]);
	printf("|\n");
	for(row=0;row<4;row++) {
		printf("%s\\ ",spaces+3-row);
		for(i=0;i<8-row;i++) printf("%s",z[field[68+row*12+i]]);
		printf("/\n");
	}
	printf("       -----------     O: %d  X: %d\n",
	       color1Count, color2Count);	
}

QString Board::getState()
{
	QString state;
	QString entry, tmp;
	Move m;

	/* remove any highlighting */
	showMove(m,0);

	/* Color + Counts */
	state += (char) ('A' + color1Count);
	state += (char) ('A' + color2Count);
	state += (char) ('A' + 3*color + field[order[0]]);
	
	/* Board (field values can be 0/1/2; 3 fields coded in one char */
	for(int i=1;i<=60;i+=3)
	  state+= (char) ('@' + 9*field[order[i]] + 
			  3*field[order[i+1]] + field[order[i+2]] );

	/* -> 23 chars */
	return state;
}	

void Board::setState(QString& state)
{
	if (state.length() != 23) return;

	color1Count = state[0] - 'A';
	color2Count = state[1] - 'A';
	color = (state[2] - 'A') / 3;
	field[order[0]] = (state[2] - 'A') %3;

	int i = 1;
	for(int j = 3; j<23; j++) {
		int w = state[j] - '@';
		field[order[i++]] = w/9;
		field[order[i++]] = (w % 9)/3;
		field[order[i++]] = w % 3;
	}
}

#include "Board.moc"

