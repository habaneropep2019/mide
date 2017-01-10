/*
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *
 * Comment:
 * This is my first "bigger" application I did with Qt and my very first KDE application.
 * So please apologize some strange coding styles --> if you can't stand some really
 * bad parts just send me a patch including you "elegant" code ;)
 *
 */   

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "kpaint.moc"

extern CardImages  *cardImage;


CardWidget::CardWidget( QWidget *parent, const char *name )
: QPushButton( parent, name )
{
	held=0;
	move(1,1);
	resize(cardWidth,cardHeight);
}

void CardWidget::paintCard(int cardType, int xPos, int yPos)
{
	if (cardType==0)
	    pm=&cardImage->deck;
	else
	    pm=&cardImage->cardP[cardType-1];
	if ( pm->size() != QSize( 0, 0 ) ) {         // is an image loaded?
		bitBlt(this, xPos, yPos, pm, 0, 0, pm->size().width(), pm->size().height());
	}
}

void CardWidget::ownClick()
{
	emit pClicked(this);
}

void CardWidget::paintEvent( QPaintEvent *)
{
	bitBlt(this, 0, 0, pm, 0, 0, pm->size().width(), pm->size().height());
}

int CardWidget::queryHeld()
{
	return held;
}
void CardWidget::setHeld(int newheld)
{
	held=newheld;
}
int CardWidget::toggleHeld()
{
	if (held == 1)  held = 0;
	else            held = 1;
	return held;
}

bool CardImages::loadCards( const char *path)
{
	char lpath[300];
	int w;
	int randomDeck;
	
	for (w=0; w<highestCard; w++) {
		sprintf(lpath,"%s%d.bmp",path,w+1);
		if(!cardP[w].load(lpath)) 
		    printf("Fatal error: bitmap %s not found \n",lpath);
	}
	
        srandom(time(NULL));

        randomDeck=(random() % 4) +1;
//        randomDeck+=1;
	
	sprintf(lpath,"%sdeck%d.bmp",path, randomDeck);
	if(!deck.load(lpath)) 
	    printf("Fatal error: bitmap %s not found \n",lpath);
	return 1;
}

