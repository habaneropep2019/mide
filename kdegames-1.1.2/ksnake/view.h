#ifndef VIEW_H
#define VIEW_H

#include <qwidget.h>

#include "trys.h"
#include "lcdrange.h"
#include "progress.h"
#include "rattler.h"

class View : public QWidget
{
    Q_OBJECT
public:
    View ( QWidget *parent=0, const char *name=0 );

    QLCDNumber *lcd;
    Trys *trys;
    Progress *pg;
    Rattler *rattler;

public slots:

protected:
    void resizeEvent( QResizeEvent * );
private:

};


#endif // VIEW_H
