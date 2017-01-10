/**************************************************************************

    klcdnumber.h  - The KLCDNumber widget (displays a lcd number)
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
#include <qwidget.h>

class QPainter;
class KTriangleButton;
class QColor;

class KLCDNumber : public QWidget
{
    Q_OBJECT
protected:
    struct digit {
        bool up;  
        bool nw;  
        bool ne;
        bool md;
        bool sw;
        bool se;
        bool bt;
    };
    /*
        up
        ---
     nw|   |ne
       |___|<------ md
       |   |
     sw|___|se
        bt
     */
    

    KLCDNumber::digit Digit[11];
/*
={
        / 0 /    {true,true,true,false,true,true,true},
        / 1 /    {false,false,true,false,false,true,false},
        / 2 /    {true,false,true,true,true,false,true},
        / 3 /    {true,false,true,true,false,true,true},
        / 4 /    {false,true,true,true,false,true,false},
        / 5 /    {true,true,false,true,false,true,true},
        / 6 /    {true,true,false,true,true,true,true},
        / 7 /    {true,false,true,false,false,true,false},
        / 8 /    {true,true,true,true,true,true,true},
        / 9 /    {true,true,true,true,false,true,true},
        /   /    {false,false,false,false,false,false,false}
    };*/
    
    int numDigits;
    bool setUserChangeValue;
    bool setUserDefaultValue;
    bool doubleclicked;

    QColor backgcolor;
    QColor LCDcolor;
    
    double value;
    double oldvalue;
    double defaultValue;

    double minValue;
    double maxValue;

    void drawVerticalBar(QPainter *qpaint,int x,int y,int w,int h,int d);
    void drawHorizBar(QPainter *qpaint,int x,int y,int w,int h,int d);
    void drawDigit(QPainter *qpaint,int x,int y,int w,int h,digit d);

    void initDigits(void);

public:
    KLCDNumber(int _numDigits,QWidget *parent,char *name);
    KLCDNumber(bool _setUserChangeValue,int _numDigits,QWidget *parent,char *name);

    void setUserSetDefaultValue(bool _userSetDefaultValue);
    void setDefaultValue(double v);
    
    void setValue(double v);
    double getValue(void) { return value; };
    double getOldValue(void) { return oldvalue; };

    double getMinValue(void) { return minValue;};
    double getMaxValue(void) { return maxValue;};
    void setRange(double min, double max);

    void setLCDBackgroundColor (int r,int g,int b);
    void setLCDColor (int r,int g,int b);

    void display (int v);
    void display (double v);

protected:

    virtual void paintEvent ( QPaintEvent *e );
    virtual void resizeEvent ( QResizeEvent *e);
    virtual void mouseDoubleClickEvent (QMouseEvent *e);
    virtual void mousePressEvent (QMouseEvent *e);
    virtual void timerEvent(QTimerEvent *e);
    void defaultValueClicked();
    
    KTriangleButton *downBtn;
    KTriangleButton *upBtn;


    
public slots:

    void decreaseValue();
    void increaseValue();
    void decreaseValueFast();
    void increaseValueFast();

signals:

    void valueChanged(double v);
    
};
