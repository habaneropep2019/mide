/* Class AbTop: the toplevel widget of KAbalone
 * 
 * Josef Weidendorfer, 9/97
*/

#ifndef _ABTOP_H_
#define _ABTOP_H_

#include <qobject.h>
#include <kapp.h>
#include <ktopwidget.h>
#include "Move.h"

class QTimer;
class QPopupMenu;
class KStdAccel;
class Board;
class BoardWidget;
class Move;
#ifdef SPION
class Spy;
#endif


class AbTop: public KTopLevelWidget
{
  Q_OBJECT

public:
  AbTop();
  ~AbTop();

  /* timer states */
  enum { noGame, gameOver, notStarted, moveShown,
	 showMove = 100, showSugg=200
  };

	
protected:
  virtual void closeEvent (QCloseEvent *);

  virtual void saveProperties( KConfig * );
  virtual void readProperties( KConfig * );


public slots:
  void timerDone();
  void newGame();
  void stopGame();
  void continueGame();
  void takeBack();
  void suggestion();
  void easy();
  void stopSearch();
  void normal();
  void hard();
  void challange();
  void changeShowMove();
  void toggleSpy();
  void help();
  void quit();
  void searchBreak();
  void moveChoosen(Move&);
  void play_red();
  void play_yellow();
  void play_both();
  void savePosition();
  void restorePosition();
  void updateSpy(QString);
  void updateBestMove(Move&,int);
  void readConfig();
  void writeConfig();

private:
  void updateStatus();
  void userMove();
  void playGame();
  void setupMenu();
  void setupToolBar();
  void setupStatusBar();
  void updateToolBar();
  void setLevel(int,int);
  void setIplay(int,int);
  bool iPlayNow();
  Move haveHint();
  void readOptions(KConfig *);
  void writeOptions(KConfig *);

  Move actMove;
  Board* board;
  int actValue;
  BoardWidget *boardWidget;
  QTimer *timer;
  KStatusBar *status;
  KToolBar *toolbar;
  int timerState;
  int depth, moveNo;
  bool showMoveLong, stop, showSpy;

  int iplay;
  enum { none, cYellow, cRed, cBoth };

  QPopupMenu *_file, *_options, *_level, *_iplay;
  int stop_id, back_id, hint_id;
  int easy_id, normal_id, hard_id, challange_id, slow_id, level_id;
  int yellow_id, red_id, both_id, iplay_id;
  int spy_id;

#ifdef SPION	
  Spy* spy;
#endif

  KStdAccel* stdAccel;
};

#endif /* _ABTOP_H_ */
