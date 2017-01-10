/**************************************************************************

    channel.h  - The KMidChannel widget (with pure virtual members)
    Copyright (C) 1998  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Send comments and bug fixes to antlarr@arrakis.es
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/

#ifndef KMIDCHANNEL_H
#define KMIDCHANNEL_H
#include <qwidget.h>
#include <qpixmap.h>

#define CHANNELHEIGHT 71
#define KEYBOARDY CHANNELHEIGHT-46

#define CHN_CHANGE_PGM 1
#define CHN_CHANGE_FORCED_STATE 2

class QFont;
class KCombo;
class KMidButton;
class KMidChannel : public QWidget
{
    Q_OBJECT

private:
    bool pressed[128]; // The 128 keys
    int channel;
    bool replay; // Indicates if music should restart playing after
    // changing the force state
    
protected:
    QPixmap *keyboard;
    QPixmap *button1;
    QPixmap *button2;

    KCombo *instrumentCombo;

    KMidButton *forcepgm;
    QFont *qcvfont;

    QPen *penB; // Black
    QPen *penW; // White
    QPen *penT; // "Transparent" for Background
    
    void paintEvent( QPaintEvent * );
public:
    KMidChannel(int i,QWidget *Parent);
    virtual ~KMidChannel();

    void drawKeyboard(QPainter *qpaint);
    void drawPressedKeys(QPainter *qpaint);
    void drawKey(QPainter *qpaint,int key);

    virtual void drawDo   (QPainter *qpaint,int x,int p) = 0;
    virtual void drawDo__ (QPainter *qpaint,int x,int p) = 0;
    virtual void drawRe   (QPainter *qpaint,int x,int p) = 0;
    virtual void drawRe__ (QPainter *qpaint,int x,int p) = 0;
    virtual void drawMi   (QPainter *qpaint,int x,int p) = 0;
    virtual void drawFa   (QPainter *qpaint,int x,int p) = 0;
    virtual void drawFa__ (QPainter *qpaint,int x,int p) = 0;
    virtual void drawSol  (QPainter *qpaint,int x,int p) = 0;
    virtual void drawSol__(QPainter *qpaint,int x,int p) = 0;
    virtual void drawLa   (QPainter *qpaint,int x,int p) = 0;
    virtual void drawLa__ (QPainter *qpaint,int x,int p) = 0;
    virtual void drawSi   (QPainter *qpaint,int x,int p) = 0;

    void noteOn(int key);
    void noteOff(int key);
    void changeInstrument(int pgm);
    void changeForceState(bool i);
    
    void reset(int level=1); // 0 only release notes, 1 also set instr to 0 ...

    void saveState(bool *p,int *pgm);
    void loadState(bool *p,int *pgm);
    
public slots:
    void pgmChanged(int i);
    void changeForcedState(bool);

signals:
    void signalToKMidClient(int *data);
    
};

#endif
