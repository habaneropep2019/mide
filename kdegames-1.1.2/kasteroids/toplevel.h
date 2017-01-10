
#ifndef __KAST_TOPLEVEL_H__
#define __KAST_TOPLEVEL_H__

#include <kstatusbar.h>
#include <ktoolbar.h>
#include <ktopwidget.h>
#include <qlcdnum.h>
#include <qdict.h>
#include <kmenubar.h>

#include "kfixedtopwidget.h"
#include "view.h"

class KAudio;

class KAstTopLevel : public KFixedTopWidget
{
    Q_OBJECT
public:
    KAstTopLevel();
    virtual ~KAstTopLevel();

private:
    void createMenuBar();
    void readSettings();
    void playSound( const char *snd );
    void readSoundMapping();

protected:
    virtual void keyPressEvent( QKeyEvent *event );
    virtual void keyReleaseEvent( QKeyEvent *event );
    virtual void focusInEvent( QFocusEvent *event );
    virtual void focusOutEvent( QFocusEvent *event );

private slots:
    void slotNewGame();
    void slotQuit();

    void slotShipKilled();
    void slotRockHit( int size );
    void slotRocksRemoved();

private:
    KAsteroidsView *view;
    KMenuBar *menu;
    QLCDNumber *scoreLCD;
    QLCDNumber *levelLCD;
    QLCDNumber *shipsLCD;

    bool   sound;
    KAudio *kas;
    QDict<char> soundDict;

    // waiting for user to press Enter to launch a ship
    bool waitShip;

    int shipsRemain;
    int score;
    int level;
};

#endif

