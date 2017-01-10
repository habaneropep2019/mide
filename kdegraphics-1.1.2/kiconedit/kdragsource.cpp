#include <qwidget.h>
#include <qimage.h>
#include <qstring.h>
#include <qevent.h>
#include <kiconloader.h>
#include <kapp.h>
#include "kdragsource.h"

KDragSource::KDragSource( const char *dragtype, QObject *dataprovider, const char *method, 
                                               QWidget *parent, const char * name )
    : QLabel( "DragSource", parent, name )
{
  provider = dataprovider;
  type = dragtype;
  if(type.left(6) == "image/")
  {
    ok = connect(this, SIGNAL(getimage(QImage*)), provider, method);
  }
  else debug("Unknown datatype: %s", dragtype);
}

KDragSource::~KDragSource()
{
}

void KDragSource::mousePressEvent( QMouseEvent * /*e*/ )
{
  debug("KDragSource::mousePressEvent");

  if(!ok) return;

  if(type.left(6) == "image/")
  {
    debug("Type: image");
    QImage img;
    emit getimage(&img);
    if(img.isNull())
      debug("Bad image");
    else
    {
      QImageDrag *di = new QImageDrag( img, this );
      debug("KDragSource::mousePressEvent - before dragCopy");
#if QT_VERSION > 140
      QPixmap pm(Icon("image.xpm"));
      QPoint pp(pm.width()/2, pm.height()/2);
      di->setPixmap(pm, pp);
#endif
      di->dragCopy();
    }
  }
  else debug("Unknown datatype: %s", type.data());

  debug("KDragSource::mousePressEvent - done");
}

void KDragSource::mouseMoveEvent( QMouseEvent * /*e*/ )
{
}
