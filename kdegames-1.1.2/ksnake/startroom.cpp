#include <stdlib.h>

#include <qpushbt.h>
#include <qlabel.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qwmatrix.h>
#include <qdstream.h>
#include <qfile.h>
#include <qstring.h>

#include <kapp.h>

#include "levels.h"
#include "startroom.h"
#include "lcdrange.h"


StartRoom::StartRoom( int init, int *newRoom, QWidget *parent, const char *name)
    : QDialog( parent, name, TRUE )
{

    setCaption(klocale->translate("Snake Race Starting Room"));

    nr = newRoom;

    QPushButton *okButton  = new QPushButton(this);
    okButton->setText(klocale->translate("OK"));
    okButton->setFixedSize(okButton->size());
    okButton->setGeometry(60,170, 100, 100);
    connect( okButton, SIGNAL(clicked()), SLOT(ok()) );

    QPushButton *cancelButton  = new QPushButton(this);
    cancelButton->setText(klocale->translate("Cancel"));
    connect( cancelButton, SIGNAL(clicked()), SLOT(accept()) );
    cancelButton->setFixedSize(cancelButton->size());
    cancelButton->setGeometry(180,170, 100, 100);

    roomRange = new LCDRange(this);
    roomRange->setRange(1, leV->max());
    roomRange->setText(klocale->translate("Starting Room"));
    roomRange->setFixedSize(100, 100);
    roomRange->setGeometry(60,30, 115, 125);

    picture = new QLabel(this);
    picture->setFrameStyle( QFrame::Panel | QFrame::Raised );
    picture->setGeometry(180,10, 115, 115);

    QFrame *separator = new QFrame(this);
    separator->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    separator->setGeometry( 20, 150, 300, 4 );

    connect( roomRange, SIGNAL(valueChanged(int)), SLOT(loadPixmap(int)));

    roomRange->setValue( init );
    loadPixmap( init );
    setFixedSize(340, 220);
}

void StartRoom::ok()
{
    *nr = roomRange->value();
    accept();
}

void StartRoom::loadPixmap(int i)
{
    QPixmap pixmap = leV->getPixmap(i);
    QWMatrix m;
    m.scale( (double)3, (double)3 );
    pixmap = pixmap.xForm( m );
    picture->setPixmap(pixmap);
}
