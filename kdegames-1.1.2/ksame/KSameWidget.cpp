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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <kapp.h>
#include <qaccel.h>
#include <qwidget.h>
#include <qpushbt.h>
#include <qpixmap.h>
#include <kmsgbox.h> 
#include <kiconloader.h>
#include "StoneWidget.h"
#include "version.h"
#include "KSameWidget.moc"
#include <qpopmenu.h> 
#include <kstatusbar.h> 
#include <kintegerline.h> 


static const int two_item = 2;
static const int three_item = 3;
static const int four_item = 4;

KSameWidget::KSameWidget() {

  KMenuBar *menu=new KMenuBar(this);

  QPopupMenu *gamemenu=new QPopupMenu;
  gamemenu->setAccel(CTRL+Key_N,
		     gamemenu->insertItem(i18n("&New"),this, SLOT(m_new())));
  gamemenu->setAccel(CTRL+Key_L,
		     gamemenu->insertItem(i18n("&Open"),this, SLOT(m_load())));
  gamemenu->setAccel(CTRL+Key_S,
		     gamemenu->insertItem(i18n("&Save"),this, SLOT(m_save())));
  gamemenu->insertSeparator();
  gamemenu->setAccel(CTRL+Key_H,
		     gamemenu->insertItem(i18n("S&how Highscore"),this, SLOT(m_showhs())));
  gamemenu->insertSeparator();
  gamemenu->setAccel(CTRL+Key_Q,
		     gamemenu->insertItem(i18n("&Quit"),this, SLOT(m_quit())));


  menu->insertItem(i18n("&Game"),gamemenu);
  menu->insertSeparator();
  optionsmenu = new QPopupMenu;
  optionsmenu->setCheckable(true);

  colorsmenu = new QPopupMenu;
  colorsmenu->setCheckable(true);
  colorsmenu->insertItem("2", two_item );
  colorsmenu->insertItem("3", three_item );
  colorsmenu->insertItem("4", four_item ); 
  connect (colorsmenu, SIGNAL (activated (int)), SLOT (m_colors(int)));
  //  optionsmenu->insertItem(i18n("Colors"),colorsmenu);

  random_item = optionsmenu->insertItem(i18n("&Random Board"),this, SLOT(m_tglboard()));
  optionsmenu->setAccel(CTRL+Key_R,random_item);
  menu->insertItem(i18n("&Options"),optionsmenu);
  menu->insertSeparator();


  QPopupMenu *help = kapp->getHelpMenu(true, QString(i18n("About SameGame")) +
				       " " KSAME_VERSION +
				       i18n("\n\nby Marcus Kreutzberger"
				       " (kreutzbe@informatik.mu-luebeck.de)"));
  menu->insertItem(i18n("&Help"),help);

  setMenu(menu);
  
  status=new KStatusBar(this);
  status->insertItem(i18n("Colors: XX"),1);
  status->insertItem(i18n("Board: XXXXXX"),2);
  status->insertItem(i18n("Marked: XXXXXX"),3);
  status->insertItem(i18n("Score: XXXXXX"),4);
  status->setAlignment(4,AlignRight);
  setStatusBar(status);
  
  stone = new StoneWidget(this,15,10);
  connect( stone, SIGNAL(s_newgame()), this, SLOT(history_clear()));
  connect( stone, SIGNAL(s_gameover()), this, SLOT(gameover()));
  connect( stone, SIGNAL(s_mark(int)), this, SLOT(set_marked(int)));   
  connect( stone, SIGNAL(s_score(int)), this, SLOT(set_score(int)));   
  connect( stone, SIGNAL(s_remove(int,int)), this, SLOT(history_add(int,int)));   
  connect(stone, SIGNAL(s_sizechanged()), this, SLOT(sizeChanged()));  

  setView(stone,false);
  stone->show();
  highscore = new HighScore();
  
  israndom=1;
  set_board(time(0));
  optionsmenu->setItemChecked(random_item, israndom); 
  set_score(0);
  set_marked(0);
  sizeChanged();   
  history.setAutoDelete(true); 
  if (!kapp->isRestored()) {
       stone->newgame(board);
  }
}
void 
KSameWidget::sizeChanged() {
     stone->setFixedSize(stone->sizeHint());
     updateRects();
}     

