/* Classes Move, MoveList
 * - represents a move in the game of Abalone
 *
 * Josef Weidendorfer, 28.8.97
*/

#ifndef _MOVE_H_
#define _MOVE_H_


class Move
{
 public:
        Move() { type = none; }
	Move(short f, char d, char t)
	  { field = f; direction = d, type = t; }		
	
	/* Type of move: Moves are searched in this order */
	enum { out2 = 0, out1with3, out1with2, push2,
		  push1with3, push1with2, move3, left3, right3,
		  left2, right2, move2, move1, none };
	static const int typeCount = none;

	bool isOutMove() const
	  { return type <= out1with2; }
	bool isPushMove() const
	  { return type <= push1with2; }
	static int maxOutType()
	  { return out1with2; }
	static int maxPushType()
	  { return push1with2; }	       
	static int maxMoveType()
	  { return move1; }	       

	QString name() const;

	void print() const;

	/* Directions */
	enum { Right=1, RightDown, LeftDown, Left, LeftUp, RightUp };
	
	short field;
	char  direction, type;
};

/* MoveList stores a fixed number of moves (for efficince) 
 * <getNext> returns reference of next move ordered according to type
 * <insert> does nothing if there isn't enough free space
 * 
 * Recommend usage (* means 0 or more times):
 *   [ clear() ; insert() * ; isElement() * ; getNext() * ] *
 */
class MoveList
{
 public:
	MoveList();
	
	static const int MaxMoves = 84;

	/* for isElement: search for moves starting with 1/2/3 fields */
	enum { all , start1, start2, start3 };
		
	void clear();
	void insert(Move);
	bool isElement(int f);
	bool isElement(Move&, int startType);
	void insert(short f, char d, char t)
	  { insert( Move(f,d,t) ); }
	int getLength()
	  { return nextUnused; }		  
		  
	bool getNext(Move&,int maxType);  /* returns false if no more moves */

 private:
	Move move[MaxMoves];
	int  next[MaxMoves];
	int  first[Move::typeCount];
	int  last[Move::typeCount];
	int  actual[Move::typeCount];
	int  nextUnused, actualType;
};

#endif /* _MOVE_H_ */

