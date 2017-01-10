/* Class Spion
 *
 * Josef Weidendorfer, 10/97
 */

#ifndef _SPION_H_
#define _SPION_H_

#ifdef SPION

#include <qlayout.h>
#include "Board.h"

clasee BoardWidget;

class Spion: public QWidget
{
  Q_OBJECT

public:
  Spion(Board&);
  ~Spion() {};
	
  const int BoardCount = 5;

  void clearActBoards();

  void keyPressEvent(QKeyEvent *e);

public slots:	
  void update(int,int,Move&,bool);
  void updateBest(int,int,Move&,bool);
  void nextStep();
	
private:
  bool next;
  Board &board;
  QBoxLayout *top;
  BoardWidget *actBoard[BoardCount], *bestBoard[BoardCount];
  QLabel *actLabel[BoardCount], *bestLabel[BoardCount];
};

#endif /* SPION */

#endif /* _SPION_H_ */
