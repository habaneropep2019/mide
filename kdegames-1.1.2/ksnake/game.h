#ifndef GAME_H
#define GAME_H

#include <kmenubar.h>
#include <qpopmenu.h>
#include <qlcdnum.h>

#include <qlist.h>
#include <qfileinf.h>

#include <kapp.h>
#include <ktopwidget.h>

#include "rattler.h"
#include "trys.h"
#include "score.h"
#include "progress.h"
#include "levels.h"

class Game : public KTopLevelWidget
{
    Q_OBJECT
public:
    Game();
    Game::~Game() {}
protected:
    //   void keyPressEvent( QKeyEvent * );
private slots:
    void ballsChecked(int);
    void skillChecked(int);
    void snakesChecked(int);
    void pixChecked(int);

    void newGame();
    void pauseGame();
    void togglePaused();
    void quitGame();

    void backgroundColor();
    void confKeys();

    void showHighScores();
    void startingRoom();

private:

    Rattler *rattler;
    QLCDNumber *lcd;

    Trys *trys;
    Score   *score;
    Progress *pg;
    Levels *levels;

    KConfig *conf;

    void menu();
    void checkMenuItems();
    void lookupBackgroundPixmaps();

    KMenuBar *menubar;
    QPopupMenu *game;
    QPopupMenu *options;
    QPopupMenu *balls;
    QPopupMenu *snakes;
    QPopupMenu *pix;

    int pauseID;

    int ballsID[4];
    int snakesID[4];
    int skillID[4];

    QArray<int> pixID;


    QList<QFileInfo> backgroundPixmaps;
};

#endif // GAME_H
