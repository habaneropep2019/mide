#ifndef BASKET_H
#define BASKET_H

#include <qobject.h>

#include <qpixmap.h>
#include <qwidget.h>
#include <qlist.h>

#include "board.h"
#include "pixServer.h"

enum Fruits { Red, Golden };

class Kaffee : public QObject
{
    Q_OBJECT
public:
    Kaffee(int pos);
    int position() { return p;}
    Fruits type() { return t;}
    bool dirty;
private slots:
    void golden();
private:
    int p;
    Fruits t;
};

class Basket : public QObject
{
    Q_OBJECT
public:
    Basket(Board *b, PixServer *p);
    void repaint(bool);
    void newApples();
    void clear();
    Fruits eaten( int i);
signals:
    void openGate();
private:
    Board   *board;
    PixServer *pixServer;
    QList<Kaffee> *list;
};

#endif // BASKET_H
