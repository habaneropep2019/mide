
#ifndef __AST_VIEW_H__
#define __AST_VIEW_H__

#include <qwidget.h>
#include <qlist.h>
#include <QwSpriteField.h>
#include "sprites.h"

class KAsteroidsView : public QWidget
{
    Q_OBJECT
public:
    KAsteroidsView( QWidget *parent = 0, const char *name = 0 );
    virtual ~KAsteroidsView();

    void reset();
    void setRockSpeed( double rs ) { rockSpeed = rs; }
    void addRocks( int num );
    void newGame();
    void endGame();
    void newShip();

    void rotateLeft( bool r ) { rotateL = r; rotateSlow = 5; }
    void rotateRight( bool r ) { rotateR = r; rotateSlow = 5; }
    void thrust( bool t ) { thrustShip = t; }
    void shoot( bool s ) { shootShip = s; shootDelay = 0; }

    void showText( const char *text, const QColor &color );
    void hideText();

signals:
    void shipKilled();
    void rockHit( int size );
    void rocksRemoved();

protected:
    void readSprites();
    void processMissiles();
    void processShip();

    virtual void resizeEvent( QResizeEvent *event );
    virtual void timerEvent( QTimerEvent * );

private:
    QwImageSpriteField field;
    QwSpriteFieldView view;
    QList<QwRealMobileSprite> rocks;
    QwSpritePixmapSequence *largeRockImg;
    QwSpritePixmapSequence *mediumRockImg;
    QwSpritePixmapSequence *smallRockImg;
    QList<KMissile> missiles;
    QwSpritePixmapSequence *missileImg;
    QList<KBit> bits;
    QwSpritePixmapSequence *bitImg;
    QwRealMobileSprite *ship;
    QwTextSprite *textSprite;

    bool rotateL;
    bool rotateR;
    bool thrustShip;
    bool shootShip;
    int  shootDelay;

    int  shipAngle;
    int  rotateSlow;

    double shipDx;
    double shipDy;

    int  textDy;

    double rockSpeed;
};

#endif

