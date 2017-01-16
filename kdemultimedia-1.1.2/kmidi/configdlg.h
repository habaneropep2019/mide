
/*
 *
 * kscd -- A simple CD player for the KDE project           
 *
 * $Id$
 * 
 * Copyright (C) 1997 Bernd Johannes Wuebben 
 * wuebben@math.cornell.edu
 *
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#ifndef _CONFIG_DLG_H_
#define _CONFIG_DLG_H_

#include <qgrpbox.h> 
#include <qdialog.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qframe.h>
#include <qchkbox.h>
#include <kcolordlg.h>

struct configstruct{
  QColor led_color;
  QColor background_color;
  bool   tooltips;
};


class ConfigDlg : public QDialog {

Q_OBJECT

public:

  ConfigDlg(QWidget *parent=0, struct configstruct * data=0,const char *name=0);
  ~ConfigDlg() {}


  struct configstruct * ConfigDlg::getData();

private slots:
  void cancelbutton();
  void okbutton();
  void set_led_color();
  void set_background_color();
  void help();
  void ttclicked();

signals:
  void color_change();

public:
  bool colors_changed;
  
private:

  struct configstruct configdata;
  QGroupBox *box;
  
  QPushButton *ok;
  QPushButton *cancel;

  QLabel *label1;
  QFrame *qframe1;
  QPushButton *button1;

  QLabel *label2;
  QFrame *qframe2;
  QPushButton *button2;

  QLabel *label3;
  QFrame *qframe3;
  QPushButton *button3;

  QLabel *label4;
  QFrame *qframe4;
  QPushButton *button4;

  QLabel *tooltipslabel;
  QCheckBox *ttcheckbox;

};
#endif
