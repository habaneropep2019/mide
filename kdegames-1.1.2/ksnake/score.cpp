#include <stdlib.h>

#include <qobject.h>
#include <qdstream.h>
#include <qfile.h>
#include <qstring.h>
#include <qpushbt.h>
#include <qframe.h>
#include <qlabel.h>
#include <qgrpbox.h>
#include <qdatetm.h>
#include <qlined.h>

#include <kapp.h>

#include "score.h"


Score::Score( QWidget *parent, const char *name )
    :QObject( parent, name )
{
    QString libDir;
    libDir.setStr(KApplication::kde_datadir().copy());
    libDir.append("/ksnake/");
    file.setName( libDir + "highScores");
    playerName = getenv("LOGNAME");
    read();
}

QString Score::formatDate(int d, int m, int y)
{
    QString s;
    s.sprintf("%d/%d/%d", d, m, y);
    if(d < 10)
	s.insert(0,'0');
    if(m < 10)
	s.insert(3,'0');
    s.remove(6,2);

    return s;
}

void Score::display(int newHall = -1, int newToday = -1)
{
    QDialog *dlg = new QDialog(0, "Hall Of Fame", TRUE);
    dlg->setCaption(klocale->translate("Snake Race High Scores"));

    ScoreBox *sb1 = new ScoreBox(dlg, 0, newHall);
    sb1->setGeometry(10, 10, 400, 225);
    sb1->setTitle(klocale->translate("Hall of Fame"));

    ScoreBox *sb2 = new ScoreBox(dlg, 0, newToday);
    sb2->setGeometry(10, 240, 400, 225);
    sb2->setTitle(klocale->translate("Today's High Scores"));

    QPushButton *b = new QPushButton( dlg);
    b->setText(klocale->translate("OK"));
    b->setAutoDefault(TRUE);
    b->setFocus();
    b->move(300, 480);
    connect( b, SIGNAL(clicked()),dlg, SLOT(accept()) );

    for ( int x = 0; x < 5; x++) {
	QString s = formatDate( hall[x].date.day(),
				hall[x].date.month(), hall[x].date.year());
	sb1->setScore(x, hall[x].points, hall[x].player, s.data());
	sb2->setScore(x, today[x].points, today[x].player, "");
    }

    dlg->exec();
    delete dlg;
}

QString Score::getPlayerName() {
    QDialog *dlg = new QDialog(0, "Hall Of Fame", TRUE);
    dlg->resize(300, 175);
    dlg->setCaption(klocale->translate("Snake Race High Scores"));
    QLabel *label  = new QLabel(klocale->translate("you have achieved a high score!\n please enter your name"),
				dlg);
    label->setAlignment(AlignCenter);
    label->setFont( QFont( "Times", 16, QFont::Bold ) );

    QLineEdit *le = new QLineEdit(dlg);
    le->setFocus();
    le->setText(playerName);
    le->selectAll();

    QFrame *sep = new QFrame( dlg);
    sep->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    QPushButton *b = new QPushButton(klocale->translate("OK"), dlg);
    b->setDefault(TRUE);
    b->setAutoDefault(TRUE);
    connect(b, SIGNAL(released()), dlg, SLOT(accept()));
    connect(le, SIGNAL(returnPressed()), dlg, SLOT(accept()));

    label->setGeometry(0, 0, 300, 50 );
    le->setGeometry(50, 65, 200, 25 );
    sep->setGeometry(0, 100, 400, 25 );
    b->setGeometry(110, 125, 80, 32 );

    dlg->exec();

    QString s = le->text();
    delete dlg;
    return s;
}

void Score::setScore(int s)
{
    read();

    bool checkHall = FALSE;
    bool checkToday = FALSE;
    int x, xx;

    for ( x = 0; x < 5; x++)
	if ( s > hall[x].points) {
	    checkHall = TRUE;
	    break;
	}

    for ( xx = 0; xx < 5; xx++)
	if ( s > today[xx].points) {
	    checkToday = TRUE;
	    break;
	}

    if (checkHall == TRUE || checkToday == TRUE) {

	playerName  = getPlayerName();

	if (checkHall) {
	    for (int i = 4; i > x && i > 0; i--)
		hall[i] = hall[i-1];

	    hall[x].points = s;
	    hall[x].player = playerName.data() ;
	    hall[x].date = QDate::currentDate();
	} else
	    x = -1;

	if (checkToday) {
	    for (int i = 4; i > xx && i > 0; i--)
		today[i] = today[i-1];

	    today[xx].points = s;
	    today[xx].player =  playerName.data();
	    today[xx].date = QDate::currentDate();
	} else
	    xx = -1;

	display(x, xx);
	write();
    }
}

