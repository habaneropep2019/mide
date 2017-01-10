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

#ifndef _KSAMEWIDGET
#define _KSAMEWIDGET
#include <qwidget.h>
#include <qpushbt.h>
#include <ktopwidget.h> 
#include <qlist.h>
#include <qpoint.h>
#include "StoneWidget.h"
#include "HighScore.h"

class KSameWidget: public KTopLevelWidget {
 Q_OBJECT
public:
    KSameWidget(); 
    ~KSameWidget();
 
private:
    StoneWidget *stone;
    HighScore *highscore;
    KStatusBar *status;
    
    QList<QPoint> history;

    QPopupMenu *colorsmenu;
    QPopupMenu *optionsmenu;
    int multispin_item;
    int random_item;

    int israndom;
    int board;

    void set_board(int no);
protected:
    void load_history();
    void store_history();

    virtual void saveProperties(KConfig *conf);
    virtual void readProperties(KConfig *conf);
public slots: 
    void sizeChanged();   
  /* File Menu */
    void m_new();
    void m_load();
    void m_save();
    void m_showhs();
    void m_quit();
    
    

    /* Options Menu */
    void m_colors(int);
    void m_tglboard();
    
    void gameover();
    void set_score(int);
    void set_marked(int); 
    void history_add(int,int);
    void history_clear();
};



#endif
