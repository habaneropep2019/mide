#ifndef KTETRIS_B_H
#define KTETRIS_B_H

#include "gtetris.h"
#include "net.h"

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <qframe.h>
#include <qtimer.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qsocknot.h>

#include <kconfig.h>
#include <kaccel.h>

#define SINGLE 1
#define MULTI  2

class Board : public QFrame, public GenericTetris
{
 Q_OBJECT

 public:
    Board( KAccel * parentkaccel, QWidget *parent=0, const char *name=0 );
    virtual ~Board();

    void initMultiGame(NetObject *net_obj);

 public slots:
    void updateNext()       { GenericTetris::updateNext(); }
    void key(QKeyEvent *e)  { keyPressEvent(e); }
    void start()            { startGame(); }
    void pause();
    void showHighScores();
    void options();
    void pMoveLeft() { if ( !(state==WaitingAfterLine) ) moveLeft(); }
    void pMoveRight() { if ( !(state==WaitingAfterLine) ) moveRight(); }
    void pDropDown();
    void pOneLineDown() { if ( !(state==WaitingAfterLine) ) oneLineDown(); }
    void pRotateLeft() { if ( !(state==WaitingAfterLine) ) rotateLeft(); }
    void pRotateRight() { if ( !(state==WaitingAfterLine) ) rotateRight(); }
    void buttonClick();
    
 private slots:
    void timeout();
    
 signals:
    void gameOverSignal();
    void drawNextSquareSignal(int x,int y,QColor *color1);
    void updateRemovedSignal(int noOfLines);
    void updateScoreSignal(int score);
    void updateLevelSignal(int level);
    void showOpponents();
    void updateOpponents();

 protected:    
    virtual void startGame();
    void gameOver();
    void updateRemoved(int noOfLines);
    void updateScore(int newScore);
    void updateLevel(int newLlevel);
    void waitAfterLine();
    void lightLines(int y, int nb, bool on);
    virtual void lightFullLines(bool on) = 0;
    virtual void removeFullLines() = 0;
    
    NetObject *net_obj;

    QTimer   *timer;
    int  timeoutTime;
    
    enum State { NoGame, Playing, Paused, WaitingAfterLine, DropDown };
    State state;
    uint loop;
    
 private:
    void paintEvent(QPaintEvent *e);
    void drawSquare(int x,int y,int value);
    void drawNextSquare(int x,int y,int value);
    void updateTimeoutTime();
    void midbutton(bool); 
    void setHighScore(int);
    void setPieceMovingKeys( bool activate );
    void showBoard();
    void stopGame();

    QLabel   *msg;
    QPushButton *pb;
    QPainter *paint;
    
    int gameType;
    QColor    colors[9];
    QString serror;

    int removedLines, oldFullLines;
    bool multiGame;
    
    bool isConfigWritable;
    KConfig *kconf;
    KAccel *kacc;
};

#endif
