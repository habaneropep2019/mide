/*
  $Id: filldlg.h,v 1.8.2.1 1999/03/15 14:26:58 dfaure Exp $
 
         kedit+ , a simple text editor for the KDE project

  Copyright 1997 Bernd Johannes Wuebben, wuebben@math.cornell.edu
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



#include <qpopmenu.h>
#include <qmenubar.h>
#include <qapp.h>
#include <qkeycode.h>
#include <qaccel.h>
#include <qobject.h>
#include <qlined.h>
#include <qradiobt.h>
#include <qfiledlg.h>
#include <qchkbox.h>
#include <qmsgbox.h>
#include <qcombo.h>
#include <qpushbt.h>
#include <qgrpbox.h>
#include <qlabel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  

#ifndef __FILL_H__
#define __FILL_H__

struct fill_struct{
  
  bool fill_column_is_set;
  bool word_wrap_is_set;
  bool backup_copies_is_set;
  int  fill_column_value;
  QString mailcmd;
};

class FillDlg : public QDialog
{
    Q_OBJECT

public:

    FillDlg ( QWidget *parent = 0, const char *name=0);

    struct fill_struct getFillCol();
    bool fill();
    bool wordwrap();
    QLineEdit *values;
    QLineEdit *mailcmd;
    QLabel *mailcmdlabel;
    void setWidgets(struct fill_struct);

private:
    
    struct fill_struct   fillstr;
    QPushButton *ok, *cancel;
    QCheckBox *fill_column;
    QCheckBox *word_wrap;
    QCheckBox *backup_copies;
    QGroupBox *frame1;

public slots:

    void synchronize(bool);
    void  checkit();

};

#endif







