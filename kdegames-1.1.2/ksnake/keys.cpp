#include <qpushbt.h>
#include <qlabel.h>
#include <qframe.h>
#include <qkeycode.h>
#include <qpixmap.h>
#include <qstring.h>

#include <kaccel.h>
#include <kapp.h>

#include "keys.h"

Keys::Keys( QWidget *parent, const char *name)
    : QDialog( parent, name, TRUE )
{

    QPushButton *okButton  = new QPushButton(this);
    okButton->setText(klocale->translate("OK"));
    okButton->setFixedSize(okButton->size());
    connect( okButton, SIGNAL(clicked()),this, SLOT(ok()) );
    okButton->move(20,210);

    QPushButton *defaultButton  = new QPushButton(this);
    defaultButton->setText(klocale->translate("Defaults"));
    defaultButton->setFixedSize(defaultButton->size());
    connect( defaultButton, SIGNAL(clicked()),this, SLOT(defaults()) );
    defaultButton->move(140,210);

    QPushButton *cancelButton  = new QPushButton(this);
    cancelButton->setText(klocale->translate("Cancel"));
    cancelButton->setFixedSize(cancelButton->size());
    connect( cancelButton, SIGNAL(clicked()),this, SLOT(reject()) );
    cancelButton->move(260,210);

    QFrame *separator = new QFrame(this);
    separator->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    separator->setGeometry( 20, 190, 340, 4 );

    for ( int x = 0; x < 4; x++) {
	QLabel *l = new QLabel(this);
	l->setAlignment(AlignCenter);
	labels[x] = l;
    }

    labels[0]->setGeometry(120, 20, 140, 20 );
    labels[1]->setGeometry(120,160, 140, 20 );
    labels[2]->setGeometry( 20, 92, 100, 20 );
    labels[3]->setGeometry(265, 92, 100, 20 );


    QString pixDir(KApplication::kde_datadir().copy());
    pixDir.append("/ksnake/pics/");

    QPushButton *up = new QPushButton(this);
    up->setPixmap( QPixmap((const char *)(pixDir + "up.xpm")));
    up->adjustSize();
    up->setFixedSize(up->size() );
    connect( up, SIGNAL(clicked()),this, SLOT(butUp()) );
    up->move(180, 50);

    QPushButton *down = new QPushButton(this);
    down->setPixmap( QPixmap((const char *)(pixDir + "down.xpm")));
    down->adjustSize();
    down->setFixedSize(down->size() );
    connect( down, SIGNAL(clicked()),this, SLOT(butDown()) );
    down->move(180, 130);

    QPushButton *left = new QPushButton(this);
    left->setPixmap( QPixmap((const char *)(pixDir + "left.xpm")));
    left->adjustSize();
    left->setFixedSize(left->size() );
    connect( left, SIGNAL(clicked()),this, SLOT(butLeft()) );
    left->move(140, 90);

    QPushButton *right = new QPushButton(this);
    right->setPixmap( QPixmap((const char *)(pixDir + "right.xpm")));
    right->adjustSize();
    right->setFixedSize(right->size() );
    connect( right, SIGNAL(clicked()),this, SLOT(butRight()) );
    right->move(220, 90);


    setCaption(klocale->translate("Change Direction Keys"));
    setFixedSize(380, 260);
    lab = 0;
    init();
}

void Keys::keyPressEvent( QKeyEvent *e )
{
	uint kCode = e->key() & ~(SHIFT | CTRL | ALT);
	QString string = keyToString(kCode);

	if (lab != 0) {
	    if ( string.isNull() )
		lab->setText(klocale->translate("Undefined key"));
	    else lab->setText(string);
	}
	else if ( lab == 0 && e->key() == Key_Escape)
	    reject();
}

void Keys::butUp()
{
    getKey(0);
}

void Keys::butDown()
{
    getKey(1);
}

void Keys::butLeft()
{
    getKey(2);
}

void Keys::butRight()
{
    getKey(3);
}

void Keys::getKey(int i)
{
    if ( lab != 0)
	focusOut(lab);

    focusIn(labels[i]);
}

void Keys::focusOut(QLabel *l)
{
    l->setFrameStyle( QFrame::NoFrame );
    l->setBackgroundColor(backgroundColor());
    l->repaint();
}

void Keys::focusIn(QLabel *l)
{
    lab = l;
    lab->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    lab->setBackgroundColor(white);
    lab->repaint();
}

void Keys::defaults()
{
    if ( lab != 0)
	focusOut(lab);

    lab = 0;

    labels[0]->setText(i18n("Up"));
    labels[1]->setText(i18n("Down"));
    labels[2]->setText(i18n("Left"));
    labels[3]->setText(i18n("Right"));
}

void Keys::init()
{
    KConfig *conf = kapp->getConfig();
    if(conf != NULL) {
	QString up("Up");
	up = conf->readEntry("upKey", (const char*) up);
	labels[0]->setText(up);

	QString down("Down");
	down = conf->readEntry("downKey", (const char*) down);
	labels[1]->setText(down);

	QString left("Left");
	left = conf->readEntry("leftKey", (const char*) left);
	labels[2]->setText(left);

	QString right("Right");
	right = conf->readEntry("rightKey", (const char*) right);
	labels[3]->setText(right);
    }
}

void Keys::ok()
{
    KConfig *conf = kapp->getConfig();
    if(conf != NULL) {
	conf->writeEntry("upKey",   (const char*) labels[0]->text() );
	conf->writeEntry("downKey", (const char*) labels[1]->text() );
	conf->writeEntry("leftKey", (const char*) labels[2]->text() );
	conf->writeEntry("rightKey",(const char*) labels[3]->text() );
    }

    accept();
}
