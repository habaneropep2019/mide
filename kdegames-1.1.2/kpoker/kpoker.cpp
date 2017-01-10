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
 * bad parts just send me a patch including your "elegant" code ;)
 *  
 */
#include <stdlib.h>

#include <qpushbt.h>
#include <qlabel.h>
#include <qpopmenu.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qmsgbox.h>
#include <qpainter.h>

#include <kapp.h>
#include <kmenubar.h>
#include <kmsgbox.h>
#include <kstdaccel.h>

// sound support
extern "C" {
#include <mediatool.h>
}
#include <kaudio.h>


#include <time.h>
#include <stdio.h>

#include "global.h"

#include "kpoker.h" 
#include "kpoker.moc"


CardImages    *cardImage;
QFont LHLabelSmallFont("Helvetica",10);
QFont LHLabelVerySmallFont("Helvetica",8);


kpok::kpok(QWidget *parent, const char *name) 
: QWidget(parent, name)
{
	int w;
	char version[270];
	
       	setFixedSize(420,220);	

        locale = kapp->getLocale();
	
	sprintf(version, "%s %s", kapp->getCaption(), KPOKER_VERSION);
	setCaption( version );
	
	
	QString bitmapdir = kapp->kde_datadir() + QString("/kpoker/pics/");
	
	kacc = new KAccel( this );
	KStdAccel stdacc; // Access to standard accelerators
	/* KKeyCode initialization */
	kacc->insertItem(i18n("Quit"), "Quit", stdacc.quit());
	kacc->insertItem(i18n("New game"), "New", "F2");
	kacc->insertItem(i18n("Help"), "Help", stdacc.help());
	// kacc->insertItem(i18n("Ok dialog"), "ok", "Return"); ??
	// kacc->insertItem(i18n("Cancel dialog"), "dialog", "Escape"); ??
	
	/* connections */
	kacc->connectItem("Quit", qApp, SLOT(quit()));
	kacc->connectItem("New", this, SLOT(initPoker()));
	
	QFont myFixedFont("Helvetica",12);

	drawButton = new QPushButton(this,0);
	
	drawButton->setText(locale->translate("Draw !"));
	drawButton->setGeometry(210-drawButton->sizeHint().width() / 2,CLHDistFromTop,drawButton->sizeHint().width(),drawButton->sizeHint().height());
	connect( drawButton, SIGNAL(clicked()), this, SLOT( drawClick() ) );
	
	for (w=0;w < 5; w++)
	  {
		  CardFrames[w] = new QFrame(this, 0);
		  CardFrames[w]->setGeometry(cardHDist+w*(cardHDist+cardWidth),cardDistFromTop,cardWidth +2,cardHeight +2);
		  CardFrames[w]->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	  }
	
	
	QFont wonFont("Helvetica", 14, QFont::Bold);
	wonLabel = new QLabel(this, 0);
	
	wonLabel->hide();
	wonLabel->setFont(wonFont);
	wonLabel->setAutoResize(true);
	wonLabel->move(this->width() /2  - wonLabel->width() / 2, wonLabelVDist);
	
	
	QFont clickToHoldFont("Helvetica", 11);
	clickToHold = new QLabel(this,0);
	
	clickToHold->hide();
	clickToHold->setFont(clickToHoldFont);
	clickToHold->setAutoResize(true);
	clickToHold->setText(locale->translate("Click a card to hold it"));
	clickToHold->move(this->width() /2  - clickToHold->width() / 2, clickToHoldVDist);
	
	
	/* load all cards into pixmaps */	
	
	cardImage = new CardImages(this,0);
	cardImage->hide();
	cardImage->loadCards(bitmapdir);
	
	for (w=0;w<5;w++) {
		cardW[w] = new CardWidget(CardFrames[w], 0);
		connect( cardW[w], SIGNAL(clicked()), cardW[w], SLOT( ownClick() ) );
		connect( cardW[w], SIGNAL(pClicked(CardWidget *)), this, SLOT(frameClick(CardWidget *)));
	}
	
	for (w=0; w<5;w++) {
		heldLabels[w] = new QLabel(this, 0);
		heldLabels[w]->hide();
		heldLabels[w]->setFont(myFixedFont);	       
		heldLabels[w]->setAutoResize(true);
		heldLabels[w]->setText(locale->translate("Held"));
		heldLabels[w]->move(
				    ((cardWidth+2) /2 - heldLabels[w]->width() /2) +
				    cardHDist+w*(cardHDist + cardWidth), cardDistFromTop -15
				    );
		cardW[w]->heldLabel = heldLabels[w];
	}
	cashFrame = new QFrame(this,0);
	cashFrame->setGeometry(CLHBorderDistance, CLHDistFromTop, CLHWidth, 30);
	cashFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	cashLabel= new QLabel(cashFrame,0);
	cashLabel->setAutoResize(true);
        cashLabel->setFont(myFixedFont);
	
	LHFrame = new QFrame(this,0);
	LHFrame->setGeometry(this->width() - CLHBorderDistance - CLHWidth, CLHDistFromTop, CLHWidth, 30);
	LHFrame->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	
	LHLabel= new QLabel(LHFrame,0);
	LHLabel->setAutoResize(true);
        LHLabel->setFont(myFixedFont);
	
	for (w=0; w<= highestCard / 4; w++)
	    cardHelp[w*4+1]=cardHelp[w*4+2]=cardHelp[w*4+3]=cardHelp[w*4+4]=w;
	
	blinkTimer = new QTimer(this);
	connect( blinkTimer, SIGNAL(timeout()), SLOT(bTimerEvent()) );
	blinkStat=0;

	waveTimer = new QTimer(this);
	connect( waveTimer, SIGNAL(timeout()), SLOT(waveTimerEvent()) );
	
	drawTimer = new QTimer(this);
	connect (drawTimer, SIGNAL(timeout()), SLOT(drawCardsEvent()) );
	
	

	srandom(time(NULL));
	
	QToolTip::add( drawButton,locale->translate("draw new cards"));
	QToolTip::add( LHLabel,locale->translate("your last hand"));
        QToolTip::add( LHFrame,locale->translate("your last hand"));
	QToolTip::add( cashLabel,locale->translate("money left"));
	QToolTip::add( cashFrame,locale->translate("money left"));
	
	initPoker();
	initSound();
}

