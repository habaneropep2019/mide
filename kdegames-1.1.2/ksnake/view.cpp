#include <qlcdnum.h>

#include "trys.h"
#include "progress.h"
#include "view.h"
#include "rattler.h"

View::View( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    lcd  = new QLCDNumber( this);
    lcd->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    trys = new Trys(this);
    pg = new Progress(this);
    rattler = new Rattler( this);

    setFixedSize(560, 560+68);
}

void View::resizeEvent( QResizeEvent * )
{
    lcd->setGeometry(420, 5, 135, 42);
    trys->setGeometry(0, 0, 405, 50);
    pg->setGeometry(5, 52, 550, 12);
    rattler->setGeometry(0, 68, rattler->width(), rattler->height());
}
