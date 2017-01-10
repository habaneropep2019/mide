/**************************************************************************

    channel4d.cpp  - KMidChannel4D widget, with 3D look and filled
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

#include "channel4d.h"
#include <qpainter.h>

KMidChannel4D::KMidChannel4D  (int chn,QWidget *parent) : KMidChannel(chn,parent)
{
    brushR = new QBrush (red);
    brushW = new QBrush (white);
    brushB = new QBrush (black);
    penR   = new QPen   (red);
};


void KMidChannel4D::drawDo(QPainter *qpaint,int x,int p)
{
    qpaint->setPen(p?(*penR):(*penB));
    
    qpaint->drawLine(x,KEYBOARDY+43,x+6,KEYBOARDY+43);
    qpaint->drawLine(x,KEYBOARDY+44,x+6,KEYBOARDY+44);
    
    qpaint->setPen(p?(*penB):(*penT));
    qpaint->drawLine(x-1,KEYBOARDY+45,x+8,KEYBOARDY+45);
    qpaint->setPen(p?(*penB):(*penW));
    qpaint->drawLine(x+7,KEYBOARDY+28,x+7,KEYBOARDY+42);

    qpaint->fillRect(x,KEYBOARDY+28,7,15,((p)?*brushR:*brushW));
    qpaint->fillRect(x,KEYBOARDY+1,5,27,((p)?*brushR:*brushW));
    qpaint->setPen(p?(*penR):(*penW));
    qpaint->drawPoint(x+5,KEYBOARDY+27);
};
void KMidChannel4D::drawDo__(QPainter *qpaint,int x,int p)
{
    qpaint->fillRect(x+6,KEYBOARDY+1,5,26,((p)?*brushR:*brushB));

    if (!p)
    {
        qpaint->setPen(*penW);
        qpaint->drawLine(x+6,KEYBOARDY+2,x+6,KEYBOARDY+25);
        qpaint->drawLine(x+7,KEYBOARDY+25,x+11,KEYBOARDY+25);
    };
    
    qpaint->setPen(p?(*penW):(*penB));
    qpaint->drawLine(x+11,KEYBOARDY+1,x+11,KEYBOARDY+27);
    qpaint->drawPoint(x+10,KEYBOARDY+27);


};
void KMidChannel4D::drawRe(QPainter *qpaint,int x,int p)
{
    qpaint->setPen(p?(*penR):(*penB));
    qpaint->drawLine(x+9,KEYBOARDY+43,x+15,KEYBOARDY+43);
    qpaint->drawLine(x+9,KEYBOARDY+44,x+15,KEYBOARDY+44);
    qpaint->setPen(p?(*penB):(*penT));
    qpaint->drawLine(x+8,KEYBOARDY+45,x+17,KEYBOARDY+45);
    qpaint->setPen(p?(*penB):(*penW));
    qpaint->drawLine(x+16,KEYBOARDY+28,x+16,KEYBOARDY+42);

    qpaint->fillRect(x+9,KEYBOARDY+28,7,15,((p)?*brushR:*brushW));
    qpaint->fillRect(x+12,KEYBOARDY+1,2,27,((p)?*brushR:*brushW));
    qpaint->setPen(p?(*penR):(*penW));
    qpaint->drawPoint(x+11,KEYBOARDY+27);
    qpaint->drawPoint(x+14,KEYBOARDY+27);

};

void KMidChannel4D::drawRe__(QPainter *qpaint,int x,int p)
{
    qpaint->fillRect(x+15,KEYBOARDY+1,5,26,((p)?*brushR:*brushB));

    if (!p)
    {
        qpaint->setPen(*penW);
        qpaint->drawLine(x+15,KEYBOARDY+2,x+15,KEYBOARDY+25);
        qpaint->drawLine(x+16,KEYBOARDY+25,x+20,KEYBOARDY+25);
    };

    qpaint->setPen(p?(*penW):(*penB));
    qpaint->drawLine(x+20,KEYBOARDY+1,x+20,KEYBOARDY+27);
    qpaint->drawPoint(x+19,KEYBOARDY+27);


};
void KMidChannel4D::drawMi(QPainter *qpaint,int x,int p)
{
    qpaint->setPen(p?(*penR):(*penB));
    qpaint->drawLine(x+18,KEYBOARDY+43,x+24,KEYBOARDY+43);
    qpaint->drawLine(x+18,KEYBOARDY+44,x+24,KEYBOARDY+44);
    qpaint->setPen(p?(*penB):(*penT));
    qpaint->drawLine(x+17,KEYBOARDY+45,x+26,KEYBOARDY+45);
    qpaint->setPen(p?(*penB):(*penW));
    qpaint->drawLine(x+25,KEYBOARDY+1,x+25,KEYBOARDY+42);

    qpaint->fillRect(x+18,KEYBOARDY+28,7,15,((p)?*brushR:*brushW));
    qpaint->fillRect(x+21,KEYBOARDY+1,4,27,((p)?*brushR:*brushW));
    qpaint->setPen(p?(*penR):(*penW));
    qpaint->drawPoint(x+20,KEYBOARDY+27);

};
void KMidChannel4D::drawFa(QPainter *qpaint,int x,int p)
{
    qpaint->setPen(p?(*penR):(*penB));
    qpaint->drawLine(x+27,KEYBOARDY+43,x+33,KEYBOARDY+43);
    qpaint->drawLine(x+27,KEYBOARDY+44,x+33,KEYBOARDY+44);
    qpaint->setPen(p?(*penB):(*penT));
    qpaint->drawLine(x+26,KEYBOARDY+45,x+35,KEYBOARDY+45);
    qpaint->setPen(p?(*penB):(*penW));
    qpaint->drawLine(x+34,KEYBOARDY+28,x+34,KEYBOARDY+42);

    qpaint->fillRect(x+27,KEYBOARDY+28,7,15,((p)?*brushR:*brushW));
    qpaint->fillRect(x+27,KEYBOARDY+1,5,27,((p)?*brushR:*brushW));
    qpaint->setPen(p?(*penR):(*penW));
    qpaint->drawPoint(x+32,KEYBOARDY+27);

};
void KMidChannel4D::drawFa__(QPainter *qpaint,int x,int p)
{
    qpaint->fillRect(x+33,KEYBOARDY+1,5,26,((p)?*brushR:*brushB));
    if (!p)
    {
        qpaint->setPen(*penW);
        qpaint->drawLine(x+33,KEYBOARDY+2,x+33,KEYBOARDY+25);
        qpaint->drawLine(x+34,KEYBOARDY+25,x+38,KEYBOARDY+25);
    };
    qpaint->setPen(p?(*penW):(*penB));
    qpaint->drawLine(x+38,KEYBOARDY+1,x+38,KEYBOARDY+27);
    qpaint->drawPoint(x+37,KEYBOARDY+27);

};

void KMidChannel4D::drawSol(QPainter *qpaint,int x,int p)
{
    qpaint->setPen(p?(*penR):(*penB));
    qpaint->drawLine(x+36,KEYBOARDY+43,x+42,KEYBOARDY+43);
    qpaint->drawLine(x+36,KEYBOARDY+44,x+42,KEYBOARDY+44);
    qpaint->setPen(p?(*penB):(*penT));
    qpaint->drawLine(x+35,KEYBOARDY+45,x+44,KEYBOARDY+45);
    qpaint->setPen(p?(*penB):(*penW));
    qpaint->drawLine(x+43,KEYBOARDY+28,x+43,KEYBOARDY+42);

    qpaint->fillRect(x+36,KEYBOARDY+28,7,15,((p)?*brushR:*brushW));
    qpaint->fillRect(x+39,KEYBOARDY+1,2,27,((p)?*brushR:*brushW));
    qpaint->setPen(p?(*penR):(*penW));
    qpaint->drawPoint(x+38,KEYBOARDY+27);
    qpaint->drawPoint(x+41,KEYBOARDY+27);

};

void KMidChannel4D::drawSol__(QPainter *qpaint,int x,int p)
{
    qpaint->fillRect(x+42,KEYBOARDY+1,5,26,((p)?*brushR:*brushB));

    if (!p)
    {
        qpaint->setPen(*penW);
        qpaint->drawLine(x+42,KEYBOARDY+2,x+42,KEYBOARDY+25);
        qpaint->drawLine(x+43,KEYBOARDY+25,x+47,KEYBOARDY+25);
    };
    qpaint->setPen(p?(*penW):(*penB));
    qpaint->drawLine(x+47,KEYBOARDY+1,x+47,KEYBOARDY+27);
    qpaint->drawPoint(x+46,KEYBOARDY+27);


};
void KMidChannel4D::drawLa(QPainter *qpaint,int x,int p)
{
    qpaint->setPen(p?(*penR):(*penB));
    qpaint->drawLine(x+45,KEYBOARDY+43,x+51,KEYBOARDY+43);
    qpaint->drawLine(x+45,KEYBOARDY+44,x+51,KEYBOARDY+44);
    qpaint->setPen(p?(*penB):(*penT));
    qpaint->drawLine(x+44,KEYBOARDY+45,x+53,KEYBOARDY+45);
    qpaint->setPen(p?(*penB):(*penW));
    qpaint->drawLine(x+52,KEYBOARDY+28,x+52,KEYBOARDY+42);

    qpaint->fillRect(x+45,KEYBOARDY+28,7,15,((p)?*brushR:*brushW));
    qpaint->fillRect(x+48,KEYBOARDY+1,2,27,((p)?*brushR:*brushW));
    qpaint->setPen(p?(*penR):(*penW));
    qpaint->drawPoint(x+47,KEYBOARDY+27);
    qpaint->drawPoint(x+50,KEYBOARDY+27);
};
void KMidChannel4D::drawLa__(QPainter *qpaint,int x,int p)
{
    qpaint->fillRect(x+51,KEYBOARDY+1,5,26,((p)?*brushR:*brushB));

    if (!p)
    {
        qpaint->setPen(*penW);
        qpaint->drawLine(x+51,KEYBOARDY+2,x+51,KEYBOARDY+25);
        qpaint->drawLine(x+52,KEYBOARDY+25,x+56,KEYBOARDY+25);
    };
    qpaint->setPen(p?(*penW):(*penB));
    qpaint->drawLine(x+56,KEYBOARDY+1,x+56,KEYBOARDY+27);
    qpaint->drawPoint(x+55,KEYBOARDY+27);

};
void KMidChannel4D::drawSi(QPainter *qpaint,int x,int p)
{
    qpaint->setPen(p?(*penR):(*penB));
    qpaint->drawLine(x+54,KEYBOARDY+43,x+60,KEYBOARDY+43);
    qpaint->drawLine(x+54,KEYBOARDY+44,x+60,KEYBOARDY+44);
    qpaint->setPen(p?(*penB):(*penT));
    qpaint->drawLine(x+53,KEYBOARDY+45,x+62,KEYBOARDY+45);
    qpaint->setPen(p?(*penB):(*penW));
    qpaint->drawLine(x+61,KEYBOARDY+1,x+61,KEYBOARDY+42);

    qpaint->fillRect(x+54,KEYBOARDY+28,7,15,((p)?*brushR:*brushW));
    qpaint->fillRect(x+57,KEYBOARDY+1,4,27,((p)?*brushR:*brushW));
    qpaint->setPen(p?(*penR):(*penW));
    qpaint->drawPoint(x+56,KEYBOARDY+27);
};
