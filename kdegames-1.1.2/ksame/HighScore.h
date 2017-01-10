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

#ifndef _HighScoreWIDGET
#define _HighScoreWIDGET
#include <qwindow.h>
#include <qpushbt.h>

class HighScore : public QWindow {
 Q_OBJECT
public:
 HighScore(QWidget *parent=0);     
     void add(int board, int score,int colors);
     
protected:
     void add(int board, int score,int colors,char *name);
     void load();
     void store();

private:
#define HS_MAXENTRY 10
     struct HSEntry {
	  int board;
	  int score;
	  int colors;
	  char name[17];
     };

     HSEntry hiscore[HS_MAXENTRY];
     int hiscore_used;
     
     char playername[17];

     int fontw,fonth;
     QSize mSizeHint;
     QPushButton *ok;
     void keyPressEvent (QKeyEvent * e);
     virtual void paintEvent ( QPaintEvent * ); 
     virtual QSize sizeHint() { return mSizeHint; }
};

#endif




