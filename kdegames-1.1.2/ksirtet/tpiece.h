#ifndef TPIECE_H
#define TPIECE_H

#define NB_TYPES 7
#define NB_CASES 4

#define TETRIS_GAME   1
#define PUYOPUYO_GAME 2


class TetrisPiece
{
public:
    TetrisPiece()                { }
    TetrisPiece(int pieceType, int gameType) 
		                         { initialize( pieceType % NB_TYPES + 1,
											   gameType ); }

    void setRandomType(int gameType)  
		                         { initialize( randomValue(NB_TYPES) + 1,
											   gameType ); }

    void rotateLeft();
    void rotateRight();

	int  getType()                           { return type; }
    int  getColor(int index)                 { return color[index]; }
		
    int  getXCoord(int index)                { return coordinates[index][0]; }
    int  getYCoord(int index)                { return coordinates[index][1]; }
    void getCoords(int index, int &x, int&y) { x = coordinates[index][0];
                                               y = coordinates[index][1]; }
    int  getMinX();
    int  getMaxX();
    int  getMinY();
    int  getMaxY();

    int    randomValue(int maxPlusOne);

 private:
	void setXCoord(int index, int value)    { coordinates[index][0] = value; }
	void setYCoord(int index, int value)    { coordinates[index][1] = value; }
	void setCoords(int index, int x, int y) { coordinates[index][0] = x;
		                                      coordinates[index][1] = y; }
	void setColor(int index, int value)     { color[index] = value; }
	
	void initialize(int pieceType, int gameType);
	
	int  type;
    int  coordinates[NB_CASES][2];
	int color[NB_CASES];
};

#endif