void Score::read()
{

    if ( file.exists() ) {
	if (file.open( IO_ReadOnly )) {
	    QDataStream s( &file );
	    for ( int x = 0; x < 5; x++)
		s >> hall[x].points >> hall[x].player >> hall[x].date;
	    for ( int x = 0; x < 5; x++) {
		s >> today[x].points >> today[x].player >> today[x].date;
		if ( today[x].date != QDate::currentDate())
		    today[x].points = 0;
	    }
	    file.close();
	}
    } else {
	for( int x = 0; x < 5; x++) {
	    hall[x].points = 0;
	    hall[x].player = "";
	    hall[x].date = QDate::currentDate();

	    today[x].points = 0;
	    today[x].player = "";
	    today[x].date = QDate::currentDate();
	}

	write();
    }
}

void Score::write()
{

    if (file.open( IO_WriteOnly )) {
	QDataStream s( &file );
	for ( int x = 0; x < 5; x++)
	    s << hall[x].points << hall[x].player << hall[x].date;
	for ( int x = 0; x < 5; x++)
	    s << today[x].points << today[x].player << today[x].date;
	file.close();
    }
}

ScoreBox::ScoreBox( QWidget *parent, const char *name , int current)
    : QWidget( parent, name )
{
    box = new QGroupBox(this);

    label = new QLabel(this);
    label->setFrameStyle( QFrame::Panel   | QFrame::Raised );
    label->setFont( QFont( "Times", 18, QFont::Bold ) );
    label->setAlignment( AlignCenter );

    for ( int x = 0; x < 5; x++) {

	QLabel *l = new QLabel(this);
	CHECK_PTR(l);
	l->setFont( QFont( "Times", 16, QFont::Bold ) );
	l->setAlignment( AlignRight );
	date[x] = l;

	QLabel *l1 = new QLabel(this);
	CHECK_PTR(l1);
	l1->setFont( QFont( "Times", 16, QFont::Bold ) );
	l1->setAlignment( AlignRight );
	points[x] = l1;

	QLabel *l2 = new QLabel(this);
	CHECK_PTR(l2);
	l2->setFont( QFont( "Times", 16, QFont::Bold ) );
	l2->setAlignment( AlignLeft );
	player[x] = l2;
    }

    if (current >= 0) {
	QColorGroup colgrp(black, backgroundColor(), black,
			   black, black, blue, black);
	date[current]->setPalette( QPalette(colgrp,colgrp,colgrp) );
	points[current]->setPalette( QPalette(colgrp,colgrp,colgrp) );
	player[current]->setPalette( QPalette(colgrp,colgrp,colgrp) );
    }
}

void ScoreBox::setTitle( const char *s )
{
    label->setText( s );
}

void ScoreBox::setScore( int x, int p, const char * pl, const char *dt)
{
    if (x > 5)
	return;

    if (p == 0) {
	points[x]->setText("");
	player[x]->setText("");
    }
    else {
	QString s;
	s.sprintf("%d",p);
	date[x]->setText(dt);
	points[x]->setText(s.data());
	player[x]->setText(pl);
    }
}

void ScoreBox::resizeEvent( QResizeEvent * )
{
    box->resize( width(), height() - 25 );
    box->setGeometry(0, 25, width(), height()-25 );

    label->setGeometry(100, 10, 200, 30 );

    int p = 65;
    for ( int x = 0; x < 5; x++) {
	date[x]->setGeometry(25, p, 80, 25);
	points[x]->setGeometry(105, p, 70, 25);
	player[x]->setGeometry(195, p, 170, 25);
	p+= 30;
    }
}
