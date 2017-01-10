/* Class Board - represents a game state
 * 
 * Josef Weidendorfer, 28.8.97
*/

#ifndef _BOARD_H_
#define _BOARD_H_

#include <qobject.h>
#include "Move.h"

class KConfig;

/* Class for best moves so far */
class MainCombination
{
public:
	MainCombination() 
	  { clear(1); }		   

	static const int maxDepth = 10;

	bool hasMove(int d) 
	  {  return (d>actMaxDepth) ? 
		    false : (move[0][d].type != Move::none); }
	
	Move& operator[](int i)
	  { return (i<0 || i>=maxDepth) ? move[0][0] : move[0][i]; }
	
	void update(int d, Move& m);
	void clear(int d);
	void setMaxDepth(int d)
	  { actMaxDepth = (d>maxDepth) ? maxDepth-1 : d; }

private:
	Move move[maxDepth][maxDepth];
	int actMaxDepth;

};		  
		

class Board : public QObject
{
  Q_OBJECT

 public:
	Board();
	~Board() {};

	/* different states of one field */
	enum {
		out = 10, free = 0, 
		color1, color2, color1bright, color2bright
	};
	static const int AllFields = 121; /* visible + ring of unvisible around */
	static const int RealFields = 61; /* number of visible fields */
	static const int MvsStored = 100;

	int debug;	

	/* fill Board with defined values */
	void begin(int startColor);  /* start of a game */
	void clear();                /* empty board     */
	
	/* fields can't be changed ! */
	const int operator[](int no);

	const int actColor() 
	  { return color; }	
	
	/* Generate list of allowed moves for player with <color>
	 * Returns a calculated value for actual position */
	void generateMoves(MoveList& list);
	
	/* Functions handling moves
	 * played moves can be taken back (<MvsStored> moves are remembered) */
	void playMove(const Move& m);
	void takeBack();    /* if not remembered, do nothing */
	int movesStored();  /* return how many moves are remembered */

	/* Show a move with highlighting
	 *  step 0: state before move
	 *       1: state after move
	 *       2: remove all marks (for blinking)
	 *       3: highlight marks (before move)
 	 *       4: highlight marks (after move)
	 * Always call with step 0 before actual playing the move !! */
	void showMove(const Move& m, int step);

	/* Only highlight start
	 * Step 0: original state
	 *      1: highlight start
	 * Always call with step 0 before actual playing the move !! */
	void showStart(const Move& m, int step);

	Move& Board::lastMove()
	  { return storedMove[storedLast]; }
	
	void showHist();


	/* Calculate a value for actual position 
	 * (greater if better for color1) */
	int calcValue();
	
	/* Value calculation is based on ratings which can be changed
	 * a little (so computer's moves aren't always the same) */
	void changeRating();

	int getColor1Count() 
	  { return color1Count; }
	int getColor2Count() 
	  { return color2Count; }
	bool isValid()
	  { return (color1Count>8 && color2Count>8); }

	/* Check that color1Count & color2Count is consisten with board */
	bool isConsistent();
	
	/* Searching best move: alpha/beta search */
	void setDepth(int d)
	  { realMaxDepth = d+1; }	  
	Move& bestMove();

	/* next move in main combination */
	Move& nextMove() { return mc[1]; }

	Move randomMove();
	void stopSearch() { breakOut = true; }

	/* Save/Restore board position (field, colorCounts) from a string */
	QString getState();
	void setState(QString& p);

	void updateSpy(bool b) { bUpdateSpy = b; }			
	
	/* simple terminal view of position */
	void print();
	
	static int fieldDiffOfDir(int d) { return direction[d]; }

	void readRating(KConfig *);
	void saveRating(KConfig *);

signals:
	void searchBreak();
	void updateBestMove(Move&,int);

	void update(int,int,Move&,bool);
	void updateBest(int,int,Move&,bool);
		  		
 private:
	void setFieldValues();	

	/* helper function for generateMoves */
	void generateFieldMoves(int, MoveList&);
	/* helper function for calcValue */
	int calcFieldValue(int,int,int);
	/* helper functions for bestMove (recursive search!) */
	int search(int, int, int);
	int search2(int, int, int);
	  
	int field[AllFields];         /* actual board */
	int color1Count, color2Count;
	int color;                    /* actual color */
	Move storedMove[MvsStored];   /* stored moves */
	int storedFirst, storedLast;  /* stored in ring puffer manner */

	/* for search */
	MainCombination mc;
	bool breakOut, inMainCombination, show, boardOk, bUpdateSpy;
	int maxDepth, realMaxDepth;	

	/* ratings; semi constant - are rotated by changeRating() */
	static int fieldValue[RealFields];

	/* constant arrays */	
	static int startBoard[AllFields];
	static int order[RealFields];
	static int direction[8];
	
	static int stoneValue[6];
	static int moveValue[5];
	static int ringValue[5], ringDiff[5];
};


inline const int Board::operator[](int no)
{
  return (no<12 || no>120) ? out : field[no];
}

#endif
