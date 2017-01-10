/*
 *   kmoon - a moon phase indicator
 *   $Id: kmoon.cpp,v 1.2 1998/12/13 20:04:19 ettrich Exp $
 *   Copyright (C) 1998  Stephan Kulow
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <kapp.h>
#include <qwidget.h>
#include "kmoon.moc"
#include <kwm.h>
#include <qwmatrix.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <kmsgbox.h>
#include <qimage.h>
#include <stdlib.h>

QList<QImage> *MoonWidget::moons = 0;

double moonphasebylunation(int lun, int phi);
time_t JDtoDate(double jd, struct tm *event_date);

MoonWidget::MoonWidget(QWidget *parent, const char *name)
    : QWidget(parent, name)
{
    struct tm * t;
    time_t clock;

    if (!moons)
	loadMoons();
    counter = -1;
    old_w = old_h = old_counter = -1;
    startTimer(100000);

    time(&clock);
    t = localtime(&clock);
    calcStatus(mktime(t));
    renderGraphic();
    KWM::setDockWindow(winId());
    kapp->setTopWidget(new QWidget());
    popup = new QPopupMenu();
    popup->insertItem(i18n("About"));
    popup->insertSeparator();
    popup->insertItem(i18n("Quit"));

    connect(popup, SIGNAL(activated(int)), SLOT(reactOn(int)));

}

void MoonWidget::reactOn(int index)
{
    if (index == 0)
	showAbout();
    if (index == 2)
	kapp->quit();
}

void MoonWidget::calcStatus( time_t time )
{
    double JDE;
    uint lun = 0;
    struct tm event_date;
    struct tm last_event;
    time_t last_new, next_new;

    do {
	last_event = event_date;
	JDE = moonphasebylunation(lun, 0);
	next_new = last_new;
	last_new = JDtoDate(JDE, &event_date);
	lun++;
    } while (last_new < time);

    counter = (time - next_new) / ( 60 * 60 * 24);
    if (counter >= 29)
	counter -= 29;
}

void MoonWidget::loadMoons()
{
    moons = new QList<QImage>();

    for (int i = 1; i <= 29; i++) {
	QString name;
	name.sprintf("%s/kmoon/pics/moon%d.gif",
		     KApplication::kde_datadir().data(),
		     i);
	QImage *p = new QImage(name);
	moons->append(p);
    }
	
}

void MoonWidget::showAbout()
{
    KMsgBox::message(0, i18n("About Moon Phase Indicator"),
		     i18n("Moon Phase Indicator for KDE\n\n"
			  "Written by Stephan Kulow <coolo@kde.org>\n"
			  "\n"
			  "Lunar code by Chris Osburn "
			  "<chris@speakeasy.org>\n"
			  "\n"
			  "Moon graphics by Tim Beauchamp "
			  "<timb@googol.com>"));
}

void MoonWidget::timerEvent( QTimerEvent * )
{
    struct tm * t;
    time_t clock;
    time(&clock);
    t = localtime(&clock);
    calcStatus(mktime(t));
    renderGraphic();
    repaint( false );
}
	
void MoonWidget::paintEvent( QPaintEvent * )
{
    renderGraphic();
    bitBlt(this, 0, 0, &pixmap, 0, 0);
}

void MoonWidget::resizeEvent( QResizeEvent *)
{
    renderGraphic();
    repaint();
}

void MoonWidget::renderGraphic()
{
    if (old_counter == counter && old_w == width() && old_h == height())
	return;
    old_counter = counter;
    old_w = width();
    old_h = height();
    QPixmap t;
    t.convertFromImage(*moons->at(counter), AvoidDither);
    QWMatrix m;
    m.scale(float(width()) / t.width(),
	    float(height()) / t.height());
    QImage im = t.xForm(m).convertToImage().convertDepth(8, AvoidDither);

    QColor col1 = yellow;
    QColor col2 = kapp->backgroundColor;

    int r1 = col1.red(), r2 = col2.red();
    int g1 = col1.green(), g2 = col2.green();
    int b1 = col1.blue(), b2 = col2.blue();

    for (int i = 0; i < im.numColors(); i++) {
	int grey = qRed(im.color(i));
	im.setColor(i, qRgb(int(float(r1 - r2) / 255 * grey) + r2,
			    int(float(g1 - g2) / 255 * grey) + g2,
			    int(float(b1 - b2) / 255 * grey) + b2));
    }
    pixmap.convertFromImage(im, AvoidDither);

    QToolTip::remove(this);
    tooltip.sprintf(i18n("Moon is %d days old."), counter + 1);
    QToolTip::add(this, tooltip);
}

void MoonWidget::mousePressEvent( QMouseEvent *e)
{
    if (e->button() == RightButton) {
	popup->popup(mapToGlobal(e->pos()));
	popup->exec();
    }
    if (e->button() == LeftButton) {
	showAbout();
    }
}

int main( int argc, char **argv)
{
    KApplication a(argc, argv);
    MoonWidget *moon = new MoonWidget();
    kapp->enableSessionManagement( TRUE );
    kapp->setTopWidget(new QWidget);
    a.setMainWidget(moon);
    moon->show();
    if (argc > 1)
	moon->calcStatus(time(0) + atoi(argv[1]) * 24 * 60 * 60);
    return a.exec();
}
