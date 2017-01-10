/* Yo Emacs, this is -*- C++ -*- */
/*
 *   ksame 0.4 - simple Game
 *   Copyright (C) 1997,1998  Marcus Kreutzberger
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

#ifndef _STONEWIDGET
#define _STONEWIDGET

#include <qpixmap.h>
#include <qwidget.h>

#define F_CHANGED 128
#define F_MARKED  64
#define F_COLORMASK 63

#define SETCHANGED(field) field|=F_CHANGED
#define SETMARKED(field) field|=(F_MARKED|F_CHANGED)
#define CLEARCHANGED(field) field&=~F_CHANGED
#define CLEARMARKED(field) field=((field)&(~F_MARKED))|F_CHANGED
#define SETCOLOR(field,color) (field)=((color)&F_COLORMASK)|F_CHANGED
#define ISCHANGED(field) ((field)&F_CHANGED)
#define ISMARKED(field) ((field)&F_MARKED)
#define GETCOLOR(field) ((field)&F_COLORMASK)



class StoneWidget : public QWidget {
private:
     Q_OBJECT

     // Spiel abhaengige Parameter
     int f_colors;
     int f_board;
     int f_score;
     int f_gameover;
     int stones_x;           // Feldgröße in Steinen.
     int stones_y;           // Feldgröße in Steinen.

public:
     StoneWidget( QWidget *parent=0, int x=10,int y=10);
     ~StoneWidget();
     
     // Zustand abfragen
     
     int board()  { return f_board; }
     int score()  { return f_score; }
     QSize size() { return QSize(stones_x,stones_y); }  
     int colors() { return f_colors; }

     QSize sizeHint ();
     
     // Das Spielfeld wird neu initialisiert.
     void newgame(int board, int colors=-1);

     // Spiel zuruecksetzen
     void reset();
     // Spielstein und verbundene Steine loeschen
     // und zusammenruecken
     void remove(int x,int y,int force=0,int withsignal=0);

     // Spielende abfragen
     int gameover();

     int xyToStone(int x,int y);
protected:

     void unmark();
     void mark(int i,int force=0);
     void r_mark(int i,unsigned char color);
     void collapse();

     void timerEvent( QTimerEvent *e );
     void paintEvent( QPaintEvent *e );
     void mousePressEvent ( QMouseEvent *e);
     void mouseMoveEvent ( QMouseEvent *e);

     // Eigenschaften des SteinBildes
     int stone_px,stone_py; // Pixelgröße eines Steines.
     int maxcolors;         // Anzahl verschiedener Steine (Y Richtung)
     int maxslices;         // Anzahl Steine eines Movies  (X Richtung)

private:
     int modified;
     int marked;            // Anzahl markierter Steine

     int stonepyy,stonepxx;

     // Array das die Zustaende der Steine speichert
     unsigned char *stones;
     int stones_size;
     // Bildnummer des Steinmovies
     int slice;          

     QPixmap stonemap;
signals: 

     // Ein neues Spiel beginnt
     void s_newgame();

     // Der Punktestand hat sich geändert.
     void s_score(int score);
     
     // Es wurden Steine markiert
     void s_mark(int m);

     // Das Spiel ist zueende. 
     void s_gameover();
     
     // Der Stein (x,y) wurde angeklickt(entfernt), 
     // alle umliegend betroffenen Steine
     // verschwinden automatisch, d.h. kein extra Signal.
     void s_remove(int x,int y);
     
     void s_sizechanged();

};

#endif





