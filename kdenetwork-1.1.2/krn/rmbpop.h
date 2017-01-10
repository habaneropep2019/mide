#ifndef RMBPOP_H
#define RMBPOP_H

#include <qwidget.h>
#include <qpopmenu.h>

class RmbPop:public QWidget
{
    Q_OBJECT
public:
    RmbPop(QWidget *_parent=0, const char *name=0);
    bool eventFilter(QObject *o, QEvent *e);
    QPopupMenu *pop;
    QWidget *parent;
};


#endif