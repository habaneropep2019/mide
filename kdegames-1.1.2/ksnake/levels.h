#ifndef LEVELS_H
#define LEVELS_H

#include <qfile.h>
#include <qstring.h>

#include <qpixmap.h>
#include <qimage.h>



class Levels
{
public:
    Levels ();
    QImage  getImage(int);
    QPixmap getPixmap(int);
    int max();
private:
    QList<QString> list;
};


extern Levels *leV;

#endif // LEVELS_H
