#ifndef TRYS_H
#define TRYS_H

#include <qwidget.h>
#include <qpixmap.h>


class Trys : public QWidget
{
    Q_OBJECT
public:
    Trys ( QWidget *parent=0, const char *name=0 );
public slots:
    void set(int);
protected:
    void  paintEvent( QPaintEvent * );
private:
    QPixmap pixmap;
    int snakes;
};


#endif // TRYS_H
