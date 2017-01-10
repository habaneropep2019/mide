#include <qwidget.h>
#include <qpixmap.h>
#include <qlist.h>

#include <stdlib.h>

#include "basket.h"
#include "board.h"
#include "pixServer.h"
#include "qtimer.h"

Kaffee::Kaffee(int pos)
{
    p = pos;
    t = Red;
    int r = random() % 40000;
    QTimer::singleShot( r, this, SLOT(golden()) );
    dirty = TRUE;
}

void Kaffee::golden()
{
    dirty = TRUE;
    t = (t == Red ? Golden : Red);
    int r = random() % 40000;
    QTimer::singleShot( r, this, SLOT(golden()) );
}

Basket::Basket(Board *b, PixServer *p)
{
    board = b;
    pixServer = p;
    list = new QList<Kaffee>;
    list->setAutoDelete( TRUE );
}

void Basket::clear()
{
    if( !list->isEmpty())
	list->clear();
}

void Basket::newApples()
{
    int x;
    int i = 0;

    while(i < 10) {
	x =  random() % board->size();
	if ((unsigned)x < board->size() && board->isEmpty(x) && x > BoardWidth+4) {
	    Kaffee *g = new Kaffee(x);
	    board->set(x, Apple);
	    list->append(g);
	    i++;
	}
    }
}

void Basket::repaint(bool dirty )
{
    Kaffee *g;
    for ( g  = list->first(); g != 0; g = list->next()) {
	if (dirty) {
	    pixServer->draw(g->position(), ApplePix, (int)g->type());
	    if (g->dirty)
		g->dirty = FALSE;
	}
	else
	    if (g->dirty) {
		pixServer->draw(g->position(), ApplePix, (int)g->type());
		g->dirty = FALSE;
	    }
    }
}

Fruits Basket::eaten(int i)
{
    Kaffee *g;
    Fruits f = Red;

    for (g = list->first(); g != 0; g = list->next() )
	{
	    if (g->position() == i) {
		f = g->type();
		list->remove(g);
		break;
	    }
	}
    if (list->isEmpty())
	emit openGate();

    return f;
}
