/*
 *   kmoon - a moon phase indicator
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

#include <qwidget.h>
#include <qimage.h>
#include <sys/types.h>
#include <time.h>

class MoonWidget : public QWidget
{
    Q_OBJECT

public:
    MoonWidget(QWidget *parent = 0, const char *name = 0);
    void calcStatus( time_t time );

protected:
    static QList<QImage> *moons;
    static void loadMoons();
    int counter;
    QPopupMenu *popup;
    QPixmap pixmap;
    int old_counter;
    int old_w, old_h;
    QString tooltip;

protected slots:
   
    void timerEvent( QTimerEvent *e);
    void showAbout();
    void reactOn(int);
    
protected:
    void renderGraphic();
    virtual void paintEvent( QPaintEvent *e);
    virtual void resizeEvent( QResizeEvent *e);
    virtual void mousePressEvent( QMouseEvent *e);
};
