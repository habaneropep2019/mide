#ifndef TL_WIDGET_H
#define TL_WIDGET_H

#include <qobject.h>
#include <qwidget.h>
#include <qpixmap.h>

// Simple RemAdmin object, should be inherited to be used
// since it doesn't really do anything.

class TLObj
{
public:
    /** Constructor */
    TLObj();
    TLObj(const char *ID);
    virtual ~TLObj(){};
    /** Dumps object contents */
    QWidget *widget;
    int width;
    int height;
    int align;
    int vslack;
    int hslack;
    QString path;
    QString ID;
};

#endif