kpok::~kpok()
{
    delete kacc;
}

void kpok::initPoker()
{
   status=0;
   int w;

   for (w=0;w<5;w++) {
      cards[w]=0;
      cardW[w]->heldLabel->hide();
      cardW[w]->show();
      cardW[w]->paintCard(cards[w],0,0);
   }
   wonLabel->hide();
   clickToHold->hide();

   drawStat=0;
   cleanFoundCard();
   setCash(100);
   setHand(locale->translate("nothing"));

   waveActive = 0;
   fCount =0;

}

int kpok::initSound()
{
   KAS = new KAudio();

   if (KAS->serverStatus()) {
      sound = false;
      return 0;
   } else
      sound = true;
   return 1;
}

void kpok::playSound(const char *soundname)
{
   char filename[300];

   if (!sound)
      return;

   strcpy(filename, kapp->kde_datadir()+"/kpoker/sounds/"+soundname);


   KAS->play(filename);
}

void kpok::setSound(int i)
{
   sound=i;
}

void kpok::drawCards(int skip[5])
{
   int got;
   int w;

   for (w=0;w<5; w++) {
      if (skip[w] == 0) {
         got=random() % highestCard;
         while (done[got] == 1) got=random() % highestCard;
         done[got]=1;
         cards[w]=(got+1);
      }
   }
}

void kpok::addFoundCard(int cardNum, int cardType)
{
   int found=0;
   int w=0;
   for (w=0; foundCards[w].cardType !=0 ; w++)
      if (foundCards[w].cardNum == cardNum) found=1;

   if (!found) {
      foundCards[w].cardNum=cardNum;
      foundCards[w].cardType=cardType;
   }
}

void kpok::cleanFoundCard()
{
   int w;
   for (w=0; w<5*2; w++)
      foundCards[w].cardType=0;
}


int kpok::testFlush()
{
   int w, mycolor;
   mycolor=cards[0] % 4;
   for (w=1;w<5;w++)
      if (mycolor != cards[w] %4)
         return 0;

   for (w=0; w<5; w++)
      addFoundCard(w,cards[w]);
   return 1;
}

int kpok::findCardTypes(int cardT[5], int card)
{
   int w;
   for (w=0; w<5; w++)
      if (cardT[w] == card)
         return 1;
   return 0;
}

int kpok::testStraight()
{
   int cardTypes[5];
   int lowest=100; /* lowest cardtype --> set to something high first :) */
   int w; 

   for (w=0; w<5; w++) {
      cardTypes[w]=cardHelp[cards[w]];
      if (cardTypes[w]  < lowest) lowest=cardTypes[w];
   }
   
   /* look for special case ace-2-3-4-5 */
   if (!(findCardTypes(cardTypes, 0) && findCardTypes(cardTypes, 12) && findCardTypes(cardTypes, 11) && findCardTypes(cardTypes, 10) && findCardTypes(cardTypes, 9)))  {

	   for (w=0; w<5; w++)
	       if (!findCardTypes(cardTypes,lowest+w))
		   return 0;
   }
       
   for (w=0; w<5; w++)
      addFoundCard(w,cards[w]);

   if (lowest == 0) return 2; // could be a royal flush :-o 
   return 1;
}


