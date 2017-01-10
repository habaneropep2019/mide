/*
    $Id: print.h,v 1.5 1997/07/22 03:16:47 wuebben Exp $

    Requires the Qt widget libraries, available at no cost at 
    http://www.troll.no
       
    Copyright (C) 1996 Bernd Johannes Wuebben   
                       wuebben@math.cornell.edu

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
  
    $Log: print.h,v $
    Revision 1.5  1997/07/22 03:16:47  wuebben
    implemented mailing

    Revision 1.1  1997/05/16 04:45:07  wuebben
    Initial revision

    Revision 1.1  1997/05/03 17:20:56  wuebben
    Initial revision

    Revision 1.1  1997/04/20 00:18:15  wuebben
    Initial revision

    Revision 1.2  1997/03/02 22:40:59  wuebben
    *** empty log message ***

    Revision 1.1  1997/01/04 17:36:44  wuebben
    Initial revision


*/


#ifndef _PRINT_DIALOG_H_
#define _PRINT_DIALOG_H_

#include <qmsgbox.h>
#include <qpixmap.h>
#include <qapp.h>
#include <qframe.h> 
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qframe.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qscrbar.h>
#include <qtooltip.h>

#include <qstring.h>
#include <qfont.h>


struct printinfo {
  QString command;
  int raw;
  int selection;
};

class PrintDialog : public QDialog {

    Q_OBJECT

public:
    PrintDialog( QWidget *parent = NULL, const char *name = NULL,
			bool modal = FALSE );

    struct printinfo getCommand();
    void setWidgets(struct printinfo pi);

signals:


private slots:



private:

    QString command;
    QLineEdit *commandbox;
    QRadioButton *commandbutton;
    QRadioButton *rawbutton;
    QRadioButton *allbutton;
    QRadioButton *selectionbutton;
   
    QButtonGroup *bg;
    QButtonGroup *bg1;

    QGroupBox	 *box1;
    QGroupBox	 *box2;

    
    QPushButton	 *ok_button;
    QPushButton	 *cancel_button;



};


#endif
