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

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <kapp.h>
#include <qpainter.h> 
#include <qpixmap.h> 
#include <kiconloader.h>
#include "StoneWidget.moc"

StoneWidget::StoneWidget( QWidget *parent, int x, int y ) 
    : QWidget(parent) {

	 stones_x=x;  // Feldgroesse setzten
	 stones_y=y;
	 stones_size=stones_x*stones_y;
	 stones=new unsigned char[stones_size];
	 
	 f_gameover=-1;
	 f_board=0;
	 f_colors=0;
	 f_score=0;
	 slice=0;
	 for (int i=0;i<stones_size;i++)
	      stones[i]=F_CHANGED;

	 if ( stonemap.isNull() )
	      stonemap = kapp->getIconLoader()->loadIcon("stones.gif");

	 stone_px=stone_py=40;
	 maxslices=30;
	 maxcolors=4;

	 stonepyy=stone_py*stones_y;
	 stonepxx=stone_px*stones_x;

	 //	 resize(stones_x*stone_px,stones_y*stone_py);
	 //	 setFixedSize(stones_x*stone_px,stones_y*stone_py);

	 setMouseTracking(true);
	
	 QColor c(115,115,115);
	 setBackgroundColor(c);

	 emit s_sizechanged();
	 startTimer( 100 ); 
}
QSize 
StoneWidget::sizeHint () {
     return QSize(stones_x*stone_px,stones_y*stone_py);
}

// Rechnet die (x,y) Koordinaten eines Steines
// in eine lineare Position um.
// Rueckgabe zwischen 0 und stones_size-1 inklusive,
// bei Fehler -1.
int 
StoneWidget::xyToStone(int x,int y) {
     if (x<0||y<0||x>=stones_x||y>=stones_y)  
	  return -1;
     return x+y*stones_x;
}

// Startet ein neues Spiel
// Signal s_newgame wird gesendet.
void 
StoneWidget::newgame(int board, int colors) {
     if (colors<2) colors=3;
     
     if (colors>maxcolors)
	  f_colors=maxcolors;
     else f_colors=colors;

     f_board=board;    
     reset();
     emit s_newgame();
}

// Setzt ein Spiel zurueck.
void 
StoneWidget::reset() {
     f_score=0;
     f_gameover=-1;
     slice=0;
     modified=1;
     marked=0;
     srandom(f_board);
     for (int i=0;i<stones_size;i++)
	  SETCOLOR(stones[i], (unsigned char)(random()%f_colors)+1);
     emit s_score(f_score);
     emit s_mark(0);
}

// Entfernt alle Steine, die  mit dem Stein(x,y) verbunden sind.
// Es kann auch ein einzelner Stein entfernt werden, wenn
// force=1 gesetzt ist.
// Die Score wird hochgezaehlt. Es werden keine Signale gesendet.
void 
StoneWidget::remove(int x,int y,int force,int withsignal) {
     mark(xyToStone(x,y),force);
     
     if (!marked) return;
     
     if (force||marked>1) {
	  // Score hochzaehlen
	  if (marked>2)
	       f_score+=(marked-2)*(marked-2);
	  // markierte Steine loeschen
	  for (int i=0;i<stones_size;i++)
	       if (ISMARKED(stones[i]))
		    stones[i]=F_CHANGED;
	  // Spielsteine zusammenruecken
	  collapse();
	  // Bonus berechen
	  if (!GETCOLOR(stones[stones_size-stones_x])) {
	       f_score+=1000;      
	  }
	  // damit Paint neuzeichnet
	  modified=1;
	  // damit gameover neu berechnet wird.
	  f_gameover=-1;
	  // Signale senden
	  if (withsignal) emit s_remove(x,y); 
	  if (withsignal) emit s_score(f_score); 
	  if (gameover()){
	       if (withsignal) emit s_gameover(); 
	  }
     }
}
int
StoneWidget::gameover() {
     register int i=stones_size-1;;
     register unsigned char color;
    
     if (f_gameover>=0) return f_gameover;
     f_gameover=0;
     while (i>=0) {
	  // leere Felder ueberspringen
	  while ( GETCOLOR(stones[i])==0 && i>=0 ) i--;
	  // Wenn Stein gefunden, 
	  // dann die Nachbarn auf gleiche Farbe pruefen.
	  while ( (color=GETCOLOR(stones[i]))!=0 && i>=0 ) {
	       // nicht ganz links und gleiche Farbe
	       if ( (i%stones_x)!=0 && GETCOLOR(stones[i-1])==color)
		    return 0;  
	       // nicht ganz oben und gleiche Farbe
	       if ((i>=stones_x)&&(GETCOLOR(stones[i-stones_x])==color)) 
		    return 0;  
	       // nicht ganz rechts und gleiche Farbe
	       if ( ((i+1)%stones_x)!=0 && GETCOLOR(stones[i+1])==color )
		    return 0;  
	       i--;
	  }
     }
     f_gameover=1;
     return 1;
}



