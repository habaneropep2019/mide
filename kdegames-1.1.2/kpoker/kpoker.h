#ifndef KPOKER_H
#define KPOKER_H

#include <qwidget.h>

#include "kpaint.h"
#include "defines.h"
#include <klocale.h>
#include <kaccel.h>

class KAudio;

typedef enum _cardtype { // I don't use this anymore. But it helps to understand the numbers :)
	        DECK=0,
	        CA=1,SA=2,HA=3,DA=4,
	        CK=5,SK=6,HK=7,DK=8,
	        CQ=9,SQ=10,HQ=11,DQ=12,
	        CJ=13,SJ=14,HJ=15,DJ=16,
	        CTEN=17,STEN=18,HTEN=19,DTEN=20,
	        CNINE=21,SNINE=22,HNINE=23,DNINE=24,
	        CEIGHT=25,SEIGHT=26,HEIGHT=27,DEIGHT=28,
	        CSEVEN=29,SSEVEN=30,HSEVEN=31,DSEVEN=32,
	        CSIX=33,SSIX=34,HSIX=35,DSIX=36,
	        CFIVE=37,SFIVE=38,HFIVE=39,DFIVE=40,
	        CFOUR=41,SFOUR=42,HFOUR=43,DFOUR=44,
	        CTHREE=45,STHREE=46,HTHREE=47,DTHREE=48,
	        CTWO=49,STWO=50,HTWO=51,DTWO=52
} cardtype;

struct fCard {
	        int cardNum;
	        int cardType;
};

class QPushButton;
class QLineEdit;
class QLabel;
class QFrame;  
class QLineEdit;
class QFrame;

class kpok : public QWidget
{
	Q_OBJECT
public:
	kpok(QWidget *parent=0, const char *name=0);
	virtual ~kpok();
	void drawCards(int skip[5]);
	void setCash(int newCash);
	int  getCash();
   int  getStatus();
   void setStatus(int);
   int  getCard(int);
   void setCard(int, int);

protected:
	void setHand(const char *newHand);
	
	int  testHand();
	void addFoundCard(int, int);
	void cleanFoundCard();
	void startBlinking();
	void stopBlinking();

	int  testFlush();
	int  testStraight();
	int  findCardTypes(int cardT[5], int card);
	void playSound(const char *filename);
   void paintEvent( QPaintEvent * );	

public slots:
	int initSound();	
	void setSound(int);	
protected slots:
	void initPoker();
	
	void drawClick();
	void frameClick(CardWidget *);
	void bTimerEvent();
	void drawCardsEvent();
       void waveTimerEvent();
   
	void displayWin(const char *hand, int cashWon);

   void startWave();
   void stopWave();

	
private:
	QFrame      *CardFrames[5];
	QPushButton *drawButton;
	QLabel      *wonLabel;

	QLabel       *cashLabel;
	QFrame       *cashFrame;
	QLabel       *LHLabel;
	QFrame       *LHFrame;
	QTimer       *blinkTimer;
	QTimer       *drawTimer;
	QTimer       *waveTimer;
   
   QLabel       *clickToHold; 
   QLabel       *heldLabels[5];

   QPicture     *wavePic;
	
	int cash;
	CardWidget    *cardW[5];
	int           cards[5];
	
	int           cardHelp[highestCard+1];
	struct fCard  foundCards[5*2];
	int           status;
	int           done[highestCard];
	int           blinkingCards[5]; // cards that should blink in the blinking timerevent
	int           blinkStat; // status of blinking
	int           drawStat; // status of drawing (which card already was drawn etc.
   bool          sound;
   KAudio       *KAS;						      
   KLocale      *locale;						      
   KAccel       *kacc;
   
   int          waveActive;
   int          fCount;
};


#endif
