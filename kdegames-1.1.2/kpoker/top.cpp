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
*/  

#include<qpopmenu.h>
#include<qlayout.h>
#include<qpixmap.h>
#include<qkeycode.h>

#include<kiconloader.h>
#include<kapp.h>
#include<ktopwidget.h>
#include<kmenubar.h>
#include<ktoolbar.h>
#include<kstatusbar.h>
#include<kmsgbox.h>

#include "kpoker.h"

#include "top.moc"
#include "global.h"

const int ID_SOUND = 100;
int soId;

PokerWindow::PokerWindow() :   KTopLevelWidget()
{
        int i;

	locale = kapp->getLocale();
	
	_kpok = new kpok(this,0);
	setView( _kpok, FALSE );
	_kpok->show();
	
	
	menu = new KMenuBar(this, "_mainMenu" );
	menu->show();
	
	
	filePopup = new QPopupMenu(0,"fpopup");
	filePopup->insertItem(locale->translate("&New game"),_kpok,SLOT(initPoker()));
	filePopup->insertSeparator();
	filePopup->insertItem(locale->translate("&Quit"), qApp, SLOT(quit()));

       	optionsPopup = new QPopupMenu(0,"oppup");
	
	soId = optionsPopup->insertItem(locale->translate("&Sound"), this, SLOT(toggleSound()));
	optionsPopup->setCheckable(TRUE);

	QPopupMenu *help = kapp->getHelpMenu(true, QString(i18n("Poker"))
					 + " " + KPOKER_VERSION + " released " + KPOKER_RELEASE_DATE
                                         + i18n("\n\nby Jochen Tuchbreiter")
                                         + " (whynot@mabi.de)"
					 + i18n("\n\nFor a list of credits see helpfile")
					 + i18n("\nSuggestions, bug reports etc. are welcome")
					     );

	menu->insertItem(locale->translate("&File"), filePopup);
	menu->insertItem(locale->translate("&Options"), optionsPopup);
	
	menu->insertSeparator();
	menu->insertItem(locale->translate("&Help"), help);

	setMenu( menu );

	if (_kpok->initSound() == 1) {
	  optionsPopup->setItemChecked(soId, TRUE);
	}
	else {
	  optionsPopup->setItemEnabled(soId, FALSE);
	}
	conf = kapp->getConfig();

	if (conf !=0) {
	  if ((i = conf->readNumEntry("Sound", -1)) != -1) {
	    if (i==0) {
	      optionsPopup->setItemChecked(soId, FALSE);
	      _kpok->setSound(0);
	    }
	    if (i==1) {
	      optionsPopup->setItemChecked(soId, TRUE);
	      _kpok->setSound(1);
            	    }
	  }
	}
}

PokerWindow::~PokerWindow()
{
        if (optionsPopup->isItemChecked(soId)) 
	  conf->writeEntry("Sound",1);
	else
	  conf->writeEntry("Sound",0);
	delete menu;
}


void PokerWindow::saveProperties(KConfig* conf)
{
  conf->writeEntry("cash", _kpok->getCash());
  if (optionsPopup->isItemChecked(soId)) 
    conf->writeEntry("Sound",1);
  else
    conf->writeEntry("Sound",0);
}

void PokerWindow::readProperties(KConfig* conf)
{
  int i;
  if ((i = conf->readNumEntry("cash", -1)) != -1) {
    _kpok->setCash(i);
  }
  if ((i = conf->readNumEntry("Sound", -1)) != -1) {
    if (i==0) {
      optionsPopup->setItemChecked(soId, FALSE);
      _kpok->setSound(0);
    }
    if (i==1) {
      optionsPopup->setItemChecked(soId, TRUE);
      _kpok->setSound(1);
    }
  }
}

int PokerWindow::toggleSound()
{
  if (optionsPopup->isItemChecked(soId) == TRUE) {
      _kpok->setSound(0);
    optionsPopup->setItemChecked(soId, FALSE);
  }
  else {
    optionsPopup->setItemChecked(soId, TRUE);
    _kpok->setSound(1);
  }
  return 1;
}