// Loesche die Markierung von allen Steinen
// wenn Steine markiert sind.
void 
StoneWidget::unmark() {
     if (!marked) return;
     for (int i=0;i<stones_size;i++) { 
	  if (ISMARKED(stones[i])) {
	       CLEARMARKED(stones[i]);
	       SETCHANGED(stones[i]);
	  }	       
     }
     marked=0;
     modified=1;
}
// Markiere den Stein an Position i
void
StoneWidget::mark(int i,int force) {
     if ((i<0)||(i>=stones_size)) {
	  unmark();
	  return;
     }
     if (ISMARKED(stones[i])) return;
     unmark();
     r_mark(i,GETCOLOR(stones[i]));
     if (!force&&marked==1) {
	  CLEARMARKED(stones[i]);
	  marked=0;
     }
     slice=0;
     if (marked>1) 
	  emit s_mark((marked-2)*(marked-2));
     else emit s_mark(0);
}
// Markiere rekursiv alle umliegenden Steine
// mit der Farbe Color
void 
StoneWidget::r_mark(int i,unsigned char color) {
     if ((i<0)||(i>=stones_size)) return;
     if (ISMARKED(stones[i])) return;
 
     int mycolor=GETCOLOR(stones[i]);
     if (!mycolor||mycolor!=color) return;
    
     SETMARKED(stones[i]);
     SETCHANGED(stones[i]);
     marked++;
     modified=1;
     // nicht ganz links
     if ((i%stones_x)!=0) r_mark(i-1,color); 
     // nicht ganz oben   
     if (i>=stones_x) r_mark(i-stones_x,color);
     // nicht ganz unten
     if (i<(stones_x-1)*stones_y) r_mark(i+stones_x,color);
     // nicht ganz rechts
     if (((i+1)%stones_x)!=0) r_mark(i+1,color);
     return;
}

// Steine zusammenruecken erst nach unten, dann nach links
void StoneWidget::collapse() {
    int j,i1,i2;
    
    for (int x=0;x<stones_x;x++) {
	i1=x+(stones_y-1)*stones_x;
	while ((GETCOLOR(stones[i1])!=0)&&(i1>=0)) i1-=stones_x;
	i2=i1;
	while (i2>=0) {
	    while ((GETCOLOR(stones[i2])==0)&&(i2>=0)) i2-=stones_x;
	    while ((GETCOLOR(stones[i2])!=0)&&(i2>=0)) {
		stones[i1]=stones[i2]|F_CHANGED;
		stones[i2]=0|F_CHANGED;	  
		i1-=stones_x;
		i2-=stones_x;
	    }
	}
    }  
    for (int x=0;x<stones_x;x++) {
	i1=x+(stones_y-1)*stones_x;
	while ((GETCOLOR(stones[i1])!=0)&&(i1<stones_size)) i1++;
	i2=i1;
	while (i2<stones_size) {
	    while ((GETCOLOR(stones[i2])==0)&&(i2<stones_size)) i2++;
	    while ((GETCOLOR(stones[i2])!=0)&&(i2<stones_size)) {
		for (j=0;j<stones_size;j+=stones_x) {
		    stones[i1-j]=stones[i2-j]|F_CHANGED;
		    stones[i2-j]=0|F_CHANGED;
		}
		i1++;
		i2++;
	    }
	}
    }  
}

void StoneWidget::timerEvent( QTimerEvent * ) {
     QPoint p=mapFromGlobal(cursor().pos());
     if (p.x()<0||p.y()<0||p.x()>=stonepxx||p.y()>=stonepyy) 
	  unmark();
     slice=(slice+1)%maxslices;
     paintEvent(0);
}

void StoneWidget::mouseMoveEvent ( QMouseEvent *e) { 
     if (gameover()) {
	  unmark();
	  return; 
     }
     int x=e->pos().x();
     int y=e->pos().y();
     if (x<0||y<0||x>=stonepxx||y>=stonepyy) return;
     mark(xyToStone(x/stone_px,y/stone_py));
}

void StoneWidget::mousePressEvent ( QMouseEvent *e) {
     if (gameover()) return;
     int x=e->pos().x();
     int y=e->pos().y();
     if (x<0||y<0||x>=stonepxx||y>=stonepyy) return;
     remove(x/stone_px,
	    y/stone_py,
	    0, // mindestens zwei Steine
	    1); // Signale senden.
     mark(xyToStone(x/stone_px,y/stone_py));
}

StoneWidget::~StoneWidget() {  
     setMouseTracking(false);
     killTimers();
     delete stones;
     //   debug("~StoneWidget\n");
}

void StoneWidget::paintEvent( QPaintEvent *e ) {
     
     int slicey;		
     int px,py;
     int redraw;  // Stein soll neu gezeichnet werden.
     unsigned char *stone=stones;
     QPainter p(this);
     QBrush backgr(blue);
     int color;
     
     //     printf("slice:%i\n",slice);
     
     for (py=0;py<stonepyy;py+=stone_py) {
	  for (px=0;px<stonepxx;px+=stone_px) {
	       
	       if (!(redraw=*stone&(F_MARKED|F_CHANGED))&&e) {
		    QRect r(px,py,stone_px,stone_py);
		    redraw=r.intersects(e->rect());
	       }
	       if (redraw) {
		    CLEARCHANGED(*stone);
		    color=GETCOLOR(*stone);
		    slicey=(color-1)*stone_py;
		    
		    if (!color) {
			 //p.fillRect(px,py,stone_px,stone_py, backgr );
			 bitBlt(this,px,py,
				&stonemap,stone_px*maxslices,0,
				stone_px,stone_py,CopyROP,FALSE);
		    } else {
			 bitBlt(this,px,py,
				&stonemap,
				(ISMARKED(*stone))?(stone_px*slice):0,
				slicey,
				stone_px,stone_py,CopyROP,FALSE);
			 
		    }   
	       }
	       stone++;  // naechster Stein.
	  }
	  
     }
}





