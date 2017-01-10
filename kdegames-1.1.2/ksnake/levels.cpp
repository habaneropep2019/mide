#include <qdir.h>
#include <qstring.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qbitmap.h>

#include <kapp.h>

#include "levels.h"

Levels *leV = 0;

Levels::Levels()
{

    leV = this;

    QString levelDir;
    levelDir.setStr(KApplication::kde_datadir().copy());
    levelDir.append("/ksnake/levels/");

    QDir d(levelDir);
    d.setFilter( QDir::Files );
    d.setSorting( QDir::Name );

    QString *s = new QString("dummy");
    list.insert(0, s);

    for ( unsigned int i=0; i<d.count(); i++ ) {
	QString *s = new QString;
	s->sprintf("%s/%s",(const char*)d.absPath(),  d[i] );
	list.append(s);
    }

    d.cd (QDir::homeDirPath() );
    if ( d.cd(".kde/share/apps/ksnake") ) {
	for ( unsigned int i=0; i<d.count(); i++ ) {
	    QString *s = new QString;
	    s->sprintf("%s/%s",(const char*)d.absPath(),  d[i] );
	    list.append(s);
	}
    }

}

int Levels::max()
{
    return ( list.count() -1 );
}

QImage Levels::getImage(int at)
{
    QString *path = list.at(at);
    QBitmap bitmap(path->data());
    QImage image = bitmap.convertToImage();
    return image;
}

QPixmap Levels::getPixmap(int at)
{
    QString *path = list.at(at);
    QPixmap pixmap(path->data());
    return pixmap;
}