KSameWidget::~KSameWidget() {
     //  debug("~KSameWidget\n"); 
}
#define MAX(a,b) ((a)>(b)?(a):(b))
void KSameWidget::m_new() {
     if (israndom) {
	  if (stone->gameover()||
	      (KMsgBox::yesNo(this, i18n("SameGame: New Game"), 
			      i18n("Do you want to resign?"), KMsgBox::STOP)==1)) {
	       srandom(time(0));
	       set_board(random());
	       stone->newgame(board);
	  }
     } else {
	  QDialog dlg(0,i18n("Board"),1);
	  int w=10,h=10;
	  
	  KIntegerLine bno(&dlg,i18n("Hello"));
	  bno.setFocus();
	  QLabel l(&bno,i18n("Please &enter the board number:"),&dlg);
	  
	  l.move(10,h);
	  l.resize(l.sizeHint());
	  w=l.width()+20;
	  h+=l.height()+10;
	  bno.move(10,h);
	  w=MAX(bno.width()+20,w);
	  h+=bno.height()+10;
     
	  QPushButton ok(i18n("OK"), &dlg);
	  ok.setDefault(true);
	  QPushButton cancel(i18n("Cancel"), &dlg);
	  
	  w=MAX(ok.width()+cancel.width()+40,w);
	  
	  ok.move(w-ok.width()-20-cancel.width(),h);
	  cancel.move(w-cancel.width()-10,h);
	  h+=MAX(ok.height(),cancel.height())+10;
	  
	  dlg.resize(w,h);
	  dlg.setFixedSize(w,h);
	  dlg.connect(&ok, SIGNAL(clicked()), SLOT(accept()) );
	  dlg.connect(&cancel, SIGNAL(clicked()), SLOT(reject()) );
	  dlg.connect(&bno, SIGNAL(returnPressed()), SLOT(accept()) );
	  bno.setValue(board);
	  bno.selectAll();
	  if (dlg.exec()) {
	       set_board(bno.value());
	       stone->newgame(board);
	  }
     }
	  
}
void KSameWidget::m_load() {
     debug(i18n("menu load not supported")); 
}
void KSameWidget::m_save() {
     debug(i18n("menu save not supported"));
}
void KSameWidget::m_showhs() {
     if (highscore->isVisible())
	  highscore->hide();
     else highscore->show();
}
void KSameWidget::m_quit() {
     if (KMsgBox::yesNo(this, i18n("SameGame - Quit"), 
			i18n("Do you really want to quit?"), KMsgBox::STOP)==1) {
	  kapp->quit();
     }
}

void KSameWidget::m_colors( int id) {
     debug(i18n("menu colors not supported"));
     int colors=id;
     char p[30];
     colors=stone->colors();
     sprintf(p,i18n("Colors: %01i"),colors);
     status->changeItem(p,1);
}

void KSameWidget::m_tglboard() {

     israndom=!israndom;
     optionsmenu->setItemChecked(random_item, israndom);
}
void KSameWidget::set_board(int no) {
     char p[30];
     if (no<0) no*=-1;
     board=no%1000000;
     sprintf(p,i18n("Board: %06i"),board);
     status->changeItem(p,2);        
}
void KSameWidget::gameover() {
     debug(i18n("GameOver"));
     highscore->add(stone->board(),stone->score(),stone->colors());
}
void KSameWidget::set_marked(int m) {
    char p[30];
    sprintf(p,i18n("Marked: %06i"),m);
    status->changeItem(p,3);   
}
void KSameWidget::set_score(int score) {
    char p[30];
    sprintf(p,i18n("Score: %06i"),score);
    status->changeItem(p,4);   
}
void KSameWidget::history_add(int x,int y) {
     history.append(new QPoint(x,y));
}
void KSameWidget::history_clear() {
     history.clear();
}

void KSameWidget::readProperties(KConfig *conf) {
     history.clear();
     
     if (!conf->hasKey("Board")||!conf->hasKey("Stones")) {
	  set_board(time(0));
	  return;
     }
	 
     board=conf->readNumEntry("Board");
     QString s=conf->readEntry("Stones");

     set_board(board);
     stone->newgame(board);
     char *p=s.data();
     while (*p) {
	  int x=-1,y=-1;
	  sscanf(p,"%02X%02X",&x,&y);
	  history.append(new QPoint(x,y));
	  stone->remove(x,y);
	  *p?*++p?*++p?*++p?p++:0:0:0:0;
     }
}

void KSameWidget::saveProperties(KConfig *conf) {
     char *tmpbuf=new char[history.count()*4+1];
     char *p=tmpbuf;
     for (QPoint *item=history.first();item;item=history.next()) {
	  sprintf(p,"%02X%02X",item->x(),item->y());
	  p+=4;
     }
     *p=0;
     conf->writeEntry("Stones",tmpbuf);
     conf->writeEntry("Board",board);
     conf->sync();
     delete[] tmpbuf;
}










