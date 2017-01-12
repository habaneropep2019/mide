
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


#ifndef _MGCONFIG_DLG_H_
#define _MGCONFIG_DLG_H_

#include <qgrpbox.h> 
#include <qdialog.h>
#include <qlined.h>
#include <qpushbt.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qframe.h>
#include <qchkbox.h>
#include <qlined.h>
#include <qradiobt.h>
#include <qbutton.h> 
#include <qbttngrp.h> 
#include <kspinbox.h>

#include "kscd.h"


class MGConfigDlg : public QDialog {

Q_OBJECT

public:

  MGConfigDlg(QWidget *parent=0, struct mgconfigstruct * data=0,const char *name=0);
  ~MGConfigDlg() {}


  struct mgconfigstruct * getData();

private slots:

  void help();
  void brightness_changed();
  void width_changed(const char*);
  void height_changed(const char*);

  
private:

  struct mgconfigstruct mgconfigdata;

  QGroupBox *box;
  
  QLabel *label1;
  KNumericSpinBox *bspin;

  QLabel *label2;
  QLineEdit *height_edit;

  QLabel *label3;
  QLineEdit *width_edit;

  QPushButton *helpbutton;
};
#endif
