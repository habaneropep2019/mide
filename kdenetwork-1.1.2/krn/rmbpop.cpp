#include "rmbpop.h"

#include "rmbpop.moc"

RmbPop::RmbPop(QWidget *_parent=0,const char *name=0)
{
    parent=_parent;
    if (parent)
        parent->installEventFilter(this);
    pop=new QPopupMenu(0,name);
}

bool RmbPop::eventFilter(QObject *,QEvent *e)
{
    if (e->type()==Event_MouseButtonPress)
    {
        QMouseEvent *m=(QMouseEvent *)e;
        if (m->button()==RightButton)
        {
            pop->move(parent->mapToGlobal(m->pos()));
            pop->show();
            return false;
        }
    }
    return false;
}
