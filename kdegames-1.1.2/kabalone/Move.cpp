/* Classes Move, MoveList
 * - represents a move in the game of Abalone
 *
 * Josef Weidendorfer, 28.8.97
*/

#include <assert.h>
#include <qstring.h>
#include <kapp.h>
#include "Move.h"
#include "Board.h"

const char* nameOfDir(int dir)
{
  dir = dir % 6;
  return 
    (dir == 1) ? klocale->translate("Right") :
    (dir == 2) ? klocale->translate("RightDown") :
    (dir == 3) ? klocale->translate("LeftDown") :
    (dir == 4) ? klocale->translate("Left") :
    (dir == 5) ? klocale->translate("LeftUp") :
    (dir == 0) ? klocale->translate("RightUp") : "??";
}

QString Move::name() const
{
  QString s,tmp;

  /* sideway moves... */
  if (type == left3 || type == right3) {
    s.setNum(field + Board::fieldDiffOfDir(direction)); /* middle of the 3 */
    s+= '/';
    s+= (type == left3) ? nameOfDir(direction-1): nameOfDir(direction+1);
    s+= '/';
    s+= klocale->translate("Side");
  }
  else if ( type == left2 || type == right2) {
    s.setNum(field);
    s+= '/';
    s+= (type == left2) ? nameOfDir(direction-1): nameOfDir(direction+1);
    s+= '/';
    s+= klocale->translate("Side");
  }
  else {
    s.setNum(field);
    s += '/';
    s += nameOfDir(direction);
    
    tmp = (type <3 ) ? klocale->translate("Out") :
          (type <6 ) ? klocale->translate("Push") : "";
    if (!tmp.isEmpty()) {
      s += '/';
      s += tmp;
    }
  }
  return s;
}

void Move::print() const
{
  printf("%s", (const char*)name() );
}


MoveList::MoveList()
{
	clear();
}

void MoveList::clear()
{
	int i;
	
	for(i=0;i<Move::typeCount;i++)
		first[i] = actual[i] = -1;
	
	nextUnused = 0;
	actualType = -1;
}

void MoveList::insert(Move m)
{
	int t = m.type;
	
	/* valid and possible ? */
	if (t <0 || t >= Move::typeCount) return;
	if (nextUnused == MaxMoves) return;

	assert( nextUnused < MaxMoves );

	/* adjust queue */
	if (first[t] == -1) {
		first[t] = last[t] = nextUnused;
	}
	else {
	        assert( last[t] < nextUnused );
		next[last[t]] = nextUnused;
		last[t] = nextUnused;
	}
	
	next[nextUnused] = -1;	
	move[nextUnused] = m;
	nextUnused++;
}

bool MoveList::isElement(int f)
{
	int i;
	
	for(i=0; i<nextUnused; i++) 
	  if (move[i].field == f) 
		return true;

	return false;
}


bool MoveList::isElement(Move &m, int startType)
{
	int i;
	
	for(i=0; i<nextUnused; i++) {
	  Move& mm = move[i];
	  if (mm.field != m.field) 
	    continue;

	  /* if direction is supplied it has to match */
	  if ((m.direction > 0) && (mm.direction != m.direction))
	    continue;

	  /* if type is supplied it has to match */	    
	  if ((m.type != Move::none) && (m.type != mm.type))
	    continue;
				      
	  if (m.type == mm.type) {
	    /* exact match; eventually supply direction */
	    m.direction = mm.direction;
	    return true;
	  }

	  switch(mm.type) {
	  case Move::left3:
	  case Move::right3:
	    if (startType == start3 || startType == all) {
	      m.type = mm.type;
	      m.direction = mm.direction;
	      return true;
	    }
	    break;
	  case Move::left2:
	  case Move::right2:
	    if (startType == start2 || startType == all) {
	      m.type = mm.type;
	      m.direction = mm.direction;
	      return true;
	    }
	    break;
	  default:
	    if (startType == start1 || startType == all) {
	      /* unexact match: supply type */
	      m.type = mm.type;
	      m.direction = mm.direction;
	      return true;
	    }
	  }
	}
	return false;
}


bool MoveList::getNext(Move& m, int maxType)
{
	if (actualType == Move::typeCount) return false;

	while(actualType < 0 || actual[actualType] == -1) {
		actualType++;
		if (actualType == Move::typeCount) return 0;
		actual[actualType] = first[actualType];
		if (actualType > maxType) return 0;
	}
	m = move[actual[actualType]];
	actual[actualType] = next[actual[actualType]];

	return true;
}


