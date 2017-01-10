/*
    $Id: print.h,v 1.4 1999/01/10 14:54:20 bieker Exp $

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
  
*/


#ifndef _PRINT_DIALOG_H_
#define _PRINT_DIALOG_H_

#include <qmsgbox.h>
#include <qapp.h>
#include <qfiledlg.h>
#include <qframe.h> 
#include <qbttngrp.h>
#include <qchkbox.h>
#include <qcombo.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>
#include <qprinter.h>
#include <qradiobt.h>
#include <qtooltip.h>

#include <qstring.h>
#include <qfont.h>
#include <kapp.h>

#define US_LETTER i18n("US Letter 8.5x11 in")
#define US_LEGAL i18n("US Legal 8.5x14 in")
#define US_LEDGER i18n("US Ledger 11x17 in")
#define US_EXECUTIVE i18n("US Executive 7.25x10.5 in")
#define JAP_LETTER i18n("Japanese Letter 18.2x25.7 cm")
#define JAP_LEGAL i18n("Japanese Legal 25.7x36.4 cm")
#define DIN_A3 i18n("DIN A3 20.7x42 cm")
#define DIN_A4 i18n("DIN A4 21x29.7 cm")
#define DIN_A5 i18n("DIN A5 15.1x21 cm")
#define DIN_A6 i18n("DIN A6 10.7x14.8 cm")
#define DIN_B4 i18n("DIN B4 25.5x36.4 cm")


struct printinfo {
  QString file;
  QString cmd;
  int lpr;
  int scale;
  int margins;
  double xmargin;
  double ymargin;
  QString pagesize;
};

class PrintDialog : public QDialog {

    Q_OBJECT

public:
    PrintDialog( QWidget *parent = NULL, const char *name = NULL ,bool = FALSE);

    struct printinfo* getInfo();
    void setWidgets(struct printinfo *pi);

protected:
    void focusInEvent ( QFocusEvent *);

signals:
    void print();

public slots:
    void choosefile();
    void ready();
    void cancel();
    void filebuttontoggled();
    void help();
    void margins_toggled(bool);

private:

    QString command;
    QLineEdit *filename;
    QLineEdit *cmdedit;
    QLabel *cmdlabel;
    QLabel *pagesizelabel;
    QLabel *xmarginlabel;    
    QLabel *ymarginlabel;

    QLineEdit *xmarginedit;    
    QLineEdit *ymarginedit;

    QRadioButton *filebutton;
    QRadioButton *lprbutton;
    QCheckBox *scalebutton;
    QCheckBox *marginbutton;
    QComboBox *papercombo;

    QButtonGroup *bg;

    QGroupBox	 *box1;

    
    QPushButton	 *ok_button;
    QPushButton	 *cancel_button;
    QPushButton	 *help_button;
    QPushButton	 *selectfile;
    
  struct printinfo pi;

};


#endif