int kpok::testHand()
{
   int matching=0;
   int w,w2;
   int isRoyal;


   if ((isRoyal=testStraight())) {
      if (testFlush()) {
         if (isRoyal == 2)
            return 10; // royal flush detected
         else
            return 9; // straight flush detected
      }

      else
         return 7;
   }
   if (testFlush()) return 8;


   for (w=0;w < 5; w++) // this searches for pairs/three/four of a kind
      for (w2=w+1; w2<5; w2++) {
         if ( cardHelp[cards[w]] == cardHelp[cards[w2]] ) {
            matching++;
            addFoundCard(w,cards[w]);
            addFoundCard(w2,cards[w2]);             
         }
      }
   return matching;
}

void kpok::setCash(int newCash)
{
   char buf[255];
   cash = newCash;
   sprintf(buf,"%s: $ %d",locale->translate("Cash"),cash);  // locale
   cashLabel->setText(buf);
   cashLabel->move(cashFrame->width() / 2 - cashLabel->width() / 2, cashFrame->height() / 2 - cashLabel->height() / 2);
}

int  kpok::getCash()
{
   return cash;
}


int  kpok::getStatus()
{
   return status;
}

void kpok::setStatus(int i)
{
   status=i;
}

int  kpok::getCard(int i)
{
   return cards[i];
}

void kpok::setCard(int num, int value)
{
   cards[num]=value;
}


void kpok::setHand(const char *newHand)
{
   char buf[255];
   QFont LHFont("Helvetica",12);
   sprintf(buf,"%s: %s",locale->translate("Last Hand"), newHand); // locale

   LHLabel->setFont(LHFont);
   LHLabel->setText(buf);

/* Okay, this makes sure that the label fits in its frame --> if the width
 * of the label is too big it simply decreases the font size.
 */
   if (LHLabel->width() > LHFrame->width() - 2) {
      LHLabel->setFont(LHLabelSmallFont);

      if (LHLabel->sizeHint().width() > LHFrame->width() -2 )
         LHLabel->setFont(LHLabelVerySmallFont);

      LHLabel->resize(LHLabel->sizeHint());
   }

   LHLabel->move(LHFrame->width() / 2 - LHLabel->width() / 2, LHFrame->height() / 2 - LHLabel->height() / 2);
}

void kpok::frameClick(CardWidget *MyCW)
{
   if (status != 0) {
      playSound("hold.wav");
      if (MyCW->toggleHeld() == 1)
         MyCW->heldLabel->show();
      else
         MyCW->heldLabel->hide();
   }
}

void kpok::drawClick()
{
   int cardsToDraw[5];
   int w;

   drawButton->setEnabled(false);

   if (status == 0) {

      wonLabel->hide();

      cleanFoundCard();
      stopBlinking();
      stopWave();

      setCash(getCash() - cashPerRound);

      for (w=0; w<5;w++) {
         cardsToDraw[w]=0; 
         cardW[w]->setHeld(0);
      }

      for (w=0; w<highestCard;w++) done[w]=0;

      for (w=0;w<5;w++) {
         cards[w]=0;
         cardW[w]->heldLabel->hide();
         cardW[w]->show();
         cardW[w]->paintCard(cards[w],0,0);
      }

      drawCards(cardsToDraw);

      if (cardW[0]->queryHeld())
         drawTimer->start(0, TRUE);
      else
         drawTimer->start(drawDelay, TRUE);

   } else {
      clickToHold->hide();
      for (w=0; w<5;w++) cardsToDraw[w] = cardW[w]->queryHeld();

      for (w=0;w<5;w++) {
         if (!cardsToDraw[w]) {
            cards[w]=0;
            cardW[w]->show();
            cardW[w]->paintCard(cards[w],0,0);
         }
      }

      drawCards(cardsToDraw);


      if (cardW[0]->queryHeld())
         drawTimer->start(0, TRUE);
      else
         drawTimer->start(drawDelay, TRUE);

   }
}

