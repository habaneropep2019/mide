
/*
 *
 *            kPPP: A front end for pppd for the KDE project
 *
 * $Id: modemcmds.h,v 1.6 1998/11/01 21:18:56 porten Exp $
 *
 * Copyright (C) 1997 Bernd Johannes Wuebben
 * wuebben@math.cornell.edu
 *
 * based on EzPPP:
 * Copyright (C) 1997  Jay Painter
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


#ifndef _MODEMCMDS_H_
#define _MODEMCMDS_H_

#include <qgrpbox.h>
#include <qdialog.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qpainter.h>
#include <qlabel.h>

#include "pppdata.h"

class ModemCommands : public QDialog {

Q_OBJECT

public:

  ModemCommands(QWidget *parent=0, const char *name=0);
  ~ModemCommands() {}


private slots:
  void cancelbutton();
  void okbutton();

private:

  QGroupBox *box;

  QPushButton *ok;
  QPushButton *cancel;

  QLineEdit *initstr;
  QLabel *label1;

  QLineEdit *initresp;
  QLabel *label2;

  QLabel *lpreinitslider;
  QLabel *lpreinit;

  QLabel *linitslider;
  QLabel *label3;

  QLineEdit *dialstr;
  QLabel *label4;

  QLineEdit *connectresp;
  QLabel *label5;

  QLineEdit *busyresp;
  QLabel *label6;

  QLineEdit *nocarrierresp;
  QLabel *label7;

  QLineEdit *nodialtoneresp;
  QLabel *label8;

  QLineEdit *hangupstr;
  QLabel *label9;

  QLineEdit *hangupresp;
  QLabel *label10;

  QLineEdit *answerstr;
  QLabel *label11;

  QLineEdit *ringresp;
  QLabel *label12;

  QLineEdit *answerresp;
  QLabel *label13;

  QLineEdit *escapestr;
  QLabel *label14;

  QLineEdit *escaperesp;
  QLabel *label15;

  QLabel *lslider;
  QLabel *label16;

  QLineEdit *volume_off, *volume_medium, *volume_high;
};
#endif