void kpok::drawCardsEvent()
{
   int testResult;

   cardW[drawStat]->show();
   cardW[drawStat]->paintCard(cards[drawStat],0,0);

   if (!cardW[drawStat]->queryHeld())
      playSound("cardflip.wav");

   if (drawStat == 4) { /* just did last card */
      drawButton->setEnabled(true);
      drawStat=0;
      if (status == 1) {
         testResult=testHand();
         switch (testResult) {
         case 1 : if (foundCards[0].cardType >= 17) {
               foundCards[0].cardType=0; foundCards[1].cardType=0; displayWin(locale->translate("nothing"),0); break;
            }
            displayWin(locale->translate("One Pair"),5);  break;
         case 2 : displayWin(locale->translate("Two Pairs"), 10); break;
         case 3 : displayWin(locale->translate("3 of a kind"), 15); break;
         case 4 : displayWin(locale->translate("Full House"), 40); break;
         case 6 : displayWin(locale->translate("4 of a kind"), 125); break;
         case 7 : displayWin(locale->translate("Straight"),20); break;
         case 8 : displayWin(locale->translate("Flush"),25); break;
         case 9 : displayWin(locale->translate("Straight Flush"),250); break;
         case 10 : displayWin(locale->translate("Royal Flush"),2000); break;

         default: displayWin(locale->translate("nothing"),0); break;
         }

         startBlinking();
         status = 0;

         if (getCash() < cashPerRound) {
            KMsgBox::message(0,locale->translate("You Lost"), 
                             locale->translate("Oops - you went bankrupt.\n"),
                             KMsgBox::EXCLAMATION,locale->translate("New game"));
            initPoker();
         }

      } else {
         clickToHold->show();
         status =1;
      }

   } else { /* only inc drawStat if not done with displaying */
      drawStat++;
      /* look at next card and if it is held instantly call drawCardEvent again */
      if (cardW[drawStat]->queryHeld())
         drawTimer->start(0,TRUE);
      else
         drawTimer->start(drawDelay,TRUE);
   }
}

void kpok::startBlinking()
{
   blinkTimer->start(650);
}

void kpok::stopBlinking()
{
   blinkTimer->stop();
   blinkStat=1;
}

void kpok::startWave()
{
   waveTimer->start(40);
   waveActive = 1;
}

void kpok::stopWave()
{
   waveTimer->stop();
   fCount = -1; /* clear image */
   repaint ( FALSE );
   waveActive = 0;
}

void kpok::waveTimerEvent()
{
   fCount = (fCount + 1) & 15;
   repaint( FALSE );
}

void kpok::bTimerEvent()
{
   int w;

   if (blinkStat) {
      for (w=0; w<5; w++) {
         if (foundCards[w].cardType != 0) {
            cardW[foundCards[w].cardNum]->hide();
         }
      }
      blinkStat=0;
   } else {
      for (w=0; w<5; w++) {
         if (foundCards[w].cardType != 0) {
            cardW[foundCards[w].cardNum]->show();
            cardW[foundCards[w].cardNum]->paintCard(foundCards[w].cardType,0,0);
         }
      }
      blinkStat=1;
   }
}

void kpok::displayWin(const char *hand, int cashWon)
{
   char buf[200];

   setHand(hand);
   setCash(getCash() + cashWon);

   if (cashWon) {
      playSound("win.wav");
      sprintf(buf,"%s %d !",locale->translate("You won $"), cashWon); // locale
   } else {
      playSound("lose.wav");
      sprintf(buf,locale->translate("Game Over")); // locale
   }
   wonLabel->setText(buf);
   wonLabel->move(this->width() / 2 - wonLabel->width() / 2, wonLabelVDist);

   if (!cashWon) {
      wonLabel->show();
   }
   else {
      wonLabel->hide();
      startWave();
   }
}


void kpok::paintEvent( QPaintEvent *)
{
   /* This was shamelessy stolen from the "hello world" example coming with Qt
      Thanks to the Qt-Guys for doing such a cool example 8-)
   */
   
   
   QString t;

   if (!waveActive) {
      return;
   }

   t = wonLabel->text();

   static int sin_tbl[16] = {
      0, 38, 71, 92, 100, 92, 71, 38,  0, -38, -71, -92, -100, -92, -71, -38};

   if ( t.isEmpty() )
      return;


   QFont wonFont("Helvetica", 18, QFont::Bold);
   
   QFontMetrics fm = QFontMetrics(wonFont);

   int w = fm.width(t) + 20;
   int h = fm.height() * 2;
   
   int pmx = this->width() / 2 - w / 2;
   int pmy = wonLabelVDist - h / 4;

   QPixmap pm( w, h );
   pm.fill( this, pmx, pmy );

   if (fCount == -1) { /* clear area */
      bitBlt( this, pmx, pmy, &pm );
      return;
   }

   QPainter p;
   int x = 10;
   int y = h/2 + fm.descent();
   int i = 0;
   p.begin( &pm );
   p.setFont( wonFont );
   p.setPen( QColor(0,0,0) );
   
   while ( t[i] ) {
      int i16 = (fCount+i) & 15;
      
      p.drawText( x, y-sin_tbl[i16]*h/800, &t[i], 1 );
      x += fm.width( t[i] );
      i++;
   }
   p.end();

// 4: Copy the pixmap to the Hello widget
   bitBlt( this, pmx, pmy, &pm );

}